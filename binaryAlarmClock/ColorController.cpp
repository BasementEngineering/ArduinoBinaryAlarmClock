#include "ColorController.h"

enum {SINGLE_COLOR_MODE, SINGLE_COLOR_FADING, TRI_COLOR_MODE, TRI_COLOR_FADING, INDIVIDUAL_DOT_COLOR_MODE, INDIVIDUAL_DOT_COLOR_FADING, TEMPERATURE_MODE};

ColorController::ColorController(int tempSensorPin, int eepromStartAdress, LEDPanel* panelObject):
oneWire(tempSensorPin), temperatureSensor(&oneWire),
  updateTimer(20, true)
{
  EEPROM_ADRESS = eepromStartAdress;
  ledPanel = panelObject;
  loadSettings();
  effectCounter = 0;
  updateTimer.start();
  onPause = false;
  updateTimer.setInterval(105 - fadingSpeed * 10);
}

void ColorController::begin(){
    temperatureSensor.begin();
}

void ColorController::update(bool forceUpdate){
  if ( updateTimer.isFinished() || forceUpdate ) {
    if (!onPause) {
      switch (effectNumber) {
        case SINGLE_COLOR_MODE: setWheelColor(colorWheelPosition1);
          break;
        case SINGLE_COLOR_FADING: fadeColor();
          break;
        case TRI_COLOR_MODE: setThreeColors(colorWheelPosition2);
          break;
        case TRI_COLOR_FADING: fadeThreeColors();
          break;
          case INDIVIDUAL_DOT_COLOR_MODE: setIndividualDotColors(colorWheelPosition3);
          break;
          case INDIVIDUAL_DOT_COLOR_FADING: fadeIndividualDotColors();
          break;
        case TEMPERATURE_MODE: convertTemperatureToColor();
          break;
        default: break;
      }
    }
    ledPanel->pushToStrip();
  }

}

void ColorController::pauseEffect() {
  onPause = true;
}

void ColorController::unpauseEffect() {
  if (onPause) {
    onPause = false;
    update(true);
  }
}

void ColorController::nextColorEffect() {
  effectNumber++;
  effectNumber %= numberOfEffects;
  update(true);
  saveSettings();
}

void ColorController::previousColorEffect() {
  effectNumber--;
  if (effectNumber < 0) {
    effectNumber = numberOfEffects;
  }
  update(true);
  saveSettings();
}

void ColorController::up() {
  if (effectNumber == SINGLE_COLOR_FADING || effectNumber == TRI_COLOR_FADING) {
    fadingSpeed++;
    if ( fadingSpeed > 10) {
      fadingSpeed = 10;
    }
    updateTimer.setInterval(105 - fadingSpeed * 10);
  }
  else if (effectNumber == SINGLE_COLOR_MODE) {
    colorWheelPosition1++;
    colorWheelPosition1 %= 256;
    update(true);
  }
  else if (effectNumber == TRI_COLOR_MODE) {
    colorWheelPosition2++;
    colorWheelPosition2 %= 256;
    update(true);
  }
  else if (effectNumber == INDIVIDUAL_DOT_COLOR_MODE) {
    colorWheelPosition3++;
    colorWheelPosition3 %= 256;
    update(true);
  }
  saveSettings();
}

void ColorController::down() {
  if (effectNumber == SINGLE_COLOR_FADING || effectNumber == TRI_COLOR_FADING) {
    fadingSpeed--;
    if ( fadingSpeed < 1) {
      fadingSpeed = 1;
    }
    updateTimer.setInterval(105 - fadingSpeed * 10);
  }
  else if (effectNumber == SINGLE_COLOR_MODE) {
    colorWheelPosition1--;
    if (colorWheelPosition1 < 0) {
      colorWheelPosition1 = 255;
    }
    update(true);
  }
  else if (effectNumber == TRI_COLOR_MODE) {
    colorWheelPosition2--;
    if (colorWheelPosition2 < 0) {
      colorWheelPosition2 = 255;
    }
    update(true);
  }
  saveSettings();
}

void ColorController::loadSettings() {

  if (EEPROM.read(EEPROM_ADRESS) == 42) {
    effectNumber = EEPROM.read(EEPROM_ADRESS + 1);
    fadingSpeed = EEPROM.read(EEPROM_ADRESS + 2);
    colorWheelPosition1 = EEPROM.read(EEPROM_ADRESS + 3);
    colorWheelPosition2 = EEPROM.read(EEPROM_ADRESS + 4);
    colorWheelPosition3 = EEPROM.read(EEPROM_ADRESS + 5);
  }
  else {
    effectNumber = 0;
    fadingSpeed = 5;
    colorWheelPosition1 = 0;
    colorWheelPosition2 = 0;
     colorWheelPosition3 = 0;
  }
}

void ColorController::saveSettings() {
  EEPROM.update(EEPROM_ADRESS, 42);
  EEPROM.update(EEPROM_ADRESS + 1, effectNumber);
  EEPROM.update(EEPROM_ADRESS + 2, fadingSpeed);
  EEPROM.update(EEPROM_ADRESS + 3, colorWheelPosition1);
  EEPROM.update(EEPROM_ADRESS + 4, colorWheelPosition2);
  EEPROM.update(EEPROM_ADRESS + 5, colorWheelPosition3);
}

void ColorController::setWheelColor(uint8_t wheelPosition) {
  uint32_t colorBuffer[18];
  for (int i = 0; i < 18; i++) {
    colorBuffer[i] = colorWheel(wheelPosition);
  }
  ledPanel->setColorBuffer(colorBuffer);
}

void ColorController::fadeColor() {
  effectCounter++;
  effectCounter %= 256;
  setWheelColor( effectCounter );
}

void ColorController::setThreeColors(uint8_t wheelPosition, uint8_t distance) {
  uint32_t colorBuffer[18];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 6; j++) {
      colorBuffer[j + (6 * i)] = colorWheel(wheelPosition + i * distance);
    }
  }
  ledPanel->setColorBuffer(colorBuffer);
}

void ColorController::fadeThreeColors(uint8_t distance) {
  effectCounter++;
  effectCounter %= 256;
  setThreeColors(effectCounter, 20);
}

void  ColorController::setIndividualDotColors(uint8_t wheelPosition, uint8_t distance) {
  uint32_t colorBuffer[18];
  for (int i = 0; i < 18; i++) {
    int position = (wheelPosition + i * distance) % 255;
    colorBuffer[i] = colorWheel(position);
  }
  ledPanel->setColorBuffer(colorBuffer);
}

void  ColorController::fadeIndividualDotColors(uint8_t distance) {
  effectCounter++;
  effectCounter %= 256;
  setIndividualDotColors(effectCounter, distance);
}

void ColorController::convertTemperatureToColor() {
  temperatureSensor.requestTemperatures();
  int temperature = temperatureSensor.getTempCByIndex(0);
  uint8_t wheelPosition;
  if (temperature <= 0) {
    wheelPosition = 170;
  }
  else if (temperature >= 35) {
    wheelPosition = 0;
  }
  else {
    wheelPosition = 170 - temperature * (170 / 35);
  }
#ifdef DEBUG
  Serial.println("Converting Temperature into a Color"); Serial.println(temperature);
  Serial.println("Temperature is: "); Serial.println(temperature);
  Serial.print("Wheel Position is: "); Serial.println(wheelPosition);
#endif

  setWheelColor(wheelPosition);
}

uint32_t ColorController::colorWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return ledPanel->color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return ledPanel->color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return ledPanel->color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
