# Codex project instructions

This repository contains the trim-tab controller project for a roughly 6 m planing boat.

## Start here

Before changing the project, read these files in order:

1. `CONTROL_REQUIREMENTS.md` — agreed behavior and acceptance checks.
2. `DESIGN_BASELINE.md` — phase status, provisional controller candidate, and unresolved design variables.
3. `ACTUATOR_DATA.md` — hardware and vessel data still required before power sizing.
4. `BENCH_VALIDATION.md` — physical checks; unchecked items are not passed.

Use the documents as the current project handoff. If they conflict, do not guess; preserve the safer constraint and record the conflict.

## Project constraints

- The current firmware and Wokwi diagram are a simulation, not marine-ready equipment.
- Use one ESP32 as the only controller MCU. Do not add a microcontroller to the power module.
- Physical manual controls must operate through an electrical path independent of ESP32 firmware and GPIO. Do not represent simulated GPIO buttons as proof of this hardware bypass.
- GPIOs and I/O expanders are logic signals only. Never connect them directly to actuator motors.
- Opposing directions for one actuator must not be energized together.
- Boot, reset, fault, invalid required data, command expiry, and stop conditions must leave software outputs off.
- Wi-Fi/web UI loss must not disable local control.
- Do not implement boat-specific AUTO targets, thresholds, or timing until their required sensor inputs and vessel data are supplied.
- Do not invent actuator voltage/current, stall current, fuse ratings, wire sizes, connector ratings, end-stop behavior, or environmental ratings. Keep unknown values explicitly marked `TBD`.
- Do not claim bench or vessel checks passed unless a result is recorded in `BENCH_VALIDATION.md`.

## Development and verification

- Preserve the Wokwi ESP32-S3 development target until the physical controller board is confirmed.
- Keep pure control rules testable without Arduino dependencies in `src/control_logic.h`.
- Run host regression checks with:
  `g++ -std=c++17 -Wall -Wextra -Werror -Isrc test/control_logic_test.cpp -o /tmp/control_logic_test && /tmp/control_logic_test`
- Build firmware with:
  `pio run -e esp32-s3-devkitc-1`
- When changing firmware or tests, inspect the GitHub Actions run for host checks, firmware compilation, Wokwi diagram lint, and simulation. Do not treat CI as proof of physical safety.
- Keep test and hardware status accurate in the project documents. List unresolved inputs as variables rather than assigning guessed values.
