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
bool is12HourFormat = false; // 預設為 24 小時制
volatile bool buttonPressedB = false;  // 預設為沒按下
volatile unsigned long pressStartTimeB = 0;
volatile unsigned long pressDurationB = 0;
volatile unsigned long lastInterruptTimeB = 0;

//長按設定為 1000 ms
const unsigned long longPressInterval = 1000;
const unsigned long debounceDelay = 200;
// 連接 DS1302
ThreeWire myWire(IO, SCLK, CE);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// 當前顯示狀態
const int showYear = 0;

const int showMonth = 1;  // 設定時的狀態
const int showDay = 2;
const int showHour = 3;
const int showMinute = 4;
const int showSecond = 5;
const int stateSize = 6;  // 設定時的 State 大小

int curState = 0;  // 初始狀態

// 設定
bool settingMode = false;  // 設定模式

void setup()
{
  Serial.begin(57600);
  setupDS1302(Rtc);  // 初始化 DS1302, 只需要在第一次執行時加入就可以了

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

  if (buttonPressedB)  // 按鈕 B 的長短按判斷, 根據 settineMode 的長短按有不同的效果
  {
    noInterrupts();
    unsigned long duration = pressDurationB;
    buttonPressedB = false;
    interrupts();
    if (duration > longPressInterval)
    {
      // 長按 (切換設定模式)
      Serial.println("偵測到長按!");
      settingMode = !settingMode;      // 轉換模式
      Rtc.SetIsRunning(!settingMode);  // SettingMode = true, 停止計時, settingMode = false, 繼續計時
      curState = 0;                    // 顯示狀態設為 0
    }
    else
    {
      // 短按 (在設定模式才有用處, 用於切換當前設定位數)
      Serial.println("偵測到短按!");
      if (settingMode == true)  // 正在設定時間, 短按按鈕 B, 增加當前數字
      {
        settingTime(now);
      }
      else
      {
        Serial.println("轉換小時!");
        // Serial.println(is12HourFormat);
        is12HourFormat = !is12HourFormat; // 轉換小時制
      }
    }
  }
  // 列印結果
  if(settingMode)
  {
    BlinkprintResult(now);
  }
  else
  {
     printResult(now);
  }
}
void settingTime(RtcDateTime &now)
{
  int year = now.Year() % 100;  // 取後面兩位數
  int month = now.Month();
  int day = now.Day();
  int hour = now.Hour();
  int minute = now.Minute();
  int second = now.Second();

  switch (curState)  // 查看現在的顯示狀態
  {
    case showYear:
      if (year == 99) year = 0;
      else year += 1;
      break;  // 年
    case showMonth:
      if (month == 12) month = 1;
      else month += 1;
      break;  // 月
    case showDay:
      if (day == 12) day = 1;
      else day += 1;
      break;  // 日
    case showHour:
      hour = (hour + 1) % 24;
      break;  // 時
    case showMinute:
      minute = (minute + 1) % 60;
      break;  // 分
    case showSecond:
      second = (second + 1) % 60;
      break;  // 秒
    default:
      Serial.println("例外發生!");
      return;
  }

  // 使用具體的日期和時間值來建立新的 RtcDateTime 對象
  now = RtcDateTime(2000 + year, month, day, hour, minute, second);

  // 設置 RTC 時間
  Rtc.SetDateTime(now);
}
// 閃爍
const int blinkInterval = 200;
unsigned long lastBlinkTime = 0;
bool blinkState = false;
void BlinkprintResult(RtcDateTime &now)
{
  if(millis() - lastBlinkTime > blinkInterval)
  {
    blinkState = !blinkState;
    lastBlinkTime = millis();
  }
  if(blinkState == true) // 睜開眼
  {
    printResult(now);
  }
  else // 閉眼
  {
    disableDisplay();
  }
}
void printResult(RtcDateTime &now)
{
  switch (curState)
  {
    case showYear: displayYear(now); break;      // 年
    case showMonth: displayMonth(now); break;    // 月
    case showDay: displayDay(now); break;        // 日
    case showHour: displayHour(now, is12HourFormat); break;      // 時
    case showMinute: displayMinute(now); break;  // 分
    case showSecond: displaySecond(now); break;  // 秒
  }
}
void flashA()  // 硬體中斷, 切換狀態的按鈕
{
  if (millis() - prevTimeA > debounceDelay)
  {
    curState = (curState + 1) % stateSize;  // 普通顯示的範圍縣市
    prevTimeA = millis();
  }
}
void flashB() // 需要判斷長短按
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