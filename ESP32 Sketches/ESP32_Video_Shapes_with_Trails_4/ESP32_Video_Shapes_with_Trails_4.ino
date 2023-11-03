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
// ITDO
//  - draws gradient rectactanges with fade out trailsx
//
//
// TODO
// - 

const int num_shapes = 6;
int xm[num_shapes], ym[num_shapes], r[num_shapes], trail_counter[num_shapes];
char fade_amount = 1;
char color = 54;
char result;

int dx[num_shapes];

void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}

void initCircles(int shape_num){
  //initialize starting points for circle i
  xm[shape_num]= random(120, XRES-120);
  ym[shape_num]= random(90, YRES-90);
  r[shape_num] = random(10,40);
}

void drawTrails(int shape_num, int dx, int dy)
{
  graphics.begin(0);
  dumpBuffer(); //redraw circles & existing trails
  fade(1);

  xm[shape_num] += dx;
  ym[shape_num] += dy;
  //graphics.fillCircle(xm[shape_num],ym[shape_num],r[0],15); 
  //graphics.drawCircle(xm[shape_num],ym[shape_num],r[0],54);

  graphics.fillRectGrad(xm[shape_num],ym[shape_num],xm[shape_num]+r[shape_num],ym[shape_num]+r[shape_num]);
  graphics.drawRect(xm[shape_num],ym[shape_num],xm[shape_num]+r[shape_num],ym[shape_num]+r[shape_num],54);

  
  graphics.end();
  fillBuffer();
}

void setup()
{
  fastStart();
  fillBuffer();
  //Serial.begin(115200);

  initCircles(0);
  initCircles(1);
  
  trail_counter[0] = 0;
  trail_counter[1] = 0;

  dx[0] = 1;
  dx[1] = -1;
  
}


void loop()
{
  //when drawTrails(0) has gone 5 cycles, start drawTrails(1)
  //each shape should draw 20 times trail_counter(i) should not go over 20 (once 20, set to 0)

  drawTrails(0, dx[0], 3);
  trail_counter[0]++;

  if(trail_counter[0]>5 || trail_counter[1] > 0){
    drawTrails(1, dx[1], 4);
    trail_counter[1]++;
  }
  

  if(trail_counter[0]==10)dx[0] = -dx[0];
  if(trail_counter[1]==15)dx[1] = -dx[1];

  if(trail_counter[0]>20){
    initCircles(0);
    trail_counter[0]=0;
  }

  if(trail_counter[1]>20){
    initCircles(1);
    trail_counter[1]=0;
  }

 
}
