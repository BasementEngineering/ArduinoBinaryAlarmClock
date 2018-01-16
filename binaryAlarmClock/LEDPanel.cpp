#include "LEDPanel.h"

LEDPanel::LEDPanel(int ledPin):
  strip(18, ledPin, NEO_GRB + NEO_KHZ800)
{
  clearInformationBuffer(true);
  clearColorBuffer();
  clearBrightnessBuffer(true);
  on = true;
}

void LEDPanel::begin() {
#ifdef DEBUG
  Serial.println("Starting LED Panel");
#endif
  strip.begin();
  strip.show();
}

void LEDPanel::setColorBuffer(uint32_t newColorBuffer[18]){
   for (int i = 0; i < 18; i++) {
    colorBuffer[i] = newColorBuffer[i];
   }
   pushToStrip();
}

void LEDPanel::clearColorBuffer() {
  for (int i = 0; i < 18; i++) {
    colorBuffer[i] = 0;
  }
}

void LEDPanel::setInformationBuffer(bool newInformationBuffer[18]) {
  for (int i = 0; i < 18; i++) {
    informationBuffer[i] = newInformationBuffer[i];
  }
  pushToStrip();
}

void LEDPanel::clearInformationBuffer(bool allOn) {
  for (int i = 0; i < 18; i++) {
    informationBuffer[i] = allOn;
  }
  pushToStrip();
}


void LEDPanel:: clearBrightnessBuffer(bool allOn) {
  for (int i = 0; i < 18; i++) {
    if (allOn) {
      brightnessBuffer[i] = 255;
    }
    else {
      brightnessBuffer[i] = 0;
    }
  }
}

void LEDPanel::setBrightness(uint8_t newBrightness) {
  for (int i = 0; i < 18; i++) {
    brightnessBuffer[i] = newBrightness;
  }
}

void LEDPanel::setMaximumBrightness(uint8_t newBrightness) {
  strip.setBrightness(newBrightness);
}

void LEDPanel::setColor(uint32_t color) {
  for (int i = 0; i < 18; i++) {
    colorBuffer[i] = color;
  }
}

void LEDPanel::turnOff(){
  on = false;
  strip.clear();
  strip.show();
}

void LEDPanel::turnOn(){
  on = true;
}

bool LEDPanel::isOn() {
  return on;
}

void LEDPanel::flash(uint32_t color, int numberOfFlashes) {

uint32_t colorBufferBackup[18];
for(int i = 0; i < 18; i++){
  colorBufferBackup[i] = colorBuffer[i];
}

  for (int i = 0; i < numberOfFlashes; i++) {
    setColor(0);
    pushToStrip();
    delay(200);
    setColor(color);
    pushToStrip();
    delay(200);
  }
  setColorBuffer(colorBufferBackup);
  pushToStrip();
}

void LEDPanel::pushToStrip() {

  if (on) {

    for (int bufferPosition = 0; bufferPosition < 18; bufferPosition++) {

      uint32_t color = colorBuffer[bufferPosition];
      color = dimmColor(color, brightnessBuffer[bufferPosition]);

      if (!informationBuffer[bufferPosition]) {
        color = 0;
      }
      strip.setPixelColor(bufferPosition, color);
    }

    strip.show();
  }
}

uint32_t LEDPanel::color(uint8_t r, uint8_t g, uint8_t b) {
  return strip.Color(r, g, b);
}

uint32_t LEDPanel::dimmColor(uint32_t color, uint8_t brightness) {

  if (brightness == 0)
  {
    return 0;
  }
  else if (brightness == 255)
  {
    return color;
  }
  else {

#ifdef DEBUG3
    Serial.println("Dimming Colour Value");
#endif

    uint8_t r, g, b;

    r = (uint8_t)(color >> 16),
    g = (uint8_t)(color >>  8),
    b = (uint8_t)color;

#ifdef DEBUG3
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
#endif

    r = ((r * brightness) >> 8) + 1;
    g = ((g * brightness) >> 8) + 1;
    b = ((b * brightness) >> 8) + 1;

#ifdef DEBUG3
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
#endif

    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b ;
  }
}


