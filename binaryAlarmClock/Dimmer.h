/* An obect of the Dimmer class manages the brightness of the LED-Panel.
 *  It has different dimming modes, that the user can cycle through.
 *  The current mode is stored in the Arduinos EEProm
 */

#ifndef DIMMER_H
#define DIMMER_H

#include <Arduino.h>
#include <EEPROM.h>
#include <Timer.h>
#include "LEDPanel.h"

class Dimmer{
public:
Dimmer(int sensorPin, int darkValue, int brightValue, int eepromAdress, LEDPanel* lightPanel);
void update();
void changeDimmerMode();
void wakeUp();
  
private:
LEDPanel* ledPanel;
Timer dimmerUpdateTimer;
Timer inputTimeoutTimer;

void loadSettings();
void saveSettings();

void dimmClockAutomatically();

int lightSensor;
int DARK_VALUE;
int BRIGHT_VALUE;
int EEPROM_ADRESS;

int dimmerMode; 
const int NUMBER_OF_MODES = 4;
};

#endif
