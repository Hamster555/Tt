#pragma once
namespace trimcontrol {
enum Direction : int { Retract=-1, Stop=0, Extend=1 };
struct PhysicalInputs {
  bool leftRetract=false, leftExtend=false, rightRetract=false, rightExtend=false;
  bool any() const { return leftRetract||leftExtend||rightRetract||rightExtend; }
};
struct ProcessedInputs {
  Direction left=Stop, right=Stop;
  bool any=false, leftConflict=false, rightConflict=false;
};
inline Direction resolveDirections(bool retract,bool extend) {
  if(retract==extend) return Stop;
  return retract?Retract:Extend;
}
inline ProcessedInputs processInputs(const PhysicalInputs& i) {
  ProcessedInputs out;
  out.left=resolveDirections(i.leftRetract,i.leftExtend);
  out.right=resolveDirections(i.rightRetract,i.rightExtend);
  out.any=i.any();
  out.leftConflict=i.leftRetract&&i.leftExtend;
  out.rightConflict=i.rightRetract&&i.rightExtend;
  return out;
}
inline bool activeLowPressed(bool pinHigh) { return !pinHigh; }
inline bool shouldServiceLocalInputs(bool wifiConnected) {
  (void)wifiConnected;
  return true; // Local control must not depend on Wi-Fi.
}
}
