#include <Adafruit_NeoPixel.h>

#define PIN 6


Adafruit_NeoPixel strip = Adafruit_NeoPixel(160, 6, NEO_GRB + NEO_KHZ800);


uint8_t program = 0;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {

        strip.setBrightness(80);
       
        rainbowCycle_coat(10);
        rainbowCycle(5);
        rainbowCycle(2);
        rainbowCycle_coat_purp(10);
        rainbowCycles(10);
        
       
    
  }

//RAINBOW SPARKLES
void rainbowCycles(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    delay(15);
      //does this go in the loop? for how many?
    for (int i = 0; i < strip.numPixels(); i++) {
      fadePixel(i + random(20), 5);
    //  fadePixel(i - random(10), 30);
     // fadePixel(i + random(15), 50);
    }
    strip.show();
    delay(wait);
  }

}

void fadePixel(int pixelID, int toPercent) {
  uint32_t currColor = strip.getPixelColor(pixelID);
  strip.setPixelColor(pixelID, strip.Color(RGB_r(currColor)*toPercent/100,RGB_g(currColor)*toPercent/100,RGB_b(currColor)*toPercent/100));
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*4; j++) { // 10 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
 
}

void rainbowCycle_coat(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*10; j++) { // 10 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel_drip_green(((i * 256 / (2*strip.numPixels())) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
 
}

void rainbowCycle_coat_purp(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 10 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel_drip_purp(((i * 256 / (2*strip.numPixels())) + j) & 255));
    }
    strip.show();
    delay(wait);
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




uint32_t Wheel_drip_red(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(100 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 100 - WheelPos * 3);
  }
}


uint32_t Wheel_drip_purp(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(100 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 100 - WheelPos * 3);
  }
}

uint32_t Wheel_drip_green(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 100 - WheelPos * 3);
  }
}




uint32_t Wheel_drip_all(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 100 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(100 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 100 - WheelPos * 3);
  }
}



//GET COLORS
 
 
inline uint8_t RGB_r(uint32_t rgb) {
  return (uint8_t)(0x7f & (rgb >> 8));
}
 
inline uint8_t RGB_g(uint32_t rgb) {
  return (uint8_t)(0x7f & (rgb >> 16));
}
 
inline uint8_t RGB_b(uint32_t rgb) {
  return (uint8_t)(0x7f & (rgb));
}

