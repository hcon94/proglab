/*
Drive forward and turn left or right when border is detected
  - Only reflectionsensor 0 and 5 are used.
*/
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>
 
// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  500 // 
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        300
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms
#define MAX_DISTANCE      25
int FORWARD_SPEED = 200;
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];

// --Setup for ultrasound--
// The pin we will receive echo calls from
const int echoPin = 3;  // Must be a PWM pin
// The pin we will use to send the signal
const int triggerPin = 6;
 
ZumoReflectanceSensorArray sensors;
NewPing sonar(triggerPin, echoPin, MAX_DISTANCE);
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12


 
void setup()
{
   sensors.init();
   button.waitForButton();
}

void loop()
{
  
  sensors.read(sensor_values);

  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensor_values[4] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    unsigned int time = sonar.ping();
    // Calculate how far this time represent
    float distance = sonar.convert_cm(time);
    Serial.println(distance);
    if (distance > 0) {
      FORWARD_SPEED = 400;
    } 
    else {
      distance = MAX_DISTANCE; // Forward full speed if no object
      FORWARD_SPEED = 220;
    } 
    // Map the 5 to 25 distance range into speed forward and backwards
    // ( 5 cm distance means full reverese speed)
    int dSpeed = map(distance, 5, MAX_DISTANCE, -FORWARD_SPEED, FORWARD_SPEED);    
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}

