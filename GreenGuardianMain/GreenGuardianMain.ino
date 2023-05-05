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
  pinMode(BUTTON_3, INPUT_PULLUP);
  pixels.setBrightness(50);           // brightness of led stick
  pixels.begin();
  setupLCD();
  setupWifi();
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

void connectWifi(){

  displayLCDmessage("(Re) Connecting To WiFi...", tft.color565(20, 70, 150), FM9, true, true, 60);

  delay(2000);

  WiFi.begin(SSID, PASS);

  if (WiFi.status() != WL_CONNECTED){

    Serial.println("Faild to connect...");

    delay(2000);

    delete wioClient;

    return setupWifi();

  }

  Serial.println("WiFi Connected!");

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

    Serial.println("Connecting to MQTT Broker...");

    if (mqttClient->connect(clientName)){ 

      Serial.println("Connected to MQTT Broker!");

      subscribeMqtt();

    } else {

      Serial.print("Failed, retrying...");

      delay(2000);

    }
  }
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

  publishMqtt();

  mqttClient->loop();

}