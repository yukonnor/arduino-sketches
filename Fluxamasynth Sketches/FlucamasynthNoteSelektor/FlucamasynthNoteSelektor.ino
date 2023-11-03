/* 
*  Allows you to cycle through Fluxamasynth's tones and
*  read which tone you are currently on. Play the tone 
*  and adjust the FX to sample the sounds that tone can create.
*  
*  Mad to use with concon's diy video shield
*/

#include "Fluxamasynth.h"

Fluxamasynth synth;

int bank = 0;                   // Bank value (0, 127)
int inst = 1;                   // Instrument value (1 - 128)
int pitch = 60;                 // middle C

//Define Analog Potentiometer Pins
const int pot1Pin = 0;
const int pot2Pin = 2; //note here that the stripboard wires cross, hence out of order
const int pot3Pin = 1;
const int pot4Pin = 3;

//Define Digial Button Pins
const int button1Pin = 13;
const int button2Pin = 12;
const int button3Pin = 11;
const int button4Pin = 10;

//Initialize button value variables:
int button1 = 0;
int button2 = 0;
int button3 = 0;
int button4 = 0;

int buttonRead1;
int buttonRead2;
int buttonRead3;
int buttonRead4;

int pot1;
int pot2;

bool note_on = 0;


int seq_length = 4;
int SEQ_1[] = {60,63,65,60};
int SEQ_2[] = {55,48,51,1};

void setup()
{
  // Serial1.begin(31250);     // Uncomment if using a Feather M0 board
  Serial.begin(9600); 
  synth.midiReset();
  synth.programChange(127, 0, 91);
  synth.pitchBendRange(0, 4);  //double the pitch bend sensitivity (4 semitones)

  //Set button pins to use internal Pullup resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);
}

void loop()
{
  //Read button values:
  //NOTE: Button press == LOW
  buttonRead1 = digitalRead(button1Pin);
  buttonRead2 = digitalRead(button2Pin);
  buttonRead3 = digitalRead(button3Pin);
  buttonRead4 = digitalRead(button4Pin);

  int potRead1 = analogRead(pot1Pin);
  pot1 = map(potRead1,0,1023,0,127);
  int potRead2 = analogRead(pot2Pin);
  pot2 = map(potRead2,0,1023,0,127);
  int potRead3 = analogRead(pot3Pin);
  int potRead4 = analogRead(pot4Pin);
  
  //programChange(bank, channel, voice) -- channel will always be 0 for this sketch
  //noteOn(channel, pitch, velocity) --Pitch is  0 - 127. 60 = middle C (C4) = 261.63Hz. 

  //if button 1 pressed, switch bank
  if (buttonRead1 == LOW) {
     if(bank == 1) {bank = 127;}
     else {bank = 1;}
     
     synth.setReverb(0, 3, pot1, 0);
     synth.setChorus(0, 4, 127, 0, 0);
     synth.setChorus(1, 7, 127, 30, 127);
     delay(20); //hacky debouncing as nothing's going on here
  }

  //if button 2 pressed, increase instrument value
  if (buttonRead2 == LOW) {
     if(inst != 128) {inst = ++inst;}
     else if (inst == 128) {inst = 1;}
     delay(20); //hacky debouncing as nothing's going on here
  }

  //printCurrentTone();

  //if button 3 pressed, play tone until released
  synth.programChange(bank, 0, inst);
  synth.programChange(1, 1, 40);
  if(buttonRead3 == LOW) {
    synth.setReverb(0, 3, pot1, 0);
    for(int i=0; i<seq_length; ++i){
      synth.noteOn(0, SEQ_1[i], 40);
      synth.noteOn(1, SEQ_2[i], 40);
      delay(200);
      synth.noteOn(0, SEQ_1[i], 0);
      synth.noteOn(1, SEQ_2[i], 0);
    }
  }  
}

void printCurrentTone(){
  Serial.print("Bank #");
  Serial.print(bank);
  Serial.print("     Instrument #");
  Serial.print(inst);
  Serial.print("     Button 1 Reads ");
  Serial.print(buttonRead1);
  Serial.print("     Button 2 Reads ");
  Serial.print(buttonRead2);
  Serial.print("     Button 3 Reads ");
  Serial.println(buttonRead3);
  Serial.println(); Serial.println(); Serial.println(); Serial.println(); Serial.println();
  Serial.println(); Serial.println(); Serial.println(); Serial.println();
}

