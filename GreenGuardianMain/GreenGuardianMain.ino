// below is the setup needed for the LCD library
#include "TFT_eSPI.h"
TFT_eSPI tft;
// below is the setup needed for the RGB light stick library
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#include <math.h>
#endif
#define PIN            BCM3
#define NUMPIXELS      10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int moisturePin = A0;
const int temperaturePin = A1;
const int ledPin = A2;
bool isTestLight = true;
bool buzzerOn = true;        // Initialize the boolean variable as true, to track if the buzzer is on
const int maxTemp = 30;      // the temperature for which the plant should not exceed
const int B = 4275;          // temperature sensor thermistor beta coefficient value, given by manufacturer
const int R0 = 100000;       // temperature sensor reference resistance

void setup(){
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(WIO_LIGHT, INPUT);
  pinMode(BUTTON_3, INPUT_PULLUP);    //assign button 3 to swap rgb stick modes
  pinMode(BUTTON_2, INPUT_PULLUP);    //assign button 2 to turn off the buzzer sound
  pinMode(WIO_BUZZER, OUTPUT);
  pixels.setBrightness(50);           // brightness of led stick
  pixels.begin();
  tft.begin();
  tft.setRotation(3);
  Serial.begin(9600);

  tft.fillScreen(TFT_GREEN);
  tft.fillRect(10,10,300,220, TFT_DARKGREEN);
  tft.fillRect(10,83,300,10, TFT_GREEN);
  tft.fillRect(10,156,300,10, TFT_GREEN);
  tft.fillRect(220,22,80,50, TFT_GREEN);
  tft.fillRect(220,105,80,40, TFT_GREEN);
  tft.fillRect(220,178,80,40, TFT_GREEN);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("Moisture",40,37);
  tft.drawString("Light",40,115);
  tft.drawString("Temp",40,187);
}

void loop(){
  int moistureLevel = analogRead(moisturePin);
  int temperatureLevel = analogRead(temperaturePin);
  int lightLevel = analogRead(WIO_LIGHT);
  if (isTestLight) {
    testLight(lightLevel);
  } else {
    testMoisture(moistureLevel);
  }
  if (digitalRead(BUTTON_3) == LOW) {
    delay(200);
    isTestLight = !isTestLight; // toggle to change mode of RGB stick
    delay(200);
  }

if (digitalRead(BUTTON_2) == LOW) { // If BUTTON_2 is being pressed
    buzzerOn = !buzzerOn; // Toggle the boolean variable
    delay(100); // Debounce the button (detects only a single signal)
  }

  testTemperature(calculateTemp(temperatureLevel));
  drawScreen(moistureLevel, lightLevel, temperatureLevel);
}

void drawScreen(int moistureLevel, int lightLevel, int temperatureLevel){
  delay(500);
  tft.fillRect(220,22,80,50, TFT_GREEN);
  tft.fillRect(220,105,80,40, TFT_GREEN);
  tft.fillRect(220,178,80,40, TFT_GREEN);

  // display moisture
  tft.setTextSize(2);
  if (moistureLevel >= 0 && moistureLevel < 300) {           // dry - dry
    tft.setTextColor(TFT_RED);
    tft.drawString("Dry",243,40);
  } else if(moistureLevel >= 300 && moistureLevel < 600) {   // moist - darkcyan
    tft.setTextColor(TFT_DARKCYAN);
    tft.drawString("Moist",232,40);
  } else if(moistureLevel >= 600 && moistureLevel <= 950){    // wet - blue
    tft.setTextColor(TFT_BLUE);
    tft.drawString("Wet",243,40);
  } else {
    tft.setTextColor(TFT_BLACK);                              // error - black (used for values that are outside the range)
    tft.drawString("ERROR",232,40);
  }

  // display light
  int range = map(lightLevel, 0, 1300, 0, 10);         // map light values to a range for percentage
  if(range < 3){
   tft.setTextColor(TFT_RED);
   tft.drawString("Low",245,118);
  } else if (range > 8){
   tft.setTextColor(TFT_RED);
   tft.drawString("High",237,118);
  } else if (range > 2 && range < 9){
   tft.setTextColor(TFT_DARKGREEN);
   tft.drawString("Good",237,118);
  } else {
   tft.setTextColor(TFT_BLACK);
   tft.drawString("ERROR",232,118);
  }

  // display temperature
  tft.setTextSize(3);
  int celcius = calculateTemp(temperatureLevel);
  if(celcius >= maxTemp){
   tft.setTextColor(TFT_RED);
  } else if (celcius > 125 || celcius < -40) {
    tft.setTextColor(TFT_BLACK);
    tft.drawString("ERROR",232,188);
  } else {
   tft.setTextColor(TFT_DARKGREEN);
  }

  celcius = 11; // temporary placeholder
  tft.drawNumber(celcius,233,188);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("C",270,188);
}


void testLight(int lightLevel){
 pixels.clear();
 int range = map(lightLevel, 0, 1300, 0, 10);         // map light values to a range to activate leds
 if(range < 3 || range > 8){
   for(int i = 0; i < range || (i == 0 && range == 0); i++){
    pixels.setPixelColor(i, pixels.Color(255,0,0));   // red for too low/high
   }
  } else {
   for(int i = 0; i < range; i++){
   pixels.setPixelColor(i, pixels.Color(255,255,0));  // yellow for sufficient
   }
 }
 pixels.show();
}


void testMoisture(int moistureLevel){
 pixels.clear();
 int range;
  if (moistureLevel >= 0 && moistureLevel < 300) {           // dry - brown
    range = map(moistureLevel, 0, 299, 0, 3);
    for(int i = 0; i < range || (i == 0 && range == 0); i++){
     pixels.setPixelColor(i, pixels.Color(255,0,0));
    }
  } else if (moistureLevel >= 300 && moistureLevel < 600) {  // moist - light blue
    range = map(moistureLevel, 300, 599, 3, 7);
    for(int i = 0; i < range; i++){
     pixels.setPixelColor(i, pixels.Color(69,165,217));
    }
  } else {                                                   // wet - dark blue
    range = map(moistureLevel, 600, 1023, 7, 10);
    for(int i = 0; i < range; i++){
     pixels.setPixelColor(i, pixels.Color(0,0,255));
    }
  }
 pixels.show();
}


int calculateTemp(int temperatureLevel){
  float R = 1023.0 / temperatureLevel - 1.0;                          // calculate the resistance of the thermistor
  R = R0 * R;                                                         // adjust resistance based on reference resistance
  int celcius = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15;      // convert to temperature using Steinhart-Hart equation
  return celcius;
}

void testTemperature(int celcius){
  if(celcius >= maxTemp){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void errorSound() {
  const unsigned long buzzerBeep = 400;
  const unsigned long shortPause = 200;
  const unsigned long longPause = 1000;
  const int buzzerFrequency = 150;

  //millis function is used to keep track of current time and the start time.
  unsigned long startTime = millis();

  // Play the buzzer if the boolean is true and the buzzerBeep time hasnt elapsed
  while (millis() - startTime < buzzerBeep && buzzerOn) { // Only play the buzzer if the boolean is true
    analogWrite(WIO_BUZZER, buzzerFrequency);
  }

  startTime = millis();

  //400ms pause
  while (millis() - startTime < shortPause) {
    analogWrite(WIO_BUZZER, 0);
  }

  startTime = millis();

  // Play the buzzer if the boolean is true and the buzzerBeep time hasnt elapsed
  while (millis() - startTime < buzzerBeep && buzzerOn) { // Only play the buzzer if the boolean is true
    analogWrite(WIO_BUZZER, buzzerFrequency);
  }

  startTime = millis();

  // Pause the buzzer for the 1000ms
  while (millis() - startTime < longPause) {
    analogWrite(WIO_BUZZER, 0);
  }
}
