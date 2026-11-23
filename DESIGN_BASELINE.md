# Design baseline and phase status

Last reviewed: 2026-11-23  
Scope: single-controller trim-tab prototype for a roughly 6 m planing boat. This is a development baseline, not a production design or approval to connect motors.

## Phase status

| Phase | Status | Exit condition |
|---|---|---|
| 1. Behavior contract | Complete as a software baseline | Open behavior variables below are resolved before automatic trim logic is enabled |
| 2. Controller and interface choice | Provisional bench candidate and interface risks documented; no purchase decision | Confirm physical display readability, I/O expansion, mounting, and delivered cost |
| 3. Actuator and power requirements | Waiting on actuator and vessel electrical data | Complete `ACTUATOR_DATA.md` from actual datasheets and measurements |
| 4. Firmware structure | Shared pure rules extracted; firmware remains a Wokwi prototype | Separate input validation, mode/state handling, output arbitration, and telemetry; no vessel algorithm before its inputs/limits are set |
| 5. Automated checks | Host control-rule checks, firmware build, diagram lint, and Wokwi startup check pass in CI; broader acceptance coverage remains | Add named endpoint/state-transition checks and preserve hardware-only checks for bench validation |
| 6. Bench validation | Checklist prepared; not run | Record results against the selected actuators, driver, supply, and physical override circuit |
| 7. Boat trials and BOM | Pending bench results and operator decisions | Review mounting, visibility, sealed enclosure, wiring, and trial plan; publish an itemized BOM with local delivered prices |


## Work completed in the current pass

- Added shared pure logic for conflicting directions, command gating, AUTO input validity, and degraded transition on required-input loss.
- Added host-side regression assertions and made CI compile/run them before the ESP32 firmware build.
- Tightened the simulator HTTP endpoints to reject malformed tab/direction/mode requests, zero-duration motion, and report rejected mode commands with an error response.
- CI run for the firmware and host-check changes passed: [run 45241019073](https://github.com/Hamster555/Tt/actions/runs/45241019073). It included host control-rule checks, firmware compilation, Wokwi diagram lint, and Wokwi startup simulation.
- The simulator remains a single-file application and AUTO still has no boat-control algorithm. The current host tests exercise pure helper rules; they do not yet cover the full web API, actuator timing, power interruption, hardwired override, or marine behavior.

## Provisional controller candidate

Use the **Waveshare ESP32-S3-Touch-AMOLED-1.75-G** as the first display/controller bench candidate. It is one ESP32-S3 controller board with an integrated 1.75-inch round touch AMOLED, QMI8658 six-axis IMU, and an LC76G GPS receiver on the GPS variant. Waveshare lists three external GPIOs and one UART on its header, so the pin budget is tight; the design must reserve a suitable interface for the physical input monitor and output-command interface. A GPIO expander can provide logic I/O for the prototype, but it must never be placed in the independent manual motor-current path.

The vendor page currently lists the family at **US$29.99–39.99** depending on version. Treat this as a reference only: price, shipping, taxes, and availability depend on the buyer's region. The GPS variant requires an external ceramic antenna. The board is not established as a sealed, sunlight-readable marine instrument; it needs an enclosure and direct outdoor readability review.

### Why this candidate

- Keeps the chosen one-MCU architecture and small-display direction.
- Bundles display, touch, IMU, Wi-Fi, and (on the -G version) GPS, leaving a plausible path within the approximate US$100 electronics target.
- Allows a separate motor power module with no MCU.
- Leaves the independent manual switch path outside firmware and GPIO expanders.

### Alternatives retained

| Option | Strength | Cost/fit issue | Current disposition |
|---|---|---|---|
| ESP32-S3-Touch-AMOLED-1.75-G | Integrated display, IMU, GPS, Wi-Fi, and one controller | Limited exposed GPIO; enclosure and glare performance unknown | Preferred bench candidate, pending checks |
| ESP32-S3-DevKitC-1 plus separate display and sensors | Exposes many pins and is a simple breadboard target | More parts, wiring, integration, and enclosure work; display/sensor selection remains | Use for generic software prototyping if the integrated board is unavailable |
| Separate display MCU plus control ESP32 | More distributed display options | Violates the agreed single-controller architecture | Rejected |

The board's onboard IMU can be evaluated for the first prototype; do not add a second IMU or buy a sensor breakout until its vibration, calibration, orientation, and dynamic performance are measured on the vessel.

## Interface and power architecture

- One ESP32-S3 is the only controller MCU.
- Display and web setup are operator interfaces; loss of Wi-Fi must not stop local control.
- Physical momentary buttons are electrically arranged to control the actuator path without the ESP32. The exact relay/contact arrangement and protection remain an electrical design task.
- Firmware may monitor switch state through an input interface. Loss or failure of that monitor must not disable the independent manual circuit.
- The ESP32 may command only a low-voltage logic interface after an output stage is selected and reviewed. GPIO pins must never drive motors.
- Any GPIO expander is for signals only; its reset, power loss, or bus failure must make software commands inactive.
- Separate control and motor power domains. Final fuses, wiring, connectors, suppression, driver, and supply ratings require actuator data and the vessel supply range.
- Display orientation, enclosure, connector sealing, cable routing, and grounding are unselected.

## Budget worksheet

Target: approximately US$100 for electronics, excluding trim tabs. This is a target, not a promise.

| Cost line | Planning entry | Status |
|---|---:|---|
| Controller/display/GPS board | US$29.99–39.99 vendor family listing | Reference price; select exact variant and delivered price |
| Input/output interface parts | TBD | Depends on GPIO allocation and physical circuit |
| Power conversion and protection | TBD | Depends on vessel electrical system and chosen controller |
| Motor driver / relays / contactors | TBD | Cannot size without actuator voltage and running/stall current |
| Switches, illumination, connectors, cable | TBD | Depends on panel layout and environment |
| IMU/GPS antenna or other sensor parts | TBD | Existing board features may cover these; verify on the bench |
| Enclosure, mounting, sealing | TBD | Must be selected for actual console location |
| Shipping, taxes, spare parts | TBD | Region and retailer dependent |

Do not close the budget or buy the power stage until the TBD rows are priced. If the US$100 target excludes the enclosure, state that explicitly in the final BOM.

## Behavior variables before AUTO is enabled

These are design inputs, not questions that block software scaffolding:

- `AUTO_REQUIRED_INPUTS`: valid/fresh sensors and their per-mode requirements.
- `GPS_LOSS_POLICY`: stop automatic commands, enter DEGRADED, require a fresh operator action before AUTO resumes.
- `DEGRADED_MANUAL_POLICY`: whether monitored physical buttons continue to work; hardware manual bypass remains independent.
- `LOW_SPEED_POLICY`: behavior for engine off, neutral, and low-speed operation.
- `PLANE_DETECTOR`, `PLANE_ENTRY_THRESHOLD`, `PLANE_EXIT_THRESHOLD`, `PLANE_DWELL_MS`: derive from vessel trials; thresholds must include hysteresis.
- `TURN_INPUT` and `TURN_POLICY`: required sensor/source and handling during turns.
- `MAX_SOFTWARE_COMMAND_MS`: bounded value established after actuator travel testing.
- `POSITION_SOURCE`: measured feedback or labeled timed estimate; never present an estimate as a measurement.
- `HOLD_MEANING`: outputs off until an explicit new operator action (baseline).
- `FAULT_CLEAR_POLICY`: fault source absent and operator acknowledgement; return to MANUAL with outputs off.

No automatic target positions, thresholds, current ratings, wire sizes, or fuse values are assigned here.

## Evidence and references

Vendor feature and header details: [Waveshare ESP32-S3-Touch-AMOLED-1.75 documentation](https://docs.waveshare.com/ESP32-S3-Touch-AMOLED-1.75). The vendor product page listed US$29.99–39.99 when reviewed on 2026-11-23: [Waveshare product page](https://www.waveshare.com/esp32-s3-touch-amoled-1.75.htm). Generic development-board information: [Espressif ESP32-S3-DevKitC-1](https://www.espressif.com/en/products/devkits/esp32-s3-series/esp32-s3-devkitc-1). I/O-expander reference if needed: [Microchip MCP23017](https://www.microchip.com/en-us/product/MCP23017). An I/O expander is not a safety interlock or motor driver.
