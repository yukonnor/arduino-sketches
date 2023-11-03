#include <Fluxamasynth.h>
 
Fluxamasynth synth;

// Tests the different sounds of the Fluxamasynth
// Up / Down buttons cycle through instruments, starting at instrument 0. Print the current instrument number
// Button 3 changes bank: bank 0 on = led 4 on / bank 127 on = led 5 on
// Button 1 triggers the current sound (starts the envelope), button 4 turns it off
// Pot 1 controls attack length
// Pot 2 controls decay length
// Pot 3 controls pitch
// Pot 4 controls chorus level
// Pot 5 controls chorus delay
// Pot 6 controls reverb level


// Define Pot pins
const int potPin[6] = { A3, A2, A4, A5, A1, A0 };

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
const int bank = 0;
const int channel[6] = { 0, 1, 2, 3, 4, 5 };
const int instrument[6] = { 1, 1, 1, 1, 1, 1 };
int vel[6] = { 127, 127, 127, 127, 127, 127 };
int volume = 255;

// Sound sequences
seq_note[6][6] = { { 60, 60, 60, 60, 60, 60 },  
                   { 60, 60, 60, 60, 60, 60 },
                   { 60, 60, 60, 60, 60, 60 },
                   { 60, 60, 60, 60, 60, 60 },
                   { 60, 60, 60, 60, 60, 60 },
                   { 60, 60, 60, 60, 60, 60 },
                   };
seq_lenth[6]; // how long the sequence is for each channel (1-6)

// Scales
scale[2][127] = {

void setup() {
  Serial.begin(9600);
  
  synth.setMasterVolume(255);   
  synth.setChannelVolume(0, 255);  
  //synth.allNotesOff(channel);   

  
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

  //Read button values (NOTE: Button press == LOW)
  for (int i=0; i<6; i++) {
    buttonRead[i] = digitalRead(buttonPin[i]);
  }

  
  changePitch(); // NOTE: changing pitch while note is playing means that noteOff doesn't catch it! Note keeps playing.

  // Read envelope pots
  int pot_0_val = analogRead(potPin[0]);
  attack_rate = (map(pot_0_val, 0, 1023, 12700, 2)) / 100.0; // convert mapped value to float (127 - 0.02)
  int pot_1_val = analogRead(potPin[1]);
  decay_rate = (map(pot_1_val, 0, 1023, 12700, 2)) / 100.0; // convert mapped value to float (127 - 0.02)

  
  /*
   * Print attack and decay rates
   * TODO: Make slower attack rates more granular: pot halfway shoud be at 1.0
  
  if (attack_rate != attack_rate_prev) {
    Serial.print("Attack rate is: "); Serial.println(attack_rate);
    attack_rate_prev = attack_rate;
  }

  if (decay_rate != decay_rate_prev) {
    Serial.print("Decay rate is: "); Serial.println(decay_rate);
    decay_rate_prev = decay_rate;
  }
  */ 


  // Hold note on when Button 1 pressed
  // Start decay when released
  if (buttonRead[0] == LOW) {
    // if attack or sustain already running, do nothing
    if (attack || sustain) {
      // do nothing
    }
    // if attack or sustain not running, stop note (if in decay) and restart note & attack phase
    else {
      // turn off note if currently running
      decay = false;
      volume = 0;
      attack_value = 0;
      decay_value = 0;
      synth.setChannelVolume(0, volume);
      synth.noteOff(0, note); 
      
      // turn on note and turn on attack phase
      synth.noteOn(0, note, 127);
      attack = true;   

      // print messages
      Serial.println("Note triggered! Starting Attack!");
      Serial.print("Attack rate is: "); Serial.println(attack_rate);
      Serial.print("Volume is: "); Serial.println(volume);
    }
  }
 
  
  // if envelope set to attack
  if (attack) {
    // Increase volume (TODO: at interval based on value of pot 1)
    // attaack_rate - if pot value is high, do a low offset (less than 1)
    //              - if pot valiue is low, do a high offset (min value --> offset is 127 to jump right to max volumne)
    // add increate rate to increase value (convert float to int)
    
    attack_value += attack_rate;
    volume = attack_value;

    Serial.print("Volume is: "); Serial.println(volume);
    
    // when we hit max volume, stop attack and start sustain:
    if ( volume >= 127 ) {
      volume = 127;
      attack = false;
      sustain = true;
      
      attack_value = 0; // reset attack_value for next time attack is triggered
     
      Serial.println("Starting sustain!");
    }   
  }
  else if (sustain) {

    Serial.print("Sustaining volume at: "); Serial.println(volume);
    
    // if button released, end sustain and start decay
    if (buttonRead[0] == HIGH) {
      sustain = false;
      decay = true;
      
      decay_value = volume;  // decay volume start at whatever the current volume is
      
      Serial.println("Starting decay!");
      Serial.print("Decay rate is: "); Serial.println(decay_rate);
    }
  }
  else if (decay) {
  // Decrease volume (TODO: at interval based on value of pot 2)
  // subtract decrease rate to decreast value (convert float to int)
  
    decay_value -= decay_rate;
    volume = decay_value;

    Serial.print("Volume is: "); Serial.println(volume);
    
    // when we hit min volume, stop decay phase and turn off note:
    if ( volume <= 0 ) {
      volume = 0;
      decay_value = 0;
      synth.noteOff(0, note); 
      decay = false;
      Serial.println("Decay finished!");
    }     
  }

  // adjust volume based on envelope
  synth.setChannelVolume(0, volume);
  // turn on LED1, brightness matches volume
  analogWrite(ledPin[0], volume);   

  delay(1);

}

void changeVoice() {
  // if UP button pressed, move to next voice
  if (buttonRead[1] == LOW && millis() - last_debounce_time > debounce) {
      instrument++;
      if ( instrument == 128) { instrument = 0; }
      
      // set the new voice
      synth.programChange(bank, 0, instrument);
      
      Serial.print("Voice is set to Bank: "); Serial.print(bank); Serial.print(" Voice: "); Serial.println(instrument); 
      last_debounce_time = millis();  
   }

   // if DOWN button pressed, move to previous voice
   if (buttonRead[4] == LOW && millis() - last_debounce_time > debounce) {
      instrument--;
      if ( instrument <= 0) { instrument = 127; }

      // set the new voice
      synth.programChange(bank, 0, instrument);
   
      Serial.print("Voice is set to Bank: "); Serial.print(bank); Serial.print(" Voice: "); Serial.println(instrument); 
      last_debounce_time = millis();  
   }

}

void changeBank() {
  // if button 3 pressed, change bank value
  if (buttonRead[2] == LOW && millis() - last_debounce_time > debounce) {
      if (bank == 0) {
        bank = 127;
      }
      else {
        bank = 0;
      }
      
      Serial.print("Voice is set to Bank: "); Serial.print(bank); Serial.print(" Voice: "); Serial.println(instrument); 
      last_debounce_time = millis();  
  }
  
  // change the program to new bank value
  synth.programChange(bank, 0, instrument);
}

void changePitch() {

  int pot_3_val = analogRead(potPin[2]);
  note = map(pot_3_val, 0, 1023, 0, 127); 
  
}

void setReverb() {
  
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
  int reverbLevel = map(pot_6_val, 0, 1023, 0, 255); // unclear in documentation if goes to 127 or 255
   
  synth.setReverb(0, reverbProgram, reverbLevel, delayFeedback);
  
}

void setChorus() {
  
  const int chorusProgram = 3;  
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
  int chorusLevel = map(pot_4_val, 0, 1023, 0, 255); // unclear in documentation if goes to 127 or 255

  int pot_5_val = analogRead(potPin[4]);
  int chorusDelay = map(pot_5_val, 0, 1023, 0, 255); // unclear in documentation if goes to 127 or 255
   
  synth.setChorus(0, chorusProgram, chorusLevel, chorusFeedback, chorusDelay);
  
}
