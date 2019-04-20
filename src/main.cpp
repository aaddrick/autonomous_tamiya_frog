#include <Servo.h> // include the arduino servo library.

/*
██╗   ██╗ █████╗ ██████╗ ██╗ █████╗ ██████╗ ██╗     ███████╗    ██████╗ ███████╗ ██████╗██╗      █████╗ ██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
██║   ██║██╔══██╗██╔══██╗██║██╔══██╗██╔══██╗██║     ██╔════╝    ██╔══██╗██╔════╝██╔════╝██║     ██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
██║   ██║███████║██████╔╝██║███████║██████╔╝██║     █████╗      ██║  ██║█████╗  ██║     ██║     ███████║██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║
╚██╗ ██╔╝██╔══██║██╔══██╗██║██╔══██║██╔══██╗██║     ██╔══╝      ██║  ██║██╔══╝  ██║     ██║     ██╔══██║██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
 ╚████╔╝ ██║  ██║██║  ██║██║██║  ██║██████╔╝███████╗███████╗    ██████╔╝███████╗╚██████╗███████╗██║  ██║██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
  ╚═══╝  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝    ╚═════╝ ╚══════╝ ╚═════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
*/


// defining steering and throttle

Servo steering; // for the Servo library, defining a steering servo
Servo throttle; // for the Servo library, defining a throttle servo

const byte steeringPin = 8;
const int steeringMin = 1000; // the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the steering servo (defaults to 544)
const int steeringMax = 2000; // the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the steering servo (defaults to 2400)
const int steering_speed = 25; // minimum number of milliseconds before evaluating the next steering step
int steering_step = 3;  // how many degrees we turn each steering cycle
byte steering_pos = 90;  // desired angle for steering

const byte throttlePin = 9;
const int throttleMin = 1000; // the pulse width, in microseconds, corresponding to the minimum (0-degree) angle on the throttle servo (defaults to 544)
const int throttleMax = 2000; // the pulse width, in microseconds, corresponding to the maximum (180-degree) angle on the throttle servo (defaults to 2400)
const int throttle_speed = 25; // minimum number of milliseconds before evaluation the next throttle step
int throttle_step = 3;  // how many degrees we move the throttle per cycle
byte throttle_pos = 90;  // desired angle for throttle


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
can be found by dividing pulseIn() time by 74.052 and dividing again by 2 to
account for the ping having to travel out to an object and reflect back to the
sensor.

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
  Serial.begin(9600);

  // the array elements are numbered from 0 to (pinCount - 1).
  // use a for loop to initialize each pin as an output:
  for (int thisPin = 0; thisPin < trigPins; thisPin++)
  {
    pinMode(trigArray[thisPin], OUTPUT);
  }

  pinMode(echoPin, INPUT);

  steering.attach(steeringPin, steeringMin, steeringMax);
  throttle.attach(throttlePin, throttleMin, throttleMax);

  // set the servos to center position and wait 5 seconds to arm ESM

  // TODO test whether or not the below chunk is needed specifically to arm the ESC.
  // if so, does it need to be both the steering and throttle, or just the throttle?
  // if not, what happens to steering.read() when there is no set value?

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
  static unsigned long steering_time; // placeholder for the last time steering updated.

  if ((millis()-steering_time) >= steering_speed)
  {
    steering_time = millis(); // save time reference for next position update

    // update steering position
    // if desired position is different from current position move one step left or right
    // TODO is positive or negative left?
    if (steering_pos > steering.read()) steering.write(steering.read() + steering_step);
    else if (steering_pos < steering.read()) steering.write(steering.read() - steering_step);
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
  static unsigned long throttle_time;  // placeholder for the last time throttle updated. Used in ThrottleControl()

  if ((millis()-throttle_time) >= throttle_speed)
  {
    throttle_time = millis(); // save time reference for next position update

    // update throttle position
    // if desired position is different from current position move one step faster or slower.
    // TODO is positive or negative more throttle?
    if (throttle_pos > throttle.read()) throttle.write(throttle.read() + throttle_step);
    else if (throttle_pos < throttle.read()) throttle.write(throttle.read() - throttle_step);
  }
}
