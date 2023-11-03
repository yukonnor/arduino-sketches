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
//  - draws a sine wave with a repeating 'glow' pattern (loops around the color values creating a repeating pattern)
//
// TODO
// - Add more fine fade levels (between 1 and 2)
// - Remove harsh line. draw from middle? add a right fade to the re-draw line? draw a circle rather than a pixel
// - Get fade to work with 2D shapes

int  x = 0;
float xrad = 0; //2pi = 1 sin wave cycle = 40 pix   2pi = xrad * 40
int  y = 0;
int cycle_length = 80; //orig 40
int amplitude = 10;     //orig 10
char cmax = 100;
int d_cmax = 4; //orig 1

void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}

//x,y is the original pixel to 'add glow to', d is direction of glow, f is fade amount of glow
void glow(int x, int y, byte d, char f){
  char c = graphics.getPixel(x,y);
  
  if(d == 1){ //up glow - sawtooth repeats -- CURRENTLY loops between 0 and 255 when only 0-54 are colors! anything > 54 = white
    for(int i = y-1; i>0; i--) {
        c = c - f;
        if(c > cmax) c = cmax;  
        graphics.drawPixel(x,i,c);
        //Serial.print("c = ");Serial.println((uint8_t)c);
    }
  }

  if(d == 2){ //down glow sawtooth overflow
    for(int i = y+1; i<YRES; i++) {
        c = graphics.getPixel(x,i-1) - f;
        if(c > cmax) c = cmax;  
        graphics.drawPixel(x,i,c);
    }
  }

  if(d == 3){ //up glow - triange overflow 
    for(int i = y-1; i>0; i--) {
        c = c - f; // confine c here? so that it HAS to be between 0 and 100?
        if(c > cmax || c < 2) f = -f;    //make sure color is at least fade amount to always keep values above 0
        c = constrain(c,0,cmax);   
        graphics.drawPixel(x,i,c);
        Serial.print("c = ");Serial.println((uint8_t)c);        
    }
  }
  
  if(d == 4){ //down glow - triange overflow 
    for(int i = y+1; i<YRES; i++) {
        c = graphics.getPixel(x,i-1) - f;
        if(c > cmax || c < 2) f = -f;
        c = constrain(c,0,cmax);  
        graphics.drawPixel(x,i,c);
        //Serial.print("c = ");Serial.println((uint8_t)c);
    }
  }
  //Serial.println();
}


void draw()
{
  graphics.begin(0);
  dumpBuffer(); //redraw circles & existing trails
  //fade(1);

  xrad = ((2*PI)/cycle_length)*x;
  y = (YRES/2)+(amplitude*sin(xrad));

  //draw sine wave
  graphics.drawPixel(x,y,54);

  //draw inverse sine wave
  //graphics.drawPixel(x,(2*(-1*y))+YRES+(YRES/2),54); //have to add YRES as orgin is at middle of screen, not at 0. Multiplyed by 2 for height

  //glow(x,y,1,4); // sawtooth overflow glow
  //glow(x,y,2,4); // sawtooth overflow glow
  glow(x,y,3,4); //
  glow(x,y,4,4);
  
  graphics.end();
  fillBuffer();
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

  //change cmax by movement amount d_cmax
  if(x%10 == 0){
    cmax = cmax + d_cmax;
    if(cmax >=200 || cmax<55) d_cmax= -d_cmax; //loop cmax between 54 and 200
  }
  
  
  Serial.print("x = ");Serial.print(x);Serial.print("  d_cmax = ");Serial.print(d_cmax);Serial.print("  cmax = ");Serial.println((uint8_t)cmax);
  
  
 
}
