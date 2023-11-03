#include <SPI.h>
#include <GD.h>
//#include <pitches.h>

const int SINE = 0;
const int NUM_POTS = 5;
const int STEP = 2;
const int triggerPin = 12;

typedef unsigned char prog_uchar;


unsigned long time_A_previous = 0;
unsigned long time_B_previous = 0;

int i = 0;
int j = 0;
bool attack_done = 0;
bool attack_2_done = 0;

int note_0;
int note_1;
int note_2;
int note_3;

void setup()
{
    //Initialize the Gameduino
    GD.begin();
    pinMode(triggerPin, INPUT); 
    Serial.begin(9600);

    //cycle note_0 values to  to be notes A/C/E
    note_0  = ValuetoNote_0(random(0,5));
    //cycle note1 values to be notes C/E/G
    note_1  = ValuetoNote_1(random(0,5));
    //cycle note_2 values to be notes G/B/D
    note_2  = ValuetoNote_2(random(0,5));

    int note_3  = 220;
}

void loop()
{
    //Get the current time
    unsigned long current_time = millis();
  
    //Envelope Generator!
    int pot_frequency_4 = analogRead(4);
    int EG_att_rate = map(pot_frequency_4, 0, 1023, 1, 100);
    int pot_frequency_5 = analogRead(5);
    int EG_dec_rate = map(pot_frequency_5, 0, 1023, 1, 100); //microseconds
    
    //Attack 1!
    if((current_time - time_A_previous) > EG_att_rate && attack_done == 0){
      GD.voice(0, SINE, note_0, i*2,i);
      GD.voice(1, SINE, note_1, i,i*2); 
      GD.voice(2, SINE, note_2, i*2,i);
      i++;
      current_time = millis();
      time_A_previous = current_time;
      if (i==25) {
          attack_done = 1; //attack is done once i hits 25
          }
      }

    //Decay 1! 
    if((current_time - time_A_previous) > EG_dec_rate && attack_done == 1) {
      GD.voice(0, SINE, note_0, i*2,i);
      GD.voice(1, SINE, note_1, i,i*2); 
      GD.voice(2, SINE, note_2, i*2,i);
      i--;
      current_time = millis();
      time_A_previous = current_time;
      if (i==0) {
          attack_done = 0; //once decay is done, restart attack and update notes
          note_0  = ValuetoNote_0(random(0,5));
          note_1  = ValuetoNote_1(random(0,5));
          note_2  = ValuetoNote_2(random(0,5));
          }  
     }
  
    //Attck 2!
    if((current_time - time_B_previous) > (EG_att_rate*2) && attack_2_done == 0){
        GD.voice(4, SINE, note_3,j*2,j*2);
        j++;
        current_time = millis();
        time_B_previous = current_time;
        delay(100);
        if (j==25) {
            attack_2_done = 1; //attack is done once i hits 50
            }
        }

    //Decay 2! 
    if((current_time - time_B_previous) > (EG_dec_rate*2) && attack_2_done == 1) {
      GD.voice(4, SINE, note_3,j*2,j*2);
      j--;
      current_time = millis();
      time_B_previous = current_time;
      if (j==0) {
          attack_2_done = 0; //once decay is done, restart attack and update notes
          if (note_3 == 220) {note_3 = 440;}
          else {note_3 = 220;}
          }  
     }

  
  
  
  Serial.print("note 3 = ");
  Serial.println(note_3); 
  //Serial.print("attack_rate = ");
  //Serial.println(EG_att_rate); 
  //Serial.print("decay_rate = ");
  //Serial.println(EG_dec_rate); 
  //delay(10);

     
    /*
    //While button is pressed (low in pin 13), trigger the envelope
    bool trigger = digitalRead(triggerPin);
    if (trigger == HIGH) {  
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
     }
  
  GD.voice(0, SINE, note_0, i,i/2);
  Serial.print("Attack = ");
  Serial.println(EG_att_rate);  
  Serial.print("Decay = ");
  Serial.println(EG_dec_rate);  
  */  
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
   if (value < 1) {return 440;}      //440hz - A1
   else if (value < 2) {return 523;} // C1
   else if (value < 3) {return 659;}
   else if (value < 4) {return 880;} // A2
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



