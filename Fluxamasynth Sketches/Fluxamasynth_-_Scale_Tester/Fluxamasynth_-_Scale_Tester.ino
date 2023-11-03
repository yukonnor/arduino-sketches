/* SCALE TESTER
 *  Tests selecting and applying different scales
 *  - Pot 1 controls note of voice 1
 *  - Pot 1 controls scale 
 *  - Pot 3 controls note of voice 2
 *  - Pot 4 controls reverb level
 *  - Pot 5 controls chorus/flanger level
 *  - Pot 6 controls chorus/flanger delay
 *  - Changing notes triggers the note to play (for TBD seconds - maybe controlled by pot) + flashes LED 1 or 3
 *  - Changing scale prints what scale is currently selected / flashes LEDS
 *  - Up / Down buttons cycle through instruments, starting at instrument 0. Print the current instrument number
 *  - Holding Button 1: Pot 4 and Pot 6 adjust volume / Pot 5 adjust note_change_debounce
 *  
 *  GLOBAL:
 *  - Scale (button 2 pressed - pot 2 adjusts)
 *  - FX (button 4 pressed - all 4 pots adjust - chorus top 3, reverb bottom 3)
 *  - 
 *  
 *  TRACK-BASED:
 *  - Note (no buttons pressed, default state - top 3 pots)
 *  - Instrument / Voice (no buttons pressed, default state - bottom 3 pots)
 *  - Max Volume (button 1 pressed, top 3 pots adjust)
 *  - Note Change Debounce / Delay (button 3 pressed - top 3 pots adjust
 *  - Maybe: Volume LFO Strength, Speed (button 5 pressed - top 3 adjust strength, bottom 3 speed)
 *  
 *  TODO: fix when pot value gets stuck between two notes
 *        - maybe only play note if note is different and changed after an amount of time
 *        - built a nice startup sequence so doesn't start at full volume
 */

#include <Fluxamasynth.h>
 
Fluxamasynth synth;

// Define Scale Arrays (all in key of C covering 4 octaves)
int scale_type = 0;
int scale_type_prev = 0;
int scale_length = 0;
int note_0_index = 0;
int note_1_index = 0;
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

// Define Pot pins
const int potPin[6] = { A3, A2, A4, A5, A1, A0 };
int pot_4_start = 0;
int pot_6_start = 0;
int vol_cycle_count = 0;


// Define PWM LED pins
const int ledPin[6] = { 11, 6, 5, 10, 9, 3 };

// Define Digital Button pins
const int buttonPin[6] = { 2, 13, 0, 7, 8, 12 };
bool buttonRead[6];

unsigned long current_time;
const int debounce = 200;   //20 for debug - 200 for production
unsigned long last_debounce_time;

// Define envelope stages
bool attack = false;
bool decay = false;
bool sustain = false;
float attack_rate = 0.2;  // needs to be above 0. High values = fast attack, low values = slow.
float attack_value = 0;   // converts attack_rate to float to be used with volumne
float decay_rate = 0.2;   // needs to be above 0. High values = fast decay, low values = slow.
float decay_value = 0;    // converts decay_rate to float to be used with volumne

float attack_rate_prev = 0.0;
float decay_rate_prev = 0.0;
 
// Sound engine
int note_0 = 60;
int note_1 = 60;
int note_0_prev = 60;
int note_1_prev = 60;
int instrument = 19;   // starting instrument - rock organ
int bank = 0;
int vel = 127;
int volume_0 = 0;
int volume_1 = 0;
bool actively_measure_vol_0 = false;
bool actively_measure_vol_1 = false;

long last_note_0_chagge_time = 0;
long last_note_1_chagge_time = 0;
int note_change_debounce = 500;    // playing with a long debounce time

bool debug = true;


void setup() {
  if(debug) {Serial.begin(9600);}
  
  synth.setMasterVolume(90);   
  synth.setChannelVolume(0, 90);  
  synth.setChannelVolume(1, 90); 
  //synth.allNotesOff(channel);   

  // set starting voice
  instrument = 19;                        // rock organ
  synth.programChange(0, 0, instrument);  // (bank, channel, voice)
  synth.programChange(0, 1, instrument);

  // set reverb
  synth.setReverb(0,5,255,100);
  synth.setReverb(1,5,255,100);
  
  // SET PIN MODES
  // Button pins
  for (int i=0; i<6; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  // LED pins
  for (int i=0; i<6; i++) {
    pinMode(ledPin[i],OUTPUT);
  }
  
}

void loop() {
  //Get the current time
  current_time = millis();

  readButtons();
  changeVoice();
  changeReverb();
  changeChorus();
  setVolume();

  // Get scale type based on pot 2 value
  int scalePotRead = analogRead(potPin[1]);  // 0 - 1023
  if( 0 < scalePotRead)  {scale_type = 0; scale_length = major_scale_len;}  // note: scale length is over index pos. by 1 to help with mapping 
  if(150 < scalePotRead) {scale_type = 1; scale_length = minor_scale_len;}
  if(300 < scalePotRead) {scale_type = 2; scale_length = pent_major_scale_len;}
  if(500 < scalePotRead) {scale_type = 3; scale_length = pent_minor_scale_len;}
  if(750 < scalePotRead) {scale_type = 4; scale_length = blues_scale_len;}
  if(1000 < scalePotRead){scale_type = 5; scale_length = chromatic_scale_len;}  

  if (scale_type != scale_type_prev) {
    if(debug) {Serial.print("SCALE_TYPE = "); Serial.println(scale_type);} 
    scale_type_prev = scale_type;
  }
  

  // select scale notes
  switch (scale_type) {
    case 0:
      getPitchValues();
      note_0 = major[note_0_index];
      note_1 = major[note_1_index]; 
      break;
    case 1:
      getPitchValues();
      note_0 = minor[note_0_index];
      note_1 = minor[note_1_index]; 
      break;
    case 2:
      getPitchValues();
      note_0 = pent_major[note_0_index];
      note_1 = pent_major[note_1_index]; 
      break;
    case 3:
      getPitchValues();
      note_0 = pent_minor[note_0_index];
      note_1 = pent_minor[note_1_index]; 
      break;
    case 4:
      getPitchValues();
      note_0 = blues[note_0_index];
      note_1 = blues[note_1_index]; 
      break;
    case 5:
      getPitchValues();
      note_0 = chromatic[note_0_index];
      note_1 = chromatic[note_1_index]; 
      break;
  }

  // PLAY SOUNDS
  // if note changes after debounce time, end previous note and start new note
  // (maybe todo) if note doesn't change, end current note after 5 seconds
  if(note_0 != note_0_prev && current_time - last_note_0_chagge_time > note_change_debounce) {
    synth.noteOff(0, note_0_prev); 
    synth.noteOn(0, note_0, 127);

    if(debug) {Serial.print("///// NOTE 0 IS PLAYING "); Serial.print(note_0); Serial.println(" ///// ");} 
    
    note_0_prev = note_0;
    last_note_0_chagge_time = current_time;
  }

  if(note_1 != note_1_prev && current_time - last_note_1_chagge_time > note_change_debounce) {
    synth.noteOff(1, note_1_prev); 
    synth.noteOn(1, note_1, 127);

    if(debug) {Serial.print("///// NOTE 1 IS PLAYING "); Serial.print(note_1); Serial.println(" ///// ");}
    
    note_1_prev = note_1;
    last_note_1_chagge_time = current_time;
  }
  
  // TURN ON LEDs
  // brightness matches pitch
  int low_note = 36;
  int high_note = 83;
  int led_1_level = map(note_0, low_note, high_note, 0, 127); // full brightness = 255
  int led_2_level = map(note_1, low_note, high_note, 0, 127);

  //Serial.print("led_1_level = "); Serial.println(led_1_level); 
  //Serial.print("led_2_level = "); Serial.println(led_2_level); 
  analogWrite(ledPin[0], led_1_level);   
  analogWrite(ledPin[2], led_2_level);   

  delay(1);

}

void changeVoice() {
  // if UP button pressed, move to next voice
  if (buttonRead[1] == LOW && millis() - last_debounce_time > debounce) {
      instrument++;
      if ( instrument == 128) { instrument = 0; }
      
      // set the new voice for both channels
      synth.programChange(0, 0, instrument);
      synth.programChange(0, 1, instrument);
      
      if(debug) {Serial.print("Voice is set to Bank: "); Serial.print(bank); Serial.print(" Voice: "); Serial.println(instrument);} 
      last_debounce_time = millis();  
   }

   // if DOWN button pressed, move to previous voice
   if (buttonRead[4] == LOW && millis() - last_debounce_time > debounce) {
      instrument--;
      if ( instrument <= 0) { instrument = 127; }

      // set the new voice
      synth.programChange(0, 0, instrument);
      synth.programChange(0, 1, instrument);
   
      if(debug) {Serial.print("Voice is set to Bank: "); Serial.print(bank); Serial.print(" Voice: "); Serial.println(instrument);} 
      last_debounce_time = millis();  
   }

}

void getPitchValues() {
  // Read note pots (1 and 3)
  int pot_0_val = analogRead(potPin[0]);
  note_0_index = map(pot_0_val, 0, 1024, 0, scale_length); // adding 1 to last index position (ex 0-27) to map values based on user tip (https://forum.arduino.cc/index.php?topic=43727.0)
  int pot_2_val = analogRead(potPin[2]);
  note_1_index = map(pot_2_val, 0, 1024, 0, scale_length); 

  //Serial.print("NOTE 0 INDEX POSITION = "); Serial.println(note_0_index); 
  //Serial.print("NOTE 1 INDEX POSITION = "); Serial.println(note_1_index); 
}

void readButtons() {
  // Read ALL button values (NOTE: Button press == LOW)
  for (int i=0; i<6; i++) {
    buttonRead[i] = digitalRead(buttonPin[i]);
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

  int pot_4_val = analogRead(potPin[3]);
  int chorusLevel = map(pot_4_val, 0, 1023, 0, 127); // unclear in documentation if goes to 127 or 255

  int pot_5_val = analogRead(potPin[4]);
  int chorusDelay = map(pot_5_val, 0, 1023, 0, 127); // unclear in documentation if goes to 127 or 255

  // set chorus for both channels
  synth.setChorus(0, chorusProgram, chorusLevel, chorusFeedback, chorusDelay);
  synth.setChorus(1, chorusProgram, chorusLevel, chorusFeedback, chorusDelay);
}

void setVolume() {

  // if button 1 is pressed pot 4 adjusts voice 0 volume and pot 6 asjusts voice 1 volume
  // Option 2: Change volume level only if pot value reaches last volume level (MOVE TO THIS OPTION)
  //           Log what the currently set volume levels are for each channel
  //           When the button is pressed start to measure what the pot value mapped to volume level is
  //           Only start to change the volume once the currently measured volume level matches the actual volume level
  //           Once we start to change the volume keep on changing it
  // if button pressed:
  //    if ( pot within x of last value OR actively measure == true):
  //       actively measure = true;
  //       measure pot value and adjust volume
  // else (actively_measure = false)
   
  if (buttonRead[0] == LOW) {
    
    int pot_4_current = analogRead(potPin[3]);
    int pot_6_current = analogRead(potPin[5]);

    int pot_mapped_level_4 = map(pot_4_current, 0, 1023, 0, 127);
    int pot_mapped_level_6 = map(pot_6_current, 0, 1023, 0, 127);

    // if the pot value comes close to the old volume level (or if we are now actively measuring volume), adjust volume
    if ((abs(pot_mapped_level_4 - volume_0)) < 3 || actively_measure_vol_0 == true) {
      
      actively_measure_vol_0 = true; // keep actively measuring once threshold triggered
      
      volume_0 = map(pot_4_current, 0, 1023, 0, 127);
      synth.setChannelVolume(0, volume_0);
      if(debug) {Serial.print("Volume for voice 0 Adjusted!! Vol 1 = ");Serial.println(volume_0);}
    }
      
    
    if ((abs(pot_mapped_level_6 - volume_1)) < 3 || actively_measure_vol_1 == true) {
      
      actively_measure_vol_1 = true; // keep actively measuring once threshold triggered
      
      volume_1 = map(pot_6_current, 0, 1023, 0, 127);
      synth.setChannelVolume(1, volume_1);
      
      //if(debug) {Serial.print("Volume for voice 1 Adjusted!! Vol 1 = ");Serial.println(volume_1);}
    }
  }
  else {
    // if button not pressed
    actively_measure_vol_0 = false;
    actively_measure_vol_1 = false;
  } 
}

void setNoteChangeDebounce() {

  // If button 1 is pressed pot 5 adjusts the note change debounce time
    
  if (buttonRead[0] == LOW) {
    
    int pot_5_current = analogRead(potPin[4]);
    int pot_mapped_level_5 = map(pot_4_current, 0, 1023, 0, 1000);
    
    // if the pot value comes close to the old debounce level (or if we are now actively measuring volume), adjust volume
    if ((abs(pot_mapped_level_5 - note_change_debounce)) < 3 || actively_measure_debounce == true) {
      
      actively_measure_debounce = true; // keep actively measuring once threshold triggered
      
      note_change_debounce = map(pot_4_current, 0, 1023, 0, 1000);
      if(debug) {Serial.print("Note Change Debounce Changed!! Debounce = ");Serial.println(note_change_debounce);}
    }   
  }
  else {
    // if button not pressed
    actively_measure_debounce = false;
  } 
}
