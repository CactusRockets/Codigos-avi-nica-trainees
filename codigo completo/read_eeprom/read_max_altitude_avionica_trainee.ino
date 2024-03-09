#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  float value;
  // put your main code here, to run repeatedly:
  EEPROM.get(0, value);
  Serial.println(value, DEC);

  delay(2000);
}
