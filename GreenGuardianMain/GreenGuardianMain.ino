// below is the setup needed for the LCD library
#include "TFT_eSPI.h"
TFT_eSPI tft;
// below is the setup needed for the RGB light stick library
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#include <math.h>
#endif
// below is the setup needed for wifi connectivity
#include <rpcWiFi.h>
#include "wifiauth.h"
// below is the setup needed for mqtt connectivity
#include <PubSubClient.h>
// below is the setup needed for used fonts 
#include "Free_Fonts.h"

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

//Insert values below:
IPAddress brokerIp(0, 0, 0, 0);
int port = 0;

//Insert values below:
char pubTopic[] = "";
char subTopic[] = "";
char pubMessage[] = "";
char clientName[] = "";

WiFiClient* wioClient = nullptr;
PubSubClient* mqttClient = nullptr;

int Y_Cord_Start_Pos = 0;
int text_Y_Margin_Offset = 0;

void displayLCDmessage(char* message, uint16_t textColor, const GFXfont* textSize, boolean centerAlign, boolean clearPrevLCD, int Y_Cord_Start_Pos = Y_Cord_Start_Pos);

void setup(){
  while (!Serial){
    Serial.begin(9600);
  }
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(WIO_LIGHT, INPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);    //assign button 2 to turn off the buzzer sound
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  pixels.setBrightness(50);           // brightness of led stick
  pixels.begin();
  setupLCD();
  setupWifi();
  setupDataDisplay();
}

void setupLCD(){

  tft.begin();

  tft.setRotation(3);

  tft.fillScreen(TFT_WHITE);

}

void setupWifi(){

  wioClient = new WiFiClient;

  WiFi.disconnect();

  connectWifi();

}

void setupDataDisplay(){
  tft.fillScreen(TFT_GREEN);
  tft.fillRect(10,10,300,220, TFT_DARKGREEN);
  tft.fillRect(10,83,300,10, TFT_GREEN);
  tft.fillRect(10,156,300,10, TFT_GREEN);
  tft.fillRect(220,22,80,50, TFT_GREEN);
  tft.fillRect(220,105,80,40, TFT_GREEN);
  tft.fillRect(220,178,80,40, TFT_GREEN);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.drawString("Moisture",40,37);
  tft.drawString("Light",40,115);
  tft.drawString("Temp",40,187);
}

void connectWifi(){

  displayLCDmessage("(Re) Connecting To WiFi...", tft.color565(20, 70, 150), FM9, true, true, 60);

  displayLCDmessage("(Connection Keeps Failing? Fix:", tft.color565(70, 50, 50), FF25, true, false);

  displayLCDmessage("1. WiFi Turned Off)", tft.color565(70, 50, 50), FF25, true, false);

  displayLCDmessage("(Screen Static For T>15s? Fix:", TFT_BLACK, FF33, true, false);

  displayLCDmessage("2. Faulty AP Point Configuration)", tft.color565(70, 50, 50), FF25, true, false);

  displayLCDmessage("Alt 2. Needs Manual Wio Restart", TFT_BLACK, FF33, true, false);

  displayLCDmessage("After Reconfiguration", TFT_BLACK, FF33, true, false);

  delay(2000);

  WiFi.begin(SSID, PASS);

  if (WiFi.status() != WL_CONNECTED){

    displayLCDmessage("Failed To Connect To WiFi", tft.color565(70, 0, 0), FF25, true, true, 60);

    displayLCDmessage("Retry:", tft.color565(0, 70, 70), FM9, true, false);

    displayLCDmessage("Press (Top Left Button)", tft.color565(0, 70, 70), FM9, true, false);

    displayLCDmessage("Return Home:", tft.color565(0, 70, 70), FM9, true, false);

    displayLCDmessage("Press (Top Right Button)", tft.color565(0, 70, 70), FM9, true, false);

    delete wioClient;

    while (true){

      if (digitalRead(BUTTON_3) == LOW){

        return setupWifi();

      } else if (digitalRead(BUTTON_1) == LOW){

        Serial.print("To Welcome Screen");

      }
    }
  }

  displayLCDmessage("WiFi Connected!", tft.color565(0, 110, 0), FF6, true, true, 110);

  delay(3000);

  setupMqtt();

}

void setupMqtt(){

  mqttClient = new PubSubClient(*wioClient);

  mqttClient->setServer(brokerIp, port);

  mqttClient->setCallback(handleSubMessage);

  connectMqtt();

}

void connectMqtt(){

  while (!mqttClient->connected()){

    displayLCDmessage("(Re) Connecting to MQTT Broker...", tft.color565(70, 50, 100), FF25, true, true, 60);

    displayLCDmessage("(Connection Keeps Failing? Fix:", TFT_BLACK, FF33, true, false);

    displayLCDmessage("1. Broker Is Not Running)", TFT_BLACK, FF33, true, false);

    displayLCDmessage("(Screen Static For T>15s? Fix:", TFT_BLACK, FF33, true, false);

    displayLCDmessage("2. Faulty BrokerIP &&/|| WiFi Loss)", TFT_BLACK, FF33, true, false);

    displayLCDmessage("Alt 2. Needs Manual Wio Restart", TFT_BLACK, FF33, true, false);

    displayLCDmessage("After Reconfiguration", TFT_BLACK, FF33, true, false);

    delay(3000);

    if (mqttClient->connect(clientName)){

      displayLCDmessage("Connected To MQTT Broker!", tft.color565(0, 110, 0), FS12, true, true, 110);

      subscribeMqtt();

      delay (3000);

    } else {

      displayLCDmessage("Failed To Connect To MQTT Broker", tft.color565(100, 0, 0), FSS9, true, true, 70);

      displayLCDmessage("Retry:", tft.color565(0, 70, 70), FM9, true, false);

      displayLCDmessage("Press (Top Left Button)", tft.color565(0, 70, 70), FM9, true, false);

      displayLCDmessage("Return Home:", tft.color565(0, 70, 70), FM9, true, false);

      displayLCDmessage("Press (Top Right Button)", tft.color565(0, 70, 70), FM9, true, false);
      
      delay(2000);

      while (true){

        if (digitalRead(BUTTON_3) == LOW){

          return connectMqtt();

        } else if (digitalRead(BUTTON_1) == LOW){

          Serial.print("To Welcome Screen");

        }
      }
    }
  }

  setupDataDisplay();

}

void publishMqtt(){

  delay(1000);

  mqttClient->publish(pubTopic, pubMessage);

}

void subscribeMqtt(){

  mqttClient->subscribe(subTopic);

}

void displayLCDmessage(char* message, uint16_t textColor, const GFXfont* textSize, boolean centerAlign, boolean clearPrevLCD, int Y_Cord_Start_Pos){

  ::Y_Cord_Start_Pos = Y_Cord_Start_Pos;

  if (clearPrevLCD){

    tft.fillScreen(TFT_WHITE);

    text_Y_Margin_Offset = 0;

  }

  tft.setTextColor(textColor);
  
  tft.setFreeFont(textSize);  

  tft.drawString(message, centerAlign ? 160 - (tft.textWidth(message) / 2) : 0, Y_Cord_Start_Pos + text_Y_Margin_Offset);

  text_Y_Margin_Offset += tft.fontHeight();

}

void handleSubMessage(char* topic, byte* payload, unsigned int length){

  char message[length + 1]; 

  for (int i = 0; i < length; i++){

    message[i] = (char) payload[i];

  }

  message[length] = '\0';

  Serial.println(message);

}

void loop(){

  if (WiFi.status() != WL_CONNECTED){
  
    return setupWifi(); 

  }

    if (!mqttClient->connected()){

    return connectMqtt();

  }

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

  publishMqtt();

  mqttClient->loop();

}

void drawScreen(int moistureLevel, int lightLevel, int temperatureLevel){
  delay(500);
  tft.fillRect(220,22,80,50, TFT_GREEN);
  tft.fillRect(220,105,80,40, TFT_GREEN);
  tft.fillRect(220,178,80,40, TFT_GREEN);

  // display moisture
  tft.setTextSize(1);
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
  }
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