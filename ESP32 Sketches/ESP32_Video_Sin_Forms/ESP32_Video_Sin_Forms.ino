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
// - modulate the "confining box" of the blob

int  x = 0;
float xrad = 0; //2pi = 1 sin wave cycle = 40 pix   2pi = xrad * 40
int  y = 0;

void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}

// Draw a polygon blob
// has a center point with 9 points around it (4 at each axis, 2 inbetween each axis)
// connect lines between neighboring points
// point 0 = (xo, random y)
// point 1 = (between x0+5 and 
int xo = XRES/2;
int yo = YRES/4; 
int polypoint_x[8], polypoint_y[8];
int dx[8], dy[8];
const int outer_container_xmin = 90;
const int outer_container_xmax = XRES-90;
const int outer_container_ymin = 20;
const int outer_container_ymax = ((YRES/2)-20);
const int inner_container_xmin = outer_container_xmin + 30;
const int inner_container_xmax = outer_container_xmax - 30;
const int inner_container_ymin = outer_container_ymin + 20;
const int inner_container_ymax = outer_container_ymax - 20;

void initPolyBlob(){
  for(int i = 0; i<12; i++)dx[i] = 1; 
  for(int i = 0; i<12; i++)dy[i] = 1;
  
  polypoint_x[0] = xo;                  polypoint_y[0] = outer_container_ymin; 
  polypoint_x[2] = outer_container_xmax;           polypoint_y[2] = yo;
  polypoint_x[4] = xo;                  polypoint_y[4] = outer_container_ymax;
  polypoint_x[6] = outer_container_xmin;           polypoint_y[6] = yo;
  
  polypoint_x[1] = random(inner_container_xmax,polypoint_x[2]);  polypoint_y[1] = random(polypoint_y[0],inner_container_ymin);
  polypoint_x[3] = random(inner_container_xmax,polypoint_x[2]);  polypoint_y[3] = random(inner_container_ymax,polypoint_y[4]); 
  polypoint_x[5] = random(polypoint_x[6],outer_container_xmin);  polypoint_y[5] = random(inner_container_ymax,polypoint_y[4]); 
  polypoint_x[7] = random(polypoint_x[6],outer_container_xmin);  polypoint_y[7] = random(polypoint_y[0],inner_container_ymin);
  
  for(int i = 0; i < 8; i++){
    if(i == 7){
      graphics.drawLine(polypoint_x[i],polypoint_y[i],polypoint_x[0],polypoint_y[0],54);
    }
    else{
      graphics.drawLine(polypoint_x[i],polypoint_y[i],polypoint_x[i+1],polypoint_y[i+1],54);
    }
  }
  
  for(int i = 0; i<8; i++){
    if(i == 0 || i== 4){
      dx[i] = 0;
    }
    else if(random(0,2) == 1){
      dx[i] = 1;
    }
    else dx[i] = -1;
  }

  for(int i = 0; i<8; i++){
    if(i == 2 || i== 6){
      dy[i] = 0;
    }
    else if(random(0,2) == 1){
      dy[i] = 1;
    }
    else dy[i] = -1;
  }

}

void drawPolyBlob(){
  //move points
  for(int i = 0; i < 8; i++){
    polypoint_x[i] = polypoint_x[i]+dx[i]; polypoint_y[i] = polypoint_y[i]+dy[i]; 
  }

  //draw
  for(int i = 0; i < 8; i++){
    if(i == 7){
      graphics.drawLine(polypoint_x[i],polypoint_y[i],polypoint_x[0],polypoint_y[0],54);
    }
    else{
      graphics.drawLine(polypoint_x[i],polypoint_y[i],polypoint_x[i+1],polypoint_y[i+1],54);
    }
  }  
}

void draw()
{
  graphics.begin(0);
  dumpBuffer(); //redraw circles & existing trails
  fade(2);

  int cycle_length = 40;
  xrad = ((2*PI)/cycle_length)*x;
  y = (YRES/2)+(10*sin(xrad));

  //draw sine wave
  graphics.drawPixel(x,y,54);

  //draw inverse sine wave
  graphics.drawPixel(x,(2*(-1*y))+YRES+(YRES/2),54); //have to add YRES as orgin is at middle of screen, not at 0. Multiplyed by 2 for height

  //draw dots at end of cycle
  if(x%cycle_length == 0)graphics.fillCircle(x,(YRES/2),4,20);

  //draw polygon blob
  drawPolyBlob();
  
  //to see the blob's boundaries
  //graphics.drawRect(outer_container_xmin, outer_container_ymin, outer_container_xmax, outer_container_ymax, 25);
  //graphics.drawRect(inner_container_xmin, inner_container_ymin, inner_container_xmax, inner_container_ymax, 25);
  
  graphics.end();
  fillBuffer();
}

void setup()
{
  fastStart();
  fillBuffer();
  Serial.begin(115200);
  
  initPolyBlob();
}


void loop()
{
  draw();
  x++;

  if(x > XRES) x=0;
  
  for(int i = 0; i<8; i++){  
    if(i == 0 || i == 4){
      dx[i] = 0; //no change....don't move the x location of top and bottom points
    }
    else if(i == 1 || i == 3){
      if(polypoint_x[i]>polypoint_x[2] || polypoint_x[i]<inner_container_xmax) dx[i]=dx[i]*-1; 
    }
    else if(i == 5 || i == 7){
      if(polypoint_x[i]>inner_container_xmin || polypoint_x[i]<polypoint_x[6]) dx[i]=dx[i]*-1;
    }
    else if(i == 2){
      if(polypoint_x[i]>outer_container_xmax || polypoint_x[i]<max(polypoint_x[1],polypoint_x[3]) || polypoint_x[i]<inner_container_xmax) dx[i]=dx[i]*-1; 
    }
    else{ //6
      if(polypoint_x[i]>min(polypoint_x[5],polypoint_x[7]) || polypoint_x[i]<outer_container_xmin || polypoint_x[i]>inner_container_xmin) dx[i]=dx[i]*-1; 
    }
  }
  
  for(int i = 0; i<8; i++){  
    if(i == 2 || i == 6){
      dy[i] = 0; //no change....don't move the y location of left and right points
    }
    else if(i == 1 || i == 7){
      if(polypoint_y[i]<polypoint_y[0] || polypoint_y[i]>inner_container_ymin) dy[i]=dy[i]*-1; 
    }
    else if(i == 3 || i == 5){
      if(polypoint_y[i]<inner_container_ymax || polypoint_y[i]>polypoint_y[4]) dy[i]=dy[i]*-1;
    }
    else if(i == 0){
      if(polypoint_y[i]<outer_container_ymin || polypoint_y[i]>min(polypoint_y[1],polypoint_y[7]) || polypoint_y[i]>>inner_container_ymin) dy[i]=dy[i]*-1; 
    }
    else{ //4
      if(polypoint_y[i]>outer_container_ymax || polypoint_y[i]<max(polypoint_y[3],polypoint_y[5]) ||polypoint_y[i]<inner_container_ymax) dy[i]=dy[i]*-1; 
    }
  }

  /*
  for(int i = 0; i < 8; i++){
    Serial.print("dx[");Serial.print(i);Serial.print("] = ");Serial.println(dx[i]);
  }

  for(int i = 0; i < 8; i++){
    Serial.print("dy[");Serial.print(i);Serial.print("] = ");Serial.println(dy[i]);
  }
  */


 
}
