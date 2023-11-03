#include <Adafruit_NeoPixel.h>

#define PIN 6
#define BUTTON_PIN        9  // Button
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(90, 6, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// COLORS
uint32_t pixelOff = strip.Color(0,0,0);
uint32_t sunshine = strip.Color(127,50,3);
uint32_t zombieGreen = strip.Color(50,127,3);
uint32_t red = strip.Color(127,0,0);
uint32_t orange = strip.Color(127,127,0);
uint32_t green = strip.Color(0,127,0);
uint32_t teal = strip.Color(0,127,127);
uint32_t blue = strip.Color(0,0,127);
uint32_t violet = strip.Color(127,0,127);
uint32_t white = strip.Color(127,127,127);
uint32_t dimWhite = strip.Color(70,70,70);
 
uint32_t colors[] = {sunshine,zombieGreen,blue,red,teal,violet,green,orange};


uint8_t program = 0;
boolean time_to_switch = false; //Setting up switch variable

void setup() {
  strip.begin();
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH); // pull-up
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    boolean button_pressed = handle_button();
  if (time_to_switch == true) {
    program++;
    time_to_switch = false;
    if (program == 10) {
      program = 0;
    }
  }
  


  switch(program) {
    case 1:
         strip.setBrightness(70);
        
        chase(sunshine); // Red
        chase(sunshine); // Green
        chase(sunshine); // Blue
        
        chaseI(0x5806D4); // Red
        chaseI(0x00FF00); // Green
        chaseI(0x0000FF); // Blue
        
        chaseII(0x5806D4); // Red
        chaseII(0x00FF00); // Green
        chaseII(0x0000FF); // Blue
        
        chaseIII(0x0000FF); // Blue
        chaseIII(0x00FF00); // Green
        chaseIII(0x0000FF); // Blue
        chaseIII(0x00FF00); // Green
        chaseIII(0x0000FF); // Blue
        chaseIII(0x00FF00); // Green
      
       
        rainbowCycle(20);
        rainbowCycle(10);
        rainbowCycle(2);
      break;
    case 2:
     rainbowCycle(0);
      break;
  }
}


boolean handle_button()
{
  int button_pressed = !digitalRead(BUTTON_PIN); // pin low -> pressed
  if (button_pressed == true) {
    time_to_switch = true;
    delay(100);
  }
  return button_pressed;
}


//CHASING STUFF 
static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}

static void chaseI(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+24; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-8, 0); // Erase pixel a few steps back
      strip.setPixelColor(i-16  , c); // Draw new pixel back
      strip.setPixelColor(i-24, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}

static void chaseII(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+24; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-8, 0); // Erase pixel a few steps back
      strip.setPixelColor(i-16  , c); // Draw new pixel back
      strip.setPixelColor(i-24, 0); // Erase pixel a few steps back
      strip.setPixelColor(i-16  , c); // Draw new pixel back
      strip.setPixelColor(i-24, 0); // Erase pixel a few steps back
      strip.setPixelColor(i-16  , c); // Draw new pixel back
      strip.setPixelColor(i-24, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}

static void chaseIII(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(25);
  }
}



// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 10 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

















//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}



