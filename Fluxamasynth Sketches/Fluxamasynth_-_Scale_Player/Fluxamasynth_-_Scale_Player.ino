/* SCALE Player
 *  
 *  last edited: 1/29/21
 *  Plays 3 notes with 3 instuments based on pot position, all locked to a scale
 *  
 *  No buttons pressed:
 *  - Pot 1 controls note of channel 0
 *  - Pot 2 controls note of channel 1 
 *  - Pot 3 controls note of channel 2
 *  - Pot 4 controls voice of channel 0
 *  - Pot 5 controls voice of channel 1
 *  - Pot 6 controls voice of channel 2
 *  
 *  Button 1 pressed:
 *  - Pot 1 sets max volume of channel 0
 *  - Pot 2 sets max volume of channel 1
 *  - Pot 3 sets max volume of channel 2
 *  - Pot 6 sets master volume (GLOBAL) // not done currently
 *  
 *  Button 2 pressed:
 *  - Pot 4 sets note change delay (debounce) time of channel 0
 *  - Pot 5 sets note change delay (debounce) time of channel 1
 *  - Pot 6 sets note change delay (debounce) time of channel 2
 *  
 *  Button 3 pressed:
 *  - Pot 1 set chorus (GLOBAL)
 *  - Pot 2 sets
 *  - Pot 3 sets
 *  - Pot 4 sets reverb
 *  - Pot 5 sets reverb
 *  - Pot 6 sets reverb
 *  
 *  Button 4 pressed:
 *  - Pot 1 sets scale! (GLOBAL)
 *    - 1: major, 2: minor, 3: pent major, 4: pent minor, 5: blues, 6: chromatic
 *  
 *  TODO: fix when pot value gets stuck between two notes
 *        - maybe only play note if note is different (done) and changed after an amount of time (done)
 *        - Maybe: Volume LFO Strength & Speed or some sort of Envelope (button 5 pressed - top 3 adjust strength, bottom 3 speed)
 *        - Create a secret autoplay mode (two buttons pressed starts mode, randomly sets notes)
 *          - pots control rate of note change (slow, fast, random (narrow range, wide range))
 *          - direction of note change (drunk, drunk jump, double drunk (alternate between low note drunk pattern and high note drunk pattern),random)
 */

#include <Fluxamasynth.h>
Fluxamasynth synth;

unsigned long current_time;

// Define Scale Arrays (all in key of C covering 4 octaves)
int scale_type = 0;
int scale_type_prev = 0;
int scale_length = 0;
int note_0_index = 0;
int note_1_index = 0;
int note_2_index = 0;
const int major_scale_len = 28;
const int minor_scale_len = 28;
const int pent_major_scale_len = 20;
const int pent_minor_scale_len = 20;
const int blues_scale_len = 24;
const int chromatic_scale_len = 48;
const byte major[major_scale_len] = { 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83 };
const byte minor[minor_scale_len] = { 36, 38, 39, 41, 43, 44, 46, 48, 50, 51, 53, 55, 56, 58, 60, 62, 63, 65, 67, 68, 70, 72, 74, 75, 77, 79, 80, 82 };
const byte pent_major[pent_major_scale_len] = { 36, 38, 40, 43, 45, 48, 50, 52, 55, 57, 60, 62, 64, 67, 69, 72, 74, 76, 79, 81 };
const byte pent_minor[pent_minor_scale_len] = { 36, 39, 41, 43, 46, 48, 51, 53, 55, 58, 60, 63, 65, 67, 70, 72, 75, 77, 79, 82 };
const byte blues[blues_scale_len] = { 36, 39, 41, 42, 43, 46, 48, 51, 53, 54, 55, 58, 60, 63, 65, 66, 67, 70, 72, 75, 77, 78, 79, 82 };
const byte chromatic[chromatic_scale_len] = { 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83 };
const int low_note = 36;
const int high_note = 83;
int scale_threshold = 10;
bool actively_measure_scale = false;

// Define Pot pins & vars
const int potPin[6] = { A3, A2, A4, A5, A1, A0 };
int pot_val[6] = { 0, 0, 0, 0, 0, 0 };
int pot_start_val[6] = { 0, 0, 0, 0, 0, 0 };
int pot_mapped[6] = { 0, 0, 0, 0, 0, 0 };

// Define PWM LED pins
const int ledPin[6] = { 11, 6, 5, 10, 9, 3 };

// Define Digital Button pins
const int buttonPin[6] = { 2, 13, 0, 7, 8, 12 };
bool buttonRead[6];
bool no_buttons_pressed = true; 
 
// Sound engine
int note[3] = { 60, 60, 60 };
int note_prev[3] = { 60, 60, 60 };
const int voice[12] = { 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 }; // starting instrument - rock organ  // a bankl of my fav voices
int voice_0_index = 0;
int voice_1_index = 0;
int voice_2_index = 0;
int bank = 0;
int vel = 127;
//int volume[3] = { 0, 0, 0 };
int pitch_threshold[3] = {10, 10, 10};
bool actively_measure_pitch[3] = { false, false, false };
bool actively_measure_voice[3] = { false, false, false };

int channel_volume[3] = { 20, 20, 20 };
bool actively_measure_vol[3] = { false, false, false };
int master_volume = 90;

long last_note_change_time[3] = { 0, 0, 0 };
int  note_change_debounce[3] = { 20, 20, 20 };  // max of maybe .75s?
bool actively_measure_debouce[3] = { false, false, false };

bool first_pass = true;

bool debug = true;


void setup() {
  if(debug) {Serial.begin(9600);}

  // SET PIN MODES
  // Button pins
  for (int i=0; i<6; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  // LED pins
  for (int i=0; i<6; i++) {
    pinMode(ledPin[i],OUTPUT);
  }
  
  synth.setMasterVolume(master_volume); 

  startupSequence();

  // set initial values based on current pot positions
  readPots();
  
  scale_length = major_scale_len; leds_on(1);
  note_0_index = map(pot_val[0], 0, 1024, 0, scale_length);
  note_1_index = map(pot_val[1], 0, 1024, 0, scale_length);
  note_2_index = map(pot_val[2], 0, 1024, 0, scale_length);
  voice_0_index = map(pot_val[3], 0, 1024, 0, 13);
  voice_1_index = map(pot_val[4], 0, 1024, 0, 13);
  voice_2_index = map(pot_val[5], 0, 1024, 0, 13);

  changeVoice();

  // record what notes are playing so that we can turn them off later
  note_prev[0] = 60;
  note_prev[1] = 65;
  note_prev[2] = 69;           
}

void loop() {
  // Get the current time
  current_time = millis();
  
  // Read the buttons and pots
  readButtons();
  readPots();

  // Run functions
  changeScale();        // Change scale type for all channels w/ pot 1 if button 4 pressed
  changePitch();        // Change notes of three channels w/ pots 1-3 if no buttons pressed
  changeVoice();        // Change voices of three channels w/ pots 4-6 if no buttons pressed
  changeVolume();       // Change volume of three channels w/ pots 1-3 if button 1 pressed
  setNoteChangeDelay(); // Change the debounce time between when notes will change if button 2 is pressed
  //changeReverb();
  //changeChorus();


  
  // PLAY SOUNDS
  // if note changes after debounce time, end previous note and start new note
  // (maybe todo) if note doesn't change, end current note after 5 seconds
  if(note[0] != note_prev[0] && current_time - last_note_change_time[0] > note_change_debounce[0]) {
    synth.noteOn(0, note_prev[0], 0); 
    synth.noteOn(0, note[0], 127);
  
    if(debug) {Serial.print("///// NEW PITCH: NOTE ");Serial.print(0);Serial.print(" IS PLAYING "); Serial.print(note[0]); Serial.println(" ///// ");} 
      
    note_prev[0] = note[0];
    last_note_change_time[0] = current_time;
  }                            

  if(note[1] != note_prev[1] && current_time - last_note_change_time[1] > note_change_debounce[1]) {
    synth.noteOn(1, note_prev[1], 0); 
    synth.noteOn(1, note[1], 127);

    if(debug) {Serial.print("///// NEW PITCH: NOTE ");Serial.print(1);Serial.print(" IS PLAYING "); Serial.print(note[1]); Serial.println(" ///// ");} 
    
    note_prev[1] = note[1];
    last_note_change_time[1] = current_time;
  }

  if(note[2] != note_prev[2] && current_time - last_note_change_time[2] > note_change_debounce[2]) {
    synth.noteOn(2, note_prev[2], 0); 
    synth.noteOn(2, note[2], 127);

    if(debug) {Serial.print("///// NEW PITCH: NOTE ");Serial.print(2);Serial.print(" IS PLAYING "); Serial.print(note[2]); Serial.println(" ///// ");} 
    
    note_prev[2] = note[2];
    last_note_change_time[2] = current_time;
  }
   
  delay(1);

}

//////////// END LOOP ///////////////////

void readButtons() {
  // Read ALL button values (NOTE: Button press == LOW)
  for (int i=0; i<6; i++) {
    buttonRead[i] = digitalRead(buttonPin[i]);
  }

  if (buttonRead[0] == HIGH && buttonRead[1] == HIGH && buttonRead[2] == HIGH && buttonRead[3] == HIGH && buttonRead[4] == HIGH && buttonRead[5] == HIGH) {
    no_buttons_pressed = true;
  }
  else {
    no_buttons_pressed = false;
  }
}

/////////////////////////////////////////

void readPots() {
  for (int i=0; i<6; i++) {
    pot_val[i] = analogRead(potPin[i]);
  }
}

/////////////////////////////////////////

void changePitch() {
  
  // Change notes of three channels w/ pots 1-3 if no buttons pressed  
  if (no_buttons_pressed) {

    if(debug && first_pass == true) {Serial.println("||| IN THE CHANGE PITCH FUNCTION |||"); delay(5);} 

    // get realtine pot values, mapped to the length of the currently selected scale, value == note on scale to play
    pot_mapped[0] = map(pot_val[0], 0, 1024, 0, scale_length);  
    pot_mapped[1] = map(pot_val[1], 0, 1024, 0, scale_length);
    pot_mapped[2] = map(pot_val[2], 0, 1024, 0, scale_length);

    // if the pot value comes close to the old pitch level (or if we are now actively measuring pitch), adjust pitch
    // todo: make (abs(pot_mapped[0] - note_0_index)) < 1 easier to read - turn into boolean like hit_last_value[0]

    pitch_threshold[0] = abs(pot_mapped[0] - note_0_index);
    pitch_threshold[1] = abs(pot_mapped[1] - note_1_index);
    pitch_threshold[2] = abs(pot_mapped[1] - note_1_index);

    if(debug && first_pass == true) {Serial.print("||| Pitch 1 Threshold = "); Serial.println(pitch_threshold[0]);} 
    
    if (pitch_threshold[0] < 1 || actively_measure_pitch[0] == true) {

      actively_measure_pitch[0] = true;
      if(debug && first_pass == true) {Serial.println("///// MEASURING PITCH \\\\\\\\"); first_pass = false;} 

      note_0_index = map(pot_val[0], 0, 1024, 0, scale_length); // adding 1 to last index position (ex 0-27) to map values based on user tip (https://forum.arduino.cc/index.php?topic=43727.0)
      setNotes(0, note_0_index);                                // determine which note to play based on scale and pot position
    }

    if (pitch_threshold[1] < 1 || actively_measure_pitch[1] == true) {

      actively_measure_pitch[1] = true;
      note_1_index = map(pot_val[1], 0, 1024, 0, scale_length); 
      setNotes(1, note_1_index);
    }

    if (pitch_threshold[2] < 1 || actively_measure_pitch[2] == true) {

      actively_measure_pitch[2] = true;
      note_2_index = map(pot_val[2], 0, 1024, 0, scale_length); // adding 1 to last index position (ex 0-27) to map values based on user tip (https://forum.arduino.cc/index.php?topic=43727.0)
      setNotes(2, note_2_index);
    }

    // TURN ON LEDs - brightness matches pitch
    int led_1_level = map(note[0], low_note, high_note, 0, 60); // full brightness = 255
    int led_2_level = map(note[1], low_note, high_note, 0, 60);
    int led_3_level = map(note[2], low_note, high_note, 0, 60);
  
    analogWrite(ledPin[0], led_1_level);   
    analogWrite(ledPin[1], led_2_level); 
    analogWrite(ledPin[2], led_3_level);  
    
  }
  else {
    // if one or more buttons pressed stop measuring pot values to determine pitch
    actively_measure_pitch[0] = false;
    actively_measure_pitch[1] = false;
    actively_measure_pitch[2] = false;
    first_pass = true;
  }
}

/////////////////////////////////////////

void changeVoice() {
  // if no buttons pressed, pots 4-6 adjust voice of channels 0-2
  // todo: only change voice once pot position close to voice position
  if (no_buttons_pressed) {

    pot_mapped[3] = map(pot_val[3], 0, 1024, 0, 13);
    pot_mapped[4] = map(pot_val[4], 0, 1024, 0, 13);
    pot_mapped[5] = map(pot_val[5], 0, 1024, 0, 13);

    // if the pot value comes close to the old voice level (or if we are now actively measuring voice), pot starts to adjust voice
    if (pot_mapped[3] - voice_0_index == 0 || actively_measure_voice[0] == true) {
      
      actively_measure_voice[0] = true;
      voice_0_index = map(pot_val[3], 0, 1024, 0, 13); // adding 1 to last index position based on user tip (https://forum.arduino.cc/index.php?topic=43727.0)
      synth.programChange(0, 0, voice[voice_0_index]);  // programChange(bank, channel, voice)
    }

    if (pot_mapped[4] - voice_1_index == 0 || actively_measure_voice[1] == true) {
      
      actively_measure_voice[1] = true;
      voice_1_index = map(pot_val[4], 0, 1024, 0, 13); 
      synth.programChange(0, 1, voice[voice_1_index]);  
    }

    if (pot_mapped[5] - voice_2_index == 0 || actively_measure_voice[2] == true) {
      
      actively_measure_voice[2] = true;
      voice_2_index = map(pot_val[5], 0, 1024, 0, 13); 
      synth.programChange(0, 2, voice[voice_2_index]);  
    } 

    // TURN ON LEDs - brightness matches instrument
    int led_4_level = map(voice_0_index, 0, 13, 0, 60); // full brightness = 255
    int led_5_level = map(voice_1_index, 0, 13, 0, 60);
    int led_6_level = map(voice_2_index, 0, 13, 0, 60);

    analogWrite(ledPin[3], led_4_level);   
    analogWrite(ledPin[4], led_5_level); 
    analogWrite(ledPin[5], led_6_level); 
  }
  else {
    // if one or more buttons pressed, don't change voices
    actively_measure_voice[0] = false;
    actively_measure_voice[1] = false;
    actively_measure_voice[2] = false;    
  }
}

///////////////////////////////////////////////

void changeScale() {
  // Get scale type based on pot 1 value when button 4 pressed
  // only start measuring once pot position is close to scale position
  // TODO: Turn off scale LEDS once not measuring scale
  
  if (buttonRead[3] == LOW) {

    // show what the current scale is
    leds_on(scale_type + 1);

    pot_mapped[0] = map(pot_val[0], 0, 1023, 0, 5);

    scale_threshold = pot_mapped[0] - scale_type;

    // if the pot value comes close to the old scale level (or if we are now actively measuring scale), adjust scale
    if (scale_threshold == 0 || actively_measure_scale == true) {

      actively_measure_scale = true;

      // change scale type
      scale_type = map(pot_val[0], 0, 1023, 0, 5);
  
      if(scale_type == 0) {scale_length = major_scale_len; leds_on(1);} 
      if(scale_type == 1) {scale_length = minor_scale_len; leds_on(2);}
      if(scale_type == 2) {scale_length = pent_major_scale_len; leds_on(3);}
      if(scale_type == 3) {scale_length = pent_minor_scale_len; leds_on(4);}
      if(scale_type == 4) {scale_length = blues_scale_len; leds_on(5);}
      if(scale_type == 5) {scale_length = chromatic_scale_len; leds_on(6);} 
      
      if (scale_type != scale_type_prev) {
        if(debug) {Serial.print("NEW SCALE_TYPE = "); Serial.println(scale_type);} 
        scale_type_prev = scale_type;
      }
    }
  }
  else {
    // if button 4 not pressed
    actively_measure_scale = false;
  } 
}

///////////////////////////////////////////////

void setNoteChangeDelay() {
  // change the 'note change' debounce time for the three channels. Slows down responsiveness of note changes.
  
  if (buttonRead[1] == LOW) {

    pot_mapped[3] = map(pot_val[3], 0, 1023, 10, 750); 
    pot_mapped[4] = map(pot_val[4], 0, 1023, 10, 750);
    pot_mapped[5] = map(pot_val[5], 0, 1023, 10, 750);

    if (abs(pot_mapped[3] - note_change_debounce[0]) < 5 || actively_measure_debouce[0] == true) {
      actively_measure_debouce[0] = true;
      note_change_debounce[0] = map(pot_val[3], 0, 1023, 10, 750); 
    }
    
    if (abs(pot_mapped[4] - note_change_debounce[1]) < 5 || actively_measure_debouce[1] == true) {
      actively_measure_debouce[1] = true;
      note_change_debounce[1] = map(pot_val[4], 0, 1023, 10, 750); 
    }
    
    if (abs(pot_mapped[5] - note_change_debounce[2]) < 5 || actively_measure_debouce[2] == true) {
      actively_measure_debouce[2] = true;
      note_change_debounce[2] = map(pot_val[5], 0, 1023, 10, 750); 
    }


    // TURN ON LEDs - brightness matches debouce level
    int led_4_level = map(note_change_debounce[0], 10, 750, 0, 60); // full brightness = 255
    int led_5_level = map(note_change_debounce[1], 10, 750, 0, 60);
    int led_6_level = map(note_change_debounce[2], 10, 750, 0, 60);

    analogWrite(ledPin[0], 0);   
    analogWrite(ledPin[1], 0); 
    analogWrite(ledPin[2], 0);
    analogWrite(ledPin[3], led_4_level);   
    analogWrite(ledPin[4], led_5_level); 
    analogWrite(ledPin[5], led_6_level); 
  }
  
  else {
    // if button 2 not pressed
    actively_measure_debouce[0] == false;
    actively_measure_debouce[1] == false;
    actively_measure_debouce[2] == false;
  }   
}

///////////////////////////////////////////////

void leds_on(int i) {
  switch (i) {
    case 1:
      analogWrite(ledPin[0], 127);
      analogWrite(ledPin[1], 0);
      analogWrite(ledPin[2], 0);
      analogWrite(ledPin[3], 0);
      analogWrite(ledPin[4], 0);
      analogWrite(ledPin[5], 0);
      break;
    case 2:
      analogWrite(ledPin[0], 127);
      analogWrite(ledPin[1], 127);
      analogWrite(ledPin[2], 0);
      analogWrite(ledPin[3], 0);
      analogWrite(ledPin[4], 0);
      analogWrite(ledPin[5], 0);
      break;
    case 3:
      analogWrite(ledPin[0], 127);
      analogWrite(ledPin[1], 127);
      analogWrite(ledPin[2], 127);
      analogWrite(ledPin[3], 0);
      analogWrite(ledPin[4], 0);
      analogWrite(ledPin[5], 0);
      break;
    case 4:
      analogWrite(ledPin[0], 127);
      analogWrite(ledPin[1], 127);
      analogWrite(ledPin[2], 127);
      analogWrite(ledPin[3], 127);
      analogWrite(ledPin[4], 0);
      analogWrite(ledPin[5], 0);
      break;
    case 5:
      analogWrite(ledPin[0], 127);
      analogWrite(ledPin[1], 127);
      analogWrite(ledPin[2], 127);
      analogWrite(ledPin[3], 127);
      analogWrite(ledPin[4], 127);
      analogWrite(ledPin[5], 0);
      break;
    case 6:
      analogWrite(ledPin[0], 127);
      analogWrite(ledPin[1], 127);
      analogWrite(ledPin[2], 127);
      analogWrite(ledPin[3], 127);
      analogWrite(ledPin[4], 127);
      analogWrite(ledPin[5], 127);
      break;
  }
}

void setNotes(int chan, int note_index) {
  // select scale notes
  switch (scale_type) {
    case 0:
      note[chan] = major[note_index];
      break;
    case 1:
      note[chan] = minor[note_index];
      break;
    case 2:
      note[chan] = pent_major[note_index];
      break;
    case 3:
      note[chan] = pent_minor[note_index];
      break;
    case 4:
      note[chan] = blues[note_index];
      break;
    case 5:
      note[chan] = chromatic[note_index];
      break;
  }
}



void changeReverb() {
  const int reverbProgram = 5;  
    // 0: room1
    // 1: room2
    // 2: room3
    // 3: hall1
    // 4: hall2
    // 5: plate
    // 6: delay  - works with delayFeedback
    // 7: pan delay - works with delayFeedback
  const int delayFeedback = 0;

  int pot_6_val = analogRead(potPin[5]);
  int reverbLevel = map(pot_6_val, 0, 1023, 0, 127); // reverb level goes up to 127 (conflicts with code ex that goes to 255)

  // set reveb for both channels
  synth.setReverb(0, reverbProgram, reverbLevel, delayFeedback);
  synth.setReverb(1, reverbProgram, reverbLevel, delayFeedback);
}

void changeChorus() {
  const int chorusProgram = 5;  
    // 0: Chorus 1
    // 1: Chorus 2
    // 2: Chorus 3
    // 3: Chorus 4
    // 4: Feedback
    // 5: Flanger
    // 6: Short Delay  -- works with chorusFeedback
    // 7: Feedback Delay -- works with chorusFeedbck
  const int chorusFeedback = 0;

  int chorusLevel = map(pot_val[3], 0, 1023, 0, 127); // unclear in documentation if goes to 127 or 255
  int chorusDelay = map(pot_val[4], 0, 1023, 0, 127); // unclear in documentation if goes to 127 or 255

  // set chorus for both channels
  synth.setChorus(0, chorusProgram, chorusLevel, chorusFeedback, chorusDelay);
  synth.setChorus(1, chorusProgram, chorusLevel, chorusFeedback, chorusDelay);
}

void changeVolume() {

  // if button 1 is pressed pots 1-3 adjust volume for channels 1-3, respectively
   
  if (buttonRead[0] == LOW) {
    
    pot_mapped[0] = map(pot_val[0], 0, 1023, 0, 100); // 0-127, but too loud over 100
    pot_mapped[1] = map(pot_val[1], 0, 1023, 0, 100);
    pot_mapped[2] = map(pot_val[2], 0, 1023, 0, 100);

    // if the pot value comes close to the old volume level (or if we are now actively measuring volume), adjust volume
    if (abs(pot_mapped[0] - channel_volume[0]) < 3 || actively_measure_vol[0] == true) {
      
      actively_measure_vol[0] = true; // keep actively measuring once threshold triggered
      
      channel_volume[0] = map(pot_val[0], 0, 1023, 0, 100);
      synth.setChannelVolume(0, channel_volume[0]);
      
      if(debug) {Serial.print("Volume for voice 0 Adjusted!! Vol 1 = ");Serial.println(channel_volume[0]);}
    }
      
    
    if ((abs(pot_mapped[1] - channel_volume[1])) < 3 || actively_measure_vol[1] == true) {
      actively_measure_vol[1] = true; // keep actively measuring once threshold triggered
      channel_volume[1] = map(pot_val[1], 0, 1023, 0, 100);
      synth.setChannelVolume(1, channel_volume[1]);
    }

    if ((abs(pot_mapped[2] - channel_volume[2])) < 3 || actively_measure_vol[2] == true) {
      actively_measure_vol[2] = true; // keep actively measuring once threshold triggered
      channel_volume[2] = map(pot_val[2], 0, 1023, 0, 100);
      synth.setChannelVolume(1, channel_volume[2]);
    }

    // TURN ON LEDs - brightness matches volume  
    analogWrite(ledPin[0], channel_volume[0]);   
    analogWrite(ledPin[1], channel_volume[1]); 
    analogWrite(ledPin[2], channel_volume[2]); 
    analogWrite(ledPin[3], 0);   
    analogWrite(ledPin[4], 0); 
    analogWrite(ledPin[5], 0);   
    
  }
  else {
    // if button not pressed
    actively_measure_vol[0] = false;
    actively_measure_vol[1] = false;
    actively_measure_vol[2] = false;
  } 
}

void startupSequence() {
  // START UP SEQUENCE
  if(debug) {Serial.print("///// START UP SEQUECE STARTED \\\\\ "); Serial.println();}
  // start quiet
  synth.setChannelVolume(0, 0);  
  synth.setChannelVolume(1, 0); 
  synth.setChannelVolume(1, 0); 

  // set startup voice to organ
  for (int i = 0; i < 3; i++) {
    synth.programChange(0, i, voice[0]); // (bank, channel, voice) // start with voice[0] - rock organ
  }

  synth.noteOn(0, 60, 127);
  synth.noteOn(1, 65, 127);
  synth.noteOn(2, 69, 127);

  // then fade up
  for (int i = 0; i < 90; i++) {
    synth.setChannelVolume(0, i);  
    synth.setChannelVolume(1, i); 
    synth.setChannelVolume(1, i); 

    analogWrite(ledPin[0], i);   
    analogWrite(ledPin[1], i); 
    analogWrite(ledPin[2], i);  
    analogWrite(ledPin[3], i);   
    analogWrite(ledPin[4], i); 
    analogWrite(ledPin[5], i); 
    
    delay(5);
  }

  delay(1000);

  // turn off notes
  synth.noteOn(0, 60, 0);
  synth.noteOn(1, 65, 0);
  synth.noteOn(2, 69, 0);
  
  if(debug) {Serial.print("///// START UP SEQUECE ENDED \\\\\ "); Serial.println();}
}
