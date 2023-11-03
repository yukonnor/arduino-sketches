// Shows how to run three Steppers at once with varying speeds
//
// Requires the Adafruit_Motorshield v2 library
//   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
// And AccelStepper with AFMotor support
//   https://github.com/adafruit/AccelStepper

// This tutorial is for Adafruit Motorshield v2 only! ds
// Will not work with v1 shields

#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS(0x60); // Default address, no jumpers

// Connect two steppers to the shield defined above.
// The 28BYJ-48 Stepper Motor has 32 steps per revolution but it uses a gearbox which leads to 2038 steps per shaft revolution
Adafruit_StepperMotor *myStepper1 = AFMS.getStepper(2038, 1);  // (steps_per_rev, stepper_num)
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(2038, 2);


// you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
// SINGLE: single-coil activation
// DOUBLE: 2 coils are activated at once (for higher torque)
// INTERLEAVE: alternates between single and double to get twice the resolution (but of course its half the speed).
// MICROSTEP: coils are PWM'd to create smooth motion between steps.


// wrappers for the first motor!
void forwardstep1() {
  myStepper1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {
  myStepper1->onestep(BACKWARD, DOUBLE);
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
  AFMS.begin(); // Start the top shield
  TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to 400KHz

  stepper1.setMaxSpeed(350.0);   // max steps per second (determined by your motor and processor) // 700 worked but jumpy
  stepper1.setAcceleration(350.0); // steps per second to accelerate / decelerate 
  stepper1.moveTo(1000);

  stepper2.setMaxSpeed(700.0);
  stepper2.setAcceleration(350.0);
  stepper2.moveTo(500);
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

