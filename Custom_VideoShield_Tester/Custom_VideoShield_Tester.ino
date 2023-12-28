
//Define Analog Potentiometer Pins
#define pot1Pin A0
#define pot2Pin A2 //note here that the stripboard wires cross, hence out of order
#define pot3Pin A1
#define pot4Pin A3

//Define Digial Button Pins
#define button1Pin 13
#define button2Pin 12
#define button3Pin 11
#define button4Pin 10

// Initialize button press count variables:
byte button1 = 0;
byte button2 = 0;
byte button3 = 0;
byte button4 = 0;

// Initialize last pot value variables:
int potRead1_last = 0;
int potRead2_last = 0;
int potRead3_last = 0;
int potRead4_last = 0;

void setup() {
  //Start serial
  Serial.begin(9600); 

  //Set button pins to use internal Pullup resistors
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);

  // Get initial values of pots
  potRead1_last = analogRead(pot1Pin);
  potRead2_last = analogRead(pot2Pin);
  potRead3_last = analogRead(pot3Pin);
  potRead4_last = analogRead(pot4Pin);
}

void loop() {

  //Read pot values:
  int potRead1 = analogRead(pot1Pin);
  int potRead2 = analogRead(pot2Pin);
  int potRead3 = analogRead(pot3Pin);
  int potRead4 = analogRead(pot4Pin);

  // Display pot values if changed
  potRead1_last = print_pot_values(1, potRead1_last, potRead1);
  potRead2_last = print_pot_values(2, potRead2_last, potRead2);
  potRead3_last = print_pot_values(3, potRead3_last, potRead3);
  potRead4_last = print_pot_values(4, potRead4_last, potRead4);
  
  //Read button values:
  //NOTE: Button press == LOW
  int buttonRead1 = digitalRead(button1Pin);
  int buttonRead2 = digitalRead(button2Pin);
  int buttonRead3 = digitalRead(button3Pin);
  int buttonRead4 = digitalRead(button4Pin);

  // Display button counter values if button pressed
  button1 = print_button_values(1, buttonRead1, button1);
  button2 = print_button_values(2, buttonRead2, button2);
  button3 = print_button_values(3, buttonRead3, button3);
  button4 = print_button_values(4, buttonRead4, button4);

  delay(100);

}

int print_pot_values(byte pot_num, int last, int current)
{
  // Only print values if significantly different from previously recorded value
  if (abs(last - current) > 10)
  {
    Serial.print("Pot");Serial.print(pot_num);Serial.print(" = "); 
    Serial.println(current); 

    return current;
  }
  else 
  {
    return last;
  }
}

byte print_button_values(byte but_num, int current_read, byte counter)
{
  if (current_read == LOW) {
     counter = counter + 1;
     Serial.print("Button");Serial.print(but_num);Serial.print(" = "); 
     Serial.println(counter); 
  }

  return counter;
}