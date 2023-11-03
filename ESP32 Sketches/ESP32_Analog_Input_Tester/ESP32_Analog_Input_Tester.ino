#include "CompositeGraphics.h"
#include "CompositeOutput.h"

const int XRES = 320;
const int YRES = 240;

char frame_buffer[XRES][YRES];

CompositeGraphics graphics(XRES, YRES);
CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);

// images included here
#include "Image.h"
#include "luni.h"
Image<CompositeGraphics> luni0(luni::xres, luni::yres, luni::pixels);

#include <soc/rtc.h>

void compositeCore(void *data)
{
  while (true)
  {
    //just send the graphics frontbuffer whithout any interruption 
    composite.sendFrameHalfResolution(&graphics.frame);
  }
}

void fastStart(){
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
  composite.init();
  graphics.init();
  xTaskCreatePinnedToCore(compositeCore, "c", 1024, NULL, 1, NULL, 0);

  graphics.begin(0);
  graphics.end();
}

void fillBuffer(){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      //fill the buffer whith what's currently displayed on the frame
      frame_buffer[i][j] = graphics.frame[j][i];
    }
  }
}

void dumpBuffer(){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      //fill the buffer whith what's currently displayed on the frame
      graphics.backbuffer[j][i] = frame_buffer[i][j];
    }
  }
}

void clearScreen(int c = 0){
  graphics.begin(c);
  graphics.end();
}

//////////////////////////////////////////////////////////////////////////////////////////
const int potPin0 = 33;
const int potPin1 = 32;
const int potPin2 = 35;
const int potPin3 = 34;

//space between: XRES/(numpots+2)
const int numpots = 4;
int potValue[numpots];
int x[numpots];
int y[numpots];



void draw()
{
  graphics.begin(0);

  //set x values
  for(int i = 0; i<numpots; i++){
    x[i] = (XRES/(numpots+1) * i) + (XRES/(numpots+1));
  }

  //draw lines between points
  for(int i = 0; i<numpots-1; i++){
    graphics.drawLine(x[i], y[i], x[i+1], y[i+1], 54);
  }

  //draw circles at points
  for(int i = 0; i<numpots; i++){  
    graphics.fillCircle(x[i],y[i],4,30);
  }
  
  graphics.end();
}

void setup() {
  fastStart();
  Serial.begin(115200);
  Serial.println("\nAnalog Input Tester");
  analogReadResolution(9); //sets the sample bits and resolution. It can be a value between 9 (0 – 511) and 12 bits (0 – 4095). Default is 12-bit resolution.
}
 
void loop() {
  
  potValue[0] = analogRead(potPin0);
  potValue[1] = analogRead(potPin1);
  potValue[2] = analogRead(potPin2);
  potValue[3] = analogRead(potPin3);

  //map values to draw on screen (not needed for serial monitor testing)
  for(int i = 0; i<numpots; i++){  
    y[i] = map(potValue[i], 0, 511, YRES, 0);
  }
  
  Serial.print("Pot 1 = ");Serial.print(potValue1);Serial.print("  Pot 2 = ");Serial.print(potValue2);Serial.print("  Pot 3 = ");Serial.print(potValue3);Serial.print("  Pot 4 = ");Serial.println(potValue4);
  delay(500);

  draw();
}
