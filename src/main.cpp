/*
  Trim Tab Controller — Wokwi starter v0.1
  Target: ESP32-S3 DevKit (proxy for Waveshare ESP32-S3 Touch AMOLED 1.75-G)
  Purpose: develop/test application logic before buying hardware.

  SAFETY NOTE:
  This is a software simulation, NOT a validated marine controller.
  The simulated GPIO outputs are NOT a substitute for independent hardware
  interlocks, fused motor wiring, or a hardware manual-override path.
*/

#include <WiFi.h>
#include <WebServer.h>

static const int PIN_LEFT_UP    = 4;
static const int PIN_LEFT_DOWN  = 5;
static const int PIN_RIGHT_UP   = 6;
static const int PIN_RIGHT_DOWN = 7;

// Placeholder logical outputs only; never connect these directly to motors.
static const int OUT_LEFT_UP    = 12;
static const int OUT_LEFT_DOWN  = 13;
static const int OUT_RIGHT_UP   = 14;
static const int OUT_RIGHT_DOWN = 15;

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

WebServer server(80);

enum Mode { MODE_MANUAL, MODE_AUTO, MODE_HOLD, MODE_DEGRADED, MODE_FAULT };
const char* modeName(Mode m) {
  switch (m) {
    case MODE_MANUAL: return "MANUAL";
    case MODE_AUTO: return "AUTO";
    case MODE_HOLD: return "HOLD";
    case MODE_DEGRADED: return "DEGRADED";
    case MODE_FAULT: return "FAULT";
  }
  return "UNKNOWN";
}

struct Actuator {
  float positionPct = 0.0f;
  float travelSeconds = 10.0f;
  int direction = 0; // -1 retract/up, +1 extend/down, 0 stop
  unsigned long lastUpdateMs = 0;
  unsigned long commandUntilMs = 0;
};

Actuator leftTab, rightTab;
Mode mode = MODE_MANUAL;
bool gpsValid = true;
float simulatedSpeedKnots = 18.0f;
float simulatedRollDeg = 0.0f;
float simulatedPitchDeg = 0.0f;
String lastEvent = "Boot: outputs safe";
unsigned long lastTelemetryMs = 0;
bool autoEnabled = false;

void logEvent(const String& s) {
  lastEvent = String(millis()) + " ms: " + s;
  Serial.println(lastEvent);
}

void allOutputsOff() {
  digitalWrite(OUT_LEFT_UP, LOW);
  digitalWrite(OUT_LEFT_DOWN, LOW);
  digitalWrite(OUT_RIGHT_UP, LOW);
  digitalWrite(OUT_RIGHT_DOWN, LOW);
}

void applyOutputPins() {
  // Simulation invariant: never energize both directions for one actuator.
  digitalWrite(OUT_LEFT_UP, leftTab.direction < 0 ? HIGH : LOW);
  digitalWrite(OUT_LEFT_DOWN, leftTab.direction > 0 ? HIGH : LOW);
  digitalWrite(OUT_RIGHT_UP, rightTab.direction < 0 ? HIGH : LOW);
  digitalWrite(OUT_RIGHT_DOWN, rightTab.direction > 0 ? HIGH : LOW);
}

void stopActuator(Actuator& a) {
  a.direction = 0;
  a.commandUntilMs = 0;
}

void stopAll(const String& reason) {
  stopActuator(leftTab);
  stopActuator(rightTab);
  allOutputsOff();
  applyOutputPins();
  logEvent(reason);
}

void commandActuator(Actuator& a, int dir, unsigned long durationMs, const String& label) {
  if (mode == MODE_FAULT || mode == MODE_DEGRADED || mode == MODE_HOLD) {
    logEvent("Command rejected in " + String(modeName(mode)));
    return;
  }
  if (dir == 0) {
    stopActuator(a);
  } else {
    a.direction = dir;
    a.commandUntilMs = millis() + durationMs;
  }
  applyOutputPins();
  logEvent(label);
}

void setMode(Mode next) {
  if (next == MODE_AUTO && !gpsValid) {
    mode = MODE_DEGRADED;
    autoEnabled = false;
    stopAll("AUTO denied: GPS invalid");
    return;
  }
  mode = next;
  autoEnabled = (next == MODE_AUTO);
  if (next != MODE_AUTO) {
    stopAll("Mode changed to " + String(modeName(next)));
  } else {
    logEvent("Mode changed to AUTO");
  }
}

void updateActuator(Actuator& a) {
  unsigned long now = millis();
  if (a.lastUpdateMs == 0) a.lastUpdateMs = now;
  float dt = (now - a.lastUpdateMs) / 1000.0f;
  a.lastUpdateMs = now;

  if (a.direction != 0) {
    a.positionPct += (a.direction * 100.0f * dt) / max(0.1f, a.travelSeconds);
    if (a.positionPct >= 100.0f) {
      a.positionPct = 100.0f;
      stopActuator(a);
      logEvent("Simulated extend endpoint");
    } else if (a.positionPct <= 0.0f) {
      a.positionPct = 0.0f;
      stopActuator(a);
      logEvent("Simulated retract endpoint");
    } else if (a.commandUntilMs != 0 && (long)(now - a.commandUntilMs) >= 0) {
      stopActuator(a);
    }
  }
}

String page() {
  String html = R"HTML(
<!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Trim Controller — Simulator</title>
<style>
body{font:16px system-ui;margin:0;background:#101820;color:#eef4f7}
main{max-width:760px;margin:auto;padding:18px}
.card{background:#1b2a34;border:1px solid #354b58;border-radius:12px;padding:14px;margin:12px 0}
button,select,input{font:inherit;padding:10px;margin:4px;border-radius:8px;border:1px solid #627987}
button{background:#d7edf7;color:#10202a;cursor:pointer}button.danger{background:#ffccbc}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
small{color:#b9cbd4}.val{font-size:1.3rem;font-weight:700}
</style></head><body><main>
<h2>Trim Controller — virtual bench</h2>
<p><small>Wokwi starter v0.1 · simulated actuators only · not for connection to real motors</small></p>
<div class="card"><div>Mode: <span class="val" id="mode">…</span></div>
<div>GPS: <span id="gps">…</span> · Speed: <span id="speed">…</span> kn</div>
<div>Roll: <span id="roll">…</span>° · Pitch: <span id="pitch">…</span>°</div></div>
<div class="grid">
<div class="card"><h3>Left tab</h3><div class="val" id="leftpos">—</div>
<button onclick="cmd('left','up')">UP / retract</button><button onclick="cmd('left','down')">DOWN / extend</button>
<button class="danger" onclick="cmd('left','stop')">STOP</button></div>
<div class="card"><h3>Right tab</h3><div class="val" id="rightpos">—</div>
<button onclick="cmd('right','up')">UP / retract</button><button onclick="cmd('right','down')">DOWN / extend</button>
<button class="danger" onclick="cmd('right','stop')">STOP</button></div></div>
<div class="card"><h3>Modes & fault injection</h3>
<button onclick="modeCmd('manual')">MANUAL</button><button onclick="modeCmd('auto')">AUTO</button><button onclick="modeCmd('hold')">HOLD</button>
<button onclick="fetch('/api/gps?valid=0')">Invalidate GPS</button><button onclick="fetch('/api/gps?valid=1')">Restore GPS</button>
<button class="danger" onclick="fetch('/api/fault?active=1')">Inject FAULT</button><button onclick="fetch('/api/fault?active=0')">Clear FAULT</button>
<p><small>Telemetry and position are simulated. Physical buttons on the diagram are momentary GPIO inputs.</small></p></div>
<div class="card"><h3>Last event</h3><div id="event">—</div></div>
<script>
async function api(url){let r=await fetch(url);return r.json()}
async function cmd(tab,dir){await api('/api/cmd?tab='+tab+'&dir='+dir+'&ms=350')}
async function modeCmd(m){await api('/api/mode?value='+m)}
async function refresh(){try{let s=await api('/api/state');
document.getElementById('mode').textContent=s.mode;
document.getElementById('gps').textContent=s.gpsValid?'VALID':'INVALID';
document.getElementById('speed').textContent=s.speed.toFixed(1);
document.getElementById('roll').textContent=s.roll.toFixed(1);
document.getElementById('pitch').textContent=s.pitch.toFixed(1);
document.getElementById('leftpos').textContent=s.left.toFixed(0)+'%';
document.getElementById('rightpos').textContent=s.right.toFixed(0)+'%';
document.getElementById('event').textContent=s.event;
}catch(e){}}
setInterval(refresh,500);refresh();
</script></main></body></html>
)HTML";
  return html;
}

void sendState() {
  String json = "{";
  json += "\"mode\":\"" + String(modeName(mode)) + "\",";
  json += "\"gpsValid\":" + String(gpsValid ? "true" : "false") + ",";
  json += "\"speed\":" + String(simulatedSpeedKnots, 1) + ",";
  json += "\"roll\":" + String(simulatedRollDeg, 1) + ",";
  json += "\"pitch\":" + String(simulatedPitchDeg, 1) + ",";
  json += "\"left\":" + String(leftTab.positionPct, 1) + ",";
  json += "\"right\":" + String(rightTab.positionPct, 1) + ",";
  json += "\"event\":\"";
  String escaped = lastEvent;
  escaped.replace("\\", "\\\\"); escaped.replace("\"", "\\\"");
  json += escaped + "\"}";
  server.send(200, "application/json", json);
}

void setupWeb() {
  server.on("/", HTTP_GET, [](){ server.send(200, "text/html", page()); });
  server.on("/api/state", HTTP_GET, [](){ sendState(); });
  server.on("/api/cmd", HTTP_GET, [](){
    String tab = server.arg("tab"), dir = server.arg("dir");
    int d = dir == "up" ? -1 : dir == "down" ? 1 : 0;
    unsigned long ms = constrain(server.arg("ms").toInt(), 0, 1500);
    if (mode == MODE_AUTO) setMode(MODE_MANUAL); // manual action cancels AUTO
    if (tab == "left") commandActuator(leftTab, d, ms, "Web command left " + dir);
    else if (tab == "right") commandActuator(rightTab, d, ms, "Web command right " + dir);
    server.send(200, "application/json", "{\"ok\":true}");
  });
  server.on("/api/mode", HTTP_GET, [](){
    String v = server.arg("value");
    if (v == "manual") setMode(MODE_MANUAL);
    else if (v == "hold") setMode(MODE_HOLD);
    else if (v == "auto") setMode(MODE_AUTO);
    server.send(200, "application/json", "{\"ok\":true}");
  });
  server.on("/api/gps", HTTP_GET, [](){
    gpsValid = server.arg("valid") == "1";
    if (!gpsValid && mode == MODE_AUTO) {
      mode = MODE_DEGRADED; autoEnabled = false;
      stopAll("GPS invalid: AUTO stopped");
    } else logEvent(String("GPS valid=") + (gpsValid ? "true" : "false"));
    server.send(200, "application/json", "{\"ok\":true}");
  });
  server.on("/api/fault", HTTP_GET, [](){
    bool active = server.arg("active") == "1";
    if (active) { mode = MODE_FAULT; autoEnabled = false; stopAll("FAULT injected: outputs stopped"); }
    else { mode = MODE_MANUAL; logEvent("FAULT cleared; MANUAL only"); }
    server.send(200, "application/json", "{\"ok\":true}");
  });
  server.begin();
}

void handlePhysicalButtons() {
  bool lu = digitalRead(PIN_LEFT_UP) == LOW;
  bool ld = digitalRead(PIN_LEFT_DOWN) == LOW;
  bool ru = digitalRead(PIN_RIGHT_UP) == LOW;
  bool rd = digitalRead(PIN_RIGHT_DOWN) == LOW;
  static bool prevAny = false;
  static bool prevConflictLeft = false;
  static bool prevConflictRight = false;
  bool any = lu || ld || ru || rd;

  if (any && mode == MODE_AUTO) {
    mode = MODE_MANUAL;
    autoEnabled = false;
    stopAll("Physical input cancels AUTO");
  }

  if (mode != MODE_FAULT) {
    if (lu && ld) {
      stopActuator(leftTab);
      if (!prevConflictLeft) logEvent("Conflicting LEFT buttons: stopped");
    } else if (lu) {
      leftTab.direction = -1; leftTab.commandUntilMs = 0;
    } else if (ld) {
      leftTab.direction = 1; leftTab.commandUntilMs = 0;
    } else stopActuator(leftTab);

    if (ru && rd) {
      stopActuator(rightTab);
      if (!prevConflictRight) logEvent("Conflicting RIGHT buttons: stopped");
    } else if (ru) {
      rightTab.direction = -1; rightTab.commandUntilMs = 0;
    } else if (rd) {
      rightTab.direction = 1; rightTab.commandUntilMs = 0;
    } else stopActuator(rightTab);

    prevConflictLeft = lu && ld;
    prevConflictRight = ru && rd;
    applyOutputPins();
  } else if (any) {
    stopAll("Physical movement inhibited in FAULT");
  }

  if (prevAny && !any) {
    stopActuator(leftTab);
    stopActuator(rightTab);
    applyOutputPins();
    logEvent("Physical buttons released: outputs stopped");
  }
  prevAny = any;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LEFT_UP, INPUT_PULLUP);
  pinMode(PIN_LEFT_DOWN, INPUT_PULLUP);
  pinMode(PIN_RIGHT_UP, INPUT_PULLUP);
  pinMode(PIN_RIGHT_DOWN, INPUT_PULLUP);
  pinMode(OUT_LEFT_UP, OUTPUT);
  pinMode(OUT_LEFT_DOWN, OUTPUT);
  pinMode(OUT_RIGHT_UP, OUTPUT);
  pinMode(OUT_RIGHT_DOWN, OUTPUT);
  allOutputsOff();

  Serial.println("\nTrim controller Wokwi starter v0.1");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to Wokwi-GUEST");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 12000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Wi-Fi connected; controller IP: ");
    Serial.println(WiFi.localIP());
    setupWeb();
  } else {
    Serial.println("Wi-Fi unavailable; physical-button simulation remains active.");
  }
  leftTab.lastUpdateMs = rightTab.lastUpdateMs = millis();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) server.handleClient();
  handlePhysicalButtons();
  updateActuator(leftTab);
  updateActuator(rightTab);

  // Deliberately not a finished boat-control algorithm.
  if (millis() - lastTelemetryMs > 1000) {
    lastTelemetryMs = millis();
    applyOutputPins();
  }
  delay(5);
}
