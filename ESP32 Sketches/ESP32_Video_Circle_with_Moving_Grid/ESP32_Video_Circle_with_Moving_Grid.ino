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
//  - draws a gradient circle in middle of screen
//  - draws a moving grid of lines behind circle
//
//
// TODO
// 

const int grid_spacing = 20;
const int num_h_lines = YRES/grid_spacing; //12 line, 20 px appart
const int num_v_lines = XRES/grid_spacing; //17
int h_line_y[num_h_lines], v_line_x[num_v_lines];

int dx = 1; //how fast the v lines move in the x-direction
int dy = 1; 

long cycle_count = 0;

//init the lines:
void initLines(){
  graphics.begin();
  
  //draw horizontal lines
  for(int i = 0; i<num_h_lines; i++){
    h_line_y[i] = (i*grid_spacing);
    graphics.drawHLine(0,XRES,h_line_y[i],54);
  }
  //draw vertical lines
  for(int i = 0; i<num_v_lines; i++){
    v_line_x[i] = (i*grid_spacing);
    graphics.drawVLine(v_line_x[i],0,YRES,54);
  }
  
  graphics.end();
}


//move and redraw lines & draw circle
void draw()
{
  graphics.begin(0);

  //move and draw vertical lines
  for(int i = 0; i<num_h_lines; i++){
    h_line_y[i] += dy; // move horizontal lines dy
    if(h_line_y[i] > YRES){h_line_y[i] = 0;}
    if(h_line_y[i] < 0){h_line_y[i] = YRES;}
    graphics.drawHLine(0,XRES,h_line_y[i],54);
  }
  //move and draw vertical lines
  for(int i = 0; i<num_v_lines; i++){
    v_line_x[i] += dx; // move vertical lines dx
    if(v_line_x[i] > XRES){v_line_x[i] = 0;}
    if(v_line_x[i] < 0){v_line_x[i] = XRES;}
    graphics.drawVLine(v_line_x[i],0,YRES,54);
  }
  

  graphics.fillCircleGrad(XRES/2,YRES/2,40,0,10,25);
  
  graphics.end();
}

void setup()
{
  fastStart();
  fillBuffer();
  //Serial.begin(115200);
  initLines();
}


void loop()
{
  //when drawTrails(0) has gone 5 cycles, start drawTrails(1)
  //each shape should draw 20 times trail_counter(i) should not go over 20 (once 20, set to 0)
  
  draw();
  delay(100);

  //go 10 cycles then randomly switch direction of dx and dy
  if((cycle_count % 10)==0){
    dx = random(-1,2)*dx;
    if(dx == 0)dx=1;
    dy = random(-1,2)*dy;
    if(dy == 0)dy=1;
  }

  cycle_count++; 
}
