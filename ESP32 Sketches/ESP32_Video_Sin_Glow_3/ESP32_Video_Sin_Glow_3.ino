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
//  - draws multiple sine waves with a regular 'glow' pattern (no looping)
//  - "basket weaves" horizontal and vertical sin waves together
//
// TODO
// - "SUM" the h and v waves together rather than drawing over

int  x = 0;
float xrad = 0; //2pi = 1 sin wave cycle = 40 pix   2pi = xrad * 40
int  y = 0;
float yrad = 0;
int cycle_length = 80; //orig 40
int amplitude = 10;     //orig 10

const int sin_waves = 13;
const int center_sin = 7;
int sin_wave[sin_waves]; //sin_wave[5] is the middle sin
int sin_wave_y[sin_waves];
int y_space = 25;
int dy = 1;
int x_space = 25;
int dx = 1;



void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}

//x,y is the original pixel to 'add glow to', d is direction of glow, f is fade amount of glow
void glow(int x, int y, byte d, int f){
  char c = graphics.getPixel(x,y);
  int min_g = y - c/f; //used to control for loop ..amount of loops to get to c = 0
  int max_g = y + c/f;
  
  if(d == 1){ //up glow - no repeats -- CURRENTLY loops between 0 and 255 when only 0-54 are colors! anything > 54 = white
    for(int i = y-1; i>min_g; i--) {
        c = c - f;
        if(c < f || c > 54) i = min_g; //end loop  
        graphics.drawPixel(x,i,c);
        //Serial.print("c = ");Serial.println((uint8_t)c);
    }
  }

  if(d == 2){ //down glow sawtooth overflow
    for(int i = y+1; i<max_g; i++) {
        c = graphics.getPixel(x,i-1) - f;
        if(c < f || c > 54) i = max_g; //end loop
        graphics.drawPixel(x,i,c);
    }
  }

  //Serial.println();
}

void draw_glow_sin(int sin_num){  
  //draw sine waves accross entire screen

  if(sin_num < 5){
    for(int i = 0; i < XRES; i++){
      xrad = ((2*PI)/cycle_length)*i;
      y = (YRES/2)+(amplitude*sin(xrad)) - (y_space * (center_sin - sin_num));
      graphics.drawPixel(i,y,54);
      
      glow(i,y,1,4);
      glow(i,y,2,4);
    }
  }

  if(sin_num >= 5){
    for(int i = 0; i < XRES; i++){
      xrad = ((2*PI)/cycle_length)*i;
      y = (YRES/2)+(amplitude*sin(xrad)) + (y_space * (sin_num - center_sin));
      graphics.drawPixel(i,y,54);
      
      glow(i,y,1,4);
      glow(i,y,2,4);  
    }
  //Serial.print("hello");
  }
}

void draw_v_glow_sin(int sin_num){  
  //draw sine waves accross entire screen

  if(sin_num < 5){
    for(int i = 0; i < YRES; i++){ //i is y
      yrad = ((2*PI)/cycle_length)*i;
      x = (XRES/2)+(amplitude*sin(yrad)) - (x_space * (center_sin - sin_num));
      graphics.drawPixel(x,i,54);
      
      glow(x,i,1,4);
      glow(x,i,2,4);
    }
  }

  if(sin_num >= 5){
    for(int i = 0; i < YRES; i++){
      yrad = ((2*PI)/cycle_length)*i;
      x = (XRES/2)+(amplitude*sin(yrad)) + (x_space * (sin_num - center_sin));
      graphics.drawPixel(x,i,54);
      
      glow(x,i,1,4);
      glow(x,i,2,4);  
    }
  //Serial.print("hello");
  }
}


void draw()
{
  graphics.begin(0);
  //dumpBuffer(); //redraw circles & existing trails
  //fade(1);

  for(int i = 0; i<sin_waves; i++){
    draw_glow_sin(i);
    draw_v_glow_sin(i);
  }
  
  graphics.end();
  //fillBuffer();
}

void setup()
{
  fastStart();
  fillBuffer();
  Serial.begin(115200);
  
}


void loop()
{
  //int d_cmax = 1; reseting every cycle with 1 was cool
  draw();
  x++;

  if(x > XRES) x=0;

  y_space = y_space + dy;
  if(y_space > 50 || y_space<20) dy= -dy; 
  
  x_space = x_space + dx;
  if(x_space > 50 || x_space<20) dx= -dx; 

  //Serial.print("dy = ");Serial.print(dy);Serial.print("  dx = ");Serial.println(dx);

}
