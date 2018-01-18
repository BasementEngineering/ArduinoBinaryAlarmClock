# ArduinoBinaryAlarmClock

Those two sketches are needed to make the Arduino based Binary Alarm Clock run.
First you run the lightSensorCalibration sketch to get the proper bright and dark values for the automatic dimmer.
Then you add the measured values to the main sketch and upload it to the Arduino.

In order for the main sketch to work properly, you will need a couple of libraries.
Here is a little list:

1. Button                       https://github.com/BasementEngineering/Button
2. Timer                        https://github.com/BasementEngineering/Timer
3. Jukebox                      https://github.com/BasementEngineering/Jukebox
4. modified RTClib              https://github.com/BasementEngineering/RTClib-modified-
5. Adafruit Neopixel            https://github.com/adafruit/Adafruit_NeoPixel
6. Arduino-temperature-control  https://github.com/milesburton/Arduino-Temperature-Control-Library
