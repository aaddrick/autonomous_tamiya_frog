# autonomous_tamiya_frog
Autonomizing a Tamiya Frog using Arduino

We're modifying a Tamiya Frog RC card which uses a Tamiya TEU-101BK ESC
The Frog was introduced in 1983, back when everything RC was servo driven,
or accepted servo wiring for inputs. It's essentially two servos for control
purposes.

TODO Clean up this README with proper markdown...

Scrap book for the code:

Throttle and steering control:

  millis()
  "Returns the number of milliseconds passed since the Arduino board
  began running the current program. This number will overflow (go
  back to zero), after approximately 50 days."

  Using delay() stops the program and everything has to wait.

  1. servo_time starts out at 0
  2. You use millis() to find out how long the board has been running.
  3. Subtract servo_time from millis() and see if it's greater than your
    SERVO_SPEED variable.
  4. If more milliseconds have passed than your SERVO_SPEED
    4a. servo_time is updated to the current millis()
    4b. action is executed
  5. Return to step 2.

// ascii banners from http://patorjk.com/
