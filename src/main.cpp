#include <Servo.h>

#define SERVO_SPEED 25    // minimum number of milliseconds per step
#define STEERING_STEP 3   //desired incremental step for steering in degrees
#define THROTTLE_STEP 3   //desired incremental step for throttle in degrees

int steering_pos;  // desired angle for steering
int throttle_pos;  // desired angle for throttle

void setup()
{
  steering.attach(8);
  throttle.attach(9);

  // set the servos to center position and wait 5 seconds to arm ESM

  // TODO test whether or not is needed specifically to arm the ESC.
  // if so, does it need to be both the steering and throttle, or just the throttle?
  // if not, what happens to steering.read() when there is no set value?

  steering.write(90); // servo center at 90 degrees. could use servo.writeMicroseconds(uS)
  throttle.write(90); // servo center at 90 degrees. could use servo.writeMicroseconds(uS)
  delayMicroseconds(5000); // TODO why would I use this instead of delay(5)?
}

void loop()
{
 DriveControl();
}


void DriveControl() // TODO break this out into separate functions and speed variables for throttle and steering
{
// http://forum.arduino.cc/index.php?topic=4854.msg37047#msg37047

/*
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
*/

 static unsigned long servo_time;

 // check time since last servo position update
 if ((millis()-servo_time) >= SERVO_SPEED)
 {
   servo_time = millis(); // save time reference for next position update

   // update steering position
   // if desired position is different from current position move one step left or right
   if (steering_pos > steering.read()) steering.write(steering.read() + STEERING_STEP);
   else if (steering_pos < steering.read()) steering.write(steering.read() - STEERING_STEP);

   // update throttle position
   // if desired position is different from current position move one step left or right
   if (throttle_pos > throttle.read()) throttle.write(throttle.read() + THROTTLE_STEP);
   else if (throttle_pos < throttle.read()) throttle.write(throttle.read() - THROTTLE_STEP);
 }
