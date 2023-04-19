#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            BCM3
#define NUMPIXELS      10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int moisturePin = A0;
const int temperaturePin = A1;

void setup(){
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(WIO_LIGHT, INPUT); 
  pixels.setBrightness(50);
  pixels.begin(); 
  Serial.begin(9600);
}

void loop(){
  int moistureLevel = analogRead(moisturePin);
  int temperatureLevel = analogRead(temperaturePin);
  int lightLevel = analogRead(WIO_LIGHT); 
  Serial.println(lightLevel);
  testLight(lightLevel);
}

void testLight(int lightLevel){
 pixels.clear();
 int range = map(lightLevel, 0, 1300, 0, 10);
 Serial.println(range);
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


