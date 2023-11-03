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

int shifty_eye_loop_count = 0;
int pupil_direction = 1;

//// FUNCTIONS ///////////

void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}


void draw_eye(int x, int y, int eye_size, char c)
{
  // draw outline of eye
  graphics.drawEllipse(x-(eye_size/2), y-(eye_size/4), x+(eye_size/2), y+(eye_size/4), c);

  // draw pupil at center of eye
  // pupil diameter should be half height of eye (.5 * eye_size/2)
  graphics.fillCircle(x,y,(eye_size/8), c);
}

void draw_shifty_eye(int x, int y, int eye_size, char c)
{
  // draw outline of eye
  graphics.drawEllipse(x-(eye_size/2), y-(eye_size/4), x+(eye_size/2), y+(eye_size/4), c);

  // how long the pupil waits for
  int pause_length = 10;
  int pause_count = 0;

  // if pupil placement (shifty_eye_loop_count) isn't at edge, move
  if(shifty_eye_loop_count > 20 || shifty_eye_loop_count < 0){
    graphics.fillCircle(pupil_x,y,(eye_size/8), c);
    pause_count++;

    // if it's paused for it's pause length, get it out of this crazy loop
    if (pause_count > pause_length){
      pupil_direction = -pupil_direction
      int pupil_x = (x - (eye_size/2)) + (pupil_direction*shifty_eye_loop_count);
      graphics.fillCircle(pupil_x,y,(eye_size/8), c);
      pause_count = 0;
    }

    // count to determine length of pause when pupil reaches edge of eye to turn back
    shifty_eye_loop_count == 0;
    
    while(pause_count<
  }
  else if{
    int pupil_x = (x - (eye_size/2)) + (pupil_direction*shifty_eye_loop_count);
    graphics.fillCircle(pupil_x,y,(eye_size/8), c);
    
  }

  // draw pupil
  // pupil diameter should be half height of eye (.5 * eye_size/2)
  // animate: starts at left side of eye, moves to right side, turns back, etc
  int pupil_x = (x - (eye_size/2)) + (pupil_direction*shifty_eye_loop_count);
  graphics.fillCircle(pupil_x,y,(eye_size/8), c);

  shifty_eye_loop_count++;

  // count to determine length of pause when pupil reaches edge of eye to turn back
  int pause_count = 0;
  if(shifty_eye_loop_count > 20 || shifty_eye_loop_count < 0){
    pupil_direction = -pupil_direction
    // count to determine length of pause when pupil reaches edge of eye to turn back
    shifty_eye_loop_count == 0;
    int pause_count = 0;
    while(pause_count<
  }
}

void draw_nose(int x, int y, int nose_size, char c)
{
  // draw simple nose where x,y is top center of nose
  // nose_size: how far left the point goes from center
  
  graphics.drawLine(x, y, x - nose_size, y + (nose_size*2), c);
  graphics.drawLine(x - nose_size, y + (nose_size*2), x + (nose_size/3), y + (nose_size*2), c); // horizontal line ends just right of center
}

void draw_closed_mouth(int x, int y, int mouth_size, char c)
{
  // draw simple closed mouth where x,y is top center of lips
  // mouth_size: how far left the corner goes from center
  int m_x[] = {x, x + (mouth_size/4), x + mouth_size, x + (mouth_size/4), x - (mouth_size/4), x - mouth_size, x - (mouth_size/4)};  // 8, 6, 4, 3, 2, 1
  int m_y[] = {y, y - (mouth_size/8), y + (mouth_size/4), y + (mouth_size/2), y + (mouth_size/2), y + (mouth_size/4), y - (mouth_size/8)};

  // draw most of mouth
  for(int i=0; i < 6; i++){
    graphics.drawLine(m_x[i],m_y[i],m_x[i+1],m_y[i+1],c);
  }

  // finishing touches
  graphics.drawLine(m_x[6],m_y[6],m_x[0],m_y[0],c);
  graphics.drawLine(m_x[2],m_y[2],m_x[5],m_y[5],c);

  Serial.print("mouth points 3 and 4 = ");  Serial.print(m_x[3]);Serial.print(" ");Serial.print(m_x[4]);  
  Serial.println();
}


void draw_face(int x, int y, int face_size, char c)
{
  // draw a face where x,y is the center point between the eyes
  // face_size: distance from center point to center of eyes
  // eye_size: 4/3 (1.33) face_size

  // draw eyes
  draw_eye(x - face_size, y, face_size*1.33, c);
  draw_eye(x + face_size, y, face_size*1.33, c);

  // draw nose
  draw_nose(x, y, face_size/2, c);

  // draw mouth
  draw_closed_mouth(x, y + (1.5*face_size), face_size, c);
}




void draw()
{
  graphics.begin(0);
  dumpBuffer();        // redraw last frame
  fade(8);             // fade last frame by 2

  draw_face(random(20,300),random(20,200), random(12,40), 54);
  
  //Serial.print(draw_count);  Serial.print(" - slope = ");Serial.print(" ");Serial.print(slope);  
  //Serial.println();

  graphics.end();
  fillBuffer();
}

void setup() {
  fastStart();
  //MIDI.begin(MIDI_CHANNEL_OMNI);   //Listens for messages on all midi channels. Adjust if necessary.
  //MIDI.setHandleControlChange(OnControlChange);
  Serial.begin(115200);             //NOTE: changed from default midi 57600 to 115200
  //Serial.println("MIDI Input Test");

}
 
void loop() {
  //MIDI.read();

  // wait 10 loops to let midi values 'catch up'
  if(loop_counter >= 10){
    //draw levels on screen
    draw();
    loop_counter = 0;
  }
  delay(20);
  loop_counter++;
}
