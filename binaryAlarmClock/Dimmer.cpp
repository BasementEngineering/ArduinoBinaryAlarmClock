#include "Dimmer.h"

enum {NO_DIMMING, SENSOR_DIMMING, AUTO_TURN_OFF, SENSOR_DIMMING_AND_AUTO_TURN_OFF };

Dimmer::Dimmer(int sensorPin, int darkValue, int brightValue, int eepromAdress, LEDPanel* lightPanel):
dimmerUpdateTimer(1000, true), inputTimeoutTimer(10000, true)
{
    DARK_VALUE = darkValue;
    BRIGHT_VALUE = brightValue;
    lightSensor = sensorPin;
    EEPROM_ADRESS = eepromAdress;
    ledPanel = lightPanel;
    loadSettings();
    
    pinMode(lightSensor, INPUT);
    dimmerUpdateTimer.start();
    if (dimmerMode == 2 || dimmerMode == 3) {
    inputTimeoutTimer.start();
  }
}

void Dimmer::update(){
  
   if (dimmerUpdateTimer.isFinished() && !inputTimeoutTimer.isActive()) {
    if (dimmerMode == SENSOR_DIMMING || dimmerMode == SENSOR_DIMMING_AND_AUTO_TURN_OFF) {
      dimmClockAutomatically();
    }
  }
  if (( dimmerMode == AUTO_TURN_OFF || dimmerMode == SENSOR_DIMMING_AND_AUTO_TURN_OFF ) && inputTimeoutTimer.isFinished() ) {
    inputTimeoutTimer.stop();
    ledPanel->turnOff();  
  }
  
}

void Dimmer::changeDimmerMode() {
  dimmerMode++;
  dimmerMode %= NUMBER_OF_MODES;

  uint32_t flashColor = ledPanel->color(230,230,230);
  ledPanel->flash(flashColor, dimmerMode + 1 );
  ledPanel->setMaximumBrightness(255);
  
  if (dimmerMode == SENSOR_DIMMING || dimmerMode == SENSOR_DIMMING_AND_AUTO_TURN_OFF) {
    dimmClockAutomatically();
  }
  if (dimmerMode == AUTO_TURN_OFF || dimmerMode == SENSOR_DIMMING_AND_AUTO_TURN_OFF) {
    inputTimeoutTimer.start();
  }
  if (dimmerMode == NO_DIMMING || dimmerMode == SENSOR_DIMMING) {
    inputTimeoutTimer.stop();
  }

  saveSettings();
}

void Dimmer::wakeUp(){
  ledPanel->turnOn();
  inputTimeoutTimer.start();
}

void Dimmer::loadSettings(){
  if(EEPROM.read(EEPROM_ADRESS) == 43){
    dimmerMode = EEPROM.read(EEPROM_ADRESS + 1);
  }
  else{
    dimmerMode = 0;
}  
}

void Dimmer::saveSettings(){
  EEPROM.update(EEPROM_ADRESS, 43);
  EEPROM.update(EEPROM_ADRESS, dimmerMode);
}

void  Dimmer::dimmClockAutomatically() {
  int reading = analogRead(lightSensor);
  
  uint8_t brightness = 255;
  uint8_t minimumBrightness = 40;
  int sensorValueRange = BRIGHT_VALUE - DARK_VALUE;
  
  if (sensorValueRange > 0) {
    int brightnessRange = 255 - minimumBrightness;
    float factor = (float)brightnessRange / (float)sensorValueRange;

    if (reading <= DARK_VALUE) {
      brightness = minimumBrightness;
    }
    else if (reading >= BRIGHT_VALUE) {
      brightness = 255;
    }
    else {
      brightness = minimumBrightness + ( reading - DARK_VALUE ) * factor;
      brightness = brightness %= 256;
    }

  }
  ledPanel->setMaximumBrightness(brightness);
}
