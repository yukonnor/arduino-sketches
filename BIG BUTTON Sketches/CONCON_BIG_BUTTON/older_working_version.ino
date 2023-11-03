/*
// TO DO:
// - it'd be nice if you could see the beginning of the 'loop' with an LED flash - BOC
// - would be able to sync up w/ starting of other loops using the 'reset' button 
// - reduce to 4 outputs
// - remove 'shift' 
// 
//

// Clock in is pin 2 ... Pin 2 is available for "Interupts"
// Clear Button is pin 4 .... this clears the whole loop sequence
// Button Delete is pin 7 This deletes the step your on
// Bank select Pin 3 .... each channel has 2 banks of patterns, so you can record 2 alternative patterns and go between with this button
// BIG BUTTON is pin 19 (A5), this is the performance button!
// Reset in is pin 6 you can add a button or a jack!
// FILL BUTTON pin 5, push this and it will continuously play the channel your on, doesnt record, push it and twist the select knob to make a fill!
// STEP LENGTH analog pin 1 (A1)
// Channel select Analog pin (A0) 
// SHIFT KNOB Analog pin (A2)
// LED (big button LED) pin 20 (A5)

// OUTPUT 1 ... pin 8
// OUTPUT 2 ... pin 9
// OUTPUT 3 ... pin 10
// OUTPUT 4 ... pin 11
// OUTPUT 5 ... pin 12
// OUTPUT 6 ... pin 13

const byte debug = 0;
unsigned long time = 0;
const byte debounce = 200;
const byte trigger_length = 10; // sets how long the output triggers / led flashes should be. Previously done with a delay

// clock stuff
// To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.
const byte clkInPin = 2;
volatile byte clkInState;          // current state of the Clock input, prev "buttonState"

const byte BigButtonPin = 19;

// Readings of the buttons
byte RecordButtonState = 0;        // BIG button
byte deletebuttonState = 0;
byte ClearButtonState = 0;




//RESET BUTTON
const byte ResetButton = 6; // Reset button pin
byte ResetButtonState = 0;  // Reading of the reset button
byte lastResetButtonState = 0;

const byte channelSelectPin = A0; 

//FILL BUTTON
int FillButton = 5;
int FillButtonState = 0;
byte FillState[6] = {0,0,0,0,0,0};       // store whether the fill is on or off for each channel

//CLEAR BUTTON
//int ClearState = 0;

//BANK BUTTON
byte BankState[6] = {0,0,0,0,0,0};        // stores whether the bank is on or off for each channel (which then selects the pattern). 
byte BankButtonState;                       // reads the state of the bank button

//Clock Keepers
byte currentStep  = 0;                // sets the shared global 'current step', used in the 'delete' function. Previously 'looper'
byte currentStep1 = 0;                      
byte currentStep2 = 0;
byte currentStep3 = 0;
byte currentStep4 = 0;
byte currentStep5 = 0;
byte currentStep6 = 0;

// PATTERN BEAT SHIFT 
const byte shiftPin = A2;
int  shiftPotRead = 0;       // Reading from the Shift Pot Value - prev "KnobVal"
byte shiftValue = 0;         // Value of how many beats to shift (0-8) - prev "KnobValue"
byte lastShiftValue = 0;      // prev "OldKnobValue"
//byte newShiftValue = 0;      // prev "NewKnobValue"
byte shiftValue1 = 0;     // prev "NewKnobValue1"
byte shiftValue2 = 0;
byte shiftValue3 = 0;
byte shiftValue4 = 0;
byte shiftValue5 = 0;
byte shiftValue6 = 0;

// PATERN LENGTH STUFF
const byte patLengthPin = A1;   // prev stepLengthPin
int patLengthPotRead = 0;       // pot reading for step length pot, prev StepLength
byte steps = 0;                 // beginning number of the steps in the sequence adjusted by StepLength


const byte clearButtonPin = 4;     
const byte deleteButtonPin = 7;
const byte bankButtonPin = 3;


//byte currentPattern = 0;       // current sequence (0-11 for the 6 channels, 2 banks each)
byte currentChannel = 0;       // current channel (0-5)
byte lastChannel = 0;          
int  CHANNELSELECT = 0 ;        // reading of channel pot

int OUT1 = 8;                  // pins for outputs
int OUT2 = 9;
int OUT3 = 10;
int OUT4 = 11;
int OUT5 = 12;
int OUT6 = 13;
int bankLEDPin ;               // pin for bank LED


// Sequence Array
// First dimension is which Channel / Bank combo (6 channels, each with 2 banks = 12 sequences)
// Second dimension is if the beat is on or off within that sequence. Each sequence can have up to 32 beats.
byte Sequence[12][32] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
 };
 

// Sequence[0]:  Channel 1, Bank Off
// Sequence[1]:  Channel 2, Bank Off  
// Sequence[2]:  Channel 3, Bank Off 
// Sequence[3]:  Channel 4, Bank Off  
// Sequence[4]:  Channel 5, Bank Off 
// Sequence[5]:  Channel 6, Bank Off  

// Sequence[6]:  Channel 1, Bank On
// Sequence[7]:  Channel 2, Bank On
// Sequence[8]:  Channel 3, Bank On
// Sequence[9]:  Channel 4, Bank On
// Sequence[10]: Channel 5, Bank On
// Sequence[11]: Channel 6, Bank On

void startupLEDSequence(){
  digitalWrite(bankLEDPin , HIGH); //this could go in a function, be redesigned for channel LEDs
  delay(200);
  digitalWrite(bankLEDPin , LOW);
  delay(180);
  digitalWrite(bankLEDPin , HIGH);
  delay(160);
  digitalWrite(bankLEDPin , LOW);
  delay(140);
  digitalWrite(bankLEDPin , HIGH);
  delay(120);
  digitalWrite(bankLEDPin , LOW);
  delay(100);
  digitalWrite(bankLEDPin , HIGH);
  delay(80);
  digitalWrite(bankLEDPin , LOW);
  delay(60);
  digitalWrite(bankLEDPin , HIGH);
  delay(40);
  digitalWrite(bankLEDPin , LOW);
  delay(20);
  digitalWrite(bankLEDPin , HIGH);
  delay(60);
  digitalWrite(bankLEDPin , LOW);
}

//  isr() - or Interupt Service Routine - quickly handle interrupts from the clock input
//  If clock input is triggered, set the reading of clock in to be HIGH
void isr()
{
  clkInState = HIGH;
}

void debuger(){
  if (debug == 1){
  Serial.print("Clock In = ");
  Serial.println(clkInState);
  
  //Serial.print("Big Button = ");
  //Serial.print(RecordButtonState);
  //Serial.print("  Bank Button = ");
  //Serial.print(ButtonBankSelectState[BankArrayNumber]);
  //Serial.print("  Clear Button = ");
  //Serial.print(ClearButtonState);
  //Serial.print("  Fill Button = ");
  //Serial.print(FillButtonState);
  //Serial.print("  Reset Button = ");
  //Serial.print(ResetButtonState);
  //Serial.print("  Delete Button = ");
  //Serial.println(DeleteButtonState);

  // POT TESTING
  Serial.print("Current Channel = ");
  Serial.print(currentChannel);  
  Serial.print("  Beat Shift = ");
  Serial.print(shiftValue);           // Should be between 0 - 8
  Serial.print("  Steps = ");
  Serial.println(steps);             // Should be between 1 - 32
  } 
}


void setup() { 
  pinMode(OUT1,OUTPUT);
  pinMode(OUT2,OUTPUT);
  pinMode(OUT3,OUTPUT);
  pinMode(OUT4,OUTPUT);
  pinMode(OUT5,OUTPUT);
  pinMode(OUT6,OUTPUT);
  pinMode(bankLEDPin , OUTPUT);
  pinMode(clkInPin, INPUT);     //SHOULDNT USE PULLUP as Clock input is high (use hardware pulldown)
  pinMode(BigButtonPin,INPUT_PULLUP);
  pinMode(deleteButtonPin,INPUT_PULLUP);
  pinMode(clearButtonPin,INPUT_PULLUP);
  pinMode(bankButtonPin, INPUT_PULLUP);
  pinMode(ResetButton, INPUT_PULLUP);
  pinMode(FillButton, INPUT_PULLUP);
  pinMode(patLengthPin, INPUT);  // pot to analog pin
  pinMode(shiftPin, INPUT);      // pot to analog pin

  // read pattern length pot value on start up
  // PUT THIS IN A FUNCTION
  patLengthPotRead = analogRead(patLengthPin);
  if(0<patLengthPotRead){steps= 1;}
  if(150<patLengthPotRead){steps= 2;}
  if(300<patLengthPotRead){steps= 4;}
  if(500<patLengthPotRead){steps= 8;}
  if(750<patLengthPotRead){steps=16;}
  if(1000<patLengthPotRead){steps=32;}    

  startupLEDSequence();  // LED flashing start up sequence

  // Interupt Service Routine for Clock Input
  // Syntax: attachInterrupt(digitalPinToInterrupt(pin), ISR, mode)
  // ISR: the interupt service routine to call when the interupt occurs (when clock pin goes high)
  // Rising: trigger when the pin goes from low to high
  attachInterrupt(0, isr, RISING);

  // if debug mode is on, run the serial monitor
  if(debug == 1){Serial.begin(9600);}
  
} //END SETUP


void loop() {

  // Get button readings
  RecordButtonState = digitalRead(BigButtonPin);
  deletebuttonState = digitalRead(deleteButtonPin);
  ClearButtonState = digitalRead(clearButtonPin); 
  ResetButtonState = digitalRead(ResetButton);
  FillButtonState = digitalRead(FillButton);
  BankButtonState = digitalRead(bankButtonPin);


  if(clkInState == HIGH) {
    currentStep  = (currentStep+1);
    currentStep1 = (currentStep1+1);
    currentStep2 = (currentStep2+1);
    currentStep3 = (currentStep3+1);
    currentStep4 = (currentStep4+1);
    currentStep5 = (currentStep5+1);
    currentStep6 = (currentStep6+1); 
    
    // Output each channels sequence
    digitalWrite(OUT1,Sequence[0 + BankState[0]] [currentStep1 + shiftValue1] || FillState[0]); // Logical OR results in a true if either of the two operands is true.
    digitalWrite(OUT2,Sequence[1 + BankState[1]] [currentStep2 + shiftValue2] || FillState[1]); 
    digitalWrite(OUT3,Sequence[2 + BankState[2]] [currentStep3 + shiftValue3] || FillState[2]); 
    digitalWrite(OUT4,Sequence[3 + BankState[3]] [currentStep4 + shiftValue4] || FillState[3]);
    digitalWrite(OUT5,Sequence[4 + BankState[4]] [currentStep5 + shiftValue5] || FillState[4]);
    digitalWrite(OUT6,Sequence[5 + BankState[5]] [currentStep6 + shiftValue6] || FillState[5]);
    delay(10);              //do this with a time diff
    digitalWrite(OUT1,LOW);
    digitalWrite(OUT2,LOW);
    digitalWrite(OUT3,LOW);
    digitalWrite(OUT4,LOW);
    digitalWrite(OUT5,LOW);
    digitalWrite(OUT6,LOW);
    clkInState = LOW;         
  } 

      


  
  // RECORD BUTTON - record the sequence of the current pattern
  if(RecordButtonState == LOW && millis() - time > debounce) {
    // Sequence[currentChannel + BankState[currentChannel]] [currentStep1 + 1 + newShiftValue1] = 1;  -- tried it this way, but doesn't work with "shift" (currentStep is dif for each channel)
    if(currentChannel == 0) {Sequence[currentChannel + BankState[currentChannel]] [currentStep1 + 1 + shiftValue1] = 1;}
    else if(currentChannel == 1) {Sequence[currentChannel + BankState[currentChannel]] [currentStep2 + 1 + shiftValue2] = 1;}
    else if(currentChannel == 2) {Sequence[currentChannel + BankState[currentChannel]] [currentStep3 + 1 + shiftValue3] = 1;}
    else if(currentChannel == 3) {Sequence[currentChannel + BankState[currentChannel]] [currentStep4 + 1 + shiftValue4] = 1;}
    else if(currentChannel == 4) {Sequence[currentChannel + BankState[currentChannel]] [currentStep5 + 1 + shiftValue5] = 1;}
    else if(currentChannel == 5) {Sequence[currentChannel + BankState[currentChannel]] [currentStep6 + 1 + shiftValue6] = 1;}
                                                                                    
    time = millis();    

    //delay (5); // seems sketchy?
  }


  // Turn on Bank LED if bank is on for the current channel
  // if Bank State = 6 then LED should be ON
  digitalWrite(bankLEDPin, BankState[currentChannel]);

 
  // BANK BUTTON
  // If the bank button is pressed set the bank state for the current channel to 6 if 0 and vice versa
  // Bank is 0 or 6 to add 6 to get the correct pattern (current channel + 6)
  if(BankButtonState == LOW && millis() - time > debounce){
     if(BankState[currentChannel] == 6){
        BankState[currentChannel] = 0;
        }
     else {
       BankState[currentChannel] = 6;
       }
     time = millis();
  }                                                                                               
    
  // SHIFT POT
  // Read the beat shift pot and determine how many beats to shift the pattern for the current channel
  shiftPotRead = analogRead(shiftPin);         
    if(0 < shiftPotRead)   { shiftValue = 0;}  
    if(127 < shiftPotRead) { shiftValue = 1;}
    if(254 < shiftPotRead) { shiftValue = 2;}
    if(383 < shiftPotRead) { shiftValue = 3;}
    if(511 < shiftPotRead) { shiftValue = 4;}
    if(638 < shiftPotRead) { shiftValue = 5;}
    if(767 < shiftPotRead) { shiftValue = 6;}
    if(895 < shiftPotRead) { shiftValue = 7;}
    if(1000 < shiftPotRead){ shiftValue = 8;}

  // if the shift value changes, set the new shift value for the current channel
  if(shiftValue != lastShiftValue){
    if(currentChannel == 0) {shiftValue1 = shiftValue;} 
    if(currentChannel == 1) {shiftValue2 = shiftValue;}   
    if(currentChannel == 2) {shiftValue3 = shiftValue;}   
    if(currentChannel == 3) {shiftValue4 = shiftValue;}   
    if(currentChannel == 4) {shiftValue5 = shiftValue;}   
    if(currentChannel == 5) {shiftValue6 = shiftValue;}     
    } 
   

  // CHANNEL SELECT POT
  CHANNELSELECT= analogRead(channelSelectPin);
  if(20>CHANNELSELECT)  {currentChannel = 0;}
  if(170<CHANNELSELECT) {currentChannel = 1;}
  if(240<CHANNELSELECT) {currentChannel = 2;}
  if(420<CHANNELSELECT) {currentChannel = 3;}
  if(750<CHANNELSELECT) {currentChannel = 4;}
  if(1000<CHANNELSELECT){currentChannel = 5;}


  // CLEAR PATTERN BUTTON
  // If the clear button is pressed, remove all triggers from the current pattern
  if(ClearButtonState == LOW){ 
    for(int i = 1; i<32; i++){
    Sequence[currentChannel + BankState[currentChannel]][i] = 0;
    }
  } 
  
  // FILL BUTTON
  // If the fill button pressed, the current channel should go in to fill mode
  // Fill can only be on one channel at a time
  if(FillButtonState == LOW){
    FillState[currentChannel] = 1;
    if(currentChannel != lastChannel){
        FillState[lastChannel] = 0;
        }
      } 
  else {FillState[currentChannel] = 0;}

  // DELETE BUTTON
  // If the delete button is pressed, do set step of the current pattern to be 0
  if(deletebuttonState == LOW && millis() - time > debounce){
    Sequence[currentChannel + BankState[currentChannel]][currentStep+1] = 0;
    time = millis();  
    }                            

  // RESET BUTTON
  // If the reset button is pressed, set the current steps to 0 (start from step 1 of the patterns)
  if(ResetButtonState == LOW && millis() - time > debounce) {
    currentStep = 0;  
    currentStep1 = 0;
    currentStep2 = 0;
    currentStep3 = 0;
    currentStep4 = 0;
    currentStep5 = 0;
    currentStep6 = 0;
    time = millis();   
   } 

  // Determine how many steps the looping pattern is
  patLengthPotRead = analogRead(patLengthPin);
  if(0<patLengthPotRead){steps= 1;}
  if(150<patLengthPotRead){steps= 2;}
  if(300<patLengthPotRead){steps= 4;}
  if(500<patLengthPotRead){steps= 8;}
  if(750<patLengthPotRead){steps=16;}
  if(1000<patLengthPotRead){steps=32;}                                    

  //this bit starts the sequence over again
  if(currentStep >= steps) {currentStep = 0;}   
  if((currentStep1  + shiftValue1) >= steps) {currentStep1 = 0;}
  if((currentStep2  + shiftValue2) >= steps) {currentStep2 = 0;}
  if((currentStep3  + shiftValue3) >= steps) {currentStep3 = 0;}
  if((currentStep4  + shiftValue4) >= steps) {currentStep4 = 0;}
  if((currentStep5  + shiftValue5) >= steps) {currentStep5 = 0;}
  if((currentStep6  + shiftValue6) >= steps) {currentStep6 = 0;}

  lastResetButtonState = ResetButtonState;   //sectoion is for the state change (debounce?) detections
  lastChannel = currentChannel;              //update the lastChannel, used for the Fill button
  lastShiftValue = shiftValue;
  
  debuger();


} // END LOOP

/*
 *     if (millis() - time >= trigger_length) {
      digitalWrite(OUT1,LOW);
      digitalWrite(OUT2,LOW);
      digitalWrite(OUT3,LOW);
      digitalWrite(OUT4,LOW);
      digitalWrite(OUT5,LOW);
      digitalWrite(OUT6,LOW);
      clkInState = LOW;        //is this needed? 
  
      time = millis();
 */
  



