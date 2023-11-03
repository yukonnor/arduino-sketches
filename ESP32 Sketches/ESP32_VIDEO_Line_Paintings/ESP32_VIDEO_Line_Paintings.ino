//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THIS SKETCH: 
//  
//   TOD0:
//   - explore 'fading out' old line groups
//          
//
// COMPOSITE: - Connect compiste RCA cable to DAC pin 25 and GND

#include <MIDI.h>
#include "CompositeGraphics.h"
#include "CompositeOutput.h"

const int XRES = 320;
const int YRES = 240;
const int white = 54;
const int black = 0;

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
int draw_count = 0;
float slope = 0;

int x0_line = 0;
int x1_line = 0;
int y0_line = 0;
int y1_line = 0;

//space between: XRES/(numfaders+2)
const int numfaders = 16;
int faderValue[numfaders];
int x[numfaders];
int y[numfaders];

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
      y[0] = map(value, 0, 127, YRES-10, 0); // yres-5 as yres seems to be off screen
      break;
    case fader2:
      y[1] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader3:
      y[2] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader4:
      y[3] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader5:
      y[4] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader6:
      y[5] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader7:
      y[6] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader8:
      y[7] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader9:
      y[8] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader10:
      y[9] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader11:
      y[10] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader12:
      y[11] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader13:
      y[12] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader14:
      y[13] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader15:
      y[14] = map(value, 0, 127, -20, 20);
      break;
    case fader16:
      y[15] = map(value, 0, 127, -15, 15);
      break;
  }
  
}

void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}



void line_group(float slope)
{
  // draw set of three paralel lines
  
  // lines should have a semi-random slope each time they're drawn
  // slope can be set outsite of this function and will act as argument
  
  int  len = 300; // length of line to draw
  // based on slope, lines should start at left or top of screen
  
  // if slope between -1 and 1 start at left edge
    //   pick random 0 or XRES for x0_line
  if(slope > -1.0 && slope < 1.0){
    x0_line = 0;
    y0_line = random(YRES); //   pick random YRES for y0_line
  }
  else {
    y0_line = 0;
    x0_line = random(XRES); //   pick random YRES for y0_line
  }
  
  
  //   based on slope, calculate x1_line and y1_line
  //   https://www.geeksforgeeks.org/find-points-at-a-given-distance-on-a-line-of-given-slope/
  int dx = (len / sqrt(1 + (slope * slope)));
  int dy = slope * dx;
  x1_line = x0_line + dx;
  y1_line = y0_line + dy;


  // draw buffer black lines between each white line
  // each line group will compose of 11 line (8 black, 3 white)
  for(int i = 0; i < 11; i++){
    if(slope > -1.0 && slope < 1.0){
      if(i == 2 || i == 5 || i == 8){
        graphics.drawLine(x0_line, y0_line+i, x1_line, y1_line+i, 54);  // white lines
      }
      else{
        graphics.drawLine(x0_line, y0_line+i, x1_line, y1_line+i, 0); // black lines
      } 
    }
    else {
      if(i == 2 || i == 5 || i == 8){
        graphics.drawLine(x0_line+i, y0_line, x1_line+i, y1_line, 54);  // white lines
      }
      else{
        graphics.drawLine(x0_line+i, y0_line, x1_line+i, y1_line, 0); // black lines
      } 
    }
  }

  //Serial.print("x0 = ");  Serial.print(x0_line);Serial.print("  x1 = ");Serial.print(x1_line);  
  //Serial.println();
  
}



void draw()
{
  graphics.begin(0);
  dumpBuffer();        // redraw last frame
  fade(2);             // fade last frame by 2

  if(draw_count%2 == 0){
    // horizontal-ish slope
    slope = random(-9,10)/10.0; // divide by 10 to create decimal
  }
  else {
    // vertical-ish slope
    bool r = random(0,2);
    if(r == 0){
      slope = random(-30,-9)/10.0; // divide by 10 to create decimal
    }
    else {
      slope = random(10,31)/10.0; // divide by 10 to create decimal
    }
  }
  

  line_group(slope);
  
  //Serial.print(draw_count);  Serial.print(" - slope = ");Serial.print(" ");Serial.print(slope);  
  //Serial.println();

  graphics.end();
  fillBuffer();
  draw_count++;
}

void setup() {
  fastStart();
  MIDI.begin(MIDI_CHANNEL_OMNI);   //Listens for messages on all midi channels. Adjust if necessary.
  MIDI.setHandleControlChange(OnControlChange);
  Serial.begin(115200);             //NOTE: changed from default midi 57600 to 115200
  //Serial.println("MIDI Input Test");

}
 
void loop() {
  MIDI.read();

  // wait 10 loops to let midi values 'catch up'
  if(loop_counter >= 10){
    //draw levels on screen
    draw();
    loop_counter = 0;
  }
  delay(20);
  loop_counter++;
}
