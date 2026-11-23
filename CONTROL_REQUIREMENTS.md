# Trim Controller behavior requirements

Status: baseline specification for the software prototype and future hardware design.  
Last updated: 2026-11-23.  
Scope: two-tab controller concept; current executable target is an ESP32-S3 Wokwi simulation.

This document separates agreed requirements, simulation acceptance checks, and open design decisions. Open items must not be treated as implemented or settled. The current firmware simulates sensor values and actuators; passing its build or Wokwi run does not validate a boat-ready controller.

## Status labels

- **Agreed** — carried forward from the project handover.
- **Prototype check** — behavior that can be asserted in the software simulation.
- **Hardware check** — requires physical wiring/components; Wokwi cannot prove it.
- **Open** — requires a design decision, component data, or boat-specific measurement.

## Agreed system constraints

1. **One controller MCU.** ESP32 is the only system controller. Do not put a second MCU in the power module.
2. **Manual operation survives controller failure.** Physical manual controls must actuate the tabs through an electrical path that does not depend on the ESP32 firmware or its GPIO outputs.
3. **Power outputs are safe by default.** Boot, reset, detected faults, and software-controlled stop conditions must de-energize motor commands. Opposite directions for one actuator must never be energized together.
4. **Manual and automatic modes are required.** The operator must be able to cancel automation. Loss or invalidity of an input required by the active automatic behavior must not allow that behavior to continue using stale data.
5. **Operator feedback and setup.** The target is a small display for underway status and a web interface for configuration rather than a complex screen menu. Button illumination is required.
6. **Modular expansion.** A base display/measurement controller should work independently. CAN/NMEA is an optional later expansion, not a first-version dependency.
7. **No hardware-specific ratings are fixed.** Actuator voltage/current, power-stage ratings, fuse values, wire sizes, and final GPIO mapping remain open until components are selected.

## Mode and command contract

The names below are used by the prototype. They describe intended software behavior, not an agreed automatic trim algorithm.

| State | Intended software behavior | Status |
|---|---|---|
| MANUAL | Automatic motion is disabled. Operator commands may move either simulated tab, subject to direction interlocks and command limits. | Agreed |
| AUTO | The controller may issue automatic motion only while the inputs required by the selected algorithm are valid. | Agreed; algorithm open |
| HOLD | Stop software-commanded motion and hold outputs off until the operator selects another mode. | Prototype behavior; confirm final user meaning |
| DEGRADED | Disable automatic motion when a required input becomes invalid. Show the cause. Do not resume AUTO automatically when data returns. | Agreed direction; exact fallback and manual permissions open |
| FAULT | Stop software-controlled motion and report the fault. Clearing a fault returns to MANUAL and must not resume previous commands. | Prototype behavior; hardware fault policy open |

### Command priorities

- A physical local input cancels AUTO before commanding manual movement.
- If both directions are requested for one tab, that tab stops; it must not choose one direction arbitrarily.
- A web command is bounded by a configured duration. Expiry stops the commanded motion.
- Entering HOLD, DEGRADED, or FAULT stops any software-controlled motion.
- Clearing a fault or restoring sensor validity does not restart motion automatically; a fresh operator action is required.
- The independent hardwired manual path remains available when the ESP32 is unpowered or failed, subject to the electrical protection design.

## Prototype acceptance checks

These are behavior-level checks to implement in the regression suite. The existing CI build/Wokwi simulation currently checks startup only; these checks are not yet an automated test suite.

| ID | Setup / action | Pass condition | Can Wokwi prove it? |
|---|---|---|---|
| P-01 | Boot or reset the controller | All software motor outputs start off; no prior command resumes. | Yes |
| P-02 | Request UP or DOWN for one tab | Only the requested direction is active for that tab. | Yes |
| P-03 | Request both directions for the same tab | Both outputs are off; a conflict event is reported. | Yes |
| P-04 | Issue a timed web command | The command stops at its configured deadline or earlier at an endpoint/stop request. | Yes |
| P-05 | Press a physical input while in AUTO | AUTO is cancelled before manual movement is applied. | Yes |
| P-06 | Enter HOLD during movement | All software outputs stop; no motion resumes when HOLD is exited. | Yes |
| P-07 | Inject FAULT during movement | All software outputs stop; clearing FAULT returns to MANUAL with outputs off. | Yes |
| P-08 | Invalidate GPS while AUTO is active | AUTO stops and the controller enters DEGRADED; restoring GPS alone does not restart AUTO. | Yes |
| P-09 | Request AUTO while a required input is invalid | AUTO is denied; outputs remain off and the invalid input is visible in status. | Yes |
| P-10 | Lose Wi-Fi while using local simulation buttons | Web access may disappear, but the local input loop continues. | Yes, for simulation behavior only |
| P-11 | Disconnect, reset, or freeze the ESP32 while a physical manual switch is operated | The hardwired manual path still operates, with protection and interlocks intact. | **No — physical bench test required** |
| P-12 | Request opposite directions through independent control paths at the same time | Hardware interlocking prevents both directions reaching one actuator, even with ESP32 failure. | **No — physical bench test required** |

## Open decisions before implementing automatic motion

1. **Actuator data:** voltage, running/stall current, stroke, travel time, end switches/feedback, duty cycle, and environmental requirements.
2. **Low-speed/engine conditions:** how neutral, engine-off, or low-speed states are detected; whether tabs should deploy or retract; priority versus manual control.
3. **Planing transition:** how to detect acceleration onto plane, target movement, thresholds, hysteresis, and time delays.
4. **Turn handling:** what turn signals are available, how turn compensation behaves, and when roll correction should pause.
5. **Sensor validity:** required inputs for each automatic behavior; freshness limits; IMU/GPS calibration; behavior during GPS loss or inconsistent readings.
6. **Motion bounds:** permitted travel, maximum drive time, position feedback versus timed estimate, and whether a displayed position is measured or estimated.
7. **Fault/manual relationship:** which electrical faults disable the independent manual path and which leave it available.
8. **Hardware selection:** exact display/ESP32 board, IMU, GPS, optional CAN/NMEA interface, connectors, and mounting/orientation.

Do not assign thresholds, current ratings, or automatic target positions until the relevant boat and component data are available.

## Current implementation boundary

The current Wokwi firmware has simulated mode names, four momentary button inputs, timed simulated commands, mock GPS validity, and simulated actuator positions. AUTO does not yet calculate trim commands from real speed, roll, pitch, or NMEA data. The Wokwi GPIO model is not the independent hardware manual override required above.

The latest CI run passed firmware compilation, Wokwi diagram lint, and the Wokwi startup-output check. That confirms the simulation pipeline works; it does not prove P-11 or P-12.
