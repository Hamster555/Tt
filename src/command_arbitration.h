#pragma once
#include <cstdint>
#include "actuator_simulation.h"
#include "mode_state.h"
namespace trimcontrol {
enum class Tab { Left, Right };
enum class CommandResult { Accepted, Invalid, ModeRejected };
constexpr uint32_t SIMULATOR_MAX_COMMAND_MS=1500;
inline SimulatedActuator& actuatorFor(Tab t,SimulatedActuator& l,SimulatedActuator& r){return t==Tab::Left?l:r;}
inline CommandResult arbitrateWebCommand(ModeState& m,Tab t,Direction d,uint32_t duration,uint32_t now,SimulatedActuator& l,SimulatedActuator& r) {
  if(d!=Retract&&d!=Stop&&d!=Extend)return CommandResult::Invalid;
  if(d!=Stop&&(duration==0||duration>SIMULATOR_MAX_COMMAND_MS))return CommandResult::Invalid;
  // TBD WEB_COMMAND_DURING_AUTO_POLICY: reject; never silently switch modes.
  if(!m.softwareCommandsAllowed())return CommandResult::ModeRejected;
  auto& a=actuatorFor(t,l,r);a.direction=d;a.commandUntilMs=d==Stop?0:now+duration;a.lastUpdateMs=now;
  return CommandResult::Accepted;
}
inline void arbitratePhysicalInputs(const ProcessedInputs& i,bool wasAny,ModeState& m,SimulatedActuator& l,SimulatedActuator& r,uint32_t now) {
  if(i.any&&m.autoActive())m.requestMode(Mode::Manual);
  // DEGRADED_MANUAL_POLICY is TBD; monitored software commands remain off.
  if(!m.softwareCommandsAllowed())return;
  if(!i.any&&wasAny){stopActuatorAt(l,now);stopActuatorAt(r,now);return;}
  // An idle panel has no command and must not erase a timed web command.
  if(!i.any)return;
  if(!wasAny||l.direction!=i.left)l.lastUpdateMs=now;
  if(!wasAny||r.direction!=i.right)r.lastUpdateMs=now;
  l.direction=i.left;r.direction=i.right;l.commandUntilMs=0;r.commandUntilMs=0;
}
}
