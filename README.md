# ElevatorControl

Secure Access Elevator Control System

Control of elevator using a finite state machine, limited user input, distance sensing, motion sensing, and motor control.
Distance sensors are used as a location finding system and the motion sensing stops the doors from closing when people are near for safety.

The Elevator functions in the following way:
1. The user requests the elevator to a specific floor by pressing the associated button.
2. The motor elevator turns on and moves to the specified floor.
3. The solenoids that control the door opens for the user to enter.
4. The user selects a destination floor.
5. The elevator closes the door after a 8 second time delay and begins the journey to the requested floor.
6. The elevator opens the door at the floor to let the user out.
7. If there are no more requests in the queue , then the elevator moves back to floor 1.
