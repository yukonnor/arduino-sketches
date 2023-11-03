//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THIS SKETCH: 
//            Draws a field of stars and moves them 'towards' you. You can throw the direction that the starts are moving and control their velocity via midi inputs
//            ESP32 receives a few inputs (midi signals sent from the 16n (or any midi device sending Control Change messages)
//              and draws a composite image based on values of the midi faders
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
byte explosion_loop_count = 0;

const int num_stars = 50;
int star_x[num_stars];
int star_y[num_stars];
int speed_x = 3;  // number of pixels to move stars per frame.
int speed_y = 2;

//explosion vars
int explosion_x;
int explosion_y;


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
      speed_x = map(value, 0, 127, -20, 20);
      break;
    case fader16:
      speed_y = map(value, 0, 127, -15, 15);
      break;
  }
  
}

void star_explosion(int x, int y)
{
  // draw an explosion starting at x,y
  // even runs
  if(explosion_loop_count % 2 == 0){
      graphics.drawPixel(explosion_x+explosion_loop_count,explosion_y,white);
      graphics.drawPixel(explosion_x-explosion_loop_count,explosion_y,white);
      graphics.drawPixel(explosion_x,explosion_y+explosion_loop_count,white);
      graphics.drawPixel(explosion_x,explosion_y+explosion_loop_count,white);
  }

  // odd runs
  if(explosion_loop_count % 2 != 0){
    graphics.drawCircle(explosion_x,explosion_y,explosion_loop_count,30); // grey circles
  }
  
  if(explosion_loop_count >= 10){
    graphics.drawCircle(explosion_x,explosion_y,12,54); // final white circle
    explosion_loop_count = 0;
    explosion_x = random(XRES);
    explosion_y = random(YRES);
  }

  explosion_loop_count++;
}

void diamond_explosion(int x, int y)
{
  // even runs
  if(explosion_loop_count % 2 == 0){
      graphics.drawPixel(explosion_x+explosion_loop_count,explosion_y,white);
      graphics.drawPixel(explosion_x-explosion_loop_count,explosion_y,white);
      graphics.drawPixel(explosion_x,explosion_y+explosion_loop_count,white);
      graphics.drawPixel(explosion_x,explosion_y+explosion_loop_count,white);
  }

  // odd runs
  if(explosion_loop_count % 2 != 0){
    //graphics.drawLine(explosion_x,explosion_y,30); // grey circles
  }
  
  if(explosion_loop_count >= 10){
    graphics.drawCircle(explosion_x,explosion_y,12,54); // final white circle
    explosion_loop_count = 0;
    explosion_x = random(XRES);
    explosion_y = random(YRES);
  }

  explosion_loop_count++;
}


void center_object(int x, int y, int r, int shaddowoffx, int shaddowoffy)
{
  // draw circle with shaddow at x,y
  
  //draw shaddow first at an offset 
  graphics.fillCircle(x + shaddowoffx,y + shaddowoffy,r,0); 
  graphics.drawCircle(x + shaddowoffx,y + shaddowoffy,r,54); // shaddow boarder

  // draw circle
  graphics.fillCircle(x,y,r,54);
}

void draw()
{
  graphics.begin(0);
 

  //move all stars in a certain direction (based on midi inputs: x,y,speed)
  for(int i = 0; i<num_stars; i++){
    star_x[i] += speed_x;
    star_y[i] += speed_y;
  }
  

  //if star goes off screen, redraw it at random location
  //TODO: redraw at random location opposite of direction
  for(int i = 0; i<num_stars; i++){  
    if(star_x[i] > XRES){
      star_x[i] = 0;            // if start leaves to right, draw on left edge of screen
      star_y[i] = random(YRES);
    }
    else if(star_x[i] < 0){
      star_x[i] = XRES;     // if start leaves to left, draw on right edge of screen
      star_y[i] = random(YRES);
    }
    else if(star_y[i] > YRES){
      star_x[i] = random(XRES);
      star_y[i] = 0;
    }
    else if(star_y[i] < 0){
      star_x[i] = random(XRES);
      star_y[i] = YRES;
    }
  }

  // draw the stars
  for(int i = 0; i<num_stars; i++){
    graphics.drawPixel(star_x[i],star_y[i],white);
  }

  star_explosion(explosion_x, explosion_y);

  center_object(XRES/2,YRES/2,30,5,5);

  graphics.end();
}

void setup() {
  fastStart();
  MIDI.begin(MIDI_CHANNEL_OMNI);   //Listens for messages on all midi channels. Adjust if necessary.
  MIDI.setHandleControlChange(OnControlChange);
  Serial.begin(115200);             //NOTE: changed from default midi 57600 to 115200
  //Serial.println("MIDI Input Test");

  explosion_x = random(XRES);
  explosion_y = random(YRES);


  // set the stars iniitial coordinates:
  for(int i = 0; i<num_stars; i++){
    star_x[i] = random(XRES);
    star_y[i] = random(YRES);
  }
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
