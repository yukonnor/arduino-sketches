#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Connect two steppers with 200 steps per revolution (1.8 degree)
// to the top shield
Adafruit_StepperMotor *myStepper1 = AFMS.getStepper(4076, 1);
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(4076, 2);


// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// wrappers for the first motor!

void forwardstep1() {  
  myStepper1->step(64, FORWARD, SINGLE);
}
void backwardstep1() {  
  myStepper1->step(64, BACKWARD, SINGLE);
}
// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->step(64, FORWARD, DOUBLE);
}
void backwardstep2() {  
  myStepper2->step(64, BACKWARD, DOUBLE);
}

/*
void forwardstep1() {  
  myStepper1->onestep(FORWARD, SINGLE);
}
void backwardstep1() {  
  myStepper1->onestep(BACKWARD, SINGLE);
}
// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {  
  myStepper2->onestep(BACKWARD, DOUBLE);
}

*/

// Now we'll wrap the 3 steppers in an AccelStepper object
AccelStepper stepper1(backwardstep1, forwardstep1); //Switching to make forward = cw
AccelStepper stepper2(forwardstep2, backwardstep2); //forward already = cw


void setup()
{  
  Serial.begin(9600);
  AFMS.begin(); // Start the bottom shield
  AFMS.begin(); // Start the top shield

  //64 steps = 1 rev (based on gear ratio entered above)
  //Max speed = 1 rev per 4 seconds

  //Max height = 120
  //Min height = 0
   
  stepper1.setMaxSpeed(16); //steps per second  
  stepper1.setAcceleration(100.0);
  stepper1.moveTo(120);
    
  stepper2.setMaxSpeed(16); //steps per second
  stepper2.setAcceleration(100.0);
  //stepper2.moveTo(0);

}

void loop()
{
  
  Serial.print("Stepper 1 Current Position = ");
  Serial.println(stepper1.currentPosition());
  Serial.print("Stepper 2 Current Position = ");
  Serial.println(stepper1.currentPosition());
  Serial.println(" ");
   
    //lift up stepper 1, once at max, start to move stepper 1 down and stepper 2 up
    if (stepper1.distanceToGo() == 0) {
       stepper1.moveTo(0);
       stepper2.moveTo(120);
       }
    
    if (stepper2.distanceToGo() == 0) {
        stepper1.moveTo(120);
        stepper2.moveTo(0);
        }


    stepper1.run();
    stepper2.run();
}


//IDEAS
/*
 * Add button to level & stop movement
 * --> Go back to state 0 (both motor1 and motor 2 at position 0 // Level)
 * 
 * Know and add Min/Max states
*/
/*
  Serial.println("Double coil steps");
  myMotor1->step(100, FORWARD, DOUBLE); 
  myMotor1->step(100, BACKWARD, DOUBLE);
  
  Serial.println("Interleave coil steps");
  myMotor1->step(100, FORWARD, INTERLEAVE); 
  myMotor1->step(100, BACKWARD, INTERLEAVE); 
  
  Serial.println("Microstep steps");
  myMotor1->step(50, FORWARD, MICROSTEP); 
  myMotor1->step(50, BACKWARD, MICROSTEP);
*/
