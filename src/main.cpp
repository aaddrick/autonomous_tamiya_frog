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

  // set the centered position to arm the ESC
  steering.write(90);
  throttle.write(90);

  // stay centered for 5 seconds
  delayMicroseconds(5000);
}

void loop()
{

}


void control()
{
 static unsigned long servo_time;

 // check time since last servo position update
 if ((millis()-servo_time) >= SERVO_SPEED) {
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
