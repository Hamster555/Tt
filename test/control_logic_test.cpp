#include "control_logic.h"

#include <cassert>

int main() {
  using trimcontrol::Direction;
  using trimcontrol::Extend;
  using trimcontrol::Retract;
  using trimcontrol::Stop;

  assert(trimcontrol::resolveDirections(false, false) == Stop);
  assert(trimcontrol::resolveDirections(true, false) == Retract);
  assert(trimcontrol::resolveDirections(false, true) == Extend);
  assert(trimcontrol::resolveDirections(true, true) == Stop);

  assert(trimcontrol::softwareCommandAllowed(false, false, false));
  assert(!trimcontrol::softwareCommandAllowed(true, false, false));
  assert(!trimcontrol::softwareCommandAllowed(false, true, false));
  assert(!trimcontrol::softwareCommandAllowed(false, false, true));

  assert(trimcontrol::autoStartAllowed(true));
  assert(!trimcontrol::autoStartAllowed(false));
  assert(!trimcontrol::shouldDegradeOnInputLoss(false, false));
  assert(trimcontrol::shouldDegradeOnInputLoss(true, false));
  assert(!trimcontrol::shouldDegradeOnInputLoss(true, true));
  return 0;
}
