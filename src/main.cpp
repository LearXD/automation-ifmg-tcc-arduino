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

#define ILLUMINATION 5
#define HUMIDITY_ALERT_LED 6
#define FAN 3

// CUSTOMIZAÇÃO DO LCD
byte degreeCustomChar[] = {B01110, B10001, B10001, B10001, B01110, B00000, B00000, B00000};
byte humidityCustomChar[] = {B00100, B00100, B01110, B01110, B11111, B11111, B11111, B01110};
byte lightCustomChar[] = {B00000, B10101, B01110, B11111, B01110, B10101, B00000, B00000};

#define DEGREE_CHAR 0
#define HUMIDITY_CHAR 1
#define LIGHT_CHAR 2

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

void temperatureMonitor()
{
  float temperature = getTemperature();
  // digitalWrite(FAN, temperature >= ENABLE_FAN_TEMPERATURE);
}

// Função que inicializa todos os componentes
void setup()
{
  Serial.begin(9600);

  pinMode(HUMIDITY_SENSOR_PIN, INPUT);
  pinMode(TEMPERATURE_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  lcd.init();

  lcd.createChar(DEGREE_CHAR, degreeCustomChar);
  lcd.createChar(HUMIDITY_CHAR, humidityCustomChar);
  lcd.createChar(LIGHT_CHAR, lightCustomChar);

  lcd.backlight();
  lcd.clear();
}

// Função que atualiza o LCD
void updateSerial()
{
  lcd.clear();

  lcd.print(getTemperature());
  lcd.print("C");
  lcd.write(DEGREE_CHAR);

  lcd.setCursor(8, 0);

  lcd.print(getLight());
  lcd.print("% ");
  lcd.write(LIGHT_CHAR);

  lcd.setCursor(0, 1);

  lcd.write(HUMIDITY_CHAR);
  lcd.print(" ");
  lcd.print(getHumidity());
  lcd.print("%");
}

// Função que executa o loop principal
void loop()
{
  updateSerial();

  // Chamada de Controladores
  temperatureMonitor();

  // Delay para impedir que o LCD fique piscando
  delay(1000);
}
