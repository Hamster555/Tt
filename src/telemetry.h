#pragma once
#include "actuator_simulation.h"
#include "mode_state.h"
namespace trimcontrol {
struct TelemetrySnapshot { Mode mode;bool requiredInputsValid;float speedKnots,rollDeg,pitchDeg,leftPositionPct,rightPositionPct; };
inline TelemetrySnapshot makeTelemetrySnapshot(const ModeState& s,float speed,float roll,float pitch,const SimulatedActuator& l,const SimulatedActuator& r) {
  return {s.mode(),s.requiredInputsValid(),speed,roll,pitch,l.positionPct,r.positionPct};
}
inline const char* modeName(Mode m) {
  switch(m){case Mode::Manual:return "MANUAL";case Mode::Auto:return "AUTO";case Mode::Hold:return "HOLD";case Mode::Degraded:return "DEGRADED";case Mode::Fault:return "FAULT";}
  return "UNKNOWN";
}
}
