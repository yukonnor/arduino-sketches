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
// TODO
// - to fix issue of top smaller circle not trailing
//  - redraw circles and THEN draw a shifted version of the previous frame (witout erasing the circles you just redrew)
//  - or just make sure that the original circle positions aren't within another circle
// - fade out trails using add/subtract color

const int num_shapes = 6;
int xm[num_shapes], ym[num_shapes], r[num_shapes];



void drawCircles(){
  //draw circles in random locations on a clear screen
  graphics.begin(0);
  
  for(int i = 0; i<num_shapes; i++){
    xm[i]= random(60, XRES-60);
    ym[i]= random(40, YRES-40);
    r[i] = random(10,40);
    graphics.fillCircle(xm[i],ym[i],r[i],15);
    graphics.drawCircle(xm[i],ym[i],r[i],54);
  }
  
  graphics.end();
  fillBuffer();
}

void drawTrails(int dx, int dy)
{
  graphics.begin(0);
  dumpBuffer(); //redraw circles & existing trails
  graphics.scrollSreen(dx,dy,0);
  
  for(int i = 0; i<num_shapes; i++){
    graphics.fillCircle(xm[i],ym[i],r[i],15);
    graphics.drawCircle(xm[i],ym[i],r[i],54);
  }
  
  graphics.end();
  fillBuffer();
}

void setup()
{
  fastStart();
  fillBuffer();
  //Serial.begin(115200);

}


void loop()
{
  int j = 0;
  
  drawCircles();
  
  while(j < 40){
    drawTrails(1,2);
    delay(80);
    j++;
  }

}
