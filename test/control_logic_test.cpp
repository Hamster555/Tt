#include "control_logic.h"
#include <cassert>
using namespace trimcontrol;
static void p01(){ControlCore c;assert(c.mode()==Mode::Manual);assert(c.leftDirection()==Stop&&c.rightDirection()==Stop);assert(c.lastEvent()==ControlEvent::BootSafe);ControlCore reset;assert(reset.leftDirection()==Stop&&reset.rightDirection()==Stop);}
static void p02(){ControlCore c;assert(c.webCommand(Tab::Left,Extend,300,100)==CommandResult::Accepted);assert(c.leftDirection()==Extend);assert(c.webCommand(Tab::Left,Retract,300,200)==CommandResult::Accepted);assert(c.leftDirection()==Retract&&c.rightDirection()==Stop);}
static void p03(){ControlCore c;PhysicalInputs i;i.leftRetract=i.leftExtend=true;c.physicalInputs(i,100);assert(c.leftDirection()==Stop);assert(c.lastEvent()==ControlEvent::PhysicalConflictLeft);}
static void p04(){
  ControlCore c;PhysicalInputs idle;
  assert(c.webCommand(Tab::Left,Extend,300,100)==CommandResult::Accepted);
  c.physicalInputs(idle,150);assert(c.leftDirection()==Extend);
  c.tick(200);assert(c.leftDirection()==Extend);
  c.tick(400);assert(c.leftDirection()==Stop&&c.lastEvent()==ControlEvent::TimedCommandExpired);
  assert(c.webCommand(Tab::Left,Extend,1000,450)==CommandResult::Accepted);
  assert(c.webCommand(Tab::Left,Stop,0,500)==CommandResult::Accepted);
  assert(c.leftDirection()==Stop);
  assert(c.webCommand(Tab::Right,Extend,1500,600)==CommandResult::Accepted);
  c.tick(10600);
  assert(c.rightDirection()==Stop&&c.rightPositionPct()==100.0f);
  assert(c.lastEvent()==ControlEvent::ExtendEndpoint);
}
static void p05(){
  ControlCore c;
  assert(c.webCommand(Tab::Right,Retract,1000,50)==CommandResult::Accepted);
  assert(c.requestMode(Mode::Auto));assert(c.rightDirection()==Stop);
  PhysicalInputs i;i.leftExtend=true;c.physicalInputs(i,100);
  assert(c.mode()==Mode::Manual&&c.leftDirection()==Extend);
  assert(c.lastEvent()==ControlEvent::PhysicalCancelsAuto);
}
static void p06(){ControlCore c;assert(c.webCommand(Tab::Left,Extend,1200,100)==CommandResult::Accepted);assert(c.requestMode(Mode::Hold));assert(c.leftDirection()==Stop);PhysicalInputs i;i.rightExtend=true;c.physicalInputs(i,200);assert(c.leftDirection()==Stop&&c.rightDirection()==Stop);assert(c.requestMode(Mode::Manual));c.physicalInputs(i,300);assert(c.leftDirection()==Stop&&c.rightDirection()==Stop);PhysicalInputs released;c.physicalInputs(released,400);c.physicalInputs(i,500);assert(c.rightDirection()==Extend);}
static void p07(){ControlCore c;assert(c.webCommand(Tab::Right,Retract,1200,100)==CommandResult::Accepted);c.setFault(true);assert(c.mode()==Mode::Fault&&c.rightDirection()==Stop);assert(!c.requestMode(Mode::Manual));assert(c.mode()==Mode::Fault);PhysicalInputs held;held.leftExtend=true;c.physicalInputs(held,200);c.setFault(false);assert(c.mode()==Mode::Manual&&c.leftDirection()==Stop&&c.rightDirection()==Stop);c.physicalInputs(held,300);assert(c.leftDirection()==Stop);PhysicalInputs released;c.physicalInputs(released,400);c.physicalInputs(held,500);assert(c.leftDirection()==Extend);}
static void p08(){ControlCore c;assert(c.requestMode(Mode::Auto));c.setRequiredInputsValid(false);assert(c.mode()==Mode::Degraded&&c.leftDirection()==Stop&&c.rightDirection()==Stop);c.setRequiredInputsValid(true);assert(c.mode()==Mode::Degraded);}
static void p09(){ControlCore c;assert(c.webCommand(Tab::Left,Extend,1000,50)==CommandResult::Accepted);c.setRequiredInputsValid(false);assert(!c.requestMode(Mode::Auto));assert(c.mode()==Mode::Degraded&&c.leftDirection()==Stop&&c.lastEvent()==ControlEvent::AutoDenied);assert(!c.telemetry(0,0,0).requiredInputsValid);}
static void p10(){ControlCore c;assert(shouldServiceLocalInputs(false));PhysicalInputs i;i.rightExtend=true;c.physicalInputs(i,100);assert(c.rightDirection()==Extend&&c.mode()==Mode::Manual);c.tick(100);assert(c.rightDirection()==Extend);c.physicalInputs(i,200);c.tick(200);assert(c.rightPositionPct()>0.9f&&c.rightPositionPct()<1.1f);}
static void gates(){ControlCore c;assert(c.webCommand(Tab::Left,Extend,0,10)==CommandResult::Invalid);assert(c.webCommand(Tab::Left,Extend,SIMULATOR_MAX_COMMAND_MS+1,10)==CommandResult::Invalid);assert(c.requestMode(Mode::Auto));assert(c.webCommand(Tab::Left,Extend,100,10)==CommandResult::ModeRejected);assert(c.requestMode(Mode::Hold));assert(c.webCommand(Tab::Left,Extend,100,10)==CommandResult::ModeRejected);}
int main(){p01();p02();p03();p04();p05();p06();p07();p08();p09();p10();gates();}
