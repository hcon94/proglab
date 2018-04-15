/*
Drive forward and turn left or right when border is detected
  - Only reflectionsensor 0 and 5 are used.
*/
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <NewPing.h>
//BT
#include <SoftwareSerial.h>//BT
#include <PLabBTSerial.h>//BT

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

//BT Definer inn/utgangspinnene som brukes for send (TX) og motta (RX) for bluetooth
//BT Define I/O ports used for transmit (TX) and receive (RX)
const int BT_RX = 0;  //BT Connect to RXD on Bluetooth unit
const int BT_TX = 1;  //BT Connect to TXD on Bluetooth unit
//BT Define the serial port for communication with bluetooth
PLabBTSerial btSerial (BT_TX, BT_RX); //BT

ZumoReflectanceSensorArray sensors;
NewPing sonar(triggerPin, echoPin, MAX_DISTANCE);
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

// If we should enable the zumo robot
boolean robotEnabled = false;


void setup()
{
   sensors.init();
   Serial.begin(9600);//BT
   Serial.println("Hello world");//BT
   // Start communication with bluetooth unit
   btSerial.begin (9600);
}

void loop()
{

  sensors.read(sensor_values);

// Bluetooth stuff:
  // See if we have received new characters
  int availableCount = btSerial.available();
  if (availableCount > 0) {

    // Define string (character array) large enough to hold all characters
    char text[availableCount];
    // Read the character. Remember to bring along size of the character array to avoid overflow
    btSerial.read(text, availableCount);

    // Output the received text
    Serial.println(text);

    // Split the text at ':', the command is made up of two parts
    char* secondPart = strchr (text, ':');
    // Ensure the split was successful (nullpointer check)
    if (secondPart == 0) {
      Serial.println("Illegal command received, stopping robot");
      robotEnabled = false;
    } else {
      // The split was succesful

      // The first character of secondPart is now ':', but it should be the end character ('\0' == 0) to end the first string
      secondPart[0] = '\0';
      // And the first character of the secondPart should be moved one step foreward
      secondPart++;

      // Now we should recognize the command
      if (strcmp ("BTN", text) == 0) {    // There is no difference between the strings
        if (strcmp ("ON", secondPart) == 0) {  // There is no difference between the strings
          // Robot should be turned on
          robotEnabled = true;
        } else if (strcmp("OFF", secondPart) == 0) {
          // Robot should stop
          robotEnabled = false;
        }
      }
    }
  }
  //BT "end"
  // If robot is enabled, it should move.
  if (robotEnabled) {
    motors.setSpeeds(200, 200);

    //Sensor stuff
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
  else {
    // If not BT enabled it should stop
    motors.setSpeeds(0, 0);
  }
}
