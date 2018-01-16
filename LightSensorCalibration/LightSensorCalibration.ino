/*This simple Sketch is part of the "Neopixel Binary Clock" Project.
 * Use it to get the Bright and Dark values from your LDR.  
 * Those values are used by the Clocks automatic dimmer to adjust the brightness according to the lighting conditions.
 */

const int LIGHT_SENSOR_PIN = A0;

void runTest();
void runCountdown();
int getSensorReading();
void printLetterByLetter(String message);

int brightValue = 0;
int darkValue = 0;

void setup() {
Serial.begin(9600);
pinMode(LIGHT_SENSOR_PIN,INPUT);
printLetterByLetter("Let's get you the right values for your Binary Clocks automatic dimmer ");
runTest();
}

void loop() {
  //empty
}

void runTest(){
 printLetterByLetter("The first value, that we are going to measure, is for bright lighting conditions."); 
 printLetterByLetter("Place the clock somewhere with a lot off light e.g. outside in the sun or underneath a desk lamp.");
 runCountdown();
 brightValue = getSensorReading();
 printLetterByLetter("The second value, that we are going to measure, is for dark lighting conditions.");
 printLetterByLetter("Simply hold your hand over the sensor to simulate nighttime.");
 runCountdown();
 darkValue = getSensorReading();

  printLetterByLetter("All right, here are the results.");
  printLetterByLetter("Your Bright Value is ");
  Serial.println(brightValue);
  printLetterByLetter("and your Dark Value is ");
  Serial.println(darkValue);
  Serial.println();
  if(brightValue > darkValue){
  printLetterByLetter("Now go ahead and replace the BRIGHT_VALUE and DARK_VALUE constants in the Binary Clock's Code with your readings.");
  }
  else{
    printLetterByLetter("Hm... strange.");  
  printLetterByLetter("Your dark value is greater than your bright value.");
   printLetterByLetter("I'm guessing that you conducted the test in the right order.");
   printLetterByLetter("You might want to check your connections again.");
  }
}

void runCountdown(){
  printLetterByLetter("The Reading starts in ");
   for(int i = 10; i > 0; i--){
    delay(1000);
    Serial.println(i);       
   }
}

void printLetterByLetter(String message){
  for(int i = 0; i < message.length(); i++){
    Serial.print(message.charAt(i));
    delay(50);
  }
  Serial.println();
  delay(500);
}

int getSensorReading(){
  printLetterByLetter("Reading sensor.");
int reading = analogRead(LIGHT_SENSOR_PIN);
delay(1000);

if(reading <= 10){
  printLetterByLetter("Hm... strange.");  
  printLetterByLetter("The reading shows Ground Potential or, in other words, 0V on your sensor pin.");
  printLetterByLetter("you might want to check your connections.");  
}
else if(reading >= 1015){
 printLetterByLetter("Hm... strange.");  
 printLetterByLetter("The reading shows High Potential or, in other words, +5V on your yensor pin.");
 printLetterByLetter("you might want to check your connections.");  
}
else{
  printLetterByLetter("Reading the sensor was successfull.");  
}
return reading;
}

