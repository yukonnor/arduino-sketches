#include <Fluxamasynth.h>
 
Fluxamasynth synth;

byte instrument = 0;
 
void setup() {
  synth.setMasterVolume(255);   
}


void loop()
{
  for (int note=60; note<85; note++) {
    synth.noteOn(0, note, 100);  
    delay(20000);
    synth.noteOn(0, note, 0);    // Be sure to turn the note off!
  }
}



/*
void loop()
{
  for (int note=60; note<85; note++) {
    synth.noteOn(0, note, 100);  
    delay(200);
    synth.noteOff(0, note);
  
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

/*
void loop()
{
  for (int note=60; note<85; note++) {
    synth.programChange(0, 0, instrument);
    synth.programChange(0, 1, instrument);
    synth.noteOn(0, note, 100);  
    delay(200);
    synth.noteOff(0, note);
  
    synth.noteOn(1, note, 100);
    synth.noteOn(1, note+3, 100);  
    synth.noteOn(1, note+7, 100);
    delay(200);
    synth.noteOff(1, note);
    synth.noteOff(1, note+3);
    synth.noteOff(1, note+7);
    instrument = (instrument+1) & 127;
  }
}
*/
