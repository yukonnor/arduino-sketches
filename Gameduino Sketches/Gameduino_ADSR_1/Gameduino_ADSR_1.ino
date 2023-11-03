#include <SPI.h>
#include <GD.h>
//#include <pitches.h>

//To guarantee no audio clipping, the sum of all voices’ amplitudes should be 255 or less.

const int SINE = 0;
const int NUM_POTS = 5;
const int STEP = 2;
const int triggerPin = 7;

typedef unsigned char prog_uchar;

int debounceDelay = 50; //50ms debounce time
int last_trigger_time = 0;

unsigned long current_time;
unsigned long previous_time_1 = 0;
unsigned long previous_time_2 = 0;
unsigned long previous_time_3 = 0;

short current_level_1 = 0; //current output level for EG #1
short current_level_2 = 0; //current output level for EG #2
short current_level_3 = 0; //current output level for EG #3

short max_level_1; //max level for EG #1  ... SUM of all max levels should never go >255
short max_level_2; //max level for EG #2
short max_level_3; //max level for EG #3

short adsr_state_1 = 0; //0 not triggered, 1 attack, 2 decay
short adsr_state_2 = 0; //0 not triggered, 1 attack, 2 decay
short adsr_state_3 = 0; //0 not triggered, 1 attack, 2 decay

short attack_rate_1;
short attack_rate_2;
short attack_rate_3;

short decay_rate_1;
short decay_rate_2;
short decay_rate_3;


int note_0;
int note_1;
int note_2;
int note_3;
int note_4;
int note_5;
int note_6;

int play_mode = 1;

bool trigger_button;
bool trigger_button_last = LOW; 



void setup()
{
    //Initialize the Gameduino
    GD.begin();
    pinMode(triggerPin, INPUT); 
    //Serial.begin(9600);

    note_0  = ValuetoNote_0(random(0,5));
    note_1  = ValuetoNote_1(random(0,5));
    note_2  = ValuetoNote_2(random(0,5));
    
    note_3  = ValuetoNote_0(random(0,5));
    note_4  = ValuetoNote_0(random(0,5));

    note_5  = ValuetoNote_5_6(random(0,2));
    note_6  = ValuetoNote_5_6(random(0,2));
}

void loop()
{
    //Get the current time
    current_time = millis();
    max_level_1 = 20;  //sets how loud each chord can get (peak of their EGs)
    max_level_2 = 50;
    max_level_3 = 50;
  
    //Get Attack and Decay Rates from Pots
    int pot_frequency_4 = analogRead(4);
    attack_rate_1 = map(pot_frequency_4, 0, 1023, 1, 100);  //with no serial: 150- /  with serial (5-200)
    attack_rate_2 = attack_rate_1*2;
    attack_rate_3 = attack_rate_1*4;
    
    int pot_frequency_5 = analogRead(5);
    decay_rate_1 = map(pot_frequency_5, 0, 1023, 1, 100);
    decay_rate_2 = decay_rate_1*2;
    decay_rate_3 = decay_rate_1*4;

    //Get Trigger Reading from Button
    trigger_button = digitalRead(triggerPin);
    
    //If in play mode 0, trigger EG by button / input
    if(play_mode == 0){
        //If new trigger present, start cycle from scratch. If trigger still high, don't do anything
        //quickly fade i (level) to 0 and Set adsr_state to 0 to begin attack
        if((current_time - last_trigger_time) > debounceDelay){                  //if enough time has elapsed to consider a new input
            if(trigger_button == HIGH && trigger_button_last == LOW){            //if button is pressed and was previously released
              for(current_level_2; current_level_2=0; current_level_2--) {     //set EG level to 0 to eliminate clicking
                  GD.voice(3, SINE, note_0, current_level_2,0);
                  GD.voice(4, SINE, note_1, 0,current_level_2); 
                  delay(5);
                  }
              //last_trigger_time = millis();
              adsr_state_1 = 1;                 //start attack (now that level is 0)
              adsr_state_2 = 1;                 //start attack (now that level is 0)
              adsr_state_3 = 1;                 //start attack (now that level is 0)
              trigger_button_last = HIGH;       //if button is 'still' pressed, it will not be ignored. 
                                                //button needs to be released to re-trigger
              }
    
          if(trigger_button == LOW && trigger_button_last == HIGH){ //if button is released, re-set so that it can re-trigger
               trigger_button_last = LOW;  
               last_trigger_time = millis();
              }
           }
         }

    //playmode 1 is constant looping. Start a new EG when level hits 0
    if(play_mode == 1){
       if(current_level_1 <= 0){
          adsr_state_1 = 1;
       }
       if(current_level_2 <= 0){
          adsr_state_2 = 1;
       }
       if(current_level_3 <= 0){
          adsr_state_3 = 1;
       } 
    }
     
    
    //EG #1: Attack & Decay! 
    AD_Envelope_1();

    //EG #2: Attack & Decay!
    AD_Envelope_2(); 

    //EG #3: Attack & Decay!
    AD_Envelope_3();
     
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

int ValuetoNote_5_6(int value){
    //cycle Pot0 values to  to be notes A/C/E
    if (value < 1) {return 220;}      //440hz - A0
    else if (value < 2) {return 261;} // C0
    else if (value < 3) {return 330;}  
}

void playChord1(int current_level){
      GD.voice(0, SINE, note_0, current_level, current_level/2); //play notes
      GD.voice(1, SINE, note_1, current_level/2, current_level); 
      GD.voice(2, SINE, note_2, current_level, current_level/2);
}

void updateChord1(){
      note_0  = ValuetoNote_0(random(0,5));         //update notes of chord
      note_1  = ValuetoNote_1(random(0,5));
      note_2  = ValuetoNote_2(random(0,5));
}

void playChord2(int current_level){
      GD.voice(3, SINE, note_3, current_level, 0); //play notes
      GD.voice(4, SINE, note_4, 0, current_level); 
}

void updateChord2(){
      note_3  = ValuetoNote_0(random(0,5));         //update notes of chord
      note_4  = ValuetoNote_0(random(0,5)); 
}

void playChord3(int current_level){
      GD.voice(5, SINE, note_5, 0, current_level); //play notes
      GD.voice(6, SINE, note_6, current_level,0); 
}

void updateChord3(){
      note_3  = ValuetoNote_5_6(random(0,2));       //update notes of chord
      note_4  = ValuetoNote_5_6(random(0,2)); 
}


void AD_Envelope_1(){
    //Attack! When in Attack Mode, increase level of chord 1 at the attack_rate
    if(adsr_state_1 == 1 && (current_time - previous_time_1 >= attack_rate_1)){
       
       playChord1(current_level_1);                    //play chord 1
       
       current_level_1++;                              //get louder
       previous_time_1 = current_time;                 //save the last time the attack loop was run
      
       if (current_level_1 >= max_level_1) {           //if we've reached the max level the attack is done
           adsr_state_1 = 2;                           //now, set adsr state to decay
       }
      
    }

    //Decay! When in Decay Mode, decrease level of chord 1 at the decay_rate
    if(adsr_state_1 == 2 && (current_time - previous_time_1 >= decay_rate_1)){ 
            
       playChord1(current_level_1);                      //continue playing chord 1
       current_level_1--;                                //get softer
       previous_time_1 = current_time;                   //save the last time the decay loop was run

       if (current_level_1 <= 0) {                       //if we've reachded a level of 0, decay is done
           adsr_state_1 = 0;                             //once decay is done, set envelope back to not started
           updateChord1();                               //and update the notes of the chord
       } 
    }    
}

void AD_Envelope_2(){
     //Attack! When in Attack Mode, increase level of chord 1 at the attack_rate
     if(adsr_state_2 == 1 && (current_time - previous_time_2 >= (attack_rate_2))){  
            
      GD.voice(3, SINE, note_3, current_level_2,0); //play notes
      GD.voice(4, SINE, note_4, 0,current_level_2); 

      current_level_2++;                              //get louder
      previous_time_2 = current_time;                 //save the last time the attack loop was run
      
      if (current_level_2 >= 50) {                    //if we've reached the max level the attack is done
          adsr_state_2 = 2;                           //now, set adsr state to decay
          }
      }

    //Decay! - When in the decay state AND when decay_rate interval has been hit, get softer
    if(adsr_state_2 == 2 && (current_time - previous_time_2 >= (decay_rate_2))){ 
            
      GD.voice(3, SINE, note_3, current_level_2,0); //play notes
      GD.voice(4, SINE, note_4, 0,current_level_2); 

      current_level_2--;                                //get softer
      previous_time_2 = current_time;                   //save the last time the decay loop was run

      if (current_level_2 <= 0) {                       //if we've reachded a level of 0, decay is done
          adsr_state_2 = 0;                             //once decay is done, set envelope back to not started
          updateChord2();                               //and update the notes of the chord
          }       
     }  
  
}

void AD_Envelope_3(){
     //Attack!
     if(adsr_state_3 == 1 && (current_time - previous_time_3 >= (attack_rate_3))){  
            
      GD.voice(5, SINE, note_5, 0,current_level_3); //play notes
      GD.voice(6, SINE, note_6, current_level_3,0); 

      current_level_3++;                              //get louder
      previous_time_3 = current_time;                 //save the last time the attack loop was run
      
      if (current_level_3 >= 50) {                    //if we've reached the max level the attack is done
          adsr_state_3 = 2;                           //now, set adsr state to decay
          }
      }

    //Decay!
    if(adsr_state_3 == 2 && (current_time - previous_time_3 >= (decay_rate_3))) { 
            
      GD.voice(5, SINE, note_5, 0,current_level_3);     //play notes
      GD.voice(6, SINE, note_6, current_level_3,0); 

      current_level_3--;                                //get softer
      previous_time_3 = current_time;                   //save the last time the decay loop was run

      if (current_level_3 <= 0) {                       //if we've reachded a level of 0, decay is done
          adsr_state_3 = 0;                             //once decay is done, set envelope back to not started
          updateChord3();                               //and update the notes of the chord
          }       
     }
}



/* OLD AD CODE
 *   //EG #1: Attack! - When in the attack state AND when the attack_rate interval has been hit, get louder
    if(adsr_state_1 == 1 && (current_time - previous_time_1 >= attack_rate_1)){  
            
      GD.voice(0, SINE, note_0, current_level_1,current_level_1/2); //play notes
      GD.voice(1, SINE, note_1, current_level_1/2,current_level_1); 
      GD.voice(2, SINE, note_2, current_level_1,current_level_1/2);

      current_level_1++;                              //get louder
      previous_time_1 = current_time;                 //save the last time the attack loop was run
      
      if (current_level_1 >= 20) {                    //if we've reached the max level the attack is done
          adsr_state_1 = 2;                           //now, set adsr state to decay
          }
      Serial.print("Attack Level = ");
      Serial.println(current_level_1); 
      }

    //EG #1: Decay! - When in the decay state AND when decay_rate interval has been hit, get softer
    if(adsr_state_1 == 2 && (current_time - previous_time_1 >= decay_rate_1)) { 
            
      GD.voice(0, SINE, note_0, current_level_1,current_level_1/2);
      GD.voice(1, SINE, note_1, current_level_1/2,current_level_1); 
      GD.voice(2, SINE, note_2, current_level_1,current_level_1/2); 

      current_level_1--;                                //get softer
      previous_time_1 = current_time;                   //save the last time the decay loop was run

      if (current_level_1 <= 0) {                       //if we've reachded a level of 0, decay is done
          adsr_state_1 = 0;                             //once decay is done, set envelope back to not started
          note_0  = ValuetoNote_0(random(0,5));         //update notes of chord
          note_1  = ValuetoNote_1(random(0,5));
          note_2  = ValuetoNote_2(random(0,5));
          }  
         
     }
  
   //EG #2: Attack! - When in the attack state AND when the attack_rate interval has been hit,` get louder
    if(adsr_state_2 == 1 && (current_time - previous_time_2 >= (attack_rate_1*2))){  
            
      GD.voice(3, SINE, note_3, current_level_2,0); //play notes
      GD.voice(4, SINE, note_4, 0,current_level_2); 

      current_level_2++;                              //get louder
      previous_time_2 = current_time;                 //save the last time the attack loop was run
      
      if (current_level_2 >= 50) {                    //if we've reached the max level the attack is done
          adsr_state_2 = 2;                           //now, set adsr state to decay
          }
      }

    //EG #2: Decay! - When in the decay state AND when decay_rate interval has been hit, get softer
    if(adsr_state_2 == 2 && (current_time - previous_time_2 >= (decay_rate_1*2))) { 
            
      GD.voice(3, SINE, note_3, current_level_2,0); //play notes
      GD.voice(4, SINE, note_4, 0,current_level_2); 

      current_level_2--;                                //get softer
      previous_time_2 = current_time;                   //save the last time the decay loop was run

      if (current_level_2 <= 0) {                       //if we've reachded a level of 0, decay is done
          adsr_state_2 = 0;                             //once decay is done, set envelope back to not started
          updateChord2();                               //and update the notes of the chord
          }       
     }  

    //EG #3: Attack! - When in the attack state AND when the attack_rate interval has been hit,` get louder
    if(adsr_state_3 == 1 && (current_time - previous_time_3 >= (attack_rate_1*4))){  
            
      GD.voice(5, SINE, note_5, 0,current_level_3); //play notes
      GD.voice(6, SINE, note_6, current_level_3,0); 

      current_level_3++;                              //get louder
      previous_time_3 = current_time;                 //save the last time the attack loop was run
      
      if (current_level_3 >= 50) {                    //if we've reached the max level the attack is done
          adsr_state_3 = 2;                           //now, set adsr state to decay
          }
      }

    //EG #3: Decay! - When in the decay state AND when decay_rate interval has been hit, get softer
    if(adsr_state_3 == 2 && (current_time - previous_time_3 >= (decay_rate_1*4))) { 
            
      GD.voice(5, SINE, note_5, 0,current_level_3);     //play notes
      GD.voice(6, SINE, note_6, current_level_3,0); 

      current_level_3--;                                //get softer
      previous_time_3 = current_time;                   //save the last time the decay loop was run

      if (current_level_3 <= 0) {                       //if we've reachded a level of 0, decay is done
          adsr_state_3 = 0;                             //once decay is done, set envelope back to not started
          updateChord3();                               //and update the notes of the chord
          }       
     }
*/


