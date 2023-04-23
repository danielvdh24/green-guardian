#include <math.h>
// setup needed for the RGB light stick library:
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <rpcWiFi.h>
#include <PubSubClient.h>
#include "wificreds.h"

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

//Insert values below:
IPAddress ip(0, 0, 0, 0);
int port = 0;

//Insert values below:
char clientName[] = "";
char pubTopic[] = "";
char subTopic[] = "";
char pubMessage[] = "";

WiFiClient wioClient;
PubSubClient mqttClient(wioClient);

void setupWifi();

void connectWifi();

void setupMqtt();

void connectMqtt();

void publishMqtt();

void subscribeMqtt();

void handleSubMessage(char* topic, byte* payload, unsigned int length);


void setup(){
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(WIO_LIGHT, INPUT);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pixels.setBrightness(50);           // brightness of led stick
  pixels.begin();
  Serial.begin(115200);

  setupWifi();

  connectWifi();

  setupMqtt();

  connectMqtt();
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
  testTemperature(temperatureLevel);

  if (!mqttClient.connected()){

    connectMqtt();

  }

  mqttClient.loop();
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

void testTemperature(int temperatureLevel){
  float R = 1023.0 / temperatureLevel - 1.0;                          // calculate the resistance of the thermistor
  R = R0 * R;                                                         // adjust resistance based on reference resistance
  float temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15;  // convert to temperature using Steinhart-Hart equation
  if(temperature >= maxTemp){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void setupWifi(){

  WiFi.mode(WIFI_STA);

  WiFi.disconnect();

}

void connectWifi(){

  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED){

    Serial.println("Connecting to WiFi...");

    delay(1000);

  }

  Serial.println("WiFi Connected!");

}

void setupMqtt(){

  mqttClient.setServer(ip, port);

  mqttClient.setCallback(handleSubMessage);

}

void connectMqtt(){

  while (!mqttClient.connected()){

    Serial.println("Connecting to MQTT Broker...");

    if (mqttClient.connect(clientName)){ //process will be moved to setupmqtt

      Serial.println("Connected to MQTT Broker!");

      publishMqtt();

      subscribeMqtt();

    } else {

      Serial.print("Failed, retrying...");

      delay(2000);

    }
  }
}

void publishMqtt(){

  mqttClient.publish(pubTopic, pubMessage);

}

void subscribeMqtt(){

  mqttClient.subscribe(subTopic);

}

void handleSubMessage(char* topic, byte* payload, unsigned int length){

  char message[length + 1]; 

  for (int i = 0; i < length; i++){

    message[i] = (char) payload[i];

  }

  message[length] = '\0';

  Serial.print(message);

}