#pragma once

namespace trimcontrol {

// Shared pure rules used by firmware and host-side regression checks.
enum Direction : int { Retract = -1, Stop = 0, Extend = 1 };

inline Direction resolveDirections(bool retractPressed, bool extendPressed) {
  if (retractPressed == extendPressed) return Stop;
  return retractPressed ? Retract : Extend;
}

inline bool softwareCommandAllowed(bool isHold, bool isDegraded, bool isFault) {
  return !isHold && !isDegraded && !isFault;
}

inline bool autoStartAllowed(bool requiredInputsValid) {
  return requiredInputsValid;
}

inline bool shouldDegradeOnInputLoss(bool isAuto, bool requiredInputsValid) {
  return isAuto && !requiredInputsValid;
}

}  // namespace trimcontrol
