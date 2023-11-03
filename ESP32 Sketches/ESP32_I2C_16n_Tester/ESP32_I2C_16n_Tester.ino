//16n faderbank i2c address: 0x34

#include <Wire.h>


void setup() {
  Wire.begin();  // with ESP32, you can use almost any pins for I2C...declare here: Wire.begin(I2C_SDA, I2C_SCL);
                 // default is GPIO21 = SDA and DGPIO22 = SCL

  
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop()
{
  Wire.beginTransmission(0x34);
  Wire.requestFrom(0x34, 6);    // request 6 bytes from slave device #2

  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
  Wire.endTransmission();

  delay(500);
}
