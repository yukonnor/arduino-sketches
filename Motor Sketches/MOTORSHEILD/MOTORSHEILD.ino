/*
Adafruit Arduino - Lesson 15. Bi-directional Motor
*/


#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

//DEFINTE VARIABLES
const int buttonPin = 1;
const int potPin = 0;

int buttonState = 0;
int lastbuttonState = 0;    // the previous reading from the input pin
int buttonpushCounter = 0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  // turn on motor
  myMotor->run(RELEASE);
  
  pinMode(buttonPin, INPUT);
}

void loop() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastbuttonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonpushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(buttonpushCounter);
    } else {
      // if the current state is LOW then the button
      // wend from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(100);
  }
  // save the current state as the last state,
  //for next time through the loop
  lastbuttonState = buttonState;

}

/*void loop() {
  
  buttonState = digitalRead(buttonPin);
  int speed = analogRead(potPin) / 4;
  
  
  if (buttonState != lastbuttonState) {
    if (buttonState == HIGH) {
       //do stuff

      buttonpushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes:  ");
      Serial.println(buttonpushCounter);
     }
   
    else {
       //do stuff

       Serial.println("off");
     }
    
     
     delay(100);
  }
  
lastbuttonState = buttonState;
  
 
}
*/
 

/*

       myMotor->run(FORWARD);
       myMotor->setSpeed(speed);  
       
     

       myMotor->run(BACKWARD);
       myMotor->setSpeed(speed);



if (buttonState == HIGH) {
    myMotor->run(FORWARD);
    myMotor->setSpeed(speed);
  }
  else {
    myMotor->run(BACKWARD);
    myMotor->setSpeed(speed);
  }
   */
