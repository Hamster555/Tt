# Bench and vessel validation checklist

Status: planned only. Do not connect the simulated firmware directly to motors. Begin only after the exact controller, actuator, output stage, protection, physical switches, and independent manual path are documented.

Record firmware commit, hardware revisions, instruments, supply voltage, actuator load, operator, date, expected result, observed result, and pass/fail for every run. Stop a run on unexpected movement, heating, odor, smoke, loss of manual control, or readings outside manufacturer ratings.

## Stage A — logic-only and output-stage bench

- [ ] Verify all outputs remain inactive through power-on, reset, brownout, and firmware update.
- [ ] Verify a command for one direction cannot activate the opposite direction on the same actuator.
- [ ] Inject simultaneous conflicting commands from each available source; verify both motor directions remain off and the event is reported.
- [ ] Exercise timed command expiry at minimum, nominal, and maximum configured durations.
- [ ] Verify physical switch input cancels AUTO before any monitored software command is applied.
- [ ] Invalidate each required sensor while AUTO is active; verify immediate configured safe transition, no stale-data continuation, and no automatic resume when data returns.
- [ ] Verify HOLD, FAULT, and reset stop software outputs; fault clear returns to MANUAL with outputs off.
- [ ] Disconnect Wi-Fi and display interfaces; verify local control and fail-safe output behavior continue.
- [ ] Disconnect or stall the ESP32 and separately remove its power while operating the physical manual switches; verify the independent electrical manual path still works.
- [ ] Exercise opposite manual directions, including simultaneous opposite requests, and verify the physical interlock prevents opposing power at the actuator.
- [ ] Measure actuator startup, running, and stall protection behavior under a controlled and current-limited test setup.
- [ ] Verify end switches, software timeouts, mechanical stops, and position feedback agree before enabling automatic movement.
- [ ] Measure controller reset/noise behavior while each actuator starts, stops, and reaches a limit.

## Stage B — environmental and installation checks

- [ ] Verify display visibility, touch/button use, and status comprehension in direct sun and expected operator posture.
- [ ] Verify sealing, strain relief, cable routing, vibration retention, corrosion protection, grounding, and service access at the intended location.
- [ ] Verify controller and driver temperatures during repeated operation at the expected ambient range.
- [ ] Check voltage at the controller during engine start, charging, and simultaneous actuator operation.
- [ ] Verify EMI/noise does not cause resets, false inputs, sensor corruption, or unwanted motion.

## Stage C — controlled vessel evaluation

Do not proceed until Stage A and B records are reviewed and the operator agrees the manual control remains dominant.

- [ ] Establish baseline tab position, speed, roll, pitch, turn, and engine/neutral observations with automatic output disabled.
- [ ] Validate sensor orientation, offsets, freshness, failure detection, and disagreement handling.
- [ ] Set speed/planing/turn thresholds from recorded observations, with explicit hysteresis and dwell.
- [ ] Test one bounded behavior at a time in a safe operating area with an observer and immediate physical stop.
- [ ] Verify GPS loss, IMU invalidity, controller reset, and power interruption under each enabled behavior.
- [ ] Review logs and mechanical effects after each run; change one variable per trial.
- [ ] Approve any automatic behavior only after documented pass criteria and rollback settings are agreed.

## Required completion record

For each checkbox, record the relevant test ID from `CONTROL_REQUIREMENTS.md`, date, commit, hardware part numbers, supply/load details, result, evidence file, issue/follow-up, and reviewer. Empty checkboxes mean untested, not passed.
