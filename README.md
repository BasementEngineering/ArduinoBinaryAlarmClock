# ArduinoBinaryAlarmClock
The Arduino based binyary alarm clock is a great learning project for those who are getting started with MCU programming.
You can find the full documentation here: 

->[Instructables](https://www.instructables.com/member/Basement%20Engineering/?cb=1522923558)

->[Youtube](https://www.youtube.com/watch?v=gJVIzod6dyY)

Those two sketches are needed to make the clock run.
First you fash and run the lightSensorCalibration sketch to get the proper bright and dark values for the automatic dimmer.
Then you add the measured values to the main sketch, upload it to the Arduino and you are good to go.

In order for the main sketch to work properly, you will need a couple of libraries.
Here is a little list:

1. [Button](https://github.com/BasementEngineering/Button)
2. [Timer](https://github.com/BasementEngineering/Timer)
3. [Jukebox](https://github.com/BasementEngineering/Jukebox)
4. [modified RTClib](https://github.com/BasementEngineering/RTClib-modified-)
5. [Adafruit Neopixel](https://github.com/adafruit/Adafruit_NeoPixel)
6. [Arduino-temperature-control](https://github.com/milesburton/Arduino-Temperature-Control-Library)
