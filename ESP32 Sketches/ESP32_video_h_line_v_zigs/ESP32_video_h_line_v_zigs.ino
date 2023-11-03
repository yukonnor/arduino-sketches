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
const int num_lines = 60;


int line_x[60];
int line_y[60];
int line_width = 100;

int indent = 0;
bool indent_increasing = 1;

//// FUNCTIONS ///////////

void fade(char amt){
  for(int i = 0; i<XRES; i++){
    for(int j = 0; j<YRES; j++) {
      graphics.pixelSubtract(i, j, amt);
    }
  }  
}

void initialize_lines()
{
  // set x values for all lines using a zig zag pattern
  for(int i = 0; i < num_lines; i++){  
    line_x[i] = 0;
  }

  // set y values for lines
  for(int i = 0; i < num_lines; i++){
    line_y[i] = 0 + (4 * i);  // space lines every 4 pixels
  }

}


void initialize_zig()
{
  
  // set x values for all lines using a zig zag pattern
  for(int i = 0; i < num_lines; i++){  
  
    // change direction every 15 lines
    if(indent_increasing == 1){
      // set the indent amount for the line
      line_x[i] += indent;
      Serial.print(i);  Serial.print(" - line x = ");Serial.print(line_x[i]);  
      Serial.println();
      // increase indent by 2 if indent is increasing for the next line
      indent += 2;
      // if indent amount is greater than 30, turn off indent increasing
      if(indent > 30){
        // update the indent amount to remove the last indent and start moving in other direction
        indent -= 2 * 2;
        indent_increasing = 0;
        //i++;
      }
    }
    else if(indent_increasing == 0){
      // set the indent amount for the line
      line_x[i] += indent;
      Serial.print(i);  Serial.print(" - line x = ");Serial.print(line_x[i]);  
      Serial.println();
      // decrease indent by 2 if indent is decreasing for the next line
      indent -= 2;
      // if indent amount is less than 0, turn on indent increasing
      if(indent < 0){
        // update the indent amount to remove the last indent and start moving in other direction
        indent += 2 * 2;
        indent_increasing = 1;
      }
    }
  }
  
}


void update_lines(){
  // store for use at end of for loop
  int x0_orig = line_x[0];
  
  // update x values for all lines to animate the zig zag pattern
  for(int i = 0; i < num_lines; i++){  
    // x0 now equals x1
    // x1 now equals x2
    // x2 now equal  x3
    // ...
    // x59 now equals orig x0 (store first!)

    // if last line
    if(i == num_lines-1){
      // set to what the top line was
      line_x[i] = x0_orig;
    }
    else{
      line_x[i] = line_x[i+1];
    }
    
  }
  
}

void draw()
{
  graphics.begin(0);
  //dumpBuffer();        // redraw last frame
  //fade(8);             // fade last frame by 2

  // update the lines to animate
  update_lines();

  // draw the lines
  for(int i = 0; i < num_lines; i++){ 
    int offset = 89;
    graphics.drawLine(line_x[i], line_y[i], line_x[i] + line_width, line_y[i], white);
    // draw copy of zig to the right
    graphics.drawLine(line_x[i] + line_width + offset, line_y[i], line_x[i] + line_width + offset + line_width, line_y[i], white);
  }
  

  graphics.end();
  //fillBuffer();
}



/////////// THE PROGRAM ///////////


void setup() {
  fastStart();
  //MIDI.begin(MIDI_CHANNEL_OMNI);   //Listens for messages on all midi channels. Adjust if necessary.
  //MIDI.setHandleControlChange(OnControlChange);
  Serial.begin(115200);             //NOTE: changed from default midi 57600 to 115200

  delay(1000);

  initialize_lines();
  initialize_zig();


}
 
void loop() {
  //MIDI.read();

  // wait 10 loops to let midi values 'catch up'
  if(loop_counter >= 10){
    //draw levels on screen
    draw();
    loop_counter = 0;
  }
  
  delay(10);
  loop_counter++;
}
