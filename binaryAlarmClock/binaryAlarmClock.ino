//Binary Neopixel Clock//
/*
  This is the Code for my Arduino based Binary Alarm Clock.
  The clock itself has the following features:
  - 18 WS2812b adressable RGB LED's
  - 4 Control Buttons(UP,OK,DOWN,ALARM);
  - light Sensor(LDR)
  - Arduino Nano
  - DS1307 RTC Module
  - DS18B20 digital Temperature Sensor
  - laptop speaker

  You can find further Documentation here
  Youtube: https://www.youtube.com/watch?v=gJVIzod6dyY&t=13s
  Instructables: https://www.instructables.com/id/DIY-Binary-Alarm-Clock/

  Created by: Jan Kettler
  Last Updated: 16.01.2018
*/

//Uncomment to get Serial Debugging Information
//#define DEBUG

//Pin Definitions
const int PIXEL_PIN = 2;
const int BUZZER_PIN = 3;

const int UP_BUTTON_PIN = 4;
const int OK_BUTTON_PIN = 5;
const int DOWN_BUTTON_PIN = 6;
const int ALARM_BUTTON_PIN = 7;

const int LIGHT_SENSOR_PIN = A0;
const int TEMPERATURE_SENSOR_PIN = A1;

//Light Sensor Values, 
//measure those for your Sensor with the LightSensorCalibration Sketch in the Code package
const int DARK_VALUE = 290;
const int BRIGHT_VALUE = 870;

//EEPROM Adresses 
const int COLOR_CONTROLLER_EEPROM_ADRESS = 20;
const int DIMMER_EEPROM_ADRESS = 30;

//libraries
#include <Button.h>
#include <Timer.h>
#include <Jukebox.h>
#include "BinaryClock.h"
#include "Dimmer.h"
#include "ColorController.h"
#include "LEDPanel.h"

//Objects
BinaryClock binaryClock;

LEDPanel ledPanel(PIXEL_PIN);
Dimmer dimmer(LIGHT_SENSOR_PIN, DARK_VALUE, BRIGHT_VALUE, DIMMER_EEPROM_ADRESS, &ledPanel);
ColorController colorController(TEMPERATURE_SENSOR_PIN, COLOR_CONTROLLER_EEPROM_ADRESS, &ledPanel);
Jukebox jukebox(BUZZER_PIN);

MultiTapButton upButton(UP_BUTTON_PIN);
MultiTapButton downButton(DOWN_BUTTON_PIN);
MultiTapButton okButton(OK_BUTTON_PIN);
MultiTapButton alarmButton(ALARM_BUTTON_PIN);

//constants
const uint32_t RED = ledPanel.color(255, 0, 0);
const uint32_t GREEN = ledPanel.color(0, 255, 0);
const uint32_t BLUEISH = ledPanel.color(0, 100, 255);
const uint32_t GREENISH = ledPanel.color(0, 255, 75);

enum {NOTHING, OK, OK_LONG, OK_DOUBLE, UP, UP_HELD_DOWN, UP_DOUBLE, DOWN, DOWN_HELD_DOWN, DOWN_DOUBLE, ALARM, ALARM_LONG, ALARM_DOUBLE};

//Function Prototypes
void updateDevices();
void setClock(int input);
int getInput();
void adjustBrightness();

void setup() {
  #ifdef DEBUG
  Serial.begin(9600);
  Serial.println("Serial Debugging enabled");
  #endif
  ledPanel.begin();
  colorController.begin();
  binaryClock.begin();
  binaryClock.loadAlarmSettings();
}

void loop() {
  updateDevices();

  int input = getInput();

#ifdef DEBUG
  if(Serial.available()){
    String emptyBuffer = Serial.readString();
    input = emptyBuffer.toInt();
  }
#endif

  if (input != NOTHING) {
    #ifdef DEBUG
  Serial.print("Input Detected: ");
  Serial.println(input);
  #endif
  dimmer.wakeUp();
  if(ledPanel.isOn() ){
    setClock(input);
  }
  }

  if (binaryClock.newDataAvailable()) {
    ledPanel.setInformationBuffer(binaryClock.getInformationBuffer());
  }

    if (binaryClock.isRinging() && !jukebox.isPlaying()) {
    jukebox.restart();
    jukebox.play();
  }
  
  if (!binaryClock.isRinging() && jukebox.isPlaying()) {
    jukebox.pause();
  }

if (!binaryClock.isInSetMode()) {
  colorController.unpauseEffect();
}

}

void updateDevices() { 
  okButton.update();
  alarmButton.update();
  upButton.update();
  downButton.update();
  binaryClock.update();
  colorController.update();
  dimmer.update();
  jukebox.update();
}

int getInput() {  
int numberOfInputs = 0;
int input = NOTHING;
  
  if (okButton.wasTapped()) {
    input = OK;
    numberOfInputs++;
  }
  else if (okButton.wasLongPressed()) {
    input = OK_LONG;
    numberOfInputs++;
  }
  else if (okButton.wasDoubleTapped()) {
    input = OK_DOUBLE;
    numberOfInputs++;
  }

  if (alarmButton.wasTapped()) {
    input = ALARM;
    numberOfInputs++;
  }
  else if (alarmButton.wasLongPressed()) {
    input = ALARM_LONG;
    numberOfInputs++;
  }
  else if (alarmButton.wasDoubleTapped()) {
    input = ALARM_DOUBLE;
    numberOfInputs++;
  }

  if (upButton.wasTapped()) {
    input = UP;
    numberOfInputs++;
  }
  else if (upButton.isHeldDown()) {
    input = UP_HELD_DOWN;
    numberOfInputs++;
  }
  else if (upButton.wasDoubleTapped()) {
    input = UP_DOUBLE;
    numberOfInputs++;
  }
  
  if (downButton.wasTapped()) {
    input = DOWN;
    numberOfInputs++;
  }
  else if (downButton.isHeldDown()) {
    input = DOWN_HELD_DOWN;
    numberOfInputs++;
  }
  else if (downButton.wasDoubleTapped()) {
    input = DOWN_DOUBLE;
    numberOfInputs++;
  }

  if(numberOfInputs > 1){
  input = NOTHING;
  }
  
  return input;
}

void setClock(int input) {
  
  if (!binaryClock.isInSetMode() ) {

    switch (input) {
      case UP: colorController.up();
        break;
     case UP_DOUBLE: colorController.nextColorEffect();
        break;
      case UP_HELD_DOWN: colorController.up();
        break;
        case DOWN: colorController.down();
        break;
     case DOWN_DOUBLE: colorController.previousColorEffect();
        break;
      case DOWN_HELD_DOWN: colorController.down();
        break;
        case OK_DOUBLE: dimmer.changeDimmerMode();
        break;
      case OK_LONG: colorController.pauseEffect();
      ledPanel.setColor(BLUEISH);
      binaryClock.enterTimeSettingMode();
        break;
      case ALARM_LONG: if (binaryClock.isRinging()) {
          binaryClock.endAlarm();
        }
        else { 
          colorController.pauseEffect();
          ledPanel.setColor(GREENISH);
          binaryClock.enterAlarmSettingMode();
        }
        break;
      case ALARM_DOUBLE: if (binaryClock.alarmActivated()) {
          binaryClock.deactivateAlarm();
          ledPanel.flash(RED);
        }
        else {
          binaryClock.activateAlarm();
          ledPanel.flash(GREEN);
        }
        break;
      case ALARM: binaryClock.snooze();
        break;
      default: break;
    }
  }
  else{
    switch (input) {
      case UP: binaryClock.up();
        break;
      case DOWN: binaryClock.down();
        break;
      case OK: binaryClock.ok();
        break;
      default: break;
    }
  }
}

