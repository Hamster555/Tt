/*
  Trim Tab Controller — Wokwi simulation only.
  GPIO pins are logical signals and must never connect directly to motors.
  The independent physical manual path is outside this software model.
*/
#include <WiFi.h>
#include <WebServer.h>
#include "control_logic.h"

static const int PIN_LEFT_UP=4, PIN_LEFT_DOWN=5, PIN_RIGHT_UP=6, PIN_RIGHT_DOWN=7;
static const int OUT_LEFT_UP=12, OUT_LEFT_DOWN=13, OUT_RIGHT_UP=14, OUT_RIGHT_DOWN=15;
const char* WIFI_SSID="Wokwi-GUEST";
const char* WIFI_PASS="";
WebServer server(80);
trimcontrol::ControlCore controller;
float simulatedSpeedKnots=18.0f, simulatedRollDeg=0.0f, simulatedPitchDeg=0.0f;
String lastEvent="Boot: outputs safe";
trimcontrol::ControlEvent lastLoggedEvent=trimcontrol::ControlEvent::BootSafe;
unsigned long lastTelemetryMs=0;

void logCoreEvent() {
  const trimcontrol::ControlEvent event=controller.lastEvent();
  if(event==lastLoggedEvent)return;
  lastLoggedEvent=event;
  lastEvent=String(millis())+" ms: "+trimcontrol::eventName(event);
  Serial.println(lastEvent);
}

void allOutputsOff() {
  digitalWrite(OUT_LEFT_UP,LOW); digitalWrite(OUT_LEFT_DOWN,LOW);
  digitalWrite(OUT_RIGHT_UP,LOW); digitalWrite(OUT_RIGHT_DOWN,LOW);
}

void applyOutputPins() {
  // Each direction is derived from one exclusive enum value.
  const auto left=controller.leftDirection(), right=controller.rightDirection();
  digitalWrite(OUT_LEFT_UP,left==trimcontrol::Retract?HIGH:LOW);
  digitalWrite(OUT_LEFT_DOWN,left==trimcontrol::Extend?HIGH:LOW);
  digitalWrite(OUT_RIGHT_UP,right==trimcontrol::Retract?HIGH:LOW);
  digitalWrite(OUT_RIGHT_DOWN,right==trimcontrol::Extend?HIGH:LOW);
}

String page() {
  return R"HTML(<!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Trim Controller simulator</title><style>body{font:16px system-ui;margin:18px;background:#101820;color:#eef4f7}button{font:inherit;padding:10px;margin:4px}section{padding:12px;background:#1b2a34;margin:12px 0;border-radius:10px}</style></head><body>
<h2>Trim Controller — virtual bench</h2><p>Simulated actuators only; not for connection to real motors.</p>
<section>Mode: <b id="mode">—</b><br>GPS validity: <span id="gps">—</span><br>Speed: <span id="speed">—</span> kn<br>Roll: <span id="roll">—</span>° · Pitch: <span id="pitch">—</span>°<br>Left: <span id="left">—</span>% · Right: <span id="right">—</span>%</section>
<section><h3>Left tab</h3><button onclick="cmd('left','up')">UP / retract</button><button onclick="cmd('left','down')">DOWN / extend</button><button onclick="cmd('left','stop')">STOP</button>
<h3>Right tab</h3><button onclick="cmd('right','up')">UP / retract</button><button onclick="cmd('right','down')">DOWN / extend</button><button onclick="cmd('right','stop')">STOP</button></section>
<section><h3>Modes and simulation</h3><button onclick="mode('manual')">MANUAL</button><button onclick="mode('auto')">AUTO</button><button onclick="mode('hold')">HOLD</button>
<button onclick="fetch('/api/gps?valid=0')">Invalidate GPS</button><button onclick="fetch('/api/gps?valid=1')">Restore GPS</button>
<button onclick="fetch('/api/fault?active=1')">Inject FAULT</button><button onclick="fetch('/api/fault?active=0')">Clear FAULT</button></section>
<section>Last event: <span id="event">—</span><p>GPIO buttons are simulated and do not prove an independent manual path.</p></section>
<script>
async function api(u){let r=await fetch(u);return r.json()} async function cmd(t,d){let r=await api('/api/cmd?tab='+t+'&dir='+d+'&ms=350');if(!r.ok)console.log(r)}
async function mode(m){await api('/api/mode?value='+m)}
async function refresh(){try{let s=await api('/api/state');for(let k of ['mode','gpsValid','speed','roll','pitch','left','right','event'])document.getElementById(k==='gpsValid'?'gps':k).textContent=k==='gpsValid'?(s.gpsValid?'VALID':'INVALID'):s[k]}catch(e){}}
setInterval(refresh,500);refresh();
</script></body></html>)HTML";
}

void sendState() {
  const auto s=controller.telemetry(simulatedSpeedKnots,simulatedRollDeg,simulatedPitchDeg);
  String json="{";
  json+="\"mode\":\""+String(trimcontrol::modeName(s.mode))+"\",";
  json+="\"gpsValid\":"+String(s.requiredInputsValid?"true":"false")+",";
  json+="\"speed\":"+String(s.speedKnots,1)+",\"roll\":"+String(s.rollDeg,1)+",\"pitch\":"+String(s.pitchDeg,1)+",";
  json+="\"left\":"+String(s.leftPositionPct,1)+",\"right\":"+String(s.rightPositionPct,1)+",\"event\":\"";
  String escaped=lastEvent; escaped.replace("\\","\\\\"); escaped.replace("\"","\\\"");
  json+=escaped+"\"}";
  server.send(200,"application/json",json);
}

void setupWeb() {
  server.on("/",HTTP_GET,[](){server.send(200,"text/html",page());});
  server.on("/api/state",HTTP_GET,[](){sendState();});
  server.on("/api/cmd",HTTP_GET,[](){
    const String tab=server.arg("tab"), dir=server.arg("dir");
    if((tab!="left"&&tab!="right")||(dir!="up"&&dir!="down"&&dir!="stop")){
      server.send(400,"application/json","{\"ok\":false,\"error\":\"invalid tab or direction\"}");return;
    }
    const int msValue=server.arg("ms").toInt();
    const trimcontrol::Direction d=dir=="up"?trimcontrol::Retract:dir=="down"?trimcontrol::Extend:trimcontrol::Stop;
    if(d!=trimcontrol::Stop&&(msValue<=0||msValue>static_cast<int>(trimcontrol::SIMULATOR_MAX_COMMAND_MS))){
      server.send(400,"application/json","{\"ok\":false,\"error\":\"invalid simulator duration\"}");return;
    }
    const auto result=controller.webCommand(tab=="left"?trimcontrol::Tab::Left:trimcontrol::Tab::Right,d,msValue<0?0:static_cast<uint32_t>(msValue),millis());
    logCoreEvent();applyOutputPins();
    const bool accepted=result==trimcontrol::CommandResult::Accepted;
    server.send(accepted?200:409,"application/json",accepted?"{\"ok\":true}":"{\"ok\":false,\"error\":\"command rejected\"}");
  });
  server.on("/api/mode",HTTP_GET,[](){
    const String v=server.arg("value");bool valid=true;
    if(v=="manual")controller.requestMode(trimcontrol::Mode::Manual);
    else if(v=="auto")valid=controller.requestMode(trimcontrol::Mode::Auto);
    else if(v=="hold")controller.requestMode(trimcontrol::Mode::Hold);
    else{server.send(400,"application/json","{\"ok\":false,\"error\":\"invalid mode\"}");return;}
    logCoreEvent();applyOutputPins();
    server.send(valid?200:409,"application/json",valid?"{\"ok\":true}":"{\"ok\":false,\"error\":\"mode request rejected\"}");
  });
  server.on("/api/gps",HTTP_GET,[](){
    const String value=server.arg("valid");
    if(value!="0"&&value!="1"){server.send(400,"application/json","{\"ok\":false}");return;}
    controller.setRequiredInputsValid(value=="1");logCoreEvent();applyOutputPins();
    server.send(200,"application/json","{\"ok\":true}");
  });
  server.on("/api/fault",HTTP_GET,[](){
    const String value=server.arg("active");
    if(value!="0"&&value!="1"){server.send(400,"application/json","{\"ok\":false}");return;}
    controller.setFault(value=="1");logCoreEvent();applyOutputPins();
    server.send(200,"application/json","{\"ok\":true}");
  });
  server.begin();
}

void handlePhysicalButtons() {
  trimcontrol::PhysicalInputs raw;
  raw.leftRetract=trimcontrol::activeLowPressed(digitalRead(PIN_LEFT_UP)==HIGH);
  raw.leftExtend=trimcontrol::activeLowPressed(digitalRead(PIN_LEFT_DOWN)==HIGH);
  raw.rightRetract=trimcontrol::activeLowPressed(digitalRead(PIN_RIGHT_UP)==HIGH);
  raw.rightExtend=trimcontrol::activeLowPressed(digitalRead(PIN_RIGHT_DOWN)==HIGH);
  controller.physicalInputs(raw,millis());logCoreEvent();applyOutputPins();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LEFT_UP,INPUT_PULLUP);pinMode(PIN_LEFT_DOWN,INPUT_PULLUP);
  pinMode(PIN_RIGHT_UP,INPUT_PULLUP);pinMode(PIN_RIGHT_DOWN,INPUT_PULLUP);
  pinMode(OUT_LEFT_UP,OUTPUT);pinMode(OUT_LEFT_DOWN,OUTPUT);
  pinMode(OUT_RIGHT_UP,OUTPUT);pinMode(OUT_RIGHT_DOWN,OUTPUT);
  allOutputsOff();applyOutputPins();
  Serial.println("\nTrim controller Wokwi starter v0.1");
  WiFi.mode(WIFI_STA);WiFi.begin(WIFI_SSID,WIFI_PASS);
  setupWeb(); // Startup and local input service do not wait for Wi-Fi.
}

void loop() {
  if(WiFi.status()==WL_CONNECTED)server.handleClient();
  if(trimcontrol::shouldServiceLocalInputs(WiFi.status()==WL_CONNECTED))handlePhysicalButtons();
  controller.tick(millis());logCoreEvent();applyOutputPins();
  if(millis()-lastTelemetryMs>1000)lastTelemetryMs=millis();
  delay(5);
}
