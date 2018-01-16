/*The ColorController class encapsulates every color effect that the clock can show.
 * It has a slim interface of only a couple of public functions.
 * Those allow the user to cycle through the effects and set things like the speed or the color.
 * All of the settings are also saved in the Arduinos EEProm.
 */

#ifndef COLORCONTROLLER_H
#define COLORCONTROLLER_H

#include <DallasTemperature.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <Timer.h>
#include "Dimmer.h"

class ColorController{
 public:
    ColorController(int tempSensorPin, int eepromStartAdress, LEDPanel* panelObject);   
    void begin();
    void update(bool forceUpdate = false);
    void pauseEffect();
    void unpauseEffect();
    void nextColorEffect();
    void previousColorEffect(); 
    void up(); //static effect: changes the color , dynamic effect: increases the speed
    void down(); //static effect: changes the color , dynamic effect: decreases the speed

  private:
    OneWire oneWire;
    DallasTemperature temperatureSensor;
    LEDPanel* ledPanel;
    Timer updateTimer;
    
    bool onPause;
    int effectNumber;
    int fadingSpeed;
    uint8_t colorWheelPosition1;
    uint8_t colorWheelPosition2;
    uint8_t colorWheelPosition3;
    int effectCounter;
    int EEPROM_ADRESS;
    const int numberOfEffects = 7;
    
    void loadSettings();
    void saveSettings();   
    void setWheelColor(uint8_t wheelPosition);
    void fadeColor();
    void setThreeColors(uint8_t wheelPosition, uint8_t distance = 20);
    void fadeThreeColors( uint8_t distance = 20);
    void fadeIndividualDotColors(uint8_t distance = 5);
    void setIndividualDotColors(uint8_t wheelPosition, uint8_t distance = 5);
    void convertTemperatureToColor();
    uint32_t colorWheel(byte WheelPos); //taken from the Adafruit example

};


#endif
