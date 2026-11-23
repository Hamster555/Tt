#pragma once
namespace trimcontrol {
enum class Mode { Manual, Auto, Hold, Degraded, Fault };
class ModeState {
 public:
  Mode mode() const { return mode_; }
  bool requiredInputsValid() const { return requiredInputsValid_; }
  bool requestMode(Mode next) {
    if(mode_==Mode::Fault&&next!=Mode::Fault)return false;
    if(next==Mode::Auto&&!requiredInputsValid_){mode_=Mode::Degraded;return false;}
    mode_=next;return true;
  }
  bool setRequiredInputsValid(bool valid) {
    requiredInputsValid_=valid;
    if(mode_==Mode::Auto&&!valid){mode_=Mode::Degraded;return true;}
    return false;
  }
  void setFault(bool active) {
    if(active) mode_=Mode::Fault;
    else if(mode_==Mode::Fault) mode_=Mode::Manual;
  }
  bool softwareCommandsAllowed() const { return mode_==Mode::Manual; }
  bool autoActive() const { return mode_==Mode::Auto; }
 private:
  Mode mode_=Mode::Manual;
  bool requiredInputsValid_=true; // AUTO_REQUIRED_INPUTS is a simulator placeholder.
};
}
