// Shows how to run three Steppers at once with varying speeds
//
// Requires the Adafruit_Motorshield v2 library
//   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
// And AccelStepper with AFMotor support
//   https://github.com/adafruit/AccelStepper

// This tutorial is for Adafruit Motorshield v2 only!
// Will not work with v1 shields

#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMStop(0x60); // Default address, no jumpers

// The SRBYJ-48 has 2038 steps per revolution
Adafruit_StepperMotor *myStepper1 = AFMStop.getStepper(2038, 1);
Adafruit_StepperMotor *myStepper2 = AFMStop.getStepper(2038, 2);


// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// DOUBLE: 2 coils are activated at once (for higher torque)
// INTERLEAVE: alternates between single and double to get twice the resolution (but at half the speed).
// MICROSTEP: coils are PWM'd to create smooth motion between steps.

// wrappers for the first motor!
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

// Now we'll wrap the 3 steppers in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

void setup()
{
  AFMStop.begin(); // Start the top shield

  stepper1.setMaxSpeed(100.0);      // maximum speed in steps per second
  stepper1.setAcceleration(100.0);  // how quickly the stepper should speed up to its max speed (or decelarate to 0) in steps per second per second. !EXPENSIVE! Only use when needed.
  stepper1.moveTo(1000);            // Set the target postiion in steps. 1000 = about half turn (based on 2038 steps per revolution)

  stepper2.setMaxSpeed(200.0);      // maximum speed in steps per second
  stepper2.setAcceleration(100.0);  // how quickly the stepper should speed up to its max speed (or decelarate to 0) in steps per second per second. !EXPENSIVE! Only use when needed.
  stepper2.moveTo(1000);            // about half turn (based on 2038 steps per revolution)

  //Serial.begin(9600);
}

void loop()
{
  // Change direction at the limits
  if (stepper1.distanceToGo() == 0)
	  stepper1.moveTo(-stepper1.currentPosition());

  if (stepper2.distanceToGo() == 0)
	  stepper2.moveTo(-stepper2.currentPosition());

  stepper1.run();
  stepper2.run();
}

