
// Summary of Midi Messages: https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message\
// Midi library docs: http://fortyseveneffects.github.io/arduino_midi_library/index.html
//
// - midi messages start with one status byte followed by 1 _or_ 2 data bytes, depending on the command
// - Note off can also be represented as a note on message with velocity = 0 (This sketch tests for both)
// - Midi Library midi types enumeration: http://fortyseveneffects.github.io/arduino_midi_library/a00008.html
// - Hex to decimal converter: https://www.rapidtables.com/convert/number/hex-to-decimal.html



#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(57600);
  Serial.println("MIDI Input Test");
}

unsigned long t=0;

void loop() {
  int type, note, velocity, channel, d1, d2;
  if (MIDI.read()) {                   // Is there a MIDI message incoming ?
                                       // True if a valid message has been stored in the structure, false if not. A valid message is a message that matches the input channel.
                                       // If the Thru is enabled and the message matches the filter, it is sent back on the MIDI output.
   
    byte type = MIDI.getType();        // getType: returns the initial 'status' byte ('note on', 'note off', 'control change'). Returns an enumerated type.
    switch (type) {                    // switch...case docs: https://www.arduino.cc/reference/en/language/structure/control-structure/switchcase/
      case midi::NoteOn:
        note = MIDI.getData1();        // getData1: Gets the first data byte of the last received message. For note messages, this is the note value.
        velocity = MIDI.getData2();    // getData2: Gets the second data byte of the last received message. For not messages, this is the velocity.
        channel = MIDI.getChannel();   // getChannel: returns midi channel of message (1-16)
        if (velocity > 0) {
          Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
        } else {
          Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
        }
        break;
      case midi::NoteOff:
        note = MIDI.getData1();
        velocity = MIDI.getData2();
        channel = MIDI.getChannel();
        Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
        break;
      default:
        d1 = MIDI.getData1();
        d2 = MIDI.getData2();
        Serial.println(String("Message, type=") + type + ", data = " + d1 + " " + d2);  //print the 'type' value + channel and the data packets 
                                        // example: type 176 --> 1011{nnnn} --> "Control Change" on channel n. *d1* is the controller number (0-119). *d2* is the controller value (0-127)
                                        // example: type 224 --> 1110{nnnn} --> "Pitch Bend Change" on channel n. *d1* are the least significant 7 bits. *d2* are the most significant 7 bits (really only need d2)
    }
    t = millis();
  }
  if (millis() - t > 10000) {
    t += 10000;
    Serial.println("(inactivity)");
  }
}
