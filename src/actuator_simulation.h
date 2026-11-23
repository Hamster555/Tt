#pragma once
#include <cstdint>
#include "input_processing.h"
namespace trimcontrol {
struct SimulatedActuator {
  float positionPct=0.0f, travelSeconds=10.0f;
  Direction direction=Stop;
  uint32_t lastUpdateMs=0, commandUntilMs=0;
};
enum class SimulationEvent { None, TimedCommandExpired, RetractEndpoint, ExtendEndpoint };
inline void stopActuator(SimulatedActuator& a){a.direction=Stop;a.commandUntilMs=0;}
inline void stopActuatorAt(SimulatedActuator& a,uint32_t now){stopActuator(a);a.lastUpdateMs=now;}
inline SimulationEvent updateActuator(SimulatedActuator& a,uint32_t now) {
  const uint32_t elapsed=now-a.lastUpdateMs;a.lastUpdateMs=now;
  if(a.direction==Stop)return SimulationEvent::None;
  a.positionPct+=(static_cast<int>(a.direction)*100.0f*(static_cast<float>(elapsed)/1000.0f))/(a.travelSeconds>0.1f?a.travelSeconds:0.1f);
  if(a.direction==Extend&&a.positionPct>=100.0f){a.positionPct=100.0f;stopActuator(a);return SimulationEvent::ExtendEndpoint;}
  if(a.direction==Retract&&a.positionPct<=0.0f){a.positionPct=0.0f;stopActuator(a);return SimulationEvent::RetractEndpoint;}
  if(a.commandUntilMs&&static_cast<int32_t>(now-a.commandUntilMs)>=0){stopActuator(a);return SimulationEvent::TimedCommandExpired;}
  return SimulationEvent::None;
}
}
