# Codex handoff — Trim Controller

This file carries the current project context into a Codex task. The original ChatGPT Project and this repository are separate; this is a concise decision and progress handoff, not a copy of the full ChatGPT conversation.

## Project objective

Develop software and hardware for a modular trim-tab controller for a roughly 6 m planing boat. The present code is a Wokwi software prototype only.

## Agreed constraints

- One ESP32 is the only controller MCU; no MCU belongs in the power module.
- Physical manual switches must have an electrical actuator path independent of ESP32 firmware and GPIO.
- Opposite directions on one actuator must never be energized together.
- Boot, reset, detected faults, and software stop conditions leave software outputs off.
- Manual input cancels AUTO. Required-input loss must stop AUTO, enter DEGRADED, and never restart AUTO automatically when data returns.
- Web UI is for setup/status; local control must not depend on Wi-Fi.
- Small display, illuminated physical buttons, and a web settings UI are the direction. CAN/NMEA is optional.
- No actuator, driver, fuse, cable, connector, enclosure, or final GPIO selection has been approved.
- Keep unknown vessel and actuator values as TBD. Do not invent AUTO thresholds or power ratings.

## Current implementation

- `src/main.cpp` is a Wokwi starter. Telemetry and tab position are simulated; AUTO does not calculate trim from boat data.
- `src/control_logic.h` contains pure direction-conflict, command-gating, AUTO input-validity, and degraded-transition rules.
- `test/control_logic_test.cpp` exercises those pure rules. CI runs the host checks, firmware build, Wokwi diagram lint, and startup simulation.
- Web simulator endpoints now validate tab, direction, mode, and nonzero motion duration and report rejected commands.
- CI passed host checks, ESP32-S3 compilation, diagram lint, and Wokwi startup simulation in run [45241019073](https://github.com/Hamster555/Tt/actions/runs/45241019073).

## Project documents

Read these before changing hardware behavior:

1. `AGENTS.md`
2. `CONTROL_REQUIREMENTS.md`
3. `DESIGN_BASELINE.md`
4. `ACTUATOR_DATA.md`
5. `BENCH_VALIDATION.md`

## Next software work

1. Expand host/simulation coverage for acceptance checks P-01 through P-10. Keep hardware-only P-11 and P-12 clearly marked as requiring physical tests.
2. Refactor the prototype into input validation, mode/state management, command arbitration, actuator simulation, and telemetry modules.
3. Keep AUTO disabled as a real boat algorithm until required sensors, speed/turn/low-speed policies, actuator travel, and safe movement limits are supplied.
4. Keep CI green and distinguish software simulation evidence from bench/vessel evidence.

## Hardware inputs still needed

Fill `ACTUATOR_DATA.md` with exact actuator models, supply range, running/stall current, stroke/travel, end switches or position feedback, duty cycle, route lengths, and the boat's electrical system. Then review the display mounting/sunlight conditions, manual panel layout, degraded-mode permissions, and whether the approximately US$100 electronics target excludes enclosure and shipping.

## Continuity note

The source ChatGPT Project is named `trimtabs`. Its synced mirror folder is not this repository; do not use its `sources/` folder as a writable checkout. Codex should open this GitHub repository or its local clone as the working project. The ChatGPT conversation remains available separately and is not automatically imported into Codex history.
