// DisplayFunctions.cpp

#include "DisplayFunctions.h"
#include <Arduino.h>

extern const int pinA;
extern const int pinB;
extern const int pinC;
extern const int pinD;
extern const int pinE;
extern const int pinF;
extern const int pinG;
extern const int D1;
extern const int D2;
extern const int D3;
extern const int D4;

void setupDS1302(RtcDS1302<ThreeWire> Rtc)
{
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();
  if (!Rtc.IsDateTimeValid())
  {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }
  if (Rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}
void printDateTime(const RtcDateTime &dt)
{
  char datestring[20];
  snprintf_P(datestring,
             sizeof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}
void displayYear(RtcDateTime now)
{
  int year = now.Year();
  int yearThousands = year / 1000;
  int yearHundreds = (year / 100) % 10;
  int yearTens = (year / 10) % 10;
  int yearUnits = year % 10;

  displayDigitAtPosition(yearThousands, D1);
  displayDigitAtPosition(yearHundreds, D2);
  displayDigitAtPosition(yearTens, D3);
  displayDigitAtPosition(yearUnits, D4);
}
void displayMD(RtcDateTime now)
{
  int month = now.Month();
  int day = now.Day();
  int monthTens = month / 10;
  int monthUnits = month % 10;
  int dayTens = day / 10;
  int dayUnits = day % 10;

  displayDigitAtPosition(monthTens, D1);
  displayDigitAtPosition(monthUnits, D2);
  displayDigitAtPosition(dayTens, D3);
  displayDigitAtPosition(dayUnits, D4);
}
void displayHM(RtcDateTime now)
{
  int hour = now.Hour();
  int minutes = now.Minute();
  int hourTens = hour / 10;
  int hourUnits = hour % 10;
  int minuteTens = minutes / 10;
  int minuteUnits = minutes % 10;

  displayDigitAtPosition(hourTens, D1);
  displayDigitAtPosition(hourUnits, D2);
  displayDigitAtPosition(minuteTens, D3);
  displayDigitAtPosition(minuteUnits, D4);
}
void displayMS(RtcDateTime now)
{
  int minutes = now.Minute();
  int seconds = now.Second();
  int minuteTens = minutes / 10;
  int minuteUnits = minutes % 10;
  int secondTens = seconds / 10;
  int secondUnits = seconds % 10;

  displayDigitAtPosition(minuteTens, D1);
  displayDigitAtPosition(minuteUnits, D2);
  displayDigitAtPosition(secondTens, D3);
  displayDigitAtPosition(secondUnits, D4);
}
void displayMonth(RtcDateTime now)
{
  int months = now.Month();
  int monthTens = months / 10;
  int monthUnits = months % 10;

  displayDigitAtPosition(monthTens, D3);
  displayDigitAtPosition(monthUnits, D4);
}
void displayDay(RtcDateTime now)
{
  int day = now.Day();
  int dayTens = day / 10;
  int dayUnits = day % 10;

  displayDigitAtPosition(dayTens, D3);
  displayDigitAtPosition(dayUnits, D4);
}
void displayHour(RtcDateTime now, bool is12HourFormat)
{
  int hour = now.Hour();  // 0 ~ 23

  if (is12HourFormat)
  {
    bool isPM = hour >= 12;
    if (hour == 0)
    {
      hour = 12; // 0 點
    }
    else if (hour > 12)
    {
      hour -= 12;  // Convert to 12 hour format
    }
    int hourTens = hour / 10;
    int hourUnits = hour % 10;

    displayDigitAtPosition(hourTens, D2);
    displayDigitAtPosition(hourUnits, D3);

    // 顯示 A, P 代表早上或是晚上
    if (isPM)
    {
      displayDigitAtPosition(11, D4);  // P = 11
    }
    else
    {
      displayDigitAtPosition(10, D4);  // A = 10
    }
  }
  else
  {
    // 普通顯示
    int hourTens = hour / 10;
    int hourUnits = hour % 10;

    displayDigitAtPosition(hourTens, D3);
    displayDigitAtPosition(hourUnits, D4);
  }
}
void displayMinute(RtcDateTime now)
{
  int minutes = now.Minute();
  int minuteTens = minutes / 10;
  int minuteUnits = minutes % 10;

  displayDigitAtPosition(minuteTens, D3);
  displayDigitAtPosition(minuteUnits, D4);
}
void displaySecond(RtcDateTime now)
{
  int seconds = now.Second();
  int secondTens = seconds / 10;
  int secondUnits = seconds % 10;

  displayDigitAtPosition(secondTens, D3);
  displayDigitAtPosition(secondUnits, D4);
}
void displayDigitAtPosition(int digit, int position)
{
  // 根據需要選擇顯示的位置
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);

  digitalWrite(position, LOW);
  displayDigit(digit);
  delay(5);  // 延遲以減少閃爍
}
void displayDigit(int digit)  // 顯示對應數字
{
  switch (digit)
  {
    case 0:
      Seg0();
      break;
    case 1:
      Seg1();
      break;
    case 2:
      Seg2();
      break;
    case 3:
      Seg3();
      break;
    case 4:
      Seg4();
      break;
    case 5:
      Seg5();
      break;
    case 6:
      Seg6();
      break;
    case 7:
      Seg7();
      break;
    case 8:
      Seg8();
      break;
    case 9:
      Seg9();
      break;
    case 10:
      SegA();
    case 11:
      SegP();
  }
}
// 10 代表 A, 11 代表 P
void SegA()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
void SegP()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
void Seg0()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, LOW);
}
void Seg1()
{
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);
}
void Seg2()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, LOW);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, HIGH);
}
void Seg3()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, HIGH);
}
void Seg4()
{
  digitalWrite(pinA, LOW);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
void Seg5()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
void Seg6()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
void Seg7()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, LOW);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, LOW);
  digitalWrite(pinG, LOW);
}
void Seg8()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, HIGH);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}
void Seg9()
{
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, HIGH);
  digitalWrite(pinC, HIGH);
  digitalWrite(pinD, HIGH);
  digitalWrite(pinE, LOW);
  digitalWrite(pinF, HIGH);
  digitalWrite(pinG, HIGH);
}