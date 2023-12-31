#include <SPI.h>
#include <GD.h>
//#include <pitches.h>

const int SINE = 0;
const int NUM_POTS = 5;
const int STEP = 2;

typedef unsigned char prog_uchar;

unsigned long time_A;
unsigned long time_A_previous = 0;
unsigned long interval_A;
int i = 0; 

void setup()
{
    //Initialize the Gameduino
    GD.begin();
    Serial.begin(9600);
}

void loop()
{
    //Get the current time
    unsigned long current_time = millis();
    

    int pot_frequency_0 = analogRead(0); //0 to 1023
    int pot_frequency_1 = analogRead(1);
    int pot_frequency_2 = analogRead(2);

   
    //cycle Pot0 values to  to be notes A/C/E
    int pot_position_0 = random(6);//PotSwitch_6(pot_frequency_0);
    int note_0  = ValuetoNote_0(pot_position_0);
     
    //cycle Pot1 values to be notes C/E/G
    int pot_position_1 = random(6);//PotSwitch_6(pot_frequency_1);
    int note_1  = ValuetoNote_1(pot_position_1);
    
    //cycle Pot2 values to be notes G/B/D
    int pot_position_2 = random(6);//PotSwitch_6(pot_frequency_2);
    int note_2  = ValuetoNote_2(pot_position_2);

    //LFO!
    int pot_frequency_4 = analogRead(4);
    int EG_att_rate = map(pot_frequency_4, 0, 1023, 100, 3000); //microseconds
    int pot_frequency_5 = analogRead(5);
    int EG_dec_rate = map(pot_frequency_5, 0, 1023, 100, 3000); //microseconds

    
    //EG Attack -- This works with delay as I don't want down to start before up ends
    for (int i = 0; i<50; i++){
    GD.voice(0, SINE, note_0, i,i/2);
    GD.voice(1, SINE, note_1, i/2,i); 
    GD.voice(2, SINE, note_2, i,i/2);

    pot_frequency_4 = analogRead(4);
    EG_att_rate = map(pot_frequency_4, 0, 1023, 100, 3000);
    //i needs to increase by 1 every interval/50
    delayMicroseconds(EG_att_rate);   //lfo or eg rate set by delay time 
    }

    //EG Decay
    for (int i = 50; i>0; i--){
    GD.voice(0, SINE, note_0, i,i/2);
    GD.voice(1, SINE, note_1, i/2,i); 
    GD.voice(2, SINE, note_2, i,i/2); 

    pot_frequency_5 = analogRead(5);
    EG_dec_rate = map(pot_frequency_5, 0, 1023, 100, 3000);
    delayMicroseconds(EG_dec_rate);   //lfo or eg rate set by delay time
    }

  //Serial.print("LFO RATE = ");
  //Serial.println(EG_att_rate);    
} //end loop

//Map pot values to 6 'switch' positions
int PotSwitch_6(int potRead){
   //Pot ranges for 6 notes: 0-170, 171-340, 341-510, 511-680, 681-850, 851-1023
   
   if (potRead < 171) {return 0;}
   else if (potRead < 341) {return 1;}
   else if (potRead < 511) {return 2;}
   else if (potRead < 681) {return 3;}
   else if (potRead < 851) {return 4;}
   else {return 5;}
}

int ValuetoNote_0(int value){
//cycle Pot0 values to  to be notes A/C/E
   if (value < 1) {return 440;} //440hz - A
   else if (value < 2) {return 523;}
   else if (value < 3) {return 659;}
   else if (value < 4) {return 880;}
   else if (value < 5) {return 1046;}
   else {return 1318;}
  
}

 
int ValuetoNote_1(int value){
//cycle Pot1 values to be notes C/E/G
   if (value < 1) {return 1046;} 
   else if (value < 2) {return 1318;}
   else if (value < 3) {return 1496;}
   else if (value < 4) {return 2092;}
   else if (value < 5) {return 2636;}
   else {return 2992;}
  
}


int ValuetoNote_2(int value){
//cycle Pot2 values to be notes G/B/D
   if (value < 1) {return 784;} 
   else if (value < 2) {return 494;}
   else if (value < 3) {return 587;}
   else if (value < 4) {return 1568;}
   else if (value < 5) {return 988;}
   else {return 1174;}
  
}

/*IDEAS
 * 
 * 1) MAP pot values to tones that harmonize well together
 *    and have a pot that controls inversions
 *    ...could then have inputs be 'detectors' of sorts rather than pots
 * 
 * 2) FM Synthn
 * 
 * 
 * 3) EG functions (either calculating or 'wave table')
 * 
 * 4) Make a table / array that maps frequency values to notes (C1, C#2, D-1)
 * 
 * 5) Digital osc: Make a table / array that maps delay times to frequencies
 *    - 440hz square wave: One cycle per 440th of a second (2272us), On for 1/880th of second (1136us), Off for 1/880th of a second  
 * 
 * FREQUENCIES BY NOTE (approx):
 * Multipy / divide by 2 to go up / down an octive
 * A  = 440hz
 * A# = 466hz
 * B  = 494hz
 * C  = 523hz
 * C# = 554hz
 * D  = 587hz
 * D# = 622hz
 * E  = 659hz
 * F  = 698hz
 * F# = 740hz
 * G  = 784hz
 * G# = 831hz
 * 
 */



