#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop()
{
  Serial.println(analogRead(A0));
  delay(1000);
}