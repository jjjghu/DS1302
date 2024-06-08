#include <RtcDS1302.h>
#include "pins.h"
#include "DisplayFunctions.h"  // 包含頭文件
// Pin 腳
const int CE = A2;   // RST
const int IO = A3;   // DAT
const int SCLK = 4;  // CLK
const int pinA = 5;
const int pinB = 6;
const int pinC = 7;
const int pinD = 8;
const int pinE = 9;
const int pinF = 10;
const int pinG = 11;
const int D1 = 12;
const int D2 = 13;
const int D3 = A0;
const int D4 = A1;

// 按鈕, 避免 debounce, 若要中斷, 需要使用 2, 3 Pin 腳
#define buttonA 2
#define buttonB 3
unsigned long prevTimeA = 0;
unsigned long prevTimeB = 0;

// 按鈕 B 需要判斷長按
volatile bool buttonPressedB = false;  // 預設為沒按下
volatile unsigned long pressStartTimeB = 0;
volatile unsigned long pressDurationB = 0;
volatile unsigned long lastInterruptTimeB = 0;
//長按設定為 1000 ms
const unsigned long longPressInterval = 1000;
const unsigned long debounceDelay = 50;
// 連接 DS1302
ThreeWire myWire(IO, SCLK, CE);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// 當前顯示狀態
const int showYear = 0;
const int showMonth = 1;
const int showHM = 2;
const int showMS = 3;
const int stateSize = 4;  // 加入 state 需要更改
int curState = 3;

bool settingMode = false;       // 設定模式
int currentAdjustPosition = 0;  // 0: Year, 1: Month, 2: Day, 3: Hour, 4: Minute, 5: Second
RtcDateTime settingTime;        // 設定的時間


void setup()
{
  Serial.begin(57600);
  // setupDS1302(Rtc);  // 初始化 DS1302, 只需要在第一次執行時加入就可以了

  pinMode(pinA, OUTPUT);  // 顯示器的七個位置
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);

  pinMode(D1, OUTPUT);  // 顯示器 ENABLE
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  pinMode(buttonA, INPUT_PULLUP);  // 按鈕輸入
  pinMode(buttonB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonA), flashA, FALLING);  // 當 button 狀態從 HIGH 到 LOW, 觸發flash
  attachInterrupt(digitalPinToInterrupt(buttonB), flashB, CHANGE);   // 改變時就會觸發
  prevTimeA = millis();
  prevTimeB = millis();
}
void loop()
{
  RtcDateTime now = now = Rtc.GetDateTime();  // 獲取當前 DS1302 的時間
  int year = now.Year();
  int month = now.Month();
  int day = now.Day();
  int hour = now.Hour();
  int minutes = now.Minute();
  int seconds = now.Second();
  if (buttonPressedB)
  {
    noInterrupts();
    unsigned long duration = pressDurationB;
    buttonPressedB = false;
    interrupts();
    if (duration >= longPressInterval)
    {
      // 長按 (切換設定模式)
      Serial.println("偵測到長按!");
      settingMode = !settingMode;
    }
    else
    {
      // 短按 (在設定模式才有用處, 用於切換當前設定位數)
      Serial.println("偵測到短按!");
      if (settingMode == true)  // 正在設定時間, 短按按鈕 B
      {
        currentAdjustPosition += 1;
        currentAdjustPosition %= 6;  // 年月日時分表, 共六種
      }
    }
  }
  // ----------------------------------------------------//
  //
  if (Serial.available())
  {
    String input = Serial.readString();
    if (input.length() == 14)
    {  // 確保輸入的長度正確
      int newYear = input.substring(0, 4).toInt();
      int newMonth = input.substring(4, 6).toInt();
      int newDay = input.substring(6, 8).toInt();
      int newHour = input.substring(8, 10).toInt();
      int newMinute = input.substring(10, 12).toInt();
      int newSecond = input.substring(12, 14).toInt();
      RtcDateTime newTime = RtcDateTime(newYear, newMonth, newDay, newHour, newMinute, newSecond);
      Rtc.SetDateTime(newTime);
      now = newTime;
      Serial.println("時間更新成功!");
    }
    else
    {
      Serial.println("錯誤的輸入: 型態應為 YYYYMMDDHHMMSS:");
    }
  }
  // ---------------------------------------------------- //
  // 列印結果
  switch (curState)
  {
    case showYear: displayYear(now); break;  // 顯示年份
    case showMonth: displayMD(now); break;   // 月份 + 日期
    case showHM: displayHM(now); break;      // 小時 + 分鐘
    case showMS: displayMS(now); break;      // 分鐘 + 秒
  }
}
void flashA()  // 硬體中斷, 切換狀態的按鈕
{
  if (millis() - prevTimeA > debounceDelay)
  {
    prevTimeA = millis();
    curState = (curState + 1) % stateSize;
  }
}
void flashB()
{
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTimeB > debounceDelay)
  {
    if (digitalRead(buttonB) == LOW)
    {
      pressStartTimeB = currentTime;
    }
    else
    {
      pressDurationB = currentTime - pressStartTimeB;
      buttonPressedB = true;
    }
    lastInterruptTimeB = currentTime;
  }
}