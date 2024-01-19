#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>

/*
VARIÁVEIS DE CONFIGURAÇÃO
*/

#define ENABLE_FAN_TEMPERATURE 30
#define TEMPERATURE_HYSTERESIS -1

#define MIN_HUMIDITY 30

#define MIN_ILLUMINATION_IN_PERIOD 20
int ILLUMINATION_PERIOD[2][3] = {{7, 0, 0}, {18, 0, 0}};

#define CLOCK_RST_PIN 2
#define CLOCK_DATA_PIN 4
#define CLOCK_CLK_PIN 5

#define HUMIDITY_SENSOR_PIN A0
#define LIGHT_SENSOR_PIN A1
#define TEMPERATURE_SENSOR_PIN 12

#define ILLUMINATION 11
#define FAN 8

/*
 INICIO DO PROGRAMA
*/

#define DEGREE_CHAR 0
#define HUMIDITY_CHAR 1
#define LIGHT_CHAR 2

byte degreeCustomChar[] = {B01110, B10001, B10001, B10001, B01110, B00000, B00000, B00000};
byte humidityCustomChar[] = {B00100, B00100, B01110, B01110, B11111, B11111, B11111, B01110};
byte lightCustomChar[] = {B00000, B10101, B01110, B11111, B01110, B10101, B00000, B00000};

LiquidCrystal_I2C lcd(0x27, 16, 2);                             // Biblioteca para comunicação com o LCD
ThreeWire myWire(CLOCK_DATA_PIN, CLOCK_CLK_PIN, CLOCK_RST_PIN); // Biblioteca para comunicação serial com o relógio
RtcDS1302<ThreeWire> rtc(myWire);                               // Biblioteca do Relógio
dht DHT;                                                        // Biblioteca de temperatura

float getHumidity()
{
  return 100.0 - (analogRead(HUMIDITY_SENSOR_PIN) / 1023.0 * 100.0);
}

int getTemperature()
{
  DHT.read11(TEMPERATURE_SENSOR_PIN);
  return DHT.temperature;
}

float getLightPercentage()
{
  return (100 - (analogRead(LIGHT_SENSOR_PIN) / 1023.0) * 100.0);
}

bool isFanEnabled = false;
void temperatureMonitor()
{
  float temperature = getTemperature();

  if (
      (temperature >= ENABLE_FAN_TEMPERATURE) ||
      (isFanEnabled && (temperature > (ENABLE_FAN_TEMPERATURE + TEMPERATURE_HYSTERESIS))))
  {
    Serial.println("[DEBUG] Fan on!");
    isFanEnabled = true;
    digitalWrite(FAN, LOW);
    return;
  }

  Serial.println("[DEBUG] Fan off!");
  digitalWrite(FAN, HIGH);
  isFanEnabled = false;
}

boolean checkHumidity()
{
  return (getHumidity() < MIN_HUMIDITY);
}

void lightingManager()
{
  float lightPercentage = getLightPercentage();
  RtcDateTime now = rtc.GetDateTime();

  const int hour = now.Hour();

  const bool isInInterval = (hour >= ILLUMINATION_PERIOD[0][0] && hour <= ILLUMINATION_PERIOD[1][0]);

  if (isInInterval)
  {
    if (lightPercentage < 20)
    {
      Serial.println("[DEBUG] Lighting on!");
      digitalWrite(ILLUMINATION, HIGH);
      return;
    }
  }

  Serial.println("[DEBUG] Lighting off!");
  digitalWrite(ILLUMINATION, LOW);
}

void initClock()
{
  rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  rtc.SetDateTime(compiled);

  if (!rtc.GetIsRunning())
  {
    Serial.println("[DEBUG] RTC was not actively running, starting now");
    rtc.SetIsRunning(true);
  }

  if (!rtc.IsDateTimeValid())
  {
    Serial.println("[DEBUG] RTC lost confidence in the DateTime!");
    rtc.SetDateTime(compiled);
  }

  if (rtc.GetIsWriteProtected())
  {
    Serial.println("[DEBUG] RTC was write protected, enabling writing now");
    rtc.SetIsWriteProtected(false);
  }

  if (!rtc.GetIsRunning())
  {
    Serial.println("[DEBUG] RTC was not actively running, starting now");
    rtc.SetIsRunning(true);
  }
}

void initDisplay()
{
  lcd.init();
  lcd.createChar(DEGREE_CHAR, degreeCustomChar);
  lcd.createChar(HUMIDITY_CHAR, humidityCustomChar);
  lcd.createChar(LIGHT_CHAR, lightCustomChar);
  lcd.backlight();
  lcd.clear();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("[DEBUG] Starting...");

  pinMode(HUMIDITY_SENSOR_PIN, INPUT);
  pinMode(TEMPERATURE_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  pinMode(ILLUMINATION, OUTPUT);
  pinMode(FAN, OUTPUT);

  initDisplay();
  initClock();

  Serial.println("[DEBUG] Started!");
}

void updateSerial()
{
  lcd.clear();

  if (checkHumidity())
  {
    lcd.print(" Umidade Baixa!");
    delay(1000);
    lcd.clear();
  }

  RtcDateTime now = rtc.GetDateTime();
  lcd.print(now.Hour());
  lcd.print(":");
  lcd.print(now.Minute());

  lcd.setCursor(0, 1);

  lcd.print(getTemperature());
  lcd.print("C");
  lcd.write(DEGREE_CHAR);

  lcd.setCursor(7, 0);

  lcd.write(LIGHT_CHAR);
  lcd.print(" ");
  lcd.print(getLightPercentage());
  lcd.print("% ");

  lcd.setCursor(7, 1);
  lcd.write(HUMIDITY_CHAR);
  lcd.print(" ");
  lcd.print(getHumidity());
  lcd.print("%");
}

int unsigned long lastSerialUpdate = millis();
void loop()
{
  Serial.println("[DEBUG] Looping...");

  if ((millis() - lastSerialUpdate) >= 2000)
  {
    updateSerial();
    lastSerialUpdate = millis();
  }

  temperatureMonitor();
  lightingManager();

  delay(100);
}