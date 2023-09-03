/*
  Inclusão de bibliotecas
*/
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>

/*
  Definição de constantes
*/
#define HUMIDITY_SENSOR_PIN A0
#define TEMPERATURE_SENSOR_PIN 2
#define LIGHT_SENSOR_PIN A2

byte degreeCustomChar[] = {
    B01110,
    B10001,
    B10001,
    B10001,
    B01110,
    B00000,
    B00000,
    B00000};
byte humidityCustomChar[] = {
    B00100,
    B00100,
    B01110,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110};
byte lightCustomChar[] = {
    B00000,
    B10101,
    B01110,
    B11111,
    B01110,
    B10101,
    B00000,
    B00000};

// Inicialização de objetos
LiquidCrystal_I2C lcd(0x27, 16, 2);
dht DHT;

// Função que obtem a umidade da terra em porcentagem
float getHumidity()
{
  return 100.0 - ((analogRead(HUMIDITY_SENSOR_PIN) / 1023.0) * 100.0);
}

// Função que obtem a temperatura do ar em unidade
int getTemperature()
{
  DHT.read11(TEMPERATURE_SENSOR_PIN);
  return DHT.temperature;
}

// Função que obtem a luminosidade em porcentagem
float getLight()
{
  return (analogRead(LIGHT_SENSOR_PIN) / 1023.0) * 100.0;
}

// Função que inicializa todos os componentes
void setup()
{
  Serial.begin(9600);

  pinMode(HUMIDITY_SENSOR_PIN, INPUT);
  pinMode(TEMPERATURE_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  pinMode(7, OUTPUT);

  lcd.init();

  lcd.createChar(0, degreeCustomChar);
  lcd.createChar(1, humidityCustomChar);
  lcd.createChar(2, lightCustomChar);

  lcd.backlight();
  lcd.clear();
}

// Função que atualiza o LCD
void updateSerial()
{
  lcd.clear();

  lcd.print(getTemperature());
  lcd.print("C");
  lcd.write(0);

  lcd.setCursor(8, 0);

  lcd.print(getLight());
  lcd.print("% ");
  lcd.write(2);

  lcd.setCursor(0, 1);

  lcd.write(1);
  lcd.print(" ");
  lcd.print(getHumidity());
  lcd.print("%");
}

// Função que executa o loop principal
void loop()
{
  updateSerial();

  Serial.println();
  Serial.println("Temperatura: " + String(getTemperature()) + " C");
  Serial.println("Umidade: " + String(getHumidity()) + " %");
  Serial.println("Luminosidade: " + String(getLight()) + " %");
  Serial.println();

  // Delay para impedir que o LCD fique piscando
  delay(1000);
}
