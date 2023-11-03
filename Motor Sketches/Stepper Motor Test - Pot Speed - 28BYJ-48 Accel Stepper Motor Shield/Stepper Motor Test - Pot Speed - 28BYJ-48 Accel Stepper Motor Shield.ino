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

// Now we'll wrap the 2 steppers in an AccelStepper object
AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

  // The 28BYJ has a max speed of ~.25 revolutions per second --> 1 revolution every 4 seconds --> ~500 steps per second (looks like arduino can't handle that)
  // Using setSpeed() and runSpeed() with 2 steppers, steppers running at ~16.2s / rev --> 16.2s / 2038 steps --> ~125 steps per second (even at maxSpeed = 200)
  // Using setSpeed() and runSpeed() with 2 steppers and i2c clock speed updated in setup(), Steppers running at ~5.6s / rev --> 5.6s / 2038 steps -->  ~364 steps per second

void setup()
{
  AFMStop.begin(); // Start the top shield
  TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to from default 1000KHz to 400KHz

  stepper1.setMaxSpeed(500.0);      // maximum speed in steps per second
  stepper1.setAcceleration(100.0);  // how quickly the stepper should speed up to its max speed (or decelarate to 0) in steps per second per second. !EXPENSIVE! Only use when needed.
  stepper1.moveTo(1000);            // Set the target postiion in steps. 1000 = about half turn (based on 2038 steps per revolution)

  stepper2.setMaxSpeed(500.0);      // maximum speed in steps per second
  stepper2.setAcceleration(100.0);  // how quickly the stepper should speed up to its max speed (or decelarate to 0) in steps per second per second. !EXPENSIVE! Only use when needed.
  stepper2.moveTo(1000);            // about half turn (based on 2038 steps per revolution)

  //Serial.begin(9600);
}



void loop()
{
  // Set up the potentiometer
  int pot0Value = analogRead(A0); // 0 - 1023. 511 = 0 speed
  //Serial.print("Pot Value = ");
  //Serial.println(pot0Value);
  
  // Change the stepper speeds based on the value of the potentiometer
  int speed1 = map(pot0Value, 0, 1023, -500, 500);
  int speed2 = map(pot0Value, 0, 1023, -500, 500);

  stepper1.setSpeed(speed1);
  stepper2.setSpeed(speed2);

  //stepper1.run();
  //stepper2.run();
  stepper1.runSpeed();
  stepper2.runSpeed();
}

