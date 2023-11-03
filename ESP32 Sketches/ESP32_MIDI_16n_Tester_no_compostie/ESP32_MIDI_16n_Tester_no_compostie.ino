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


#include <MIDI.h>

//ESP32 RX Pin RX2 Used:
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// Hardware Serial, Custom Baud
//struct CustomBaud : public midi::DefaultSettings{
//    static const long BaudRate = 115200; // Baud rate for hairless
//};

const int XRES = 320;
const int YRES = 240;
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
  
  //Serial.println(String("Control Change, ch=") + channel + ", control #= " + number + ", value= " + value);  
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
      y[14] = map(value, 0, 127, YRES-10, 0);
      break;
    case fader16:
      y[15] = map(value, 0, 127, YRES-10, 0);
      break;
  }
  
}


void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);   //Listens for messages on all midi channels. Adjust if necessary.
  MIDI.setHandleControlChange(OnControlChange);
  Serial.begin(115200);             //NOTE: changed from default midi 57600 to 115200
  Serial.println("MIDI Input Test");
}
 
void loop() {
  MIDI.read();

}
