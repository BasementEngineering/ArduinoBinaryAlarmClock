/*An object of the BinaryClock class does all of the talking, when it comes to communicating with the DS1307 RTC Module.
 *Besides that, it offers a couple of time and alarm related functions to the user.
 *When it is finished doing all of it's calculations, it outputs a so called informationBuffer and sets the newBufferAvailable flag.
 *This informationBuffer is simply a boolean buffer, including 18 values, indicating which LED's should be on or off. 
 *It is later passed on to an LEDPanel object.
 */

#ifndef BINARYCLOCK_H
#define BINARYCLOCK_H

#include <RTClib.h>
#include "Timer.h"
#include <Wire.h>

class BinaryClock{
  
public:

BinaryClock();
void begin();
void update();

bool newDataAvailable();
bool isRinging();
bool alarmActivated();
bool isInSetMode();
bool* getInformationBuffer();

void snooze();
void endAlarm();
void loadAlarmSettings();
void activateAlarm();
void deactivateAlarm();
void enterTimeSettingMode();
void enterAlarmSettingMode();

void leaveSetMode();
void up();
void down();
void ok();

private:
Timer blinkTimer;
Timer timeoutTimer;
Timer alarmTimeout;
Timer snoozeTimer;
RTC_DS1307 rtc;
DateTime alarmTime;
DateTime setTime;

int previousSecond;
int row;
bool informationBuffer[18];

int inSetMode; 
bool alarmIsActive;
bool rowIsOn;
bool newBufferAvailable;
bool alarm;

static const int maximumValues[3];
static const int numberOfBits[3]; //used on the strip for the values of hours, minutes and seconds
static const int startPositions[4]; //where on the strip the number is printed

void showSettings();
void showTime(bool refresh = false);
void checkAlarm(DateTime now);
void saveNewTime();
void saveAlarmSettings();
void clearInformationBuffer();
void blinkRow();
void setValue(int input);
int amPmFormat(int hour);
void addBinaryNumber(int bits, int startPosition, int value);
};

#endif
