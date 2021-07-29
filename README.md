# Voice controlled hospital bed inclination

This is a more advanced control program for the Particle Argon WIFI microcontroller used in
[this Instructables hospital bed controller](https://www.instructables.com/Voice-Controlled-Hospital-Bed-by-Wifi/).
Instead of using multiple IFTTT recipes (one for up and one for down, for each joint,) this uses a single recipe, passing an entire command string to the Particle microcontroller. Then a state machine parses the sudo-natural language commands.

*Digital assistant trigger* **move the bed** *command string*

*Hey Google, move the bed up 20 degrees.*

*Hey Google, move the bed legs up 10 degrees.*

*Hey Google, move the bed down.*

*Hey Google, move the bed up halfway.*

*Hey Google, stop the bed now.*

![Command Finite State Machine](https://github.com/knickers/bed-controller/raw/main/img/commands-fsm.png)

## Named positions

- halfway
	- Move head by 35 degrees
	- Move feet by 17 degrees
- all the way
	- Move head by 50 degrees
	- Move feet by 25 degrees

## Tasks

- [x] Parse commands
	- [x] Use a single IFTTT recipe
	- [x] String tokenizer
	- [x] State machine command parser
- [ ] Time-based angle estimation
	- [x] Calculate duration from estimated actuator speed
	- [ ] Account for actuator acceleration and deceleration time
- [ ] Measure DC current through pendant switches
	- [ ] Watch DC current to track actuator load and end-stops
	- [ ] Preload joint slop after detecting actuator end-stop
- [ ] Measure angle directly at bed joint
	- [x] Hall effect angle sensor
	- [ ] Calibrate sensor reading map to real angle
