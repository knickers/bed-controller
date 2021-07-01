# Bed Controller

Voice control hospital bed inclination.

*Digital assistant trigger* **move the bed** command string

*Hey Google, move the bed up 20 degrees*

*Hey Google, move the bed legs up 10 degrees*

*Hey Google, move the bed down*

*Hey Google, move the bed up halfway*

[Command Finite State Machine](https://github.com/knickers/bed-controller/raw/main/img/commands-fsm.png)

- Time-based angle estimation
	- [x] Basic operation.
	- [ ] Account for actuator acceleration and deceleration time.
- Measure DC current through pendant switches.
	- [ ] Watch DC current to track actuator load and end-stops.
	- [ ] Preload joint slop after detecting actuator end-stop.
- Measure angle directly at bed joint.
	- [x] Hall effect angle sensor.
	- [ ] Calibrate sensor reading map to real angle.
