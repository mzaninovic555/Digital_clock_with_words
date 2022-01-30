#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
 
#define DATA 9                    // digital 2 to pin 14 on the 74HC595
#define LATCH 10                   // digital 3 to pin 12 on the 74HC595
#define CLOCK 8                   // digital 4 to pin 11 on the 74HC595
 
 
byte registerByte1;           // 0 IT IS (R) | 1 HALF(R) | 2 TEN(R)  | 3 QUARTER (R) | 4 TWENTY (R) | 5 FIVE (R) | 6 MINUTES(R) | 7 TO (R)
byte registerByte2;           // 0 PAST (R)  | 1 ONE | 2 THREE | 3 TWO | 4 FOUR | 5 FIVE | 6 SIX | 7 SEVEN  
byte registerByte3;           // 0 EIGHT | 1 NINE | 2 TEN | 3 ELEVEN | 4 TWELVE | 5 O'CLOCK
 
tmElements_t tm;
 
void setup()
{
  Serial.begin(9600);
  while (!Serial) ; 
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
 
  
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
}
 
void loop()
{
  timeToBytes();
  updateShiftRegisters();
  printTime();
}
 
void timeToBytes()
{
  if (RTC.read(tm)) {
    int englishHour = shortHour(tm.Hour);
    int half = 0;
    registerByte1 = 0;
    registerByte2 = 0;
    registerByte3 = 0;
    bitSet(registerByte1,0);    // "IT IS"
    
      if (tm.Minute >= 5 && tm.Minute <= 10) {
      bitSet(registerByte1,5);    // "FIVE"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte2,0);    // "PAST"
    } else if (tm.Minute >= 10 && tm.Minute <= 14) {
      bitSet(registerByte1,2);    // "TEN"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte2,0);    // "PAST"
    } else if (tm.Minute >= 15 && tm.Minute <= 19) {
      bitSet(registerByte1,3);    // "QUARTER"
      bitSet(registerByte2,0);    // "PAST"
    } else if (tm.Minute >= 20 && tm.Minute <= 24) {
      bitSet(registerByte1,4);    // "TWENTY"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte2,0);    // "PAST"
    } else if (tm.Minute >= 25 && tm.Minute <= 29) {
      bitSet(registerByte1,4);    // "TWENTY"
      bitSet(registerByte1,5);    // "FIVE"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte2,0);    // "PAST"
    } else if (tm.Minute >= 30 && tm.Minute <= 34) {
      bitSet(registerByte1,1);    // "HALF"
      bitSet(registerByte2,0);    // "PAST"
    } else if (tm.Minute >= 35 && tm.Minute <= 39) {
      bitSet(registerByte1,4);    // "TWENTY"
      bitSet(registerByte1,5);    // "FIVE"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte1,7);    // "TO"
      half = 1;
    } else if (tm.Minute >= 40 && tm.Minute <= 44)  {
      bitSet(registerByte1,4);    // "TWENTY"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte1,7);    // "TO"
      half = 1;
    } else if (tm.Minute >= 45 && tm.Minute <= 49) {
      bitSet(registerByte1,3);    // "QUARTER"
      bitSet(registerByte1,7);    // "TO"
      half = 1;
    }else if (tm.Minute >= 50 && tm.Minute <= 54) {
      bitSet(registerByte1,2);    // "TEN"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte1,7);    // "TO"
      half = 1; 
    }else if (tm.Minute >= 55 && tm.Minute <= 59) {
      bitSet(registerByte1,5);    // "FIVE"
      bitSet(registerByte1,6);    // "MINUTES"
      bitSet(registerByte1,7);    // "TO"
      half = 1;
    } 
 
    englishHour = shortHour( englishHour + half);
    if ( englishHour == 1 ) {
      bitSet(registerByte2,1);    // "ONE"
    } else if ( englishHour == 2 ) {
      bitSet(registerByte2,3);    // "TWO"
    } else if ( englishHour == 3 ) {
      bitSet(registerByte2,2);    // "THREE"
    } else if ( englishHour == 4 ) {
      bitSet(registerByte2,4);    // "FOUR"
    } else if ( englishHour == 5 ) {
      bitSet(registerByte2,5);    // "FIVE"
    } else if ( englishHour == 6 ) {
      bitSet(registerByte2,6);    // "SIX"
    } else if ( englishHour == 7 ) {
      bitSet(registerByte2,7);    // "SEVEN"
    } else if ( englishHour == 8 ) {
      bitSet(registerByte3,0);    // "EIGHT"
    } else if ( englishHour == 9 ) {
      bitSet(registerByte3,1);    // "NINE"
    } else if ( englishHour == 10 ) {
      bitSet(registerByte3,2);    // "TEN"
    } else if ( englishHour == 11 ) {
      bitSet(registerByte3,3);    // "ELEVEN"
    } else if ( englishHour == 12 ) {
      bitSet(registerByte3,4);    // "TWELVE"
    }
    bitSet(registerByte3,5);    // "O'CLOCK"
  }
}
 
void updateShiftRegisters()
{
   digitalWrite(LATCH, LOW);
   shiftOut(DATA, CLOCK, MSBFIRST, registerByte3);
   shiftOut(DATA, CLOCK, MSBFIRST, registerByte2);
   shiftOut(DATA, CLOCK, MSBFIRST, registerByte1);
   digitalWrite(LATCH, HIGH);
}
 
void addTime(int addHour, int addMinute)
{
  if (RTC.read(tm)) {
 
    if (addHour > 0) {
      tm.Hour = tm.Hour + addHour;
      if (tm.Hour > 12) {
        tm.Hour = tm.Hour - 12;
      }
    }
    
    if (addMinute > 0) {
      tm.Minute = tm.Minute + addMinute;
      if (tm.Minute > 59) {
        tm.Minute = tm.Minute - 60;
        addTime(1,0);
      }
    }  
 
    tm.Second = 0;
    RTC.write(tm); 
  }
}
 
int shortHour(int tempHour)
{
  tempHour = tempHour % 12;
  return tempHour == 0 ? 12 : tempHour;
}
 
 
void printTime() {
 
  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
    Serial.print("registerByte3 = ");
    Serial.print(registerByte3, BIN);
    Serial.println();
    Serial.print("registerByte2 = ");
    Serial.print(registerByte2, BIN);
    Serial.println();
    Serial.print("registerByte1 = ");
    Serial.print(registerByte1, BIN);
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}
 
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
