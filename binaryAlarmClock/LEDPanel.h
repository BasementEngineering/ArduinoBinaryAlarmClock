/*An LEDPanel object is basicaly a collection of Buffers for the brightness value,
*the color value and the binary state of each LED on the strip. 
*Whenever pushToStrip() is called, those three Buffers are overlayed and sent to the WS2812b strip.
*/
 
#ifndef LEDPANEL_H
#define LEDPANEL_H

#include <Adafruit_NeoPixel.h>

#define DEBUG

class LEDPanel {
  public:
    void wakeUp();
    LEDPanel(int ledPin);
    void begin();

    void setColorBuffer(uint32_t newColorBuffer[18]);
    void clearColorBuffer();
    void setInformationBuffer(bool newInformationBuffer[18]);
    void clearInformationBuffer(bool allOn = false);
    void clearBrightnessBuffer(bool allOn = true);
    void setBrightness(uint8_t newBrightness);
    void setMaximumBrightness(uint8_t newBrightness);
    void setColor(uint32_t color);

    void turnOff();
    void turnOn();
    bool isOn();

    void flash(uint32_t color, int numberOfFlashes = 2);
    void pushToStrip();

    uint32_t color(uint8_t r, uint8_t g, uint8_t b);

  private:
    bool on;
    Adafruit_NeoPixel strip;
    bool informationBuffer[18];
    uint32_t colorBuffer[18]; //holds the color of each LED
    uint8_t brightnessBuffer[18]; //holds individual brightness of each LED
    uint32_t dimmColor(uint32_t color, uint8_t brightness); //inspire by functions inside the Adafruit Neopixel library

};

#endif
