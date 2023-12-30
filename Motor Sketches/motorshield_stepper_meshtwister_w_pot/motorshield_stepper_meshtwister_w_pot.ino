// Runs two Steppers at once at a slow speed back and forth to twist mesh
//
// Requires the Adafruit_Motorshield v2 library
//   https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
// And AccelStepper with AFMotor support
//   https://github.com/adafruit/AccelStepper

// This is for Adafruit Motorshield v2 only! ds


#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

#define pot1Pin A0
int pot_value = 0;
int pot_value_last = 0;

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
  Serial.begin(9600); // open the serial port at 9600 bps:
  
  AFMS.begin(); // Start the top shield
  TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to 400KHz

  // MaxSpeed: max steps per second (determined by your motor and processor) 
  // 700 worked but jumpy
  // 350 is smooth but somewhat fast!
  // 200 is good for slower video twisting
  stepper1.setMaxSpeed(200.0);   
  stepper1.setAcceleration(200.0); // steps per second to accelerate / decelerate 
  stepper1.moveTo(200);

  stepper2.setMaxSpeed(200.0);
  stepper2.setAcceleration(200.0);
  stepper2.moveTo(200);  // moves from +200 to -200 --> ~70deg
}

void loop()
{
  // Get max speed from pot position
  pot_value = analogRead(pot1Pin);

  if (abs(pot_value_last - pot_value) > 25) 
  {
    int max_speed = map(pot_value, 0, 1023, 0, 200);

    Serial.print("Pot value: ");Serial.print(pot_value);Serial.print(" Max Speed: ");Serial.println(max_speed);
    
    stepper1.setMaxSpeed(max_speed);  
    stepper2.setMaxSpeed(max_speed);

    stepper1.setAcceleration(max_speed);
    stepper2.setAcceleration(max_speed);

    pot_value_last = pot_value;   
  }
  
  // Change direction at the limits
  if (stepper1.distanceToGo() == 0)
    stepper1.moveTo(-stepper1.currentPosition());

  if (stepper2.distanceToGo() == 0)
    stepper2.moveTo(-stepper2.currentPosition());


  stepper1.run();
  stepper2.run();
}

