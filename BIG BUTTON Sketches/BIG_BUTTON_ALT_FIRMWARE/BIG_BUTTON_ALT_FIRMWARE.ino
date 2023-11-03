//UPDATE IN PROGRESS//
//DO NOT USE//

//KOSMO BIG BUTTON 2016 SAM BATTLE
//email address is computer@lookmumnocomputer.com
//look mum no computer
//lookmumnocomputer@gmail.com
//www.facebook.com/lookmumnocomputer
//ITS AN ABSOLUTE MESS BUT IT WORKS.... SORT OF.... 
//YOU NEED QUITE A CLEAN CLOCK TRIGGER SIGNAL. QUITE A SHORT PULSE!
//To make it work better add this circuit to the clock input :-
//https://www.cgs.synth.net/modules/cgs24_gatetotrigger.html

//the premise of this is a simple performance sequencer. 
//it is used in synth bike mk2 to sequencer the drums.
//i figured whats the point in not sharing it!!!
//dont be a pleb and steal it for a product or some shit. Build it and
//enjoy it as a musical instrument :)

// Clock in is pin 2
// Clear Button is pin 4 .... this clears the whole loop sequence
// Button Delete is pin 7 This deletes the step your on
// Bank select Pin 3 .... each channel has 2 banks of patterns, so you can record 2 alternative patterns and go between with this button
// BIG BUTTON is pin 19 (A5), this is the performance button!
// Reset in is pin 6 you can add a button or a jack!
// FILL BUTTON pin 5, push this and it will continuously play the channel your on, doesnt record, push it and twist the select knob to make a fill!
// STEP LENGTH analog pin 1 (A1)
// Channel select Analog pin (A0)
// SHIFT KNOB Analog pin (A2)
// LED (big button LED) pin 18 (A4)

// OUTPUT 1 ... pin 8
// OUTPUT 2 ... pin 9
// OUTPUT 3 ... pin 10
// OUTPUT 4 ... pin 11
// OUTPUT 5 ... pin 12
// OUTPUT 6 ... pin 13


///////////////////////////
   // ALT FIRMWARE INFO
///////////////////////////

//USE AT YOUR OWN RISK
//Code may have bugs! I haven't worked in C before.

//ALT FIRMWARE 2018 SKODDIE
//Email ?s to skoddiethecat@gmail.com
//facebook.com/skoddienoise

//Stripboard layout and additional info @ https://www.lookmumnocomputer.com/projects/#/big-button/

//This firmware REQUIRES a few modifications to the layout
//Use a rotary switch with a minimum of 8 positions
//Replace the 2k resistors with 1k resistors and bridge all 8 pins with them
//Wire 3 more 1k resistors in series between the last pin and GND for a total of 10 resistors on the switch

//NEW FEATURES//
//There are 2 new positions on the channel switch
//Switching into them will leave the big button on channel 6 unless a new mode is entered

//CHANNEL 7 - Mode 3//
//Press BANK to enter mode 3
//Sequence length options changes from 1/2/4/8/16/32 to 1/3/6/9/12/24
//Press BANK again to return to default

//CHANNEL 8 - Clock Divider//
//Press BANK to cycle clock division
//This will cause the whole sequencer to ratchet the incoming clock
//Available divisions are 1/2/4/8/16/32
//Turning the channel knob to 1-6 will operate as normal

//Press DELETE to switch the sequencer into ROTATING CLOCK DIVIDER mode
//The unit will now divide the clock on each output, based on the bank

//Bank 1 - Power of 2: 2/4/8/16/32/64
//Bank 2 - :
//Bank 3 - Fibonacci Sequence: 2/3/5/8/13/21
//Bank 4 - :
//Access banks 3 & 4 by pressing BANK on Channel 7

//Press the BIG BUTTON to rotate the division by 1
//Press CLEAR to reverse the direction of rotation (toggle)
//FILL will still work on a per-channel basis
//RESET will reset all channels

//Return to sequencer mode by pressing DELETE on Channel 8

//This has been a complete refactor, so it should be considered a hard fork
//I am running mine as a Eurorack module per LMNC's instructions of wiring +12v to the regulator and GND to GND
///////////////////////////

//VARIABLES//
volatile byte clockState = LOW;

const byte clkIn = 2;
const byte ButtonBankSelect = 3;
const byte ButtonClear = 4;
const byte FillButton = 5;
const byte ResetButton = 6;
const byte ButtonDelete = 7;
const byte BigButton = 19;

const byte OUT1 = 8;
const byte OUT2 = 9;
const byte OUT3 = 10;
const byte OUT4 = 11;
const byte OUT5 = 12;
const byte OUT6 = 13;
const byte BankLED = 18;

byte RecordButtonState = 0;
byte DeleteButtonState = 0;
byte ClearButtonState = 0;
byte ResetButtonState = 0;

byte LastRecordButtonState = 0;
byte LastDeleteButtonState = 0;

byte LastResetButtonState = 0;

byte ClockKeep = 0;
byte ResetSteps = 33;

int channelKnob = 0;


//FILL BUTTON

byte FillButtonState = 0;
byte Fill1 = 0;
byte Fill2 = 0;
byte Fill3 = 0;
byte Fill4 = 0;
byte Fill5 = 0;
byte Fill6 = 0;

//CLEAR

byte ClearState = 0;

//Bank Button Latching
long time = 0;
long debounce = 150;
int ButtonBankSelectState[7];


int BankArrayShift1 = 0;
int BankArrayShift2 = 0;
int BankArrayShift3 = 0;
int BankArrayShift4 = 0;
int BankArrayShift5 = 0;
int BankArrayShift6 = 0;
int BankClear = 0;
int BankRecord = 0;
int BankArrayNumber = 0;
int ButtonBankSelectButton = 0;

//SHIFT KNOB

int KnobValue = 0;
int BankPush1 = 0;
int BankPush2 = 0;
int BankPush3 = 0;
int BankPush4 = 0;
int BankPush5 = 0;
int BankPush6 = 0;
int KnobVal = 0;
int OldKnobValue = 0;
int NewKnobValue = 0;
int NewKnobValue1 = 0;
int NewKnobValue2 = 0;
int NewKnobValue3 = 0;
int NewKnobValue4 = 0;
int NewKnobValue5 = 0;
int NewKnobValue6 = 0;

byte looper = 0;
byte Channel = 1;
byte StepLength = 0;
byte steps = 0;

byte ChannelSelectState1 = 0;
byte ChannelSelectState2 = 0;
byte ChannelSelectState3 = 0;
byte ChannelSelectState4 = 0;
byte ChannelSelectState5 = 0;
byte ChannelSelectState6 = 0;

byte stepState = 0;
byte BankButtonState = 0;


byte BankState[7] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW};
byte Bankprevious[7] = {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
byte Sequence[14][43] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
};

byte stepArray[2][6] = {
    {1,2,4,8,16,32,},
    {1,3,6,9,12,24,},
};

#define TOLERANCE 0

//A lot of ints have been changed to bytes to save memory at runtime
//Not sure if this will have measurable effects, just seemed like the right thing to do
//Worried about bugs, especially on the sequence array


//SETUP//
void setup() {
pinSetup();
readPanel();
//setStepLength();
if(StepLength>0)   {steps = StepArray[stepState][0];}
if(StepLength>150) {steps = StepArray[stepState][1];}
if(StepLength>300) {steps = StepArray[stepState][2];}
if(StepLength>500) {steps = StepArray[stepState][3];}
if(StepLength>750) {steps = StepArray[stepState][4];}
if(StepLength>1000){steps = StepArray[stepState][5];}
channelSelect();
attachInterrupt(digitalPinToInterrupt(2), isr, RISING);

//STARTUP ANIMATION//
digitalWrite(BankLED, HIGH);
delay(200);
digitalWrite(BankLED, LOW);
delay(180);
digitalWrite(BankLED, HIGH);
delay(160);
digitalWrite(BankLED, LOW);
delay(140);
digitalWrite(BankLED, HIGH);
delay(120);
digitalWrite(BankLED, LOW);
delay(100);
digitalWrite(BankLED, HIGH);
delay(80);
digitalWrite(BankLED, LOW);
delay(60);
digitalWrite(BankLED, HIGH);
delay(40);
digitalWrite(BankLED, LOW);
delay(20);
digitalWrite(BankLED, HIGH);
delay(60);
digitalWrite(BankLED, LOW);
}


//MAIN LOOP//
void loop() {
readPanel();
byte stepArray[2][6];

if(clockState == HIGH) {sequenceAdv();}

if(RecordButtonState != LastRecordButtonState){bigButton();}

//LED FLASHING??//
if((ClockKeep == 1)  || (ClockKeep == 5) || (ClockKeep == 9)  || (ClockKeep == 13) ||  (ClockKeep == 17)  || (ClockKeep == 21) || (ClockKeep == 25) || (ClockKeep == 29) && (BankRecord == 0)) {digitalWrite(BankLED,HIGH);}
if((ClockKeep == 1)  || (ClockKeep == 5) || (ClockKeep == 9)  || (ClockKeep == 13) ||  (ClockKeep == 17)  || (ClockKeep == 21) || (ClockKeep == 25) || (ClockKeep == 29) && (BankRecord == 3)) {digitalWrite(BankLED,LOW);}
else {digitalWrite(BankLED, BankState[BankArrayNumber]);}

//SHIFT KNOB//
shiftKnob();

//CHANNEL SELECT//
channelSelect();

//BANK BUTTON//
ButtonBankSelectState[BankArrayNumber] = BankButtonState;
if(ButtonBankSelectState[BankArrayNumber] == HIGH && Bankprevious[BankArrayNumber] == LOW && millis() - time > debounce){
    if(BankState[BankArrayNumber] == HIGH) {BankState[BankArrayNumber] = LOW;}
    else                                   {BankState[BankArrayNumber] = HIGH;time = millis();}
}

//BANK SELECT//
       if((BankState [1] == LOW) && (ChannelSelectState1  == HIGH))                                  {Channel = 1; BankArrayNumber = 1;  BankArrayShift1 = 0;     BankRecord = 0;  ClearState = 1;}
  else if((BankState [2] == LOW) && (ChannelSelectState2  == HIGH))                                  {Channel = 2; BankArrayNumber = 2;  BankArrayShift2 = 0;     BankRecord = 0;  ClearState = 2;}
  else if((BankState [3] == LOW) && (ChannelSelectState3  == HIGH))                                  {Channel = 3; BankArrayNumber = 3;  BankArrayShift3 = 0;     BankRecord = 0;  ClearState = 3;} 
  else if((BankState [4] == LOW) && (ChannelSelectState4  == HIGH))                                  {Channel = 7; BankArrayNumber = 4;  BankArrayShift4 = 0;     BankRecord = 0;  ClearState = 7;} 
  else if((BankState [5] == LOW) && (ChannelSelectState5  == HIGH))                                  {Channel = 8; BankArrayNumber = 5;  BankArrayShift5 = 0;     BankRecord = 0;  ClearState = 8;} 
  else if((BankState [6] == LOW) && (ChannelSelectState6  == HIGH))                                  {Channel = 9; BankArrayNumber = 6;  BankArrayShift6 = 0;     BankRecord = 0;  ClearState = 9;} 
  
  
  else if((BankState [1] == HIGH) && (ChannelSelectState1  == HIGH))                                 {Channel = 1; BankArrayNumber = 1;  BankArrayShift1 = 3;     BankRecord = 3;  ClearState = 1;}
  else if((BankState [2] == HIGH) && (ChannelSelectState2  == HIGH))                                 {Channel = 2; BankArrayNumber = 2;  BankArrayShift2 = 3;     BankRecord = 3;  ClearState = 2;}
  else if((BankState [3] == HIGH) && (ChannelSelectState3  == HIGH))                                 {Channel = 3; BankArrayNumber = 3;  BankArrayShift3 = 3;     BankRecord = 3;  ClearState = 3;} 
  else if((BankState [4] == HIGH) && (ChannelSelectState4  == HIGH))                                 {Channel = 7; BankArrayNumber = 4;  BankArrayShift4 = 3;     BankRecord = 3;  ClearState = 7;} 
  else if((BankState [5] == HIGH) && (ChannelSelectState5  == HIGH))                                 {Channel = 8; BankArrayNumber = 5;  BankArrayShift5 = 3;     BankRecord = 3;  ClearState = 8;}
  else if((BankState [6] == HIGH) && (ChannelSelectState6  == HIGH))                                 {Channel = 9; BankArrayNumber = 6;  BankArrayShift6 = 3;     BankRecord = 3;  ClearState = 9;}

if(ClearButtonState == HIGH){clearBank();}

   if(FillButtonState == HIGH) {drumFill();}
else if (FillButtonState == LOW) {Fill1 = 0; Fill2 = 0; Fill3 = 0; Fill4 = 0;  Fill5 = 0;  Fill6 = 0;}

if(DeleteButtonState == HIGH){Sequence[Channel+BankRecord][looper+1] = 0;}

if((ResetButtonState != LastResetButtonState) && (ResetButtonState == HIGH)){resetSequence();} 

//setStepLength();
if(StepLength>0)   {steps = StepArray[stepState][0];}
if(StepLength>150) {steps = StepArray[stepState][1];}
if(StepLength>300) {steps = StepArray[stepState][2];}
if(StepLength>500) {steps = StepArray[stepState][3];}
if(StepLength>750) {steps = StepArray[stepState][4];}
if(StepLength>1000){steps = StepArray[stepState][5];}

sequenceRestart();


//STATE CHANGE DETECTION//
 LastRecordButtonState = RecordButtonState;
 LastResetButtonState = ResetButtonState;
 Bankprevious[BankArrayNumber] = ButtonBankSelectState[BankArrayNumber]; 
}
 

void isr() {
    clockState = HIGH;
}

void pinSetup() {
//OUTS
pinMode(OUT1,OUTPUT);
pinMode(OUT2,OUTPUT);
pinMode(OUT3,OUTPUT);
pinMode(OUT4,OUTPUT);
pinMode(OUT5,OUTPUT);
pinMode(OUT6,OUTPUT);
pinMode(BankLED, OUTPUT);

//INS
pinMode(clkIn, INPUT);
pinMode(BigButton,INPUT);
pinMode(ButtonDelete,INPUT);
pinMode(ButtonClear,INPUT);

pinMode(ButtonBankSelect, INPUT);
pinMode(ResetButton, INPUT);
pinMode(FillButton, INPUT);
}

void readPanel() {
RecordButtonState = digitalRead(BigButton);
DeleteButtonState = digitalRead(ButtonDelete);
ClearButtonState = digitalRead(ButtonClear); 
ResetButtonState = digitalRead(ResetButton);
FillButtonState = digitalRead(FillButton);
BankButtonState = digitalRead(ButtonBankSelect);
channelKnob = analogRead(0);
StepLength = analogRead(1);
KnobVal = analogRead(2);
}

void sequenceAdv() {
    looper    = (looper   +1);
    BankPush1 = (BankPush1+1);
    BankPush2 = (BankPush2+1);
    BankPush3 = (BankPush3+1);
    BankPush4 = (BankPush4+1);
    BankPush5 = (BankPush5+1);
    BankPush6 = (BankPush6+1);
    ClockKeep = (ClockKeep+1);
    
    digitalWrite(OUT1,Sequence [1+BankArrayShift1][BankPush1 + NewKnobValue1] || (Fill1));
    digitalWrite(OUT2,Sequence [2+BankArrayShift2][BankPush2 + NewKnobValue2] || (Fill2));
    digitalWrite(OUT3,Sequence [3+BankArrayShift3][BankPush3 + NewKnobValue3] || (Fill3));
    digitalWrite(OUT4,Sequence [7+BankArrayShift4][BankPush4 + NewKnobValue4] || (Fill4));
    digitalWrite(OUT5,Sequence [8+BankArrayShift5][BankPush5 + NewKnobValue5] || (Fill5));
    digitalWrite(OUT6,Sequence [9+BankArrayShift6][BankPush6 + NewKnobValue6] || (Fill6));
    delay(10);
    digitalWrite(OUT1,LOW);
    digitalWrite(OUT2,LOW);
    digitalWrite(OUT3,LOW);
    digitalWrite(OUT4,LOW);
    digitalWrite(OUT5,LOW);
    digitalWrite(OUT6,LOW);
    clockState = LOW;
}

void channelSelect() {
     if(20>channelKnob)  {(ChannelSelectState1 = HIGH); (ChannelSelectState2 = LOW); (ChannelSelectState3 = LOW); (ChannelSelectState4 = LOW); (ChannelSelectState5 = LOW);  (ChannelSelectState6 = LOW);}
else if(170<channelKnob) {(ChannelSelectState1 = LOW); (ChannelSelectState2 = HIGH); (ChannelSelectState3 = LOW); (ChannelSelectState4 = LOW); (ChannelSelectState5 = LOW);  (ChannelSelectState6 = LOW);}
else if(240<channelKnob) {(ChannelSelectState1 = LOW); (ChannelSelectState2 = LOW); (ChannelSelectState3 = HIGH); (ChannelSelectState4 = LOW); (ChannelSelectState5 = LOW);  (ChannelSelectState6 = LOW);}
else if(420<channelKnob) {(ChannelSelectState1 = LOW); (ChannelSelectState2 = LOW); (ChannelSelectState3 = LOW);  (ChannelSelectState4 = HIGH);(ChannelSelectState5 = LOW);  (ChannelSelectState6 = LOW);}
else if(750<channelKnob) {(ChannelSelectState1 = LOW); (ChannelSelectState2 = LOW); (ChannelSelectState3 = LOW);  (ChannelSelectState4 = LOW); (ChannelSelectState5 = HIGH); (ChannelSelectState6 = LOW);}
else if(1000<channelKnob){(ChannelSelectState1 = LOW); (ChannelSelectState2 = LOW); (ChannelSelectState3 = LOW);  (ChannelSelectState4 = LOW); (ChannelSelectState5 = LOW);  (ChannelSelectState6 = HIGH);}
}

void bigButton() {
     if((RecordButtonState == HIGH) && (Channel == 1)) {Sequence[Channel+BankRecord][BankPush1+1+NewKnobValue1] = 1;}
else if((RecordButtonState == HIGH) && (Channel == 2)) {Sequence[Channel+BankRecord][BankPush2+1+NewKnobValue2] = 1;}
else if((RecordButtonState == HIGH) && (Channel == 3)) {Sequence[Channel+BankRecord][BankPush3+1+NewKnobValue3] = 1;}
else if((RecordButtonState == HIGH) && (Channel == 7)) {Sequence[Channel+BankRecord][BankPush4+1+NewKnobValue4] = 1;}
else if((RecordButtonState == HIGH) && (Channel == 8)) {Sequence[Channel+BankRecord][BankPush5+1+NewKnobValue5] = 1;}
else if((RecordButtonState == HIGH) && (Channel == 9)) {Sequence[Channel+BankRecord][BankPush6+1+NewKnobValue6] = 1;}                                                                                                                  

delay(5); 
}

/*
void setStepLength() {
if(StepLength>0)   {steps = StepArray[stepState][0];}
if(StepLength>150) {steps = StepArray[stepState][1];}
if(StepLength>300) {steps = StepArray[stepState][2];}
if(StepLength>500) {steps = StepArray[stepState][3];}
if(StepLength>750) {steps = StepArray[stepState][4];}
if(StepLength>1000){steps = StepArray[stepState][5];}
}
*/

void clearBank() {
  for(int i = 1; i<42; i++){
    Sequence[ClearState+BankRecord][i] = 0;
    }
}

void drumFill() {
   if (Channel == 1) {Fill1 = 1; Fill2 = 0; Fill3 = 0; Fill4 = 0;  Fill5 = 0;  Fill6 = 0;} 
else if (Channel == 2) {Fill1 = 0; Fill2 = 1; Fill3 = 0; Fill4 = 0;  Fill5 = 0;  Fill6 = 0;}
else if (Channel == 3) {Fill1 = 0; Fill2 = 0; Fill3 = 1; Fill4 = 0;  Fill5 = 0;  Fill6 = 0;}
else if (Channel == 7) {Fill1 = 0; Fill2 = 0; Fill3 = 0; Fill4 = 1;  Fill5 = 0;  Fill6 = 0;}
else if (Channel == 8) {Fill1 = 0; Fill2 = 0; Fill3 = 0; Fill4 = 0;  Fill5 = 1;  Fill6 = 0;}
else if (Channel == 9) {Fill1 = 0; Fill2 = 0; Fill3 = 0; Fill4 = 0;  Fill5 = 0;  Fill6 = 1;}
}

void resetSequence() {
   looper = 0;  
ClockKeep = 0;
BankPush1 = 0;
BankPush2 = 0;
BankPush3 = 0;
BankPush4 = 0;
BankPush5 = 0;
BankPush6 = 0;
}

void sequenceRestart() {
if(looper >= steps) {looper = 0;}
if(ClockKeep >= 32) {looper = 0; ClockKeep = 0;}
if((BankPush1  + NewKnobValue1) >= steps) {BankPush1 = 0;}
if((BankPush2  + NewKnobValue2) >= steps) {BankPush2 = 0;}
if((BankPush3  + NewKnobValue3) >= steps) {BankPush3 = 0;}
if((BankPush4  + NewKnobValue4) >= steps) {BankPush4 = 0;}
if((BankPush5  + NewKnobValue5) >= steps) {BankPush5 = 0;}
if((BankPush6  + NewKnobValue6) >= steps) {BankPush6 = 0;}
}

void shiftKnob() {
     if (KnobVal>0)   {KnobValue = 0;}
else if (KnobVal>127) {KnobValue = 1;}
else if (KnobVal>254) {KnobValue = 2;}
else if (KnobVal>383) {KnobValue = 3;}
else if (KnobVal>511) {KnobValue = 4;}
else if (KnobVal>638) {KnobValue = 5;}
else if (KnobVal>767) {KnobValue = 6;}
else if (KnobVal>895) {KnobValue = 7;}
else if (KnobVal>1000){KnobValue = 8;}

int diff = abs(KnobValue - OldKnobValue);

     if((diff > TOLERANCE) && (ChannelSelectState1 == HIGH)) {NewKnobValue1 = KnobValue;OldKnobValue = KnobValue;}     
else if((diff > TOLERANCE) && (ChannelSelectState2 == HIGH)) {NewKnobValue2 = KnobValue;OldKnobValue = KnobValue;}  
else if((diff > TOLERANCE) && (ChannelSelectState3 == HIGH)) {NewKnobValue3 = KnobValue;OldKnobValue = KnobValue;}
else if((diff > TOLERANCE) && (ChannelSelectState4 == HIGH)) {NewKnobValue4 = KnobValue;OldKnobValue = KnobValue;}
else if((diff > TOLERANCE) && (ChannelSelectState5 == HIGH)) {NewKnobValue5 = KnobValue;OldKnobValue = KnobValue;}
else if((diff > TOLERANCE) && (ChannelSelectState6 == HIGH)) {NewKnobValue6 = KnobValue;OldKnobValue = KnobValue;}
}

//SKODDIE - I AM NOT USING THIS RIGHT NOW AS IT WAS COMMENTED OUT, BUT I//
//DONT WANT TO DELETE IT YET. PROBABLY DON'T COMPILE IT?//
void unusedShiftKnobThing() {
//THIS MAKES SURE WHEN THE KNOB SHIFTS THE PATTERN IT DOESNT LOSE A LOAD OF IT!//
for(int ii = 1; ii<14; ii++){
Sequence[ii][33]=Sequence[ii][1];
Sequence[ii][34]=Sequence[ii][2];
Sequence[ii][35]=Sequence[ii][3];
Sequence[ii][36]=Sequence[ii][4];
Sequence[ii][37]=Sequence[ii][5];
Sequence[ii][38]=Sequence[ii][6];
Sequence[ii][39]=Sequence[ii][7];
Sequence[ii][40]=Sequence[ii][8];
Sequence[ii][41]=Sequence[ii][9]; 
}
