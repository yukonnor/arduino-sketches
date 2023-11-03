// ConstantSpeed.pde
// -*- mode: C++ -*-
//
// Shows how to run AccelStepper in the simplest,
// fixed speed mode with no accelerations
// Requires the Adafruit_Motorshield v2 library 
//   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
// And AccelStepper with AFMotor support 
//   https://github.com/adafruit/AccelStepper

// This tutorial is for Adafruit Motorshield v2 only!
// Will not work with v1 shields

#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 


Adafruit_StepperMotor *myStepper1 = AFMS.getStepper(4076, 1);
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(4076, 2);

// Set AccelStepper Wrappers for the Motors
void forwardstep1() {  
  myStepper1->step(64,BACKWARD, SINGLE); //run cw
}
void backwardstep1() {  
  myStepper1->step(64,FORWARD, SINGLE);
}

void forwardstep2() {  
  myStepper2->step(64,FORWARD, SINGLE);  //run cw
}
void backwardstep2() {  
  myStepper2->step(64,BACKWARD, SINGLE);
}


AccelStepper Astepper1(forwardstep1, backwardstep1); // use functions to step
AccelStepper Astepper2(forwardstep2, backwardstep2); // use functions to step

void setup()
{  
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  
}

void loop()
{  
  int max_height = 50;
  int min_height = 0;
  int mspeed = 100; //steps per second
/*
  Serial.print("LET'S GO TO POSITION 4");
  Serial.println();

  //MOVE TO POSITION 4
  Astepper1.setSpeed(mspeed); //steps per second
  Astepper2.setSpeed(mspeed); //steps per second
  for(int m = 0; m < (max_height+5); m++) {
  Serial.print("m = ");
  Serial.print(m);
  Serial.print("   and   stepper1 position = ");
  Serial.println(Astepper1.currentPosition());
  Astepper1.moveTo(max_height);  
  Astepper2.moveTo(max_height);
  Astepper1.run();   
  Astepper2.run();
  delay(20);
  }
  */
  
  Serial.println();
  Serial.print("HEY SHAKERBOY -- LET'S GO TO POSITION 3");
  Serial.println();
  
  //MOVE TO POSITION 3
  Astepper1.setSpeed(-mspeed); //steps per second
  Astepper2.setSpeed(mspeed); //steps per second
  for(int m = 0; m < (max_height+5); m++) {
  Serial.print("m = ");
  Serial.print(m);
  Serial.print("   and   stepper1 position = ");
  Serial.print(Astepper1.currentPosition());
  Serial.print("   and   stepper2 position = ");
  Serial.println(Astepper2.currentPosition());
  Astepper1.moveTo(min_height);  
  Astepper2.moveTo(max_height); 
  Astepper1.run();   
  Astepper2.run();
  //delay(5);
  }

  Serial.println();
  Serial.print("HEY NOW -- LET'S GO TO POSITION 1 --> All the way downnn");
  Serial.println();

/*
  //MOVE TO POSITION 1
  Astepper1.setSpeed(-mspeed); //steps per second
  Astepper2.setSpeed(-mspeed); //steps per second
  for(int m = 0; m < (max_height+5); m++) {
  Serial.print("m = ");
  Serial.print(m);
  Serial.print("   and   stepper1 position = ");
  Serial.print(Astepper1.currentPosition());
  Serial.print("   and   stepper2 position = ");
  Serial.println(Astepper2.currentPosition());
  Astepper1.moveTo(min_height);  
  Astepper2.moveTo(min_height);
  Astepper1.run();   
  Astepper2.run();
  delay(20);
  }

  Serial.println();
  Serial.print("SHAKEY BOY FOR POSITION 2 - WRAP'R UP!");
  Serial.println();
*/
  //MOVE TO POSITION 2
  Astepper1.setSpeed(mspeed); //steps per second
  Astepper2.setSpeed(-mspeed); //steps per second
  for(int m = 0; m < (max_height+5); m++) {
  Serial.print("m = ");
  Serial.print(m);
  Serial.print("   and   stepper1 position = ");
  Serial.print(Astepper1.currentPosition());
  Serial.print("   and   stepper2 position = ");
  Serial.println(Astepper2.currentPosition());
  Astepper1.moveTo(max_height);  
  Astepper2.moveTo(min_height); //stay at max height
  Astepper1.run();   
  Astepper2.run();
  //delay(5);
  }
  
}
