# Actuator and vessel input register

Complete this from the actual actuator datasheet, supplier confirmation, and vessel measurements. Keep unknowns marked `TBD`; do not substitute guessed values. This is a sizing worksheet, not a wiring diagram.

## One copy per actuator model

| Variable | Left actuator | Right actuator | Evidence / notes |
|---|---|---|---|
| Manufacturer and exact model | TBD | TBD | Datasheet URL and revision |
| Rated supply voltage and allowed range | TBD | TBD | Include actual boat supply |
| Running current, extend / retract | TBD | TBD | Measure under representative load |
| Stall / locked-rotor current | TBD | TBD | Datasheet or controlled measurement |
| Travel time for full stroke | TBD | TBD | Record load, supply voltage, and direction |
| Stroke length | TBD | TBD | mm |
| Built-in end switches | TBD | TBD | Normally-open/closed, polarity, interrupting rating |
| Position-feedback output | TBD | TBD | Type, range, resolution, failure behavior |
| Duty cycle / thermal limits | TBD | TBD | Include ambient and mounting |
| Mechanical load / force | TBD | TBD | Manufacturer rating and installation geometry |
| Environmental rating | TBD | TBD | Water, salt, vibration, temperature |
| Cable length and conductor | TBD | TBD | Installed route and connector |
| Manufacturer protection requirements | TBD | TBD | Fuse, suppression, polarity, grounding |
| Current at representative operating load | TBD | TBD | Measure during bench testing |

## Vessel and installation variables

| Variable | Value | Evidence / notes |
|---|---|---|
| Nominal and measured supply voltage range at controller | TBD | Include engine off, cranking, charging, and transients |
| Existing battery chemistry/system voltage | TBD | Verify vessel installation |
| Available protected accessory feed | TBD | Circuit rating and cable path |
| Controller/display mounting location | TBD | Exposure, vibration, operator sightline |
| Required visibility in direct sun | TBD | Bench or vessel observation |
| Enclosure / ingress target | TBD | Based on actual mounting location |
| Distance from controller to actuators | TBD | Route length, not straight-line distance |
| Distance from switches to actuators/controller | TBD | Needed for wiring and voltage-drop review |
| Tab mechanical end stops and safe travel | TBD | Vessel/actuator documentation |
| Vessel speed range and planing transition | TBD | GPS log and operator observations |
| Expected turn/roll conditions | TBD | Boat-specific data |
| Manual control layout and illumination voltage | TBD | Panel location and user preference |

## Sizing calculations to complete after data is entered

1. Select one protected motor channel per actuator, with a documented mechanism that prevents opposite-direction energization.
2. Rate each output path for the selected actuator's allowed voltage and measured/declared current, including startup and stall behavior. Do not use the board's GPIO or an I/O expander as the motor path.
3. Calculate the upstream supply and protection for the allowed simultaneous loads and environmental/thermal derating.
4. Calculate conductor size from current, route length, allowable voltage drop, insulation temperature, bundling, and marine environment; verify protection coordinates with conductor capacity.
5. Select the control DC/DC converter from the measured supply range, transient protection plan, controller/display peak load, and temperature.
6. Check end-switch behavior, mechanical limits, timeout, and position-feedback plausibility.
7. Record assumptions and show the completed calculations in the final BOM/design review.

No motor driver, fuse, or wire rating is approved until this register is supported by actual part data and a reviewed calculation.
