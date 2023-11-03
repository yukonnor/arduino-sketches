#include <Fluxamasynth.h>
 
Fluxamasynth synth;

byte instrument = 0;
int loop_counter = 0;
 
void setup() {
  synth.setMasterVolume(255);   
}


/*
// CHANGE VOLUME BEFORE NOTES CREATED
void loop()
{
  for (int note=60; note<85; note++) {
    synth.noteOn(0, note, 100);  
    delay(200);
    synth.noteOff(0, note);

    int volume = random(127);
    synth.setChannelVolume(1, volume);
    synth.noteOn(1, note, 100);
    synth.noteOn(1, note+3, 100);  
    synth.noteOn(1, note+7, 100);
    delay(200);
    synth.noteOff(1, note);
    synth.noteOff(1, note+3);
    synth.noteOff(1, note+7);
  }
}
*/

// CHANGE VOLUME WHILE NOTE PLAYING
void loop() {

  int note = 60;
  loop_counter++;

  // turn on notes
  if (loop_counter == 1) {
    synth.noteOn(1, note, 100);
    synth.noteOn(1, note+3, 100);  
    synth.noteOn(1, note+7, 100);
  }

  // update volume of channel every 500 loops
  if (loop_counter % 500 == 0){
    int volume = random(20,127);
    synth.setChannelVolume(1, volume);
  }
  

  // turn off notes
  if (loop_counter == 5000) {
    synth.noteOff(1, note);
    synth.noteOff(1, note+3);
    synth.noteOff(1, note+7);
    loop_counter = 0;
  }
  
}
