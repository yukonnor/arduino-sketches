//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THIS SKETCH: ESP32 receives midi signals sent from the 16n (or any midi device sending Control Change messages)
//              and draws a composite image based on values of the midi faders
//
// 16n: this sketch is setup to accept messages from any midi channel and where the CC faders are set to 
//      controller #'s 32 thru 47. If you adjust the 16n's controller numbers, this sketch will need to b adjusted
//
// MIDI: - midi message type 176 is a "Control Change" 
//       - 176 -> 1011{nnnn} --> CC on channel nnnn with data: d1 d2
//       - *d1* is the controller number (0-119). *d2* is the controller value (0-127)
//       - Midi Library midi types enumeration: http://fortyseveneffects.github.io/arduino_midi_library/a00008.html
//       - Hex to decimal converter: https://www.rapidtables.com/convert/number/hex-to-decimal.html
//
// COMPOSITE: - Connect compiste RCA cable to DAC pin 25 and GND

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

//space between: XRES/(numfaders+2)
const int numfaders = 16;
float faderValue[numfaders];

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

// declare variables for stars
int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50; // how big the cube is - orig 50
int view_plane = 75; // how far away the window/plane/2D projection is away from the viewer - orig 64 (z must be > view_plane)
                     // smaller value makes cube smaller. Moving the window closer allows you to see more of the 3d world (i think) 
float angle = PI/60; // orig PI/60 = 3 deg
float x_angle = 0;   

const int shape = 2; //1 for cube, 2 for pyramid

float cube3d[8][3] = {
  {xOff - cSize,yOff + cSize,zOff - cSize}, //back, top left
  {xOff + cSize,yOff + cSize,zOff - cSize}, //back, top right
  {xOff - cSize,yOff - cSize,zOff - cSize}, //back, bottom left
  {xOff + cSize,yOff - cSize,zOff - cSize}, //back, bottom right
  {xOff - cSize,yOff + cSize,zOff + cSize}, //front, top left
  {xOff + cSize,yOff + cSize,zOff + cSize}, //front, top right
  {xOff - cSize,yOff - cSize,zOff + cSize}, //front, bottom left
  {xOff + cSize,yOff - cSize,zOff + cSize}  //front, bottom right
};
unsigned char cube2d[8][2];

float pyramid3d[5][3] = {
  {xOff - cSize,yOff - cSize,zOff + cSize}, //front, bottom left
  {xOff + cSize,yOff - cSize,zOff + cSize}, //front, bottom right
  {xOff - cSize,yOff - cSize,zOff - cSize}, //back, bottom left
  {xOff + cSize,yOff - cSize,zOff - cSize}, //back, bottom right
  {xOff        ,yOff + cSize,zOff        }  //tip
};
unsigned char pyramid2d[5][2];

float mapf(long x, long in_min, long in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// callback function when a control change message is received
void OnControlChange(byte channel, byte number, byte value)
{
  if(number > 119 || value > 127) return; //exit function if received bad data
  
  //Serial.print(number);Serial.print(" ");Serial.print(value);  
  //Serial.println();

  switch(number){
    case fader1:
      faderValue[0] = mapf(value, 0, 127, -PI/30, PI/30); // x rotate (if 0-42, rotate neg / if 43-85, no rotate / if 86-127, rotate pos)
      break;
    case fader2:
      faderValue[1] = mapf(value, 0, 127, -PI/30, PI/30); // y rotate
      break;
    case fader3:
      faderValue[2] = mapf(value, 0, 127, -PI/30, PI/30); // z rotate
      break;
    case fader4:
      faderValue[3] = map(value, 0, 127, YRES-10, 0); // x random rotate mode (on/off)
      break;
    case fader5:
      faderValue[4] = map(value, 0, 127, YRES-10, 0); // y random rotate mode (on/off)
      break;
    case fader6:
      faderValue[5] = map(value, 0, 127, YRES-10, 0); // z random rotate mode (on/off)
      break;
    case fader7:
      faderValue[6] = map(value, 0, 127, YRES-10, 0); // random rotate speed
      break;
    case fader8:
      faderValue[7] = map(value, 0, 127, YRES-10, 0); // fade amount
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
      faderValue[12] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader14:
      faderValue[13] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader15:
      faderValue[14] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader16:
      faderValue[15] = map(value, 0, 127, YRES-10, 0);
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


void printcube() {
  //calculate 2d points
  // NOTE: if z < view_plane that means that the z coordinate is between the projection window and the 'eye' viewing it. this is bad
  //       the object should remainin far enough away from the view plane (in z direction) so that z > view_plane
  for(byte i = 0; i < 8; i++) {
    cube2d[i][0] = (unsigned char)((cube3d[i][0] * view_plane / cube3d[i][2]) + (XRES/2)); // cube2d x = cube3d x * (distance to view plane / cube3d z)
    cube2d[i][1] = (unsigned char)((cube3d[i][1] * view_plane / cube3d[i][2]) + (YRES/2)); // cube2d y = cube3d y * (distance to view plane / cube3d z)
  }
  //TV.delay_frame(1);
  //TV.clear_screen();
  draw_cube();
}

void printpyramid() {
  //calculate 2d points
  // NOTE: if z < view_plane that means that the z coordinate is between the projection window and the 'eye' viewing it. this is bad
  //       the object should remainin far enough away from the view plane (in z direction) so that z > view_plane
  for(byte i = 0; i < 5; i++) {
    pyramid2d[i][0] = (unsigned char)((pyramid3d[i][0] * view_plane / pyramid3d[i][2]) + (XRES/2)); // cube2d x = cube3d x * (distance to view plane / cube3d z)
    pyramid2d[i][1] = (unsigned char)((pyramid3d[i][1] * view_plane / pyramid3d[i][2]) + (YRES/2)); // cube2d y = cube3d y * (distance to view plane / cube3d z)
  }
  //TV.delay_frame(1);
  //TV.clear_screen();
  draw_pyramid();
}

//
void zrotate(float q) {
  float tx,ty,temp;
  if(shape == 1){ //if cube:
    for(byte i = 0; i < 8; i++) {
      tx = cube3d[i][0] - xOff;
      ty = cube3d[i][1] - yOff;
      temp = tx * cos(q) - ty * sin(q);
      ty = tx * sin(q) + ty * cos(q);
      tx = temp;
      cube3d[i][0] = tx + xOff;
      cube3d[i][1] = ty + yOff;
    }
  }
  else {
    for(byte i = 0; i < 5; i++) {
      tx = pyramid3d[i][0] - xOff;
      ty = pyramid3d[i][1] - yOff;
      temp = tx * cos(q) - ty * sin(q);
      ty = tx * sin(q) + ty * cos(q);
      tx = temp;
      pyramid3d[i][0] = tx + xOff;
      pyramid3d[i][1] = ty + yOff;
    }
  }
}

void yrotate(float q) {
  float tx,tz,temp;
  if(shape == 1){ //if cube:
    for(byte i = 0; i < 8; i++) {
      tx = cube3d[i][0] - xOff;
      tz = cube3d[i][2] - zOff;
      temp = tz * cos(q) - tx * sin(q);
      tx = tz * sin(q) + tx * cos(q);
      tz = temp;
      cube3d[i][0] = tx + xOff;
      cube3d[i][2] = tz + zOff;
    }
  }
  else {
    for(byte i = 0; i < 5; i++) {
      tx = pyramid3d[i][0] - xOff;
      tz = pyramid3d[i][2] - zOff;
      temp = tz * cos(q) - tx * sin(q);
      tx = tz * sin(q) + tx * cos(q);
      tz = temp;
      pyramid3d[i][0] = tx + xOff;
      pyramid3d[i][2] = tz + zOff;
    }
  }
}

void xrotate(float q) {
  float ty,tz,temp;
  if(shape == 1){ //if cube:
    for(byte i = 0; i < 8; i++) {
      ty = cube3d[i][1] - yOff;
      tz = cube3d[i][2] - zOff;
      temp = ty * cos(q) - tz * sin(q);
      tz = ty * sin(q) + tz * cos(q);
      ty = temp;
      cube3d[i][1] = ty + yOff;
      cube3d[i][2] = tz + zOff;
    }
  }
  else {
    for(byte i = 0; i < 5; i++) {
      ty = pyramid3d[i][1] - yOff;
      tz = pyramid3d[i][2] - zOff;
      temp = ty * cos(q) - tz * sin(q);
      tz = ty * sin(q) + tz * cos(q);
      ty = temp;
      pyramid3d[i][1] = ty + yOff;
      pyramid3d[i][2] = tz + zOff;
    }
  }
}

void draw_cube() {
  graphics.begin(0);
  dumpBuffer(); //optional fading effect
  fade(6);      //optional fading effect
  
  graphics.drawThicLine(cube2d[0][0],cube2d[0][1],cube2d[1][0],cube2d[1][1],54);
  graphics.drawThicLine(cube2d[0][0],cube2d[0][1],cube2d[2][0],cube2d[2][1],54);
  graphics.drawThicLine(cube2d[0][0],cube2d[0][1],cube2d[4][0],cube2d[4][1],54);
  graphics.drawThicLine(cube2d[1][0],cube2d[1][1],cube2d[5][0],cube2d[5][1],54);
  graphics.drawThicLine(cube2d[1][0],cube2d[1][1],cube2d[3][0],cube2d[3][1],54);
  graphics.drawThicLine(cube2d[2][0],cube2d[2][1],cube2d[6][0],cube2d[6][1],54);
  graphics.drawThicLine(cube2d[2][0],cube2d[2][1],cube2d[3][0],cube2d[3][1],54);
  graphics.drawThicLine(cube2d[4][0],cube2d[4][1],cube2d[6][0],cube2d[6][1],54);
  graphics.drawThicLine(cube2d[4][0],cube2d[4][1],cube2d[5][0],cube2d[5][1],54);
  graphics.drawThicLine(cube2d[7][0],cube2d[7][1],cube2d[6][0],cube2d[6][1],54);
  graphics.drawThicLine(cube2d[7][0],cube2d[7][1],cube2d[3][0],cube2d[3][1],54);
  graphics.drawThicLine(cube2d[7][0],cube2d[7][1],cube2d[5][0],cube2d[5][1],54);
  graphics.end();
  fillBuffer();
}

void draw_pyramid() {
  graphics.begin(0);
  dumpBuffer(); //optional fading effect
  fade(6);      //optional fading effect
  
  graphics.drawThicLine(pyramid2d[0][0],pyramid2d[0][1],pyramid2d[1][0],pyramid2d[1][1],54);
  graphics.drawThicLine(pyramid2d[0][0],pyramid2d[0][1],pyramid2d[2][0],pyramid2d[2][1],54);
  graphics.drawThicLine(pyramid2d[0][0],pyramid2d[0][1],pyramid2d[4][0],pyramid2d[4][1],54);
  graphics.drawThicLine(pyramid2d[1][0],pyramid2d[1][1],pyramid2d[3][0],pyramid2d[3][1],54);
  graphics.drawThicLine(pyramid2d[1][0],pyramid2d[1][1],pyramid2d[4][0],pyramid2d[4][1],54);
  graphics.drawThicLine(pyramid2d[2][0],pyramid2d[2][1],pyramid2d[3][0],pyramid2d[3][1],54);
  graphics.drawThicLine(pyramid2d[2][0],pyramid2d[2][1],pyramid2d[4][0],pyramid2d[4][1],54);
  graphics.drawThicLine(pyramid2d[3][0],pyramid2d[3][1],pyramid2d[4][0],pyramid2d[4][1],54);
  graphics.end();
  fillBuffer();
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
  /*
  int rsteps = random(10,60);
  switch(random(6)) {
    case 0:
      for (int i = 0; i < rsteps; i++) {
        zrotate(angle); //rotates the current position of the cube by angle
        printcube();
      }
      break;
    case 1:
      for (int i = 0; i < rsteps; i++) {
        zrotate(2*PI - angle); //rotates the current position by neg angle (2PI = 360 deg)
        printcube();
      }180/
      break;
    case 2:
      for (int i = 0; i < rsteps; i++) {
        xrotate(angle);
        printcube();
      }
      break;
    case 3:
      for (int i = 0; i < rsteps; i++) {
        xrotate(2*PI - angle);
        printcube();
      }
      break;
    case 4:
      for (int i = 0; i < rsteps; i++) {
        yrotate(angle);
        printcube();
      }
      break;
    case 5:
      for (int i = 0; i < rsteps; i++) {
        yrotate(2*PI - angle);
        printcube();
      }
      break;
    }
    */

    if(faderValue[0] > 0.01 || faderValue[0] < -0.01) xrotate(faderValue[0]); //rotate in x direction (unless fader is near center)
    if(faderValue[1] > 0.01 || faderValue[1] < -0.01) yrotate(faderValue[1]); //rotate in x direction (unless fader is near center)
    if(faderValue[2] > 0.01 || faderValue[2] < -0.01) zrotate(faderValue[2]); //rotate in x direction (unless fader is near center)
    //printcube();
    printpyramid();
    
    //draw rotating cube
    //draw_cube();
    draw_pyramid();
    loop_counter = 0;
    
  }

  loop_counter++;
  delay(5);
}
