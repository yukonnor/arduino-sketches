#include "Fluxamasynth.h"
Fluxamasynth synth;

// Eno's Music for Airports #2: F2 G#2 C3 C#3 D#3 F3 G#3

// Define Pot pins
const int potPin[6] = { A3, A2, A4, A5, A1, A0 };

// Define PWM LED pins
const int ledPin[6] = { 11, 6, 5, 10, 9, 3 };

// Store the output values of the LEDs
int led_val[6];

int tones[6] = { 29, 32, 36, 37, 39, 41 };        // aka the Pitch: F2 G#2 C3 C#3 D#3 F3 G#3
int instruments[6] = { 55, 55, 55, 55, 55, 55 };  // which instrument is used (all start at 55 aka Cello)
int tapeLength[6] = { 30, 31, 32, 33, 34, 35  };  // not used?
int startingPoint[6] = { 0, 1, 2, 3, 4, 5 };      // not used?
int length[6] = { 100, 110, 120, 130, 140, 150 }; // how long the instrument sounds for during each loop (static / hard coded)
int lengthCount[6] = { 0, 0, 0, 0, 0, 0 };        // keeps tracks of how many cycles have gone through in order to turn off sound of each instrument after {length} cycles
int tempo[6] = { 1000, 2000, 3000, 4000, 5000, 6000 };  // Loop length. Adjusted by the pots. 
int tempoCount[6] = { 0, 0, 0, 0, 0, 0 };
long int startTime = 0;

void setup() {
  Serial.begin(9600);
  int seed = 0;
  for (int i=0; i<6; i++) {
    seed += analogRead(i);
  }

  randomSeed(seed);
  delay(100);
  for (int i=0; i<6; i++) {
    synth.setChannelVolume(i, 255 );  
    synth.allNotesOff(i);    
    synth.setReverb(i, 5, 64, 0);
    pan(5-i,127/(6*i));
  }
  changeProgram();
  startTime = 0;
}

void changeProgram() {
   for (int i=0; i<6; i++) {
      synth.programChange(0, i,random(127));
   }
}

void loop() {
  if (millis() > (startTime + 100000)) {
    changeProgram();
    startTime = millis();
    Serial.println("changeProgram");
  }
  
  int ledTempo = 0;
  
  for (int i=0; i<6; i++) {
    int val = analogRead(potPin[i]);
    tempo[i] = 5100 - map(val, 0, 1023, 100, 5000); 
    tempoCount[i]++;
    if (tempoCount[i] > tempo[i]) {
      if (lengthCount[i] == 0) {
        synth.noteOn(i, tones[i], 127);
        analogWrite(ledPin[i], 100);      // turn LED on
      }
      lengthCount[i]++;
      if (lengthCount[i] > length[i]) {
        synth.noteOff(i, tones[i]);
        analogWrite(ledPin[i], 0);        // turn LED off
        lengthCount[i] = 0;
        tempoCount[i] = 0;
      }
    }
 
  }  
  delay(1);
}

void pan(int channel, int value) {
  byte command[3] = { (0xb0 | (channel & 0x0f)), 0x0A, (value) };
  synth.fluxWrite(command, 3);
}
