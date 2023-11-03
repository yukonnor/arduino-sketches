#include <PgmChange.h>
#include <Fluxamasynth.h>

Fluxamasynth synth;

// Define Digital Button pins
const int buttonPin[6] = { 2, 13, 0, 7, 8, 12 };
bool buttonRead[6];

// Define PWM LED pins
const int ledPin[6] = { 11, 6, 5, 10, 9, 3 };

// Define Pot pins
const int potPin[6] = { A3, A2, A4, A5, A1, A0 };

// Store the output values of the LEDs
int led_val[6];

unsigned long current_time;
const int debounce = 200;   //20 for debug - 200 for production
unsigned long last_debounce_time;


void setup() {
  Serial.begin(9600);

  // SET PIN MODES
  // Button pins
  for (int i=0; i<6; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  // LED pins
  for (int i=0; i<6; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
}

void loop() {

  //Get the current time
  current_time = millis();

  // Read pot values and set brightness of LEDs
  for (int i=0; i<6; i++) {
    int val = analogRead(potPin[i]);
    led_val[i] = map(val, 0, 1023, 0, 255); 
    if (led_val[i] > 10) {
      Serial.print("Pot "); Serial.print(i); Serial.print(" twisted! Value at "); Serial.print(led_val[i]);Serial.print(". Lit LED on pin ");Serial.println(ledPin[i]);
    }
    analogWrite(ledPin[i], led_val[i]);
  }

  //Read button values (NOTE: Button press == LOW)
  for (int i=0; i<6; i++) {
    buttonRead[i] = digitalRead(buttonPin[i]);
  }

  // if button 1 pressed, light up LED 1
  if (buttonRead[0] == LOW && millis() - last_debounce_time > debounce) {
      analogWrite(ledPin[0], 255);
      Serial.print("Button 1 pressed! Lit LED 1 on pin "); Serial.println(ledPin[0]); 
      last_debounce_time = millis();  
   }

  // if button 2 pressed, light up LED 2
  if (buttonRead[1] == LOW && millis() - last_debounce_time > debounce) {
      analogWrite(ledPin[1], 255); 
      Serial.println("Button 2 pressed!");   
      last_debounce_time = millis();  
   }

  // if button 3 pressed, light up LED 3
  if (buttonRead[2] == LOW && millis() - last_debounce_time > debounce) {
      analogWrite(ledPin[2], 255);   
      Serial.println("Button 3 pressed!"); 
      last_debounce_time = millis();  
   }

  // if button 4 pressed, light up LED 4
  if (buttonRead[3] == LOW && millis() - last_debounce_time > debounce) {
      analogWrite(ledPin[3], 255); 
      Serial.println("Button 4 pressed!"); 
      last_debounce_time = millis();  
   }

  // if button 5 pressed, light up LED 5
  if (buttonRead[4] == LOW && millis() - last_debounce_time > debounce) {
      analogWrite(ledPin[4], 255); 
      Serial.println("Button 5 pressed!"); 
      last_debounce_time = millis();  
   }

  // if button 6 pressed, light up LED 6
  if (buttonRead[5] == LOW && millis() - last_debounce_time > debounce) {
      analogWrite(ledPin[5], 255);  
      Serial.println("Button 6 pressed!"); 
      last_debounce_time = millis();  
   }
  
  delay(1);
}
