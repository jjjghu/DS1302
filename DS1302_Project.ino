#include <RtcDS1302.h>
const int CE   = A2; // RST
const int IO   = A3; // DAT
const int SCLK = 4; // CLK
const int pinA = 5;
const int pinB = 6;
const int pinC = 7;
const int pinD = 8;
const int pinE = 9;
const int pinF = 10;
const int pinG = 11;
const int D1 = 12;
const int D2 = 13;
#define D3 A0
#define D4 A1

// 按鈕, 避免 debounce, 若要中斷, 需要使用 2, 3 Pin腳
#define buttonA 2
unsigned long prevTimeA = 0;
const int interval = 500;

ThreeWire myWire(IO,SCLK,CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

const int showYear = 0;
const int showMonth = 1;
const int showHM = 2;
const int showMS = 3;
const int stateSize = 4; // 加入 state 需要更改
int curState = 3;
void setup() 
{
  Serial.begin(57600);
  setupDS1302(); // 初始化 DS1302

  pinMode(pinA, OUTPUT); // 顯示器的七個位置
  pinMode(pinB, OUTPUT);     
  pinMode(pinC, OUTPUT);     
  pinMode(pinD, OUTPUT);     
  pinMode(pinE, OUTPUT);     
  pinMode(pinF, OUTPUT);     
  pinMode(pinG, OUTPUT);   

  pinMode(D1, OUTPUT); // 顯示器 ENABLE
  pinMode(D2, OUTPUT);  
  pinMode(D3, OUTPUT);  
  pinMode(D4, OUTPUT);
  
  pinMode(buttonA, INPUT); // 按鈕輸入
  prevTimeA = millis();
}
RtcDateTime now;
void loop() 
{
    now = Rtc.GetDateTime(); // 獲取當前 DS1302 的時間
    if(millis() - prevTimeA > interval)
    {
      if(digitalRead(buttonA) == HIGH)
      {
        curState = (curState + 1) % stateSize;
        prevTimeA = millis();
      }
    }
    int year = now.Year();
    int month = now.Month();
    int day = now.Day();
    int hour = now.Hour();
    int minutes = now.Minute();
    int seconds = now.Second();
    switch(curState)
    {
      case showYear: displayYear(); break; // 顯示年份 
      case showMonth: displayMonth(); break; // 月份 + 日期
      case showHM: displayHM(); break; // 小時 + 分鐘
      case showMS: displayMS(); break; // 分鐘 + 秒
    }
}
//////////////////////////////////////////////////////////
void setupDS1302()
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
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

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
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
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
               dt.Second() );
    Serial.print(datestring);
}
void displayYear()
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
void displayMonth()
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
void displayHM()
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
void displayMS()
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
void displayDigitAtPosition(int digit, int position)
{
    // 根據需要選擇顯示的位置
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH);
    
    digitalWrite(position, LOW);
    displayDigit(digit);
    delay(5); // 延遲以減少閃爍
}
void displayDigit(int digit) //顯示對應數字
{
  switch (digit)
  {
      case 0: Seg0(); break;
      case 1: Seg1(); break;
      case 2: Seg2(); break;
      case 3: Seg3(); break;
      case 4: Seg4(); break;
      case 5: Seg5(); break;
      case 6: Seg6(); break;
      case 7: Seg7(); break;
      case 8: Seg8(); break;
      case 9: Seg9(); break;
  }
}
void  Seg0()
{
  //0
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, HIGH);   
  digitalWrite(pinE, HIGH);   
  digitalWrite(pinF, HIGH);   
  digitalWrite(pinG, LOW);   
}void Seg1()
{
  //1
  digitalWrite(pinA, LOW);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, LOW);   
  digitalWrite(pinE, LOW);   
  digitalWrite(pinF, LOW);   
  digitalWrite(pinG, LOW);   
}void Seg2()
{
  //2
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, LOW);   
  digitalWrite(pinD, HIGH);   
  digitalWrite(pinE, HIGH);   
  digitalWrite(pinF, LOW);   
  digitalWrite(pinG, HIGH);   
}void Seg3()
{
  //3
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, HIGH);   
  digitalWrite(pinE, LOW);   
  digitalWrite(pinF, LOW);   
  digitalWrite(pinG, HIGH);   
}void Seg4()
{
  //4
  digitalWrite(pinA, LOW);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, LOW);   
  digitalWrite(pinE, LOW);   
  digitalWrite(pinF, HIGH);   
  digitalWrite(pinG, HIGH);   
}void Seg5()
{
  //5
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, LOW);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, HIGH);   
  digitalWrite(pinE, LOW);   
  digitalWrite(pinF, HIGH);   
  digitalWrite(pinG, HIGH);   
}void Seg6()
{
  //6
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, LOW);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, HIGH);   
  digitalWrite(pinE, HIGH);   
  digitalWrite(pinF, HIGH);   
  digitalWrite(pinG, HIGH);   
}void Seg7()
{
  //0
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, LOW);   
  digitalWrite(pinE, LOW);   
  digitalWrite(pinF, LOW);   
  digitalWrite(pinG, LOW);   
}void Seg8()
{
  //8
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
  //9
  digitalWrite(pinA, HIGH);   
  digitalWrite(pinB, HIGH);   
  digitalWrite(pinC, HIGH);   
  digitalWrite(pinD, HIGH);   
  digitalWrite(pinE, LOW);   
  digitalWrite(pinF, HIGH);   
  digitalWrite(pinG, HIGH);   
}
//////////////////////////////////////////////////////////
