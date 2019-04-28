#include <Servo.h>

Servo steering;
Servo throttle;

const unsigned long maxDistance = 60; // in inches
bool goingForward = 1;

const byte frontTrigPin = 10;
const byte frontEchoPin = 11;

const byte leftTrigPin = 2;
const byte leftEchoPin = 3;

const byte rightTrigPin = 4;
const byte rightEchoPin = 5;

const byte rearTrigPin = 6;
const byte rearEchoPin = 7;

struct sensorData {
  unsigned long front;
  unsigned long left;
  unsigned long right;
  unsigned long rear;
};


void setup()
{
  pinMode(frontTrigPin, OUTPUT); // Front HC-SR04 Trigger Pin
  pinMode(frontEchoPin, INPUT); // Front HC-SR04 Echo Pin

  pinMode(leftTrigPin, OUTPUT); // Left HC-SR04 Trigger Pin
  pinMode(leftEchoPin, INPUT); // Left HC-SR04 Echo Pin

  pinMode(rightTrigPin, OUTPUT); // Right HC-SR04 Trigger Pin
  pinMode(rightEchoPin, INPUT); // Right HC-SR04 Echo Pin

  pinMode(rearTrigPin, OUTPUT); // Rear HC-SR04 Trigger Pin
  pinMode(rearEchoPin, INPUT); // Rear HC-SR04 Echo Pin

  steering.attach(8, 700, 1300);
  throttle.attach(9, 1425, 1510);

  steering.write(90);
  throttle.write(90);
  delay(2000);
}

void loop()
{
  sensorData data;

  SensorPolling(data);
  ThrottleControl(data);
  SteeringControl(data);
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
  duration = pulseIn(echoPin, HIGH, maxDistance * 74.052 * 2);
  distance = duration/74.052/2;

  if (distance > maxDistance)
  {
    return(maxDistance);
  }
  else
  {
    return(distance);
  }
}



void SteeringControl(sensorData data)
{
  byte steering_pos;  // desired angle for steering between 0 and 180



  int turnRight = map(data.left, 0, maxDistance, 90, 0);
  int turnLeft = map(data.right, 0, maxDistance, -90, 0);

  if (goingForward==0) {
    steering_pos = 90 - turnRight - turnLeft; // steer backwards if you're in reverse
  }
  else{
    steering_pos = 90 + turnRight + turnLeft; // otherwise steer normally
  }



  static unsigned long time; // placeholder for the last time steering updated.
  int cycle = 10; // minimum number of milliseconds before evaluation the next steering step
  int step = 7; // how many degrees we move the throttle per cycle

  if ((millis()-time) >= cycle)
  {
    time = millis(); // save time reference for next position update

    // update steering position
    // if desired position is different from current position move one step left or right
    if (steering_pos > steering.read()) steering.write(steering.read() + step);
    else if (steering_pos < steering.read()) steering.write(steering.read() - step);
  }
}

void ThrottleControl(sensorData data)
{
  byte throttle_pos;  // desired angle for throttle between 0 and 180


  static unsigned long reverseTime;

  if (throttle_pos > 90) {
    reverseTime = millis();
    goingForward = 1;
  }

  else if (millis()-reverseTime > 3000 && goingForward == 1)
  {
    goingForward = 0;
    throttle.write(89);
    delay(100);
    throttle.write(0);
    delay(100);
    throttle.write(89);
  }




  int throttleDown = map(data.front, 0, maxDistance, -90, 0);
  int throttleUp = map(data.rear, 0, maxDistance, 0, 90);

  throttle_pos = 90 + throttleDown + throttleUp;



  static unsigned long time;  // placeholder for the last time throttle updated.
  int cycle = 25; // minimum number of milliseconds before evaluation the next throttle step
  int step = 10; // how many degrees we move the throttle per cycle

  if ((millis()-time) >= cycle)
  {
    time = millis(); // save time reference for next position update

    // update throttle position
    // if desired position is different from current position increase or decrease throttle by one step
    if (throttle_pos > throttle.read()) throttle.write(throttle.read() + step);
    else if (throttle_pos < throttle.read()) throttle.write(throttle.read() - step);

  }
}
