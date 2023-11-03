/*
NOTE! This was not a successful test. Couldn't get both motors and LEDs to use the same board.
It may be possible with some more research.
*/

#include <Adafruit_NeoPixel.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>


// STEPPER MOTOR STUFF: 
Adafruit_MotorShield AFMS(0x60); // Default address, no jumpers
Adafruit_StepperMotor *myStepper1 = AFMS.getStepper(2038, 1);  // (steps_per_rev, stepper_num)
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(2038, 2);

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

AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);


// NEOPIXEL STUFF: 
#define LED_PIN    6
#define LED_COUNT 120

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // (Total number of pixels, Arduino pin number, Pixel type flags)


// Some functions of our own for creating animated effects -----------------
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}


//////////////////////////////////////////////////////////

void setup() {
  AFMS.begin(); // Start the top shield
  TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to 400KHz

  stepper1.setMaxSpeed(350.0);   // max steps per second (determined by your motor and processor) // 700 worked but jumpy
  stepper1.setAcceleration(350.0); // steps per second to accelerate / decelerate 
  stepper1.moveTo(1000);

  stepper2.setMaxSpeed(700.0);
  stepper2.setAcceleration(350.0);
  stepper2.moveTo(500);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels (turns off as no pixel colors have been defined yet)
  strip.setBrightness(5);  // Set BRIGHTNESS (max = 255)
}

void loop() {
  // stepper motor stuff
  // Change direction at the limits
  if (stepper1.distanceToGo() == 0)
	  stepper1.moveTo(-stepper1.currentPosition());

  if (stepper2.distanceToGo() == 0)
	  stepper2.moveTo(-stepper2.currentPosition());


  stepper1.run();
  stepper2.run();

  
  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}


