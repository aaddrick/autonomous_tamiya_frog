#include <Servo.h> // include the arduino servo library.

Servo steering; // for the Servo library, defining a steering servo
Servo throttle; // for the Servo library, defining a throttle servo

byte steering_pos;  // desired angle for steering between 0 and 180
byte throttle_pos;  // desired angle for throttle between 0 and 180

// defining HC-SR04 ultrasonic sensors

const byte frontTrigPin = 3; // pin to trigger for front HC-SR04
const byte leftTrigPin = 4; // pin to trigger for left HC-SR04
const byte rightTrigPin = 5; // pin to trigger for right HC-SR04
const byte rearTrigPin = 6; // pin to trigger for rear HC-SR04
const byte echoPin = 7;    // pin we're listen to all the HC-SR04's on

unsigned long frontInches;
unsigned long leftInches;
unsigned long rightInches;
unsigned long rearInches;
unsigned long duration;
unsigned long distance;


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

// timeout for the pulseIn() function, lest we wait 1 second per reading if
// nothing's around.
// feet * convert to inches * convert to microseconds * double distance
unsigned long timeout = measuringDistance * 12 * 74.052 * 2;

/*
███████╗███████╗████████╗██╗   ██╗██████╗
██╔════╝██╔════╝╚══██╔══╝██║   ██║██╔══██╗
███████╗█████╗     ██║   ██║   ██║██████╔╝
╚════██║██╔══╝     ██║   ██║   ██║██╔═══╝
███████║███████╗   ██║   ╚██████╔╝██║
╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝
*/

void setup()
{
  Serial.begin(9600); // Begin serial communication so we can monitor things

  pinMode(10, OUTPUT); // Front HC-SR04 Trigger Pin
  pinMode(11, INPUT); // Front HC-SR04 Echo Pin

  pinMode(2, OUTPUT); // Left HC-SR04 Trigger Pin
  pinMode(3, INPUT); // Left HC-SR04 Echo Pin

  pinMode(4, OUTPUT); // Right HC-SR04 Trigger Pin
  pinMode(5, INPUT); // Right HC-SR04 Echo Pin

  pinMode(6, OUTPUT); // Rear HC-SR04 Trigger Pin
  pinMode(7, INPUT); // Rear HC-SR04 Echo Pin

  /*
  servo.attach(pin, min, max)
  pin = the pin the servo is connected to
  min = the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the servo (defaults to 544)
  max = the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the servo (defaults to 2400)
  */
  steering.attach(8, 1300, 2000);
  throttle.attach(9, 1400, 1600);

  // set the servos to neutral position and wait 5 milliseconds to arm ESC
  // Only throttle requires neutral to arm, but it's nice pointing the wheels forward too
  steering.write(90); // servo center at 90 degrees.
  throttle.write(90); // servo center at 90 degrees.
  delayMicroseconds(5000);
}

/*
██╗      ██████╗  ██████╗ ██████╗
██║     ██╔═══██╗██╔═══██╗██╔══██╗
██║     ██║   ██║██║   ██║██████╔╝
██║     ██║   ██║██║   ██║██╔═══╝
███████╗╚██████╔╝╚██████╔╝██║
╚══════╝ ╚═════╝  ╚═════╝ ╚═╝
*/

void loop()
{
  SensorPolling();
  Navigation();
  ThrottleControl();
  SteeringControl();
}

/*
███████╗███████╗███╗   ██╗███████╗ ██████╗ ██████╗     ██████╗  ██████╗ ██╗     ██╗     ██╗███╗   ██╗ ██████╗
██╔════╝██╔════╝████╗  ██║██╔════╝██╔═══██╗██╔══██╗    ██╔══██╗██╔═══██╗██║     ██║     ██║████╗  ██║██╔════╝
███████╗█████╗  ██╔██╗ ██║███████╗██║   ██║██████╔╝    ██████╔╝██║   ██║██║     ██║     ██║██╔██╗ ██║██║  ███╗
╚════██║██╔══╝  ██║╚██╗██║╚════██║██║   ██║██╔══██╗    ██╔═══╝ ██║   ██║██║     ██║     ██║██║╚██╗██║██║   ██║
███████║███████╗██║ ╚████║███████║╚██████╔╝██║  ██║    ██║     ╚██████╔╝███████╗███████╗██║██║ ╚████║╚██████╔╝
╚══════╝╚══════╝╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝    ╚═╝      ╚═════╝ ╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝ ╚═════╝
*/

long SensorPolling()
{

  frontInches = dist(frontTrigPin);
  Serial.print(frontInches);
  Serial.println(" inches in front.");

  leftInches = dist(leftTrigPin);
  Serial.print(leftInches);
  Serial.println(" inches to the left.");

  rightInches = dist(rightTrigPin);
  Serial.print(rightInches);
  Serial.println(" inches to the right.");

  rearInches = dist(rearTrigPin);
  Serial.print(rearInches);
  Serial.println(" inches behind.");

}

unsigned long dist(byte trigPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration/74.052/2;
  return(distance);
}

/*
███╗   ██╗ █████╗ ██╗   ██╗██╗ ██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
████╗  ██║██╔══██╗██║   ██║██║██╔════╝ ██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██╔██╗ ██║███████║██║   ██║██║██║  ███╗███████║   ██║   ██║██║   ██║██╔██╗ ██║
██║╚██╗██║██╔══██║╚██╗ ██╔╝██║██║   ██║██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
██║ ╚████║██║  ██║ ╚████╔╝ ██║╚██████╔╝██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
╚═╝  ╚═══╝╚═╝  ╚═╝  ╚═══╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
*/

void Navigation()
{
  // Finish Navigation
}

/*
███████╗████████╗███████╗███████╗██████╗ ██╗███╗   ██╗ ██████╗      ██████╗ ██████╗ ███╗   ██╗████████╗██████╗  ██████╗ ██╗
██╔════╝╚══██╔══╝██╔════╝██╔════╝██╔══██╗██║████╗  ██║██╔════╝     ██╔════╝██╔═══██╗████╗  ██║╚══██╔══╝██╔══██╗██╔═══██╗██║
███████╗   ██║   █████╗  █████╗  ██████╔╝██║██╔██╗ ██║██║  ███╗    ██║     ██║   ██║██╔██╗ ██║   ██║   ██████╔╝██║   ██║██║
╚════██║   ██║   ██╔══╝  ██╔══╝  ██╔══██╗██║██║╚██╗██║██║   ██║    ██║     ██║   ██║██║╚██╗██║   ██║   ██╔══██╗██║   ██║██║
███████║   ██║   ███████╗███████╗██║  ██║██║██║ ╚████║╚██████╔╝    ╚██████╗╚██████╔╝██║ ╚████║   ██║   ██║  ██║╚██████╔╝███████╗
╚══════╝   ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝      ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚══════╝
*/

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

/*
████████╗██╗  ██╗██████╗  ██████╗ ████████╗████████╗██╗     ███████╗     ██████╗ ██████╗ ███╗   ██╗████████╗██████╗  ██████╗ ██╗
╚══██╔══╝██║  ██║██╔══██╗██╔═══██╗╚══██╔══╝╚══██╔══╝██║     ██╔════╝    ██╔════╝██╔═══██╗████╗  ██║╚══██╔══╝██╔══██╗██╔═══██╗██║
   ██║   ███████║██████╔╝██║   ██║   ██║      ██║   ██║     █████╗      ██║     ██║   ██║██╔██╗ ██║   ██║   ██████╔╝██║   ██║██║
   ██║   ██╔══██║██╔══██╗██║   ██║   ██║      ██║   ██║     ██╔══╝      ██║     ██║   ██║██║╚██╗██║   ██║   ██╔══██╗██║   ██║██║
   ██║   ██║  ██║██║  ██║╚██████╔╝   ██║      ██║   ███████╗███████╗    ╚██████╗╚██████╔╝██║ ╚████║   ██║   ██║  ██║╚██████╔╝███████╗
   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝    ╚═╝      ╚═╝   ╚══════╝╚══════╝     ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚══════╝
*/

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

/*

Steering - left low - right hig
sttering min 1300
steering max 2000

Throttle
throttle low reverse = 1600
throttle full foward is 1470
break range start
break range stop





*/
