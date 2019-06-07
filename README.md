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


/*
The speed of sound is 0.0135039 inches per microsecond. Approx distance value
from HC-SR04 in inches can be found by dividing pulseIn() time by 74.052 and
dividing again by 2 to account for the ping having to travel out to an object
and reflect back to the sensor.

Max distance for the HC-SR04 is 400cm or 157 inches.
157 inches is 23,253 microseconds roundtrip (0.0232 seconds).

The pulseIn() function has a timeout of 1 second unless you define it.
That's enough time for 6,752 inches which is far past our max measurable
distance. It's also far too slow and will potentially hold up our program if
there's nothing to reflect off of.
*/

/*
timeout for the pulseIn() function, lest we wait 1 second per reading if
nothing's around.

meauring distance in feet * convert to inches * convert to microseconds * double distance
*/

/*
servo.attach(pin, min, max)
pin = the pin the servo is connected to
min = the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the servo (defaults to 544)
max = the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the servo (defaults to 2400)
*/

// set the servos to neutral position and wait 5 milliseconds to arm ESC
// Only throttle requires neutral to arm, but it's nice pointing the wheels forward too


  // is there something in front and something behind?
  if (data.front < measuringDistance && data.rear < measuringDistance)
  {
    throttle_pos = 90 + frontMap - rearMap;
  }

  // is there something in front and nothing behind?
  else if (data.front < measuringDistance && data.rear > measuringDistance - 1)
  {
    throttle_pos = 90 - frontMap;
  }

  // is there nothing in front? Who cares what's behind then?
  else
  {
    throttle_pos = 180;
  }
