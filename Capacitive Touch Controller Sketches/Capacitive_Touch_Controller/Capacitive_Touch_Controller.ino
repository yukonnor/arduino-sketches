/* NOTES ABOUT SKETCH:
 * Code to support a CV/Gate control surface with: 
 * - two 'trigger' controls (X/Y) and one mono output for each
 * - two 'cv level' controls (A/B) and one mono output for each
 * -- 'high' level cv is good around ~4.5V (wobble bug peaks 9V)
 *
 * Sliders (A & B): 
 * - Sets external CV of 0V to 5V based on finger position (gradient)
 * - Maintains level of A/B when removing finger
 * - Indicates level of A/B sliders via A/B LED brightness
 * 
 * Gates (X & Y):
 * - Sets external CV: 0V or 4.8V (on-off) based on whether or not finger is touching pad
 * 
 * TODO: 
 * - figure out why there is latency (turning off serial helped!)
 * - map pins 
 * - ergonimics
 * - solder protoboard of what's currently breadboarded
 * - figue out case design - copper foil on wood then laquer over
 * 
 * 
 * NOTES ABOUT CapacitiveSensor:
 * Uses a high value resistor e.g. 10 megohm between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50 kilohm - 50 megohm. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 * Best results are obtained if sensor foil and wire is covered with an insulator such as paper or plastic sheet
 */
#include <Wire.h> //I2C for DACs
#include <Adafruit_MCP4725.h> //I2C for DACs
#include <CapacitiveSensor.h>

Adafruit_MCP4725 dac; //rename library
#define DAC_RESOLUTION (8) //8-bit resolution

CapacitiveSensor   cs_x = CapacitiveSensor(2,3);    // 470K resistor between pins 2 & 4, pin 4 is sensor pin, add wire, foil
CapacitiveSensor   cs_y = CapacitiveSensor(2,4);    // 470K resistor between pins 2 & 5, pin 5 is sensor pin, add wire, foil
CapacitiveSensor   cs_a = CapacitiveSensor(5,6);    // 300K resistor between pins 5 & 6, pin 6 is sensor pin, add wire, foil
CapacitiveSensor   cs_b = CapacitiveSensor(5,8);    // 300K resistor between pins 8 & 7, pin 7 is sensor pin

//Define LED Variables
const int ledPin_A = 9;         // the PWM pin the LED for A is attached to
const int ledPin_B = 10;        // the PWM pin the LED for A is attached to
byte brightness_A = 0;           //  how bright the LED for A is. Should be between 0 - 255
byte brightness_B = 0;  

//X & Y Gate Pins
const int xPin = 12;
const int yPin = 13;

//CapTouch Variables
int cap_reading_x;
int cap_reading_y;
int dirty_reading_a;
int cap_reading_a; //for smoothed values
int dirty_reading_b;
int cap_reading_b; //for smoothed values

//Push button Variables
const int button_a_Pin = 0;
int button_a_State = 0; //0 if off, 1 is on
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
const byte debounceDelay = 200;    // the debounce time; increase if the output flickers

//DAC Variables
unsigned int dac_0_out = 0;
unsigned int dac_1_out = 0;

//Define Smoothing Variables
const byte numReadings = 75;       // smoothness time: 10 immediate, 50 med, 100 long (yet still playable)
int readings_a[numReadings];      // array of the readings from the analog input
int readings_b[numReadings]; 
byte readIndex_a = 0;              // the index of the current reading
byte readIndex_b = 0;
int total_a = 0;                  // the running total
int total_b = 0; 
int average_reading_a = 0;        // the average
int average_reading_b = 0; 

void setup()                    
{
  // declare LED pins and XY Gate pins to be outputs:
  pinMode(ledPin_A, OUTPUT);
  pinMode(ledPin_B, OUTPUT);
  pinMode(xPin, OUTPUT);
  pinMode(yPin, OUTPUT);
  pinMode(button_a_Pin, INPUT_PULLUP);

  //Smoothing - initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings_a[thisReading] = 0;
  }
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings_b[thisReading] = 0;
  }

  dac.begin(0x60);
  dac.setVoltage(0,false);
  dac.begin(0x61);
  dac.setVoltage(0,false);
  
  //Serial.begin(9600);
}

void loop()                    
{
    //Define CapSense variables and readings  
    long start = millis();
    cap_reading_x =  cs_x.capacitiveSensor(30); //on-off based on logic below
    cap_reading_y =  cs_y.capacitiveSensor(30); //on-off based on logic below
    dirty_reading_a =  cs_a.capacitiveSensor(30); //0 - 600 with 470K resistor
    dirty_reading_b =  cs_b.capacitiveSensor(30); //0 - 680 with 470K resistor (maybe try to decrease?)

    int button_a_Read = digitalRead(button_a_Pin); 
    
    //On off Logic: if button pressed (LOW) and if state is off, turn on
    if (button_a_Read == LOW && ((millis() - lastDebounceTime) > debounceDelay)) {
        button_a_State = !button_a_State;
        lastDebounceTime = millis();
       }

    //IF "MAINTAIN" Button if OFF, record A values constantly. If ON, keep last touched value using "Smooth_Maintain" function
    if (button_a_State == 0) {
        cap_reading_a = Smooth_A(dirty_reading_a); //A reading is now smoothed
       }
       else {
         cap_reading_a = Smooth_Maintain_A(dirty_reading_a, 40, 35); //hold above, touch threshold - A value is now maintained if finger released
         }
    
    cap_reading_b = Smooth_B(dirty_reading_b); //B reading is now smoothed
    cap_reading_a = max(cap_reading_a, 35);
    cap_reading_b = max(cap_reading_b, 35); //hacky way to 'eliminate' bleeding
    //when a was touched, b was going up to ~35. This makes 35 the '0 value for b (along with mapping functions below)
    //any level below that was bareley noticibale anyways.  

    //Logic for X & Y Gate On Off
    cap_reading_x = On_Off_Threshold(cap_reading_x, 140); //if x is > 140, it is HIGH. Else LOW.
    cap_reading_y = On_Off_Threshold(cap_reading_y, 140); 
    
    //SET THEM GATE OUTPUTS
    digitalWrite(xPin, cap_reading_x);
    digitalWrite(yPin, cap_reading_y); 

    //Map A & B Values
    dac_0_out = map(cap_reading_a, 35, 400, 0, 4095);    
    dac_1_out = map(cap_reading_b, 35, 400, 0, 4095);  
    dac_0_out = min(dac_0_out, 4095); //make sure dacs don't go above 4095 
    dac_1_out = min(dac_1_out, 4095);
    
    brightness_A = map(cap_reading_a, 35, 400, 0, 200); //map the captouch A to led's brightness. 0 off, 255 max bright    
    if (brightness_A < 5) {  
       brightness_A = 0;} //turn off led if low

    brightness_B = map(cap_reading_b, 35, 400, 0, 200); //map the captouch A to led's brightness. 0 off, 255 max bright    
    if (brightness_B < 5) {  
       brightness_B = 0;  }  
    
    //SET THOSE DAC OUTPUTS
    dac.begin(0x60);
    dac.setVoltage(dac_0_out, false);
    dac.begin(0x61);
    dac.setVoltage(dac_1_out, false);

    //light up those LEDs!
    analogWrite(ledPin_A, brightness_A);
    analogWrite(ledPin_B, brightness_B);
    
    debug(0); //if 1, print serial readings
       
} //end Loop









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
  if (readIndex_a >= numReadings) {
    // ...wrap around to the beginning:
    readIndex_a = 0;
    }
  // calculate the average:
  average_reading_a = total_a / numReadings;
  return average_reading_a;

} //end Smooth_A


int Smooth_B(int dirty_reading) {
  //See Smooth_A for comments / explination
  total_b = total_b - readings_b[readIndex_b];
  readings_b[readIndex_b] = dirty_reading;
  total_b = total_b + readings_b[readIndex_b];
  readIndex_b = readIndex_b + 1;

  if (readIndex_b >= numReadings) {
     readIndex_b = 0;
     }

  average_reading_b = total_b / numReadings;
  return average_reading_b;
} //end Smooth_B


//Use this function to maintain the 'last touched value' if finger is removed
int Smooth_Maintain_A(int dirty_reading, int hold_above_value, int touch_threshold) {  //used 80 and 10
  //Be sure to include global variables and intializer in setup()

  // subtract the last reading:
  total_a = total_a - readings_a[readIndex_a];
  // read from the sensor:
  if (dirty_reading > hold_above_value ) {  //if above this touched value, keep at that value when sensor released
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
  if (readIndex_a >= numReadings) {
    // ...wrap around to the beginning:
    readIndex_a = 0;
    }
  // calculate the average:
  average_reading_a = total_a / numReadings;

  return average_reading_a;

} //end Smooth_Maintain_A 

int Smooth_Maintain_B(int dirty_reading, int hold_above_value, int touch_threshold) {  //used 80 and 10
  //See Smooth_Maintain_B for comments
  total_b = total_b - readings_b[readIndex_b];
  if (dirty_reading > hold_above_value ) {  
    readings_b[readIndex_b] = dirty_reading;}
    else if (dirty_reading > touch_threshold) {  
      readings_b[readIndex_b] = 0;  
      }
  total_b = total_b + readings_b[readIndex_b];
  readIndex_a = readIndex_a + 1;

  if (readIndex_a >= numReadings) {
    readIndex_a = 0;
    }
  average_reading_a = total_a / numReadings;
  return average_reading_a;
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
    Serial.print("Button A State = ");
    Serial.print(button_a_State);              
    Serial.print("\t");
    //Serial.print("X Touch = ");
    //Serial.print(cap_reading_x);              // print sensor X
    //Serial.print("\t");
    //Serial.print("Y Touch = ");
    //Serial.print(cap_reading_y);              // print sensor Y
    //Serial.print("\t");
    Serial.print("Dirty A = ");
    Serial.print(dirty_reading_a);              // print sensor Y
    Serial.print("\t");
    Serial.print("A Touch = ");
    Serial.print(cap_reading_a);              // print sensor A
    Serial.print("\t");   
    Serial.print("Brightness A = ");
    Serial.print(brightness_A);               // print brightness
    Serial.println("\t");
    //Serial.print("B Touch = ");
    //Serial.print(cap_reading_b);              // print sensor B
    //Serial.print("\t");
    //Serial.print("Brightness B = ");
    //Serial.print(brightness_B);
    //Serial.println("\t");
    }

} //end debug
 

