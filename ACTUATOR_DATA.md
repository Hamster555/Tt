# Actuator and vessel input register

Complete this from the actual actuator datasheet, supplier confirmation, and vessel measurements. Keep unknowns marked `TBD`; do not substitute guessed values. This is a sizing worksheet, not a wiring diagram.

## Public manufacturer references (candidates only; not selected)

These catalog entries match the requested nominal 12 V class and approximate stroke lengths. They are not confirmed compatible with the boat's tab geometry, mount spacing, load, or 12–14 V charging range. Do not copy them into the left/right actuator fields above until the exact part is selected and its current data is verified.

| Candidate | Manufacturer-published voltage and stroke | Current information found | Open checks |
|---|---|---|---|
| Lenco 101 Standard, part 15054-001 | 12 V variant; 2.25 in (57.15 mm) stroke | Operating and stall current: TBD; the Lenco product page does not publish actuator draw | Confirm allowed supply range including charging voltage, application geometry, force/load, and current with Lenco |
| Lenco 102, part 15059-001 | 12 V variant; 4.25 in (107.95 mm) stroke | Operating and stall current: TBD; the Lenco product page does not publish actuator draw | Lenco describes this as a trolling-tab / OEM model; confirm suitability, allowed supply range, geometry, and current with Lenco |
| Bennett BOLT BEA2000 | 12 V actuator; 2.5 in (63.5 mm) stroke | Bennett publishes 20 A maximum actuator draw; stall/locked-rotor and normal loaded current remain TBD | Confirm allowed supply range including charging voltage, load-specific current, mounting geometry, and system control compatibility |

The Lenco installation manual wiring diagram lists a 20 A fuse/breaker on a 12 V two-tab kit. That is the kit's upstream protection instruction, not a published per-actuator running or stall-current figure. Bennett's published 20 A maximum actuator draw is model-specific and must not be generalized to other actuators. These candidate figures are not a fuse, wire, connector, or driver selection for this project.

Sources: [Lenco 100 Series actuator specifications](https://lencomarine.navico.com/trim/actuators/), [Bennett BOLT actuator specifications](https://bennetttrimtabs.com/product/bolt-electric-trim-tab-systems/), [Bennett BOLT installation manual](https://www.bennetttrimtabs.com/installManuals/BOLT-Set-Manual-WEB-052123.pdf), [Lenco owner manual copy with system wiring diagram](https://www.boathut.com.au/boathutbasel/wp-content/uploads/2020/07/Lenco_Trim_Tab_Manual.pdf), [Bennett BEA2000 catalog flyer with stroke dimension](https://bennetttrimtabs.com/wp-content/uploads/2018/08/BOLT-Flyer2018.pdf). Accessed 2026-11-23.

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
