#include <MIDI.h>
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
//ESP32 RX Pin RX2 Used:
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

byte loop_counter = 0;

int  x = 0;
float xrad = 0; //2pi = 1 sin wave cycle = 40 pix   2pi = xrad * 40
int  y = 0;
float yrad = 0;
int cycle_length = 80;   //orig 40  MODULATE
int amplitude = 10;      //orig 10  MODULATE

int glow_amount = 4;   // MODULATE
int color_sum_amount = 54; //MODULATE

const int sin_waves = 13;
const int center_sin = 7;
int sin_wave_x[sin_waves]; //the x position of the veritcal sine waves
int sin_wave_y[sin_waves]; // the y position of the horizontal sine waves
int y_space = 35;
int x_space = 35;
int dy_direction = 1;
int dy_value = 1;
int dx_direction = 1;
int dx_value = 1;

const int numfaders = 16;
int faderValue[numfaders];


// declare midi CC numbers for the 16n faders
const byte fader1 = 32;
const byte fader2 = 33;
const byte fader3 = 34;
const byte fader4 = 35;
const byte fader5 = 36;
const byte fader6 = 37;
const byte fader7 = 38;
const byte fader8 = 39;
const byte fader9 = 40;
const byte fader10 = 41;
const byte fader11 = 42;
const byte fader12 = 43;
const byte fader13 = 44;
const byte fader14 = 45;
const byte fader15 = 46;
const byte fader16 = 47;

// callback function when a control change message is received
void OnControlChange(byte channel, byte number, byte value)
{
  if(number > 119 || value > 127) return; //exit function if received bad data
  
  //Serial.println(String("Control Change, ch=") + channel + ", control #= " + number + ", value= " + value);  // <-- caused a bunch of latency
  //better:
  Serial.print(number);Serial.print(" ");Serial.print(value);  
  Serial.println();

  switch(number){
    case fader1:
      glow_amount = map(value, 0, 127, 12, 2); 
      break;
    case fader2:
      color_sum_amount = map(value, 0, 127, 0, 54); 
      break;
    case fader3:
      faderValue[2] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader4:
      faderValue[3] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader5:
      faderValue[4] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader6:
      faderValue[5] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader7:
      faderValue[6] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader8:
      faderValue[7] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader9:
      faderValue[8] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader10:
      faderValue[9] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader11:
      faderValue[10] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader12:
      faderValue[11] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader13:
      dx_value = map(value, 0, 127, 0, 4); // map speed at which h lines move appart/together
      break;
    case fader14:
      dy_value = map(value, 0, 127, 0, 4); // map dy speed
      break;
    case fader15:
      cycle_length = map(value, 0, 127, 10, 160);  // map cycle_length
      break;
    case fader16:
      amplitude = map(value, 0, 127, 1, 30);       // map amplitude
      break;
  }
  
  
}

//x,y is the original pixel to 'add glow to', d is direction of glow, f is fade amount of glow
// Higher fade amount, less glow
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

void draw_h_line_sin(int sin_num){  
  //draw sine waves accross entire screen

  // if sin wave is above center sin wave, move up
  if(sin_num < 5){
    for(int i = 0; i < XRES; i++){
      xrad = ((2*PI)/cycle_length)*i;
      y = (YRES/2)+(amplitude*sin(xrad)) - (y_space * (center_sin - sin_num)); // the y position of the horizontal line
      graphics.drawPixel(i,y,54);
      
      glow(i,y,1,glow_amount); //glow_amount_h was a constant "4" before
      glow(i,y,2,glow_amount);
    }
  }
  
  // if sin wave is center sin wave, don't move. If below center sin wave, move down
  if(sin_num >= 5){
    for(int i = 0; i < XRES; i++){
      xrad = ((2*PI)/cycle_length)*i;
      y = (YRES/2)+(amplitude*sin(xrad)) + (y_space * (sin_num - center_sin));
      graphics.drawPixel(i,y,54);
      
      glow(i,y,1,glow_amount); //glow_amount_h was a constant "4" before
      glow(i,y,2,glow_amount);  
    }
  //Serial.print("hello");
  }
}

void draw_v_line_sin(int sin_num){  
  //draw sine waves accross entire screen

  if(sin_num < 5){
    for(int i = 0; i < YRES; i++){ //i is y
      yrad = ((2*PI)/cycle_length)*i;
      x = (XRES/2)+(amplitude*sin(yrad)) - (x_space * (center_sin - sin_num));
      //graphics.drawPixel(x,i,54);
      graphics.pixelAdd(x,i,color_sum_amount);
      
      glow(x,i,3,glow_amount);
      glow(x,i,4,glow_amount);
    }
  }

  if(sin_num >= 5){
    for(int i = 0; i < YRES; i++){
      yrad = ((2*PI)/cycle_length)*i;
      x = (XRES/2)+(amplitude*sin(yrad)) + (x_space * (sin_num - center_sin));
      //graphics.drawPixel(x,i,54);
      graphics.pixelAdd(x,i,color_sum_amount); 
      
      glow(x,i,3,glow_amount);
      glow(x,i,4,glow_amount);  
    }
  //Serial.print("hello");
  }
}


void draw()
{
  graphics.begin(0);
  //dumpBuffer(); //redraw circles & existing trails
  //fade(1);

  for(int i = 0; i<sin_waves; i++) draw_h_line_sin(i);
  for(int i = 0; i<sin_waves; i++) draw_v_line_sin(i); 
  
  graphics.end();
  //fillBuffer();
}

void setup()
{
  fastStart();
  MIDI.begin(MIDI_CHANNEL_OMNI);   //Listens for messages on all midi channels. Adjust if necessary.
  MIDI.setHandleControlChange(OnControlChange);
  Serial.begin(115200);
  //analogReadResolution(9);  //sets the sample bits and resolution. (0-511) 
  
}


void loop()
{
  //read midi cc messages from 16n
  MIDI.read();

  // wait 10 loops to let midi values 'catch up'
  if(loop_counter >= 50){
    //int d_cmax = 1; reseting every cycle with 1 was cool
    draw();
    x++;
    if(x > XRES) x=0;

    //loops between expanding and contracting between set min/max space valudes
    y_space = y_space + (dy_direction*dy_value);
    if(y_space > 50 || y_space<30) dy_direction=-dy_direction; 
  
    x_space = x_space + (dx_direction*dx_value);
    if(x_space > 50 || x_space<30) dx_direction=-dx_direction; 
    
    loop_counter = 0;
  }

  //Serial.print("dy = ");Serial.print(dy);Serial.print("  dx = ");Serial.println(dx);
  
  loop_counter++;
}
