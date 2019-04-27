#include <Servo.h> // include the arduino servo library.

Servo steering; // for the Servo library, defining a steering servo
Servo throttle; // for the Servo library, defining a throttle servo

byte steering_pos;  // desired angle for steering between 0 and 180
byte throttle_pos;  // desired angle for throttle between 0 and 180

bool goingForward = 1;
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

// measuringDistance is how far away we think obstacles are relevant.
// in feet, since it's easiest for me to think about.
unsigned long measuringDistance = 5;

/*
timeout for the pulseIn() function, lest we wait 1 second per reading if
nothing's around.

meauring distance in feet * convert to inches * convert to microseconds * double distance
*/

unsigned long timeout = measuringDistance * 12 * 74.052 * 2;

const byte frontTrigPin = 10; // pin to trigger for front HC-SR04
const byte frontEchoPin = 11; // pin to trigger for front HC-SR04

const byte leftTrigPin = 2; // pin to trigger for left HC-SR04
const byte leftEchoPin = 3; // pin to trigger for left HC-SR04

const byte rightTrigPin = 4; // pin to trigger for right HC-SR04
const byte rightEchoPin = 5; // pin to trigger for right HC-SR04

const byte rearTrigPin = 6; // pin to trigger for rear HC-SR04
const byte rearEchoPin = 7; // pin to trigger for rear HC-SR04

struct sensorData {
  unsigned long front;
  unsigned long left;
  unsigned long right;
  unsigned long rear;
};


void setup()
{
  Serial.begin(9600); // Begin serial communication so we can monitor things

  pinMode(frontTrigPin, OUTPUT); // Front HC-SR04 Trigger Pin
  pinMode(frontEchoPin, INPUT); // Front HC-SR04 Echo Pin

  pinMode(leftTrigPin, OUTPUT); // Left HC-SR04 Trigger Pin
  pinMode(leftEchoPin, INPUT); // Left HC-SR04 Echo Pin

  pinMode(rightTrigPin, OUTPUT); // Right HC-SR04 Trigger Pin
  pinMode(rightEchoPin, INPUT); // Right HC-SR04 Echo Pin

  pinMode(rearTrigPin, OUTPUT); // Rear HC-SR04 Trigger Pin
  pinMode(rearEchoPin, INPUT); // Rear HC-SR04 Echo Pin

  /*
  servo.attach(pin, min, max)
  pin = the pin the servo is connected to
  min = the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the servo (defaults to 544)
  max = the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the servo (defaults to 2400)
  */
  steering.attach(8, 1300, 2000); // restricted to avoid banging into things and compensate for crooked steering linkage
  throttle.attach(9, 1400, 1600); // esc calibrated at 1000 - 2000, restricted to keep it from going too fast

  // set the servos to neutral position and wait 5 milliseconds to arm ESC
  // Only throttle requires neutral to arm, but it's nice pointing the wheels forward too
  steering.write(90); // servo center at 90 degrees.
  throttle.write(90); // servo center at 90 degrees.
  delayMicroseconds(5000); // wait for .005 seconds
}

void loop()
{
  sensorData data;

  SensorPolling(data);
  Navigation(data);
  EnableReverse();
  ThrottleControl();
  SteeringControl();
}

void SensorPolling(sensorData &data)
{
  data.front = dist(frontTrigPin, frontEchoPin);
  data.left = dist(leftTrigPin, leftEchoPin);
  data.right = dist(rightTrigPin, rightEchoPin);
  data.rear = dist(rearTrigPin, rearEchoPin);
}

unsigned long dist(byte trigPin, byte echoPin)
{
  unsigned long duration;
  unsigned long distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration/74.052/2;
  return(distance);
}

void Navigation(sensorData data)
{
  int leftMap;
  int rightMap;

  int frontMap;
  int rearMap;

  // STEERING
  leftMap = map(data.left, 0, measuringDistance, 90, 0); // convert left distance to a steering offset
  rightMap = map(data.right, 0, measuringDistance, 90, 0); // convert right distance to a steering offset
  if (goingForward==0) {
    steering_pos = 90 + leftMap - rightMap; // steer backwards if you're in reverse
  }
  else{
    steering_pos = 90 - leftMap + rightMap; // otherwise steer normally
  }

  //THROTTLE
  frontMap = map(data.front, 0, measuringDistance, 80, 0); // convert left distance to a steering offset
  rearMap = map(data.rear, 0, measuringDistance, 0, 90); // convert right distance to a steering offset

  throttle_pos = 90 - frontMap + rearMap;
}

void SteeringControl()
{
  static unsigned long time; // placeholder for the last time steering updated.
  int cycle = 25; // minimum number of milliseconds before evaluation the next steering step
  int step = 5; // how many degrees we move the throttle per cycle

  if ((millis()-time) >= cycle)
  {
    time = millis(); // save time reference for next position update

    // update steering position
    // if desired position is different from current position move one step left or right
    if (steering_pos > steering.read()) steering.write(steering.read() + step);
    else if (steering_pos < steering.read()) steering.write(steering.read() - step);
  }
}

void ThrottleControl()
{
  static unsigned long time;  // placeholder for the last time throttle updated.
  int cycle = 25; // minimum number of milliseconds before evaluation the next throttle step
  int step = 3; // how many degrees we move the throttle per cycle

  if ((millis()-time) >= cycle)
  {
    time = millis(); // save time reference for next position update

    // update throttle position
    // if desired position is different from current position increase or decrease throttle by one step
    if (throttle_pos > throttle.read()) throttle.write(throttle.read() + step);
    else if (throttle_pos < throttle.read()) throttle.write(throttle.read() - step);
  }
}

void EnableReverse()
{
  static unsigned long time; // placeholder for the last time steering updated.


  if (throttle_pos > 1500) {
    time = millis();
    goingForward = 1;
  }
  else if (millis()-time > 200 && goingForward == 1)
  {
    goingForward = 0;
    throttle.write(1500);
    delay(0.1);
    throttle.write(1000);
    delay(0.1);
    throttle.write(1500);
  }

}
