#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>

/*
VARIÁVEIS DE CONFIGURAÇÃO
*/

#define ENABLE_FAN_TEMPERATURE 30
#define TEMPERATURE_HYSTERESIS 2

#define MIN_HUMIDITY 30

#define MIN_ILLUMINATION_IN_PERIOD 20
int ILLUMINATION_PERIOD[2][3] = {{12, 0, 0}, {18, 0, 0}};

#define CLOCK_RST_PIN 8
#define CLOCK_DATA_PIN 7
#define CLOCK_CLK_PIN 6

#define HUMIDITY_SENSOR_PIN A0
#define LIGHT_SENSOR_PIN A1
#define TEMPERATURE_SENSOR_PIN 12

#define ILLUMINATION 2
#define HUMIDITY_ALERT_LED 13
#define FAN 4

/*
 INICIO DO PROGRAMA
*/

byte degreeCustomChar[] = {B01110, B10001, B10001, B10001, B01110, B00000, B00000, B00000};
byte humidityCustomChar[] = {B00100, B00100, B01110, B01110, B11111, B11111, B11111, B01110};
byte lightCustomChar[] = {B00000, B10101, B01110, B11111, B01110, B10101, B00000, B00000};

#define DEGREE_CHAR 0
#define HUMIDITY_CHAR 1
#define LIGHT_CHAR 2

LiquidCrystal_I2C lcd(0x27, 16, 2);
ThreeWire myWire(CLOCK_DATA_PIN, CLOCK_CLK_PIN, CLOCK_RST_PIN);
RtcDS1302<ThreeWire> rtc(myWire);
dht DHT;

bool isFanEnabled = false;

float getHumidity()
{
  return 100.0 - ((analogRead(HUMIDITY_SENSOR_PIN) / 1023.0) * 100.0);
}

int getTemperature()
{
  DHT.read11(TEMPERATURE_SENSOR_PIN);
  return DHT.temperature;
}

float getLightPercentage()
{
  return (analogRead(LIGHT_SENSOR_PIN) / 1023.0) * 100.0;
}

void temperatureMonitor()
{
  float temperature = getTemperature();

  if (
      (temperature >= ENABLE_FAN_TEMPERATURE)) //||
                                               // (isFanEnabled && temperature >= (ENABLE_FAN_TEMPERATURE - TEMPERATURE_HYSTERESIS)))
  {
    Serial.println("Fan on!");
    isFanEnabled = true;
    digitalWrite(FAN, HIGH);
    return;
  }

  Serial.println("Fan off!");
  digitalWrite(FAN, LOW);
  isFanEnabled = false;
}

void humidityMonitor()
{
  float humidity = getHumidity();

  if (humidity < MIN_HUMIDITY)
  {
    Serial.println("Humidity alert!");
    digitalWrite(HUMIDITY_ALERT_LED, HIGH);
    return;
  }

  Serial.println("Humidity alert!");
  digitalWrite(HUMIDITY_ALERT_LED, LOW);
}

void lightingManager()
{
  float lightPercentage = getLightPercentage();
  // RtcDateTime now = rtc.GetDateTime();

  const int hour = 15;  // now.Hour();
  const int minute = 0; // now.Minute();
  const int second = 0; // now.Second();

  const bool isInInterval = (hour >= ILLUMINATION_PERIOD[0][0] && hour <= ILLUMINATION_PERIOD[1][0]) &&
                            (minute >= ILLUMINATION_PERIOD[0][1] && minute <= ILLUMINATION_PERIOD[1][1]) &&
                            (second >= ILLUMINATION_PERIOD[0][2] && second <= ILLUMINATION_PERIOD[1][2]);

  if (isInInterval)
  {
    if (lightPercentage < 20)
    {
      Serial.println("Lighting on!");
      digitalWrite(ILLUMINATION, HIGH);
      return;
    }
  }

  Serial.println("Lighting off!");
  digitalWrite(ILLUMINATION, LOW);
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting...");

  pinMode(HUMIDITY_SENSOR_PIN, INPUT);
  pinMode(TEMPERATURE_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  pinMode(ILLUMINATION, OUTPUT);
  pinMode(HUMIDITY_ALERT_LED, OUTPUT);
  pinMode(FAN, OUTPUT);

  // rtc.Begin();
  // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  // rtc.SetDateTime(compiled);

  lcd.init();
  lcd.createChar(DEGREE_CHAR, degreeCustomChar);
  lcd.createChar(HUMIDITY_CHAR, humidityCustomChar);
  lcd.createChar(LIGHT_CHAR, lightCustomChar);
  lcd.backlight();
  lcd.clear();
  Serial.println("Started!");
}

void updateSerial()
{
  lcd.clear();

  lcd.print(getTemperature());
  lcd.print("C");
  lcd.write(DEGREE_CHAR);

  lcd.setCursor(8, 0);

  lcd.print(getLightPercentage());
  lcd.print("% ");
  lcd.write(LIGHT_CHAR);

  lcd.setCursor(0, 1);

  lcd.write(HUMIDITY_CHAR);
  lcd.print(" ");
  lcd.print(getHumidity());
  lcd.print("%");
}

void loop()
{
  Serial.println("Looping...");

  updateSerial();

  // temperatureMonitor();
  digitalWrite(FAN, HIGH);
  humidityMonitor();
  lightingManager();

  delay(1000);
}
