#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

const byte RS = 6, BL = 9, BN = 5, EN = 7, D4 = 10, D5 = 11, D6 = 12, D7 = 13;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(4, 3);
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void printTimeOnDisplay(TinyGPSTime&, TinyGPSDate&, LiquidCrystal&);
void sendTimeOnSerial(TinyGPSTime&);
void smartDelay(unsigned long);


void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  pinMode(BN, OUTPUT);
  pinMode(BL, OUTPUT);

  analogWrite(BN, 0);
  analogWrite(BL, 128);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Setup done");
  smartDelay(1000);
  lcd.clear();
}

void loop()
{
  sendTimeOnSerial(gps.time);
  printTimeOnDisplay(gps.time, gps.date, lcd);
  smartDelay(1000);
}

void printTimeOnDisplay(TinyGPSTime& time, TinyGPSDate& date,
  LiquidCrystal& lcd)
{
  lcd.setCursor(0, 0);

  lcd.print(time.hour());
  lcd.print(':');
  lcd.print(time.minute());
  lcd.print(':');
  lcd.print(time.second());
  lcd.print('.');
  lcd.print(time.centisecond());
  lcd.print("    ");

  if (time.isUpdated())
    lcd.print(" *");

  lcd.setCursor(0, 1);
  lcd.print(date.day());
  lcd.print('/');
  lcd.print(date.month());
  lcd.print('/');
  lcd.print(date.year());
  lcd.print(" UTC");
}

void sendTimeOnSerial(TinyGPSTime& time)
{
  Serial.print(time.hour());
  Serial.print(':');
  Serial.print(time.minute());
  Serial.print(':');
  Serial.print(time.second());
  Serial.print('.');
  Serial.print(time.centisecond());
  Serial.println();
}

void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}