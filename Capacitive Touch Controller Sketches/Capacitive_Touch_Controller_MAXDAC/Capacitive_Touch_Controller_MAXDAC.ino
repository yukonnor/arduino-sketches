/* NOTES ABOUT SKETCH:
 * Code to support a CV/Gate control surface with: 
 * - two 'trigger' controls (X&Y) and one gate output for each
 * - four 'cv level' controls (A-D) and one cv output for each
 *
 * Touch 'Sliders' (A-D): 
 * - Sets external CV of 0V to 5V based on finger position (gradient)
 * - Latch Mode: Maintains level of A-D when removing finger
 * - Indicates level of A-D sliders via A-D LED brightness
 * 
 * Gates (X & Y):
 * - Sets external CV: 0V or 4.8V (on-off) based on whether or not finger is touching pad
 * 
 * TODO: 
 * - continue to callibrate touch values
 * - write 'menu' functions triggered by long press of buttons (EG rate (numReadings value), LFO, random)
 * 
 */
#include <MAXDAC.h>
#include <Wire.h> //I2C for DACs
#include <CapacitiveSensor.h>

maxdac dac(B0101000); //address for dac (B0101{AD0}{AD1}{AD2})

CapacitiveSensor   cs_x = CapacitiveSensor(2,4);    // 470K resistor between pins 2 & 4, pin 4 is sensor pin, add wire, foil
CapacitiveSensor   cs_y = CapacitiveSensor(2,3);    // 470K resistor between pins 2 & 5, pin 5 is sensor pin, add wire, foil
CapacitiveSensor   cs_a = CapacitiveSensor(13,A2);    // 300K resistor between pins 5 & 6, pin 6 is sensor pin, add wire, foil
CapacitiveSensor   cs_b = CapacitiveSensor(13,A3);    // 300K resistor between pins 8 & 7, pin 7 is sensor pin
CapacitiveSensor   cs_c = CapacitiveSensor(13,8);
CapacitiveSensor   cs_d = CapacitiveSensor(13,7);

//Define LED Variables
const int ledPin_A = 11;         // the PWM pin the LED for A is attached to
const int ledPin_B = 10;        
const int ledPin_C = 6;
const int ledPin_D = 5;
byte brightness_A = 0;           //  how bright the LED for A is. Should be between 0 - 255
byte brightness_B = 0;  
byte brightness_C = 0;
byte brightness_D = 0;

//X & Y Gate OUT Pins
const int xPin = 12;
const int yPin = 9;

//CapTouch Variables
int cap_reading_x;
bool reading_x;
int cap_reading_y;
bool reading_y;
int dirty_reading_a;
int cap_reading_a; //for smoothed values
int dirty_reading_b;
int cap_reading_b; //for smoothed values
int dirty_reading_c;
int cap_reading_c; //for smoothed values
int dirty_reading_d;
int cap_reading_d; //for smoothed values

//Push button Variables
const int button_a_Pin = A0;
const int button_b_Pin = A1;
const int button_c_Pin = A6;
const int button_d_Pin = A7;
int button_a_Read;
int button_b_Read;
int button_c_Read;
int button_d_Read;
bool button_a_State = 0;      //0 is off, 1 is on
bool button_b_State = 0;
bool button_c_State = 0;
bool button_d_State = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
const byte debounceDelay = 200;      // the debounce time; increase if the output flickers

//DAC Variables
unsigned int dac_a_out = 0;  //0 - 255 (0 - ~5V)
unsigned int dac_b_out = 0;
unsigned int dac_c_out = 0;
unsigned int dac_d_out = 0;

//Define Smoothing Variables
const byte numReadings_a = 10;       // smoothness time: 10 immediate, 50 med, 100 long (yet still playable) Golden: **75**
const byte numReadings_b = 50;
const byte numReadings_c = 75;
const byte numReadings_d = 100;
int readings_a[numReadings_a];       // array of the readings from the analog input
int readings_b[numReadings_b]; 
int readings_c[numReadings_c];
int readings_d[numReadings_d];
byte readIndex_a = 0;              // the index of the current reading
byte readIndex_b = 0;
byte readIndex_c = 0;
byte readIndex_d = 0;
int total_a = 0;                   // the running total
int total_b = 0; 
int total_c = 0; 
int total_d = 0; 
int average_reading_a = 0;         // the average
int average_reading_b = 0; 
int average_reading_c = 0; 
int average_reading_d = 0; 

void setup()                    
{
  // declare LED pins and XY Gate pins to be outputs:
  pinMode(ledPin_A, OUTPUT);
  pinMode(ledPin_B, OUTPUT);
  pinMode(ledPin_C, OUTPUT);
  pinMode(ledPin_D, OUTPUT);
  pinMode(xPin, OUTPUT);
  pinMode(yPin, OUTPUT);
  pinMode(button_a_Pin, INPUT_PULLUP);
  pinMode(button_b_Pin, INPUT_PULLUP);
  pinMode(button_c_Pin, INPUT);
  pinMode(button_d_Pin, INPUT);

  //Smoothing - Initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings_a; thisReading++) {readings_a[thisReading] = 0;}
  for (int thisReading = 0; thisReading < numReadings_b; thisReading++) {readings_b[thisReading] = 0;}
  for (int thisReading = 0; thisReading < numReadings_c; thisReading++) {readings_c[thisReading] = 0;}
  for (int thisReading = 0; thisReading < numReadings_d; thisReading++) {readings_d[thisReading] = 0;}
  
  //Serial.begin(9600);
}

void loop()                    
{
    //Define CapSense variables and readings  
    long start = millis();
    cap_reading_x =  cs_x.capacitiveSensor(30); //on-off based on logic below
    cap_reading_y =  cs_y.capacitiveSensor(30); //on-off based on logic below
    dirty_reading_a =  cs_a.capacitiveSensor(30); //0 - 215
    dirty_reading_b =  cs_b.capacitiveSensor(30); //0 - 215 
    dirty_reading_c =  cs_c.capacitiveSensor(30); //0 - 215 
    dirty_reading_d =  cs_d.capacitiveSensor(30); //0 - 215 

    
    //On-Off Logic for A&B: if button pressed (LOW) and if state is off, turn on & vice versa
    button_a_State = Toggle_Button(button_a_Pin, button_a_Read, button_a_State);
    button_b_State = Toggle_Button(button_b_Pin, button_b_Read, button_b_State);
    
    //On-Off Logic for C&D (analog read):
    button_c_Read = analogRead(button_c_Pin);
    if (button_c_Read > 1020 && ((millis() - lastDebounceTime) > debounceDelay)) {
      button_c_State = !button_c_State;
      lastDebounceTime = millis();
      }

    button_d_Read = analogRead(button_d_Pin);
    if (button_d_Read > 1020 && ((millis() - lastDebounceTime) > debounceDelay)) {
      button_d_State = !button_d_State;
      lastDebounceTime = millis();
      }

    int max_touch_a = 200; //at one point was: 350
    int max_touch_b = 210; //350
    int max_touch_c = 200; //340
    int max_touch_d = 200; //330
    
    int touch_threshold_a = 36;
    int touch_threshold_b = 36;
    int touch_threshold_c = 36;
    int touch_threshold_d = 70;
    

    //IF "Latching" Buttons OFF, record cap_touch values constantly. If ON, keep last touched value using "Smooth_Maintain" function
    if (button_a_State == 0) {cap_reading_a = Smooth_A(dirty_reading_a);} //Reading is now smoothed
       //Touch value is now maintained @ last touched if finger released:
       else {cap_reading_a = Smooth_Maintain_A(dirty_reading_a, (touch_threshold_a+10), touch_threshold_a);} //Input, Hold above value, Touch threshold value
       //if input is above this 'hold above' value, keep at that value when sensor released
       //if the input above the 'touch_threshold' but below the 'hold_above_value', let fall to 0
    if (button_b_State == 0) {cap_reading_b = Smooth_B(dirty_reading_b);} 
       else {cap_reading_b = Smooth_Maintain_B(dirty_reading_b, (touch_threshold_b+10), touch_threshold_b);} 

    if (button_c_State == 0) {cap_reading_c = Smooth_C(dirty_reading_c);} 
       else {cap_reading_c = Smooth_Maintain_C(dirty_reading_c, (touch_threshold_c+10), touch_threshold_c);} 

    if (button_d_State == 0) {cap_reading_d = Smooth_D(dirty_reading_d);} 
       else {cap_reading_d = Smooth_Maintain_D(dirty_reading_d, (touch_threshold_d+10), touch_threshold_d);} 
    
    //Hacky way to 'eliminate' bleeding. When, for example, A was touched, B was going up to ~60. 
    //This makes 60 the '0 value' (along with mapping functions below)
    cap_reading_a = max(cap_reading_a, touch_threshold_a);
    cap_reading_b = max(cap_reading_b, touch_threshold_b); 
    cap_reading_c = max(cap_reading_c, touch_threshold_c); 
    cap_reading_d = max(cap_reading_d, touch_threshold_d);  

    //Logic for X & Y Gate On Off
    reading_x = On_Off_Threshold(cap_reading_x, 200); //if x is > 200, it is HIGH. Else LOW.
    reading_y = On_Off_Threshold(cap_reading_y, 200); 
    
    //SET THEM GATE OUTPUTS
    digitalWrite(xPin, reading_x);
    digitalWrite(yPin, reading_y); 

    //Map A - D Cap Values to DAC Values
    dac_a_out = map(cap_reading_a, touch_threshold_a, max_touch_a, 0, 255); //MAXDAC pin 3 out  
    dac_b_out = map(cap_reading_b, touch_threshold_b, max_touch_b, 0, 255); //MAXDAC pin 2 out
    dac_c_out = map(cap_reading_c, touch_threshold_c, max_touch_c, 0, 255); //MAXDAC pin 0 out   
    dac_d_out = map(cap_reading_d, touch_threshold_d, max_touch_d, 0, 255); //MAXDAC pin 1 out 
    dac_a_out = min(dac_a_out, 255);   //make sure dacs don't go above 255 
    dac_b_out = min(dac_b_out, 255);
    dac_c_out = min(dac_c_out, 255); 
    dac_d_out = min(dac_d_out, 255);
    
    brightness_A = map(cap_reading_a, touch_threshold_a, 400, 0, 150); //map the captouch A to led's brightness. 0 off, 255 max bright    
    if (brightness_A < 5) {  
       brightness_A = 0;} //turn off led if low

    brightness_B = map(cap_reading_b, touch_threshold_b, 400, 0, 150);     
    if (brightness_B < 5) {  
       brightness_B = 0;  }  

    brightness_C = map(cap_reading_c, touch_threshold_c, 300, 0, 150);     
    if (brightness_C < 5) {  
       brightness_C = 0;  } 

    brightness_D = map(cap_reading_d, touch_threshold_d, 400, 0, 150);     
    if (brightness_D < 5) {  
       brightness_D = 0;  } 
    
    //SET THOSE DAC OUTPUTS
    dac.write(0, dac_c_out);
    dac.write(1, dac_d_out);
    dac.write(2, dac_b_out);
    dac.write(3, dac_a_out);

    //light up those LEDs!
    analogWrite(ledPin_A, brightness_A);
    analogWrite(ledPin_B, brightness_B);
    analogWrite(ledPin_C, brightness_C);
    analogWrite(ledPin_D, brightness_D);
    
    debug(0); //if 1, print serial readings
       
} //end Loop //end Loop //end Loop //end Loop //end Loop //end Loop //end Loop //end Loop //end Loop //end Loop 



int Smooth_A(int dirty_reading) {
  //Be sure to include global variables and intializer in setup()
  
  // subtract the last reading:
  total_a = total_a - readings_a[readIndex_a];
  // read from the sensor:
  readings_a[readIndex_a] = dirty_reading;
  // add the reading to the total:
  total_a = total_a + readings_a[readIndex_a];
  // advance to the next position in the array:
  readIndex_a = readIndex_a + 1;

  // if we're at the end of the array...
  if (readIndex_a >= numReadings_a) {
    // ...wrap around to the beginning:
    readIndex_a = 0;
    }
  // calculate the average:
  average_reading_a = total_a / numReadings_a;
  return average_reading_a;

} //end Smooth_A


int Smooth_B(int dirty_reading) {
  //See Smooth_A for comments / explination
  total_b = total_b - readings_b[readIndex_b];
  readings_b[readIndex_b] = dirty_reading;
  total_b = total_b + readings_b[readIndex_b];
  readIndex_b = readIndex_b + 1;

  if (readIndex_b >= numReadings_b) {
     readIndex_b = 0;
     }

  average_reading_b = total_b / numReadings_b;
  return average_reading_b;
} //end Smooth_B

int Smooth_C(int dirty_reading) {
  //See Smooth_A for comments / explination
  total_c = total_c - readings_c[readIndex_c];
  readings_c[readIndex_c] = dirty_reading;
  total_c = total_c + readings_c[readIndex_c];
  readIndex_c = readIndex_b + 1;

  if (readIndex_c >= numReadings_c) {
     readIndex_c = 0;
     }

  average_reading_c = total_c / numReadings_c;
  return average_reading_c;
} //end Smooth_C

int Smooth_D(int dirty_reading) {
  //See Smooth_A for comments / explination
  total_d = total_d - readings_d[readIndex_d];
  readings_d[readIndex_d] = dirty_reading;
  total_d = total_d + readings_d[readIndex_d];
  readIndex_d = readIndex_d + 1;

  if (readIndex_d >= numReadings_d) {
     readIndex_d = 0;
     }

  average_reading_d = total_d / numReadings_d;
  return average_reading_d;
} //end Smooth_D

//Use this function to maintain the 'last touched value' if finger is removed
int Smooth_Maintain_A(int dirty_reading, int hold_above_value, int touch_threshold) {  //used 80 and 10
  //Be sure to include global variables and intializer in setup()

  // subtract the last reading:
  total_a = total_a - readings_a[readIndex_a];
  // read from the sensor:
  if (dirty_reading > hold_above_value ) {  //if input is above this 'hold above' value, keep at that value when sensor released
  readings_a[readIndex_a] = dirty_reading;}
  //if sensor is being TOUCHED (dirty measures over 10) below the min_a_value, bring average down to 0
  else if (dirty_reading > touch_threshold) {  //if it's touched above the 'touch_threshold' but below the 'hold_above_value', let fall to 0
  readings_a[readIndex_a] = 0;  
  }
  // add the reading to the total:
  total_a = total_a + readings_a[readIndex_a];
  // advance to the next position in the array:
  readIndex_a = readIndex_a + 1;

  // if we're at the end of the array...
  if (readIndex_a >= numReadings_a) {
    // ...wrap around to the beginning:
    readIndex_a = 0;
    }
  // calculate the average:
  average_reading_a = total_a / numReadings_a;

  return average_reading_a;

} //end Smooth_Maintain_A 

int Smooth_Maintain_B(int dirty_reading, int hold_above_value, int touch_threshold) {  
  //See Smooth_Maintain_A for comments
  total_b = total_b - readings_b[readIndex_b];
  if (dirty_reading > hold_above_value ) {  
    readings_b[readIndex_b] = dirty_reading;}
    else if (dirty_reading > touch_threshold) {  
      readings_b[readIndex_b] = 0;  
      }
  total_b = total_b + readings_b[readIndex_b];
  readIndex_b = readIndex_b + 1;

  if (readIndex_b >= numReadings_b) {
    readIndex_b = 0;
    }
  average_reading_b = total_b / numReadings_b;
  return average_reading_b;
} 

int Smooth_Maintain_C(int dirty_reading, int hold_above_value, int touch_threshold) {  
  //See Smooth_Maintain_A for comments
  total_c = total_c - readings_c[readIndex_b];
  if (dirty_reading > hold_above_value ) {  
    readings_c[readIndex_c] = dirty_reading;}
    else if (dirty_reading > touch_threshold) {  
      readings_c[readIndex_c] = 0;  
      }
  total_c = total_c + readings_c[readIndex_b];
  readIndex_c = readIndex_c + 1;

  if (readIndex_c >= numReadings_c) {
    readIndex_c = 0;
    }
  average_reading_c = total_c / numReadings_c;
  return average_reading_c;
} 

int Smooth_Maintain_D(int dirty_reading, int hold_above_value, int touch_threshold) {  
  //See Smooth_Maintain_A for comments
  total_d = total_d - readings_d[readIndex_d];
  if (dirty_reading > hold_above_value ) {  
    readings_d[readIndex_d] = dirty_reading;}
    else if (dirty_reading > touch_threshold) {  
      readings_d[readIndex_d] = 0;  
      }
  total_d = total_d + readings_d[readIndex_d];
  readIndex_d = readIndex_d + 1;

  if (readIndex_d >= numReadings_d) {
    readIndex_d = 0;
    }
  average_reading_d = total_d / numReadings_d;
  return average_reading_d;
} 


boolean Toggle_Button(int button_pin, boolean button_read, boolean button_state) {   
  
  button_read = digitalRead(button_pin);
   
  if (button_read == LOW && ((millis() - lastDebounceTime) > debounceDelay)) {
      button_state = !button_state;
      lastDebounceTime = millis();
      }
      return button_state;
}

boolean Analog_Toggle_Button(int button_pin, boolean button_read, boolean button_state) {   
  
  //button_read = analogRead(button_pin);
   
  if (button_read > 1020 && ((millis() - lastDebounceTime) > debounceDelay)) {
      button_state = !button_state;
      lastDebounceTime = millis();
      }
      return button_state;
}


boolean On_Off_Threshold(int on_off, int threshold) {

      if (on_off < threshold){
        on_off = LOW;
        }
        else {
        on_off = HIGH;  
        }

      return on_off;  
}

void debug(boolean debug_on){

  if(debug_on = 1){
    //Serial.print("Millis = ");
    //Serial.print(millis() - start);           // check on performance in milliseconds
    //Serial.print("Button C State = ");
    //Serial.print(button_c_State);              
    //Serial.print("\t");
    //Serial.print("X Touch = ");
    //Serial.print(reading_x);              // print sensor X cap_reading_x
    //Serial.print("\t");
    //Serial.print("Y Touch = ");
    //Serial.print(reading_y);              // print sensor Y
    //Serial.print("\t");
    Serial.print("Dirty A = ");
    Serial.print(dirty_reading_a);           
    Serial.print("\t");
    //Serial.print("A Touch = ");
    //Serial.print(cap_reading_a);            
    //Serial.print("\t");   
    //Serial.print("Brightness A = ");
    //Serial.print(brightness_A);              
    //Serial.print("\t");
    Serial.print("Dirty B = ");
    Serial.print(dirty_reading_b);             
    Serial.print("\t");
    //Serial.print("Brightness B = ");
    //Serial.print(brightness_B);
    //Serial.print("\t");
    Serial.print("Dirty C = ");
    Serial.print(dirty_reading_c);              
    Serial.print("\t");
    //Serial.print("C Touch = ");
    //Serial.print(cap_reading_c);   
    //Serial.print("\t");
    Serial.print("Dirty D = ");
    Serial.print(dirty_reading_d);             
    Serial.print("\t");
    //Serial.print("D Touch = ");
    //Serial.print(cap_reading_d);  
    //Serial.print("\t");
    Serial.println("\t");

    delay(2);
    }

} //end debug

/*
 * 3 RUN MODES for each channel (for now):
 * 1) CV Level Mode (Long) - CapSense readings control CV level
 * 2) CV Level Mode (Short) - CapSense readings control CV level
 * 2) LFO Mode (triangle wave) - Capsense readings control LFO rate
 * 
 * Menu Mode - Button presses cycle through above modes
 *              - Blinking led informs what mode you've selected
 * 
 * 
 * If button_a pressed for more than 1000ms, sent long_press_a = 1
 * If long_press_a = 1, A channel is in menu mode 
 *    mode_selector_a = 0 initially (CV Level Mode)
 *    if button a is short_pressed, increment mode_selector_a (cycle back to 0 after number_of_modes reached)
 * 
 */

