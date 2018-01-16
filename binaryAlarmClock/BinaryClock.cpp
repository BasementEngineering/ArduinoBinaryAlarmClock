#include "BinaryClock.h"

const int BinaryClock::maximumValues[3] = {24, 60, 60};//{hh,mm,ss}
const int BinaryClock::numberOfBits[3] = {4, 6, 6}; //used on the strip for the values of hours, minutes and seconds
const int BinaryClock::startPositions[4] = {0, 6, 12, 5}; //{hh,mm,ss,pm}determines where on the strip the number is printed

BinaryClock::BinaryClock():
  rtc(), blinkTimer(600, true), timeoutTimer(10000, false) , alarmTimeout(120000, false) , snoozeTimer(300000, false),  //Yep, we have a looot of timers
  alarmTime(0, 0, 0, 0, 0, 0), setTime(0, 0, 0, 0, 0, 0)
{
  clearInformationBuffer();
  previousSecond = -1;
  row = 0;
  inSetMode = false;
  alarmIsActive = false;
  rowIsOn = false;
  newBufferAvailable = false;
  alarm = false;
}

void BinaryClock::begin() {
  rtc.begin();
  if (! rtc.isrunning()) {
     rtc.adjust(DateTime(0,0,0,0,0,0));
    // following line sets the RTC to the date & time this sketch was compiled
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void BinaryClock::update() {
  if (timeoutTimer.isFinished()) {
    leaveSetMode();
    timeoutTimer.stop();
  }

  if (!inSetMode) {
    showTime();
  }
  else {
    blinkRow();
  }
}

bool BinaryClock::newDataAvailable() {
  return newBufferAvailable;
}

bool BinaryClock::isRinging() {
  if (alarmIsActive) {
    return alarm;
  }
  return false;
}

bool BinaryClock::alarmActivated() {
  return alarmIsActive;
}

bool BinaryClock::isInSetMode() {
  return inSetMode;
}

bool* BinaryClock::getInformationBuffer() {
  newBufferAvailable = false;
  return informationBuffer;
}

void BinaryClock::snooze() {
  alarmTimeout.stop();
  alarm = false;
  snoozeTimer.start();
}

void BinaryClock::endAlarm() {
  alarm = false;
  alarmTimeout.stop();
  snoozeTimer.stop();
}

void BinaryClock::loadAlarmSettings() {
  DateTime now = rtc.now();

  uint8_t y = now.year();
  uint8_t m = now.month();
  uint8_t d = now.day();
  uint8_t hh = 0;
  uint8_t mm = 0;
  uint8_t ss = 0;
  alarmIsActive = false;

  if (rtc.readnvram(0) == 42) {
    hh = rtc.readnvram(1);
    mm = rtc.readnvram(2);
    alarmIsActive = rtc.readnvram(3);
  }

  DateTime timeBuffer(y, m, d, hh, mm, ss);

  alarmTime = timeBuffer;
}

void BinaryClock::activateAlarm() {
  alarmIsActive = true;
  saveAlarmSettings();
}

void BinaryClock::deactivateAlarm() {
  snoozeTimer.stop();
  alarmIsActive = false;
  saveAlarmSettings();
}

void BinaryClock::enterTimeSettingMode() {
  endAlarm();
  setTime = rtc.now();
  blinkTimer.start();
  timeoutTimer.start();
  inSetMode = 1;
  showSettings();
}

void BinaryClock::enterAlarmSettingMode() {
  endAlarm();
  setTime = alarmTime;
  timeoutTimer.start();
  blinkTimer.start();
  inSetMode = 2;
  showSettings();
}

void BinaryClock::leaveSetMode() {
  blinkTimer.stop();
  timeoutTimer.stop();
  inSetMode = false;
  row = 0;
}

void BinaryClock::up() {
  if (inSetMode) {
    setValue(1);
    timeoutTimer.reset();
  }
}

void BinaryClock::down() {
  if (inSetMode) {
    setValue(-1);
    timeoutTimer.reset();
  }
}

void BinaryClock::ok() {

  if (inSetMode) {

    switch (row) {
      case 0: row++;
        break;
      case 1: row++;
        if (inSetMode == 2) {
          alarmTime = setTime;
          saveAlarmSettings();
          leaveSetMode();
        }
        break;
        case 2: if(inSetMode == 1) {
          saveNewTime();
          leaveSetMode();
        }
        break;

      default: break;
    }
    timeoutTimer.reset();
  }
}

void BinaryClock::showSettings() {
  uint8_t values[3] = { setTime.hour(), setTime.minute(), setTime.second()};

  values[0] = amPmFormat(values[0]);
  
  if (!rowIsOn) {
    values[row] = 0;
  }

  for (int i = 0; i < 3; i++) {
    if (!rowIsOn && row == i) {
      values[i] = 0;
    }
    addBinaryNumber(numberOfBits[i], startPositions[i], values[i]);
  }

  newBufferAvailable = true;

}

void BinaryClock::showTime(bool refresh) {

  DateTime now = rtc.now();

  if (previousSecond != now.second()) {
    previousSecond = now.second();
    checkAlarm(now);
    newBufferAvailable = true;

#ifdef DEBUG1
    Serial.print("It is ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.println(now.second());
#endif

    int _hour = now.hour();
    _hour = amPmFormat(_hour);

    int values[3] = {_hour, now.minute(), now.second()};

    for (int i = 0; i < 3; i++) {
      addBinaryNumber(numberOfBits[i], startPositions[i], values[i]);
    }
  }
}

void BinaryClock::checkAlarm(DateTime now) {

  if (snoozeTimer.isFinished() ||
      (now.hour() == alarmTime.hour() &&
       now.minute() == alarmTime.minute() &&
       now.second() == alarmTime.second() ) )
  {
    alarm = true;
    alarmTimeout.start();
  }

  if (alarmTimeout.isFinished()) {
    endAlarm();
  }

}

void BinaryClock::saveNewTime() {
  rtc.adjust(setTime);
}

void BinaryClock::saveAlarmSettings() {
  uint8_t buffer[4] = {42, alarmTime.hour(), alarmTime.minute(), alarmIsActive};
  rtc.writenvram(0, buffer, 4);
}

void BinaryClock::clearInformationBuffer() {
  for (int i = 0; i < 18; i++) {
    informationBuffer[i] = 0;
  }
}

void BinaryClock::blinkRow() {

  if (blinkTimer.isFinished()) {
    rowIsOn = !rowIsOn;
    showSettings();
  }
}

void BinaryClock::setValue(int input) {

int value = 0;
int maxValue = maximumValues[row];

switch (row){
    case 0: value = setTime.hour();
    break;
    case 1: value = setTime.minute();
    break;
    case 2: value = setTime.second();
    break;
    default: break;
  }
  
  value += input;
  if (value < 0) {
    value = maxValue + value;
  }
  value = value % maxValue;

  switch (row){
    Serial.print("value has been set to: ");
    Serial.println(value);
    case 0: setTime.setHour(value);
    Serial.print("Hour has been set to: ");
    Serial.println(setTime.hour());
    break;
    case 1: setTime.setMinute(value);
    Serial.print("Minute has been set to: ");
    Serial.println(setTime.minute());
    break;
    case 2: setTime.setSecond(value);
    Serial.print("Second has been set to: ");
    Serial.println(setTime.second());
    break;
    default: break;
  }
  showSettings();
}

int BinaryClock::amPmFormat(int hour) {
  if (hour > 12) {
    hour %= 12;
    informationBuffer[startPositions[3]] = 1;
  }
  else {
    informationBuffer[startPositions[3]] = 0;
  }
  return hour;
}

void BinaryClock::addBinaryNumber(int bits, int startPosition, int value) {

#ifdef DEBUG
  Serial.print("Converting the decimal number");
  Serial.print(value);
  Serial.print(" to binary ");
#endif

  bool binaryBuffer[bits];

  int stripPosition = startPosition;
  int bufferPosition = bits - 1;

  for (int i = 0; i < bits; i++) {
    binaryBuffer[i] = value % 2;
    value /= 2;
  }

  while (bufferPosition >= 0) {

#ifdef DEBUG
    Serial.print(binaryBuffer[bufferPosition]);
#endif

    informationBuffer[stripPosition] = binaryBuffer[bufferPosition];
    stripPosition++;
    bufferPosition--;
  }

#ifdef DEBUG
  Serial.println();
#endif

}
