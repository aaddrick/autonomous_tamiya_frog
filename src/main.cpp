#include <Servo.h>

#define STEERING_STEP 3   //desired incremental step for steering in degrees
#define THROTTLE_STEP 3   //desired incremental step for throttle in degrees

int steering_pos;  // desired angle for steering
int throttle_pos;  // desired angle for throttle


void setup()
{
  steering.attach(8);
  throttle.attach(9);

  // set the servos to center position and wait 5 seconds to arm ESM

  // TODO test whether or not the below chunk is needed specifically to arm the ESC.
  // if so, does it need to be both the steering and throttle, or just the throttle?
  // if not, what happens to steering.read() when there is no set value?

  steering.write(90); // servo center at 90 degrees. could use servo.writeMicroseconds(uS)
  throttle.write(90); // servo center at 90 degrees. could use servo.writeMicroseconds(uS)
  delayMicroseconds(5000); // TODO why would I use this instead of delay(5)?
}

void loop()
{
  // TODO sensor function - first check what you see
  // TODO planning function - Then tie it into what you're doing
  ThrottleControl(throttle_pos);
  SteeringControl(steering_pos);
}

void SteeringControl(steering_time)
{
  static int steering_speed = 25; // minimum number of milliseconds per step
  static unsigned long steering_time; // placeholder for the last time steering updated.

  if ((millis()-steering_time) >= steering_speed)
  {
    steering_time = millis(); // save time reference for next position update

    // update steering position
    // if desired position is different from current position move one step left or right
    // TODO is positive or negative left?
    if (steering_pos > steering.read()) steering.write(steering.read() + STEERING_STEP);
    else if (steering_pos < steering.read()) steering.write(steering.read() - STEERING_STEP);
  }
}

void ThrottleControl(throttle_time)
{
  static int throttle_speed = 25; // minimum number of milliseconds per step
  static unsigned long throttle_time;  // placeholder for the last time throttle updated. Used in ThrottleControl()

  if ((millis()-throttle_time) >= throttle_speed)
  {
    throttle_time = millis(); // save time reference for next position update

    // update throttle position
    // if desired position is different from current position move one step faster or slower.
    // TODO is positive or negative more throttle?
    if (throttle_pos > throttle.read()) throttle.write(throttle.read() + THROTTLE_STEP);
    else if (throttle_pos < throttle.read()) throttle.write(throttle.read() - THROTTLE_STEP);
  }
}
