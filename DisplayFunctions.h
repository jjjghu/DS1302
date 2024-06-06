// DisplayFunctions.h

#ifndef DISPLAYFUNCTIONS_H
#define DISPLAYFUNCTIONS_H

#include <RtcDS1302.h>

void setupDS1302(RtcDS1302<ThreeWire> Rtc);
void printDateTime(const RtcDateTime &dt);

void displayYear(RtcDateTime now);
void displayMD(RtcDateTime now);
void displayHM(RtcDateTime now);
void displayMS(RtcDateTime now);

void displayDay(RtcDateTime now);
void displayHour(RtcDateTime now);
void displayMinute(RtcDateTime now);
void displaySecond(RtcDateTime now);

void displayDigitAtPosition(int digit, int position);
void displayDigit(int digit);

void Seg0();
void Seg1();
void Seg2();
void Seg3();
void Seg4();
void Seg5();
void Seg6();
void Seg7();
void Seg8();
void Seg9();

#endif // DISPLAYFUNCTIONS_H
