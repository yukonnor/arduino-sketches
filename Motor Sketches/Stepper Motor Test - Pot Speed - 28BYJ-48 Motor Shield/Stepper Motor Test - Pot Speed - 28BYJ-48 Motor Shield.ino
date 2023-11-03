/*
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2
---->  http://www.adafruit.com/products/1438
*/

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// The 28BYJ has 2038 steps per revolution (1.8 degree) to motor port #2 (M3 and M4)
Adafruit_StepperMotor *Stepper1 = AFMS.getStepper(2038, 1);
Adafruit_StepperMotor *Stepper2 = AFMS.getStepper(2038, 2);
long lastTime = 0;

  // The 28BYJ has a max speed of ~.25 revolutions per second --> 1 revolution every 4 seconds --> ~500 steps per second (looks like arduino can't handle that)
  // With one stepper, "step()" and serial running, Stepper running at ~8.4s / rev --> 8.4s / 2038 steps --> ~242 steps per second (even at setSpeed = 15)
  // With one stepper, "onestep()" and serial off, Stepper running at ~7.8s / rev --> 7.8s / 2038 steps --> ~261 steps per second
  // With two steppers, "onestep()" and serial off, Steppers running at ~15.4s / rev
  // With two steppers, "onestep()" and serial off and i2C clock speed updated in setup(), Steppers running at ~5.6s / rev --> 5.6s / 2038 steps -->  ~364 steps per second
  // NOTE: You can speed it up more if needed by creating a new quickstep class which bypasses the microstep logic in the library: https://forums.adafruit.com/viewtopic.php?p=292119#p292119

void setup() {
  //Serial.begin(9600);           // set up Serial library at 9600 bps
  //while (!Serial);
  //Serial.println("Stepper test!");


  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
  // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    //Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }
  //Serial.println("Motor Shield found.");

  TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to from default 1000KHz to 400KHz

  Stepper1->setSpeed(15);  // 15 rpm
  Stepper2->setSpeed(15);  // 15 rpm
}

void loop() {
  // record current time
  long currentTime = millis(); 

  // Set up the potentiometer
  int pot0Value = analogRead(A0); // 0 - 1023. 511 = 0 speed


  
  // Change the stepper speeds based on the value of the potentiometer
  // Note, this library uses direction in the onestep call. Using the following to go forward, stop, reverse based on pot position
  if (pot0Value > 530) {
    int stepdelayTime = map(pot0Value, 531, 1023, 20, 0);

    if ((currentTime - lastTime) > stepdelayTime) {
      Stepper1->onestep(FORWARD, DOUBLE);
      Stepper2->onestep(FORWARD, DOUBLE);

      lastTime = currentTime;
    }
  }
  else if (pot0Value > 490) {
    // do nothing
  }
  else {
    int stepdelayTime = map(pot0Value, 0, 490, 0, 20);

    if ((currentTime - lastTime) > stepdelayTime) {
      Stepper1->onestep(BACKWARD, DOUBLE);
      Stepper2->onestep(BACKWARD, DOUBLE);

      lastTime = currentTime;
    }
  }




  // NOTE: Using the Motor Shield librarby, the stepping commands are 'blocking' and will return once the steps have finished.
  



}