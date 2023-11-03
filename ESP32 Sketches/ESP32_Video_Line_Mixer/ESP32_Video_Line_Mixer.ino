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
//  - draws multiple sine waves with a regular 'glow' pattern 
//  - animates the distance between the waves
//  - sums the colors of the waves together, creating an insane organic pattern
//
// TODO
// - "SUM" the h and v waves together rather than drawing over

int  x = 0;
float xrad = 0; //2pi = 1 sin wave cycle = 40 pix   2pi = xrad * 40
int  y = 0;
float yrad = 0;
int cycle_length = 80; //orig 40
int amplitude = 10;     //orig 10  (20 peak to peak)

const int num_waves = 13;
const int center_wave = 7;

int y_space = 35;
int dy = 1;
int x_space = 35;
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
  int min_yg = y - c/f; //used to control for loop ..amount of loops to get to c = 0
  int max_yg = y + c/f;
  int min_xg = x - c/f; 
  int max_xg = x + c/f;
  
  if(d == 1){ //up glow - no repeats 
    for(int i = y-1; i>min_yg; i--) {
        c = c - f;
        if(c < f || c > 54) i = min_yg; //end loop  
        //graphics.drawPixel(x,i,c);
        graphics.pixelAdd(x,i,c);
        //Serial.print("c = ");Serial.println((uint8_t)c);
    }
  }

  if(d == 2){ //down glow - no repeats 
    for(int i = y+1; i<max_yg; i++) {
        c = c - f;
        if(c < f || c > 54) i = max_yg; //end loop
        //graphics.drawPixel(x,i,c);
        graphics.pixelAdd(x,i,c);
    }
  }

  if(d == 3){ //lef glow - no repeats 
    for(int i = x-1; i>min_xg; i--) {
        c = c - f;
        if(c < f || c > 54) i = min_xg; //end loop  
        //graphics.drawPixel(i,y,c);
        graphics.pixelAdd(i,y,c);
        //Serial.print("c = ");Serial.println((uint8_t)c);
    }
  }

  if(d == 4){ //right glow - no repeats 
    for(int i = x+1; i<max_xg; i++) {
        c = c - f;
        if(c < f || c > 54) i = max_xg; //end loop
        //graphics.drawPixel(i,y,c);
        graphics.pixelAdd(i,y,c);
    }
  }

  //Serial.println();
}

void draw_h_wave(int wave_num){  
  //draw triangle waves accross entire screen
  //amplitude = 10 (above and below y0)
  //cycle length = 80 (which contains 4 sections)
  float y0 = 0;
  float dy = (float)amplitude/((float)cycle_length/4); //.5 

  if(wave_num < 5){
    for(int i = 0; i < XRES; i++){ // i = x position
      y0 = (y0+dy);
      y = y0 + (YRES/2) - (y_space * (center_wave - wave_num));
      
      graphics.drawPixel(i,y,54);

      if(y0 >= amplitude || y <= -amplitude) dy = -dy;
      
      glow(i,y,1,4);
      glow(i,y,2,4);
    }
  }

  if(wave_num >= 5){
    for(int i = 0; i < XRES; i++){
      y0 = (y0+dy);
      y = y0 + (YRES/2) + (y_space * (wave_num - center_wave));
      
      graphics.drawPixel(i,y,54);

      if(y0 >= amplitude || y0 <= -amplitude) dy = -dy;
      
      glow(i,y,1,4);
      glow(i,y,2,4);  

      //Serial.print("x = ");Serial.print(i);Serial.print("  y = ");Serial.print(y);Serial.print("  y0 = ");Serial.print(y0);Serial.print("  dy = ");Serial.println(dy);
    }
  //Serial.print("hello");
  }
}

void draw_h_glow_sin(int wave_num){  
  //draw sine waves accross entire screen

  if(wave_num < 5){
    for(int i = 0; i < XRES; i++){
      xrad = ((2*PI)/cycle_length)*i;
      y = (YRES/2)+(amplitude*sin(xrad)) - (y_space * (center_wave - wave_num));
      graphics.drawPixel(i,y,54);
      
      glow(i,y,1,4);
      glow(i,y,2,4);
    }
  }

  if(wave_num >= 5){
    for(int i = 0; i < XRES; i++){
      xrad = ((2*PI)/cycle_length)*i;
      y = (YRES/2)+(amplitude*sin(xrad)) + (y_space * (wave_num - center_wave));
      graphics.drawPixel(i,y,54);
      
      glow(i,y,1,4);
      glow(i,y,2,4);  
    }
  //Serial.print("hello");
  }
}

void draw_v_glow_sin(int wave_num){  
  //draw sine waves accross entire screen

  if(wave_num < 5){
    for(int i = 0; i < YRES; i++){ //i is y
      yrad = ((2*PI)/cycle_length)*i;
      x = (XRES/2)+(amplitude*sin(yrad)) - (x_space * (center_wave - wave_num));
      //graphics.drawPixel(x,i,54);
      graphics.pixelAdd(x,i,54);
      
      glow(x,i,3,4);
      glow(x,i,4,4);
    }
  }

  if(wave_num >= 5){
    for(int i = 0; i < YRES; i++){
      yrad = ((2*PI)/cycle_length)*i;
      x = (XRES/2)+(amplitude*sin(yrad)) + (x_space * (wave_num - center_wave));
      //graphics.drawPixel(x,i,54);
      graphics.pixelAdd(x,i,54);
      
      glow(x,i,3,4);
      glow(x,i,4,4);  
    }
  //Serial.print("hello");
  }
}


void draw()
{
  graphics.begin(0);
  //dumpBuffer(); //redraw circles & existing trails
  //fade(1);

  
  for(int i = 0; i<num_waves; i++){
    draw_h_wave(i);
    //draw_v_wave(i); 
    //draw_h_glow_sin(i);
    draw_v_glow_sin(i); 
  }
  

  //draw_h_wave(center_wave);
  
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
  if(y_space > 50 || y_space<30) dy= -dy; 
  
  x_space = x_space + dx;
  if(x_space > 50 || x_space<30) dx= -dx; 

  //Serial.print("dy = ");Serial.print(dy);Serial.print("  dx = ");Serial.println(dx);

}
