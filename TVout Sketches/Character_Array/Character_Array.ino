/*
 * SKETCH DESCRIPTION: Coat screen in characters. Vary characters to make shapes and movement.
 * -----------------
 * 
 * todo: - find resolution of screen (size of array) 
 *          - TVout res: 120x96
 *          - 8x8 Font: 15x12 characters (actually 14x11 -> 154 total)
 *          - 4x6 Font: 30x16 characters (acutally 29x14 -> 435
 *       - clear character by character rather than rewriting entire string (flashes)
 *          - could try to replace existing character with " "? 
 * 
 */

#include <TVout.h>
#include <video_gen.h>

#include "TVout.h"
#include <fontALL.h>

//Name TVout class as "TV" to make easier to type
TVout TV;

//Define Analog Potentiometer Pins
const int pot1Pin = 0;
const int pot2Pin = 2; //note here that the stripboard wires cross, hence out of order
const int pot3Pin = 1;
const int pot4Pin = 3;

//Define Digital Button Pins
const int button1Pin = 13;
const int button2Pin = 12;
const int button3Pin = 11;
const int button4Pin = 10;

//Initialize button value variables:
int button1 = 0;
int button2 = 0;
int button3 = 0;
int button4 = 0;

//Define time variables
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

//Define Shape Location & Size Variables
uint8_t x; //x loction
uint8_t y; //y location
uint8_t s; //size of sides
int8_t xm; //x movement distance
int8_t ym; //y movement distance

char next_char;
char first_char;

//const int  array_length = 155; //154+1
//char character_map[array_length] = "////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////"; //currently 150

const int  array_length = 406; 
char character_map[array_length];// ="////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||////||||///////||||////|||||||//////";

void setup() {
  TV.begin(NTSC,120,96); //Screen resolution
  //Serial.begin(9600);
  
  //Set button pins to use internal Pullup resistors
  pinMode(button1Pin, INPUT_PULLUP);
  
  TV.select_font(font4x6); //4x6, 8x8
  
  TV.clear_screen();    

}

void loop() {

  int position_x = 0;
  //record values of pots 
  //use 10k pots
  int potRead1 = analogRead(pot1Pin); 
  int potRead2 = analogRead(pot2Pin);
  int potRead3 = analogRead(pot3Pin);

  //record values of buttons
  int buttonRead1 = digitalRead(button1Pin); //clear screen button

  //map pot1 values from 10 to 100 
  //controls tail length
  potRead1 = map(potRead1,0,1023,10,100);

  //map pot2 values from 1 to 3
  //controls x speed
  potRead2 = map(potRead2,0,1023,1,3);

  //map pot3 values from 1 to 3
  //controls y speed
  potRead3 = map(potRead3,0,1023,1,3);
  

  //Serial.print(current_array);

  for(int i=0; i<array_length; i++){
  //cycle_thru(i );
  random_char(i);
  }
  
  TV.println(0,0,character_map);  //see if you can replace/insert characters mid-string
  TV.delay_frame(50);
  TV.clear_screen();   
}



void cycle_thru(int i){
  first_char = character_map[0];
  next_char = character_map[i+1];
  character_map[array_length-1] = first_char;
  character_map[i] = next_char;
 
}

void random_char(int i){
  int pos_char_length = 3; //possible characters + 1 (null)
  char possible_char[pos_char_length] = "/|"; //2 characters
  character_map[i] = possible_char[(random(1,pos_char_length)-1)]; //random(incl,excl)
 
}

