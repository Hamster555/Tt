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

Current source branch: work/codex-trimtab-modules, latest code commit 36d859e (source only; no firmware release).

- src/main.cpp is the Wokwi adapter. The shared ControlCore in src/control_logic.h is called by firmware and host tests.
- Behavior is split into input_processing.h, mode_state.h, command_arbitration.h, actuator_simulation.h, and telemetry.h.
- test/control_logic_test.cpp exercises the same state, command, input, timing, simulation, and telemetry paths used by firmware for P-01 through P-10. P-11 and P-12 remain physical bench checks only.
- Wi-Fi startup is non-blocking. Local input handling runs every loop whether or not Wi-Fi is connected.
- AUTO remains a validity-gated placeholder and does not calculate trim from boat data.
- Earlier baseline CI run [45241019073](https://github.com/Hamster555/Tt/actions/runs/45241019073) passed, but predates this refactor.
- CI run [45258585571](https://github.com/Hamster555/Tt/actions/runs/45258585571) exposed the P-10 endpoint bug; the fix uses direction-specific endpoint checks. CI run [45259694035](https://github.com/Hamster555/Tt/actions/runs/45259694035), attempt 2, passed P-01–P-10, ESP32-S3 build, Wokwi lint, and Wokwi simulation. Attempt 1 hit a Wokwi API transport close (code 1006); retry passed.
- Local verification could not run on the Codex host: g++, pio, and wokwi-cli are unavailable, and WOKWI_CLI_TOKEN is not present locally.

## Project documents

Read these before changing hardware behavior:

1. `AGENTS.md`
2. `CONTROL_REQUIREMENTS.md`
3. `DESIGN_BASELINE.md`
4. `ACTUATOR_DATA.md`
5. `BENCH_VALIDATION.md`

## Next software work

1. Resolve open software and hardware decisions in CONTROL_REQUIREMENTS.md, DESIGN_BASELINE.md, and ACTUATOR_DATA.md only from owner decisions, datasheets, and measurements.
2. Gather exact actuator and vessel electrical data; no current draw or allowed 14 V charging range is confirmed for the candidate actuators.
3. Plan and record physical P-11/P-12 bench validation after a real independent manual path and hardware interlocks are built. Do not describe the firmware as marine-ready.

## Hardware inputs still needed

Fill ACTUATOR_DATA.md with exact actuator models, supply range, running/stall current, stroke/travel, end switches or position feedback, duty cycle, route lengths, and the boat's electrical system. Then review display mounting/sunlight conditions, manual panel layout, degraded-mode permissions, web-command priority in AUTO, and whether the approximately US$100 electronics target excludes enclosure and shipping.

## Current unresolved software variables

- AUTO_REQUIRED_INPUTS: GPS validity is currently only a simulation placeholder.
- WEB_COMMAND_DURING_AUTO_POLICY: prototype rejects web movement in AUTO; confirm final web/local semantics.
- DEGRADED_MANUAL_POLICY: prototype inhibits monitored firmware movement in DEGRADED; independent hardwired manual control is not simulated.
- MAX_SOFTWARE_COMMAND_MS: TBD from actuator travel and bench data. The 1500 ms limit is only a simulator API ceiling.
- POSITION_SOURCE: simulated percentage is not measured actuator position.
- HOLD_MEANING: prototype stops outputs and requires release plus a fresh press after returning to MANUAL; confirm operator meaning.

## Continuity note

The source ChatGPT Project is named `trimtabs`. Its synced mirror folder is not this repository; do not use its `sources/` folder as a writable checkout. Codex should open this GitHub repository or its local clone as the working project. The ChatGPT conversation remains available separately and is not automatically imported into Codex history.
