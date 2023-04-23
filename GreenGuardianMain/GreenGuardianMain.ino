// below is the setup needed for the LCD library
#include "TFT_eSPI.h"
TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);
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
const int maxTemp = 30;      // the temperature for which the plant should not exceed
const int B = 4275;          // temperature sensor thermistor beta coefficient value, given by manufacturer
const int R0 = 100000;       // temperature sensor reference resistance

void setup(){
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(WIO_LIGHT, INPUT);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pixels.setBrightness(50);           // brightness of led stick
  pixels.begin();
  tft.begin();
  tft.setRotation(3);
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);
  Serial.begin(9600);
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
  testTemperature(calculateTemp(temperatureLevel));
  drawScreen(moistureLevel, lightLevel, temperatureLevel);
}

void drawScreen(int moistureLevel, int lightLevel, int temperatureLevel){
  spr.fillSprite(TFT_GREEN);
  spr.fillRect(10,10,300,220, TFT_DARKGREEN);
  spr.fillRect(10,83,300,10, TFT_GREEN);
  spr.fillRect(10,156,300,10, TFT_GREEN);
  spr.fillRect(220,22,80,50, TFT_GREEN);
  spr.fillRect(220,105,80,40, TFT_GREEN);
  spr.fillRect(220,178,80,40, TFT_GREEN);
  spr.setTextColor(TFT_WHITE);
  spr.setTextSize(3);
  spr.drawString("Moisture",40,37);
  spr.drawString("Light",40,115);
  spr.drawString("Temp",40,187);

  // display moisture
  spr.setTextSize(2);
  if (moistureLevel >= 0 && moistureLevel < 300) {           // dry - dry
    spr.setTextColor(TFT_RED);
    spr.drawString("Dry",243,40);
  } else if(moistureLevel >= 300 && moistureLevel < 600) {   // moist - darkcyan
    spr.setTextColor(TFT_DARKCYAN);
    spr.drawString("Moist",232,40);
  } else if(moistureLevel >= 600 && moistureLevel <= 950){    // wet - blue
    spr.setTextColor(TFT_BLUE);
    spr.drawString("Wet",243,40);
  } else {
    spr.setTextColor(TFT_BLACK);                              // error - black (used for values that are outside the range)
    spr.drawString("ERROR",232,40)
  }

  // display light
  int range = map(lightLevel, 0, 1300, 0, 10);         // map light values to a range for percentage
  if(range < 3){
   spr.setTextColor(TFT_RED);
   spr.drawString("Low",245,118);
  } else if (range > 8){
   spr.setTextColor(TFT_RED);
   spr.drawString("High",237,118);
  } else (range > 2 && range < 9){
   spr.setTextColor(TFT_DARKGREEN);
   spr.drawString("Good",237,118);
  } else {
   spr.setTextColor(TFT_BLACK);
   spr.drawString("ERROR",232,118);
  }

  // display temperature
  spr.setTextSize(3);
  int celcius = calculateTemp(temperatureLevel);
  if(celcius >= maxTemp){
   spr.setTextColor(TFT_RED);
  } else if (celcius < -40 || celcius > 125); {
    spr.setTextColor(TFT_BLACK);
    spr.drawString("ERROR",232,188);
  } else{
   spr.setTextColor(TFT_DARKGREEN);
  }
  spr.drawNumber(celcius,233,188);
  spr.setTextColor(TFT_BLACK);
  spr.drawString("C",270,188);
  spr.pushSprite(0,0); // push to LCD
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