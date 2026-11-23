#pragma once
#include <cstdint>
#include "command_arbitration.h"
#include "input_processing.h"
#include "mode_state.h"
#include "telemetry.h"
namespace trimcontrol {
enum class ControlEvent { BootSafe,ModeChanged,AutoDenied,ModeRejectedFault,AutoStoppedInputLoss,FaultInjected,FaultCleared,PhysicalCancelsAuto,PhysicalConflictLeft,PhysicalConflictRight,PhysicalInhibited,PhysicalReleased,WebCommand,WebCommandRejected,TimedCommandExpired,RetractEndpoint,ExtendEndpoint,InputValidityChanged };
class ControlCore {
 public:
  bool requestMode(Mode next) {
    const Mode previous=state_.mode();
    if(!state_.requestMode(next)){stopAll();event_=state_.mode()==Mode::Fault?ControlEvent::ModeRejectedFault:ControlEvent::AutoDenied;return false;}
    // No automatic motion law exists yet; mode changes never carry a pending
    // manual command into AUTO or another mode.
    stopAll();
    if(next==Mode::Hold||next==Mode::Degraded||next==Mode::Fault||
       ((previous==Mode::Hold||previous==Mode::Degraded||previous==Mode::Fault)&&next==Mode::Manual))
      requirePhysicalRelease_=true;
    event_=ControlEvent::ModeChanged;return true;
  }
  void setRequiredInputsValid(bool valid) {
    if(state_.setRequiredInputsValid(valid)){stopAll();requirePhysicalRelease_=true;event_=ControlEvent::AutoStoppedInputLoss;}
    else event_=ControlEvent::InputValidityChanged;
  }
  void setFault(bool active){state_.setFault(active);stopAll();requirePhysicalRelease_=true;event_=active?ControlEvent::FaultInjected:ControlEvent::FaultCleared;}
  CommandResult webCommand(Tab t,Direction d,uint32_t ms,uint32_t now) {
    auto result=arbitrateWebCommand(state_,t,d,ms,now,left_,right_);
    event_=result==CommandResult::Accepted?ControlEvent::WebCommand:ControlEvent::WebCommandRejected;
    return result;
  }
  ProcessedInputs physicalInputs(const PhysicalInputs& raw,uint32_t now) {
    const auto i=processInputs(raw);const bool wasAuto=state_.autoActive();
    if(i.any&&wasAuto){state_.requestMode(Mode::Manual);stopAll();event_=ControlEvent::PhysicalCancelsAuto;}
    if(requirePhysicalRelease_){
      if(!i.any){requirePhysicalRelease_=false;previousPhysicalAny_=false;}
      else {previousPhysicalAny_=true;event_=ControlEvent::PhysicalInhibited;return i;}
    }
    const bool wasAny=previousPhysicalAny_;arbitratePhysicalInputs(i,wasAny,state_,left_,right_,now);
    if(i.any&&!state_.softwareCommandsAllowed())event_=ControlEvent::PhysicalInhibited;
    else if(i.leftConflict)event_=ControlEvent::PhysicalConflictLeft;
    else if(i.rightConflict)event_=ControlEvent::PhysicalConflictRight;
    else if(!i.any&&wasAny)event_=ControlEvent::PhysicalReleased;
    previousPhysicalAny_=i.any;return i;
  }
  void tick(uint32_t now) {
    auto l=updateActuator(left_,now),r=updateActuator(right_,now);
    if(l==SimulationEvent::TimedCommandExpired||r==SimulationEvent::TimedCommandExpired)event_=ControlEvent::TimedCommandExpired;
    else if(l==SimulationEvent::RetractEndpoint||r==SimulationEvent::RetractEndpoint)event_=ControlEvent::RetractEndpoint;
    else if(l==SimulationEvent::ExtendEndpoint||r==SimulationEvent::ExtendEndpoint)event_=ControlEvent::ExtendEndpoint;
  }
  TelemetrySnapshot telemetry(float s,float r,float p)const{return makeTelemetrySnapshot(state_,s,r,p,left_,right_);}
  Direction leftDirection()const{return left_.direction;}Direction rightDirection()const{return right_.direction;}
  Mode mode()const{return state_.mode();}bool requiredInputsValid()const{return state_.requiredInputsValid();}
  ControlEvent lastEvent()const{return event_;}float leftPositionPct()const{return left_.positionPct;}float rightPositionPct()const{return right_.positionPct;}
  void stopAll(){stopActuator(left_);stopActuator(right_);}
 private:
  ModeState state_;SimulatedActuator left_,right_;bool previousPhysicalAny_=false,requirePhysicalRelease_=false;ControlEvent event_=ControlEvent::BootSafe;
};
inline const char* eventName(ControlEvent e) {
  switch(e){
  case ControlEvent::BootSafe:return "Boot: outputs safe";
  case ControlEvent::ModeChanged:return "Mode changed";
  case ControlEvent::AutoDenied:return "AUTO denied: required inputs invalid";
  case ControlEvent::ModeRejectedFault:return "Mode change rejected while FAULT is active";
  case ControlEvent::AutoStoppedInputLoss:return "Required input invalid: AUTO stopped";
  case ControlEvent::FaultInjected:return "FAULT injected: outputs stopped";
  case ControlEvent::FaultCleared:return "FAULT cleared; MANUAL only";
  case ControlEvent::PhysicalCancelsAuto:return "Physical input cancels AUTO";
  case ControlEvent::PhysicalConflictLeft:return "Conflicting LEFT buttons: stopped";
  case ControlEvent::PhysicalConflictRight:return "Conflicting RIGHT buttons: stopped";
  case ControlEvent::PhysicalInhibited:return "Physical software command inhibited in current mode";
  case ControlEvent::PhysicalReleased:return "Physical buttons released: outputs stopped";
  case ControlEvent::WebCommand:return "Web command accepted";
  case ControlEvent::WebCommandRejected:return "Web command rejected in current mode or invalid";
  case ControlEvent::TimedCommandExpired:return "Timed command expired";
  case ControlEvent::RetractEndpoint:return "Simulated retract endpoint";
  case ControlEvent::ExtendEndpoint:return "Simulated extend endpoint";
  case ControlEvent::InputValidityChanged:return "Required input validity changed";
  }
  return "Unknown event";
}
}
