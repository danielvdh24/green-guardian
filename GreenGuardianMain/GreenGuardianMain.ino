//Setup for LCD library
#include "TFT_eSPI.h"
TFT_eSPI tft;
//Setup for RGB light stick library
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#endif
//Setup for wifi connectivity
#include <rpcWiFi.h>
#include "wifiauth.h"
//Setup for mqtt connectivity
#include <PubSubClient.h>
//Setup for custom fonts
#include "Free_Fonts.h"
//Setup for temperature sensor
#include "DHT.h"
#include <unordered_map>
#include <ctime>
using namespace std;

//Pin definitions for rgb stick and temperature sensor
#define DHTPIN A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define PIN            BCM3
#define NUMPIXELS      10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int moisturePin = A0;
const int temperaturePin = A1;
const int ledPin = A2;
bool isTestLight = true;
bool buzzerOn = true;        //Initialize the boolean variable as true, to track if the buzzer is on
const int maxTemp = 30;      //Temperature limit for plant

//variable for data history
const int queueSize = 336; // maximum size of the queue
int lightQueue[queueSize]; // array to hold the queue
int moistureQueue[queueSize]; // array to hold the moisture queue
int temperatureQueue[queueSize]; // array to hold the queue
int lightIndex = 0; // current index of the queue
int moistureIndex = 0; // current index of the moisture queue
int temperatureIndex = 0; // current index of the moisture queue
unsigned long previousMillis = 0; // previous time when a value was added to the queue
const unsigned long interval = 1800000; // interval between adding values to the queue (30 mins)


//Variables to keep track of mode setup and connectivity status
bool onlineMode = false;
bool showStartingScreen = true;
bool modeIsSetup = false;
bool wifiIsConnected = false;
bool brokerIsConnected = false;

//Insert values below:
IPAddress brokerIp(0, 0, 0, 0);
int port = 0;

//Insert values below:
char pubTopic[] = "";
char subTopic[] = "";
char clientName[] = "";

WiFiClient* wioClient = nullptr;
PubSubClient* mqttClient = nullptr;

int Y_Cord_Start_Pos = 0;
int text_Y_Margin_Offset = 0;

std::unordered_map<std::string, int> commands = {
  {"pub5", 1},
  {"pub60", 2},
  {"pub300", 3},
  {"pub1800", 4},
  {"stoppub", 5},
};

boolean doPub = false;
int timeSincePub = 0;
int pubFrequencySec = 0;

int localTime = 0;

int ledScedStartTime = 0;
int ledScedEndTime = 0;

int moistureLevel = 0;
int temperatureLevel = 0;
int lightLevel = 0;

void displayLCDmessage(char* message, uint16_t textColor, const GFXfont* font, boolean centerAlign, boolean clearPrevLCD, int Y_Cord_Start_Pos = Y_Cord_Start_Pos);

void setup(){
  while(!Serial){
    Serial.begin(9600);
  }
  pinMode(WIO_5S_LEFT, INPUT);
  pinMode(WIO_5S_RIGHT, INPUT);
  pinMode(WIO_5S_PRESS, INPUT);
  tft.begin();
  tft.setRotation(3);
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(WIO_LIGHT, INPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  pinMode(WIO_BUZZER, OUTPUT);
  pixels.setBrightness(50);
  pixels.begin();
}

void setupWifi(){
  wioClient = new WiFiClient;
  WiFi.disconnect();
}

void setupDataDisplay(){
  tft.setFreeFont(NULL);
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

//Function to add a value to a queue and calculate the average value
long addValueToQueueAndCalculateAverage(int value, int* queue, int& index) {
  //Add the value to the queue
  queue[index] = value;

  //Increment the queue index, wrapping around if necessary
  index = (index + 1) % queueSize;

  //Calculate the average total value of the queue
  long totalValue = 0;
  for (int i = 0; i < queueSize; i++) {
    totalValue += queue[i];
  }
  long averageValue = totalValue / queueSize;

  //Return the average value
  return averageValue;
}

void drawStartingScreen() {
  tft.setFreeFont(NULL);
  tft.setTextSize(1);
  tft.fillScreen(TFT_DARKGREEN);
  tft.fillEllipse(145, 100, 5, 40, TFT_GREEN);
  tft.fillEllipse(160, 100, 5, 40, TFT_GREEN);
  tft.fillEllipse(175, 100, 5, 40, TFT_GREEN);
  tft.fillRect(130, 120, 60, 50, tft.color565(150, 75, 0));
  tft.fillRoundRect(125, 115, 70, 15, 10, tft.color565(150, 75, 0));
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.setCursor(40, 20);
  tft.print("Green Guardian");
  tft.setTextSize(2);
  tft.setCursor(40, 200);
  tft.print("Online");
  tft.setCursor(200, 200);
  tft.print("Offline");
  handleSwitchInput();
}

void handleSwitchInput() {
  int selectedMode = 0;
  while (true) {
    if (digitalRead(WIO_5S_LEFT) == LOW){
      toggleOnline();
      selectedMode = 1;
    }
    if (digitalRead(WIO_5S_RIGHT) == LOW){
      toggleOffline();
      selectedMode = 2;
    }
    if (digitalRead(WIO_5S_PRESS) == LOW && selectedMode == 1){
      //Replace with online screen
      onlineMode = true;
      return;
    }
    if (digitalRead(WIO_5S_PRESS) == LOW && selectedMode == 2){
      //Replace with offline screen
      onlineMode = false;
      return;
    }
  }
}

void toggleOnline(){
  tft.drawRect(25, 197, 100, 20, TFT_CYAN);
  tft.drawRect(190, 197, 100, 20, TFT_DARKGREEN);
}

void toggleOffline(){
  tft.drawRect(190, 197, 100, 20, TFT_CYAN);
  tft.drawRect(25, 197, 100, 20, TFT_DARKGREEN);
}

void connectWifi(){
  displayLCDmessage("(Re) Connecting To WiFi...", tft.color565(20, 70, 150), FM9, true, true, 50);
  displayLCDmessage("(Connection Keeps Failing? Fix:", tft.color565(70, 50, 50), FF25, true, false);
  displayLCDmessage("1. WiFi Turned Off", tft.color565(70, 50, 50), FF25, true, false);
  displayLCDmessage("2. Missing Code WiFi Config", tft.color565(70, 50, 50), FF25, true, false);
  displayLCDmessage("3. Faulty AP Point Config)", tft.color565(70, 50, 50), FF25, true, false);
  displayLCDmessage("Alt 2 & 3. Needs Manual Wio Restart", TFT_BLACK, FF33, true, false);
  displayLCDmessage("After Reconfiguration", TFT_BLACK, FF33, true, false);
  delay(1500);

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
        return;
      } else if (digitalRead(BUTTON_1) == LOW){
        showStartingScreen = true;
        return;
      }
    }
  }
  displayLCDmessage("WiFi Connected!", tft.color565(0, 110, 0), FF6, true, true, 110);
  delay(3000);
  wifiIsConnected = true;
}

void setupMqtt(){
  mqttClient = new PubSubClient(*wioClient);
  mqttClient->setServer(brokerIp, port);
  mqttClient->setCallback(handleSubMessage);
}

void connectMqtt(){
  displayLCDmessage("(Re) Connecting to MQTT Broker...", tft.color565(70, 50, 100), FF25, true, true, 60);
  displayLCDmessage("(Connection Keeps Failing? Fix:", TFT_BLACK, FF33, true, false);
  displayLCDmessage("1. Broker Is Not Running)", TFT_BLACK, FF33, true, false);
  displayLCDmessage("(Screen Static For T>10s? Fix:", TFT_BLACK, FF33, true, false);
  displayLCDmessage("2. Faulty BrokerIP &&/|| WiFi Loss)", TFT_BLACK, FF33, true, false);
  displayLCDmessage("Alt 2. Needs Manual Wio Restart", TFT_BLACK, FF33, true, false);
  displayLCDmessage("After Reconfiguration", TFT_BLACK, FF33, true, false);
  delay(4000);

  if (mqttClient->connect(clientName)){
    displayLCDmessage("Connected To MQTT Broker!", tft.color565(0, 110, 0), FS12, true, true, 110);
    brokerIsConnected = true;
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
        return;
      } else if (digitalRead(BUTTON_1) == LOW){
        showStartingScreen = true;
        return;
      }
    }
  }
}

void publishMqtt(){}

void subscribeMqtt(){
  mqttClient->subscribe(subTopic);
}

void displayLCDmessage(char* message, uint16_t textColor, const GFXfont* font, boolean centerAlign, boolean clearPrevLCD, int Y_Cord_Start_Pos){
  ::Y_Cord_Start_Pos = Y_Cord_Start_Pos;
  if (clearPrevLCD){
    tft.fillScreen(TFT_WHITE);
    text_Y_Margin_Offset = 0;
  }
  tft.setTextColor(textColor);
  tft.setFreeFont(font);
  tft.drawString(message, centerAlign ? 160 - (tft.textWidth(message) / 2) : 0, Y_Cord_Start_Pos + text_Y_Margin_Offset);
  text_Y_Margin_Offset += tft.fontHeight();
}

void handleSubMessage(char* topic, byte* payload, unsigned int length){

  if (isdigit((char) payload[0])){

    string str_time = "";

    for (int i = 0; i < length; i++){
      str_time += (char) payload[i];
    }

    localTime = stoi(str_time);

  } else {

  string msg = "";

  boolean isFirstEncounter = true;

  boolean isSecondEncounter = true;

  string commandKey = "";

  char symbol = '\0';

for (int i = 0; i < length; i++){

  symbol = (char) payload[i];

  if (symbol == ';'){

    if (isFirstEncounter){
      commandKey = msg;
      isFirstEncounter = false;

      if (i == length - 1){
        break;
      }

    } else {

      if (isSecondEncounter){
        ledScedStartTime = stoi(msg);
        isSecondEncounter = false;

      } else {

        ledScedEndTime = stoi(msg);
      }

    }

    msg.clear();

    continue;
  }

  msg += symbol;

  }

  switch(commands.at(commandKey)){
    case 1:
      timeSincePub = 0;
      pubFrequencySec = 5;
      doPub = true;
      break;

    case 2:
      timeSincePub = 0;
      pubFrequencySec = 60;
      doPub = true;
      break;

    case 3:
      timeSincePub = 0;
      pubFrequencySec = 300;
      doPub = true;
      break;

    case 4:
      timeSincePub = 0;
      pubFrequencySec = 1800;
      doPub = true;
      break;

    case 5:
      doPub = false;
      timeSincePub = 0;
      break;
  }
  }
}

void loop(){

  if (showStartingScreen){
    drawStartingScreen();
    modeIsSetup = false;
    showStartingScreen = false;
  }
  if(onlineMode){
    if (WiFi.status() != WL_CONNECTED){
      wifiIsConnected = false;
      tft.setTextSize(NULL);
      while (!wifiIsConnected){
        setupWifi();
        connectWifi();
        if (showStartingScreen){
          return;
        }
      }
      setupMqtt();
    }

    if (!mqttClient->connected()){
      brokerIsConnected = false;
      modeIsSetup = false;
      tft.setTextSize(NULL);
      while (!brokerIsConnected){
        connectMqtt();
        if (showStartingScreen){
          return;
        }
      }
    }
  }

  if (!modeIsSetup){
    setupDataDisplay();
    modeIsSetup = true;
  }

    moistureLevel = analogRead(moisturePin);
    temperatureLevel = dht.readTemperature();
    lightLevel = analogRead(WIO_LIGHT);
    if (isTestLight) {
      testLight(lightLevel);
    } else {
      testMoisture(moistureLevel);
    }
    if (digitalRead(BUTTON_3) == LOW) {
      delay(200);
      isTestLight = !isTestLight; //Toggle to change mode of RGB stick
      delay(200);
    }

  if (digitalRead(BUTTON_2) == LOW) {
      buzzerOn = !buzzerOn;
      delay(100);
    }

    drawScreen(moistureLevel, lightLevel, temperatureLevel);
    processSend(moistureLevel, lightLevel, temperatureLevel);
  if(onlineMode){

    if(doPub){

      timeSincePub++;

      if(timeSincePub == pubFrequencySec){
      timeSincePub = 0;
      publishMqtt();
      }
    }

    mqttClient->loop();
  }
}

void drawScreen(int moistureLevel, int lightLevel, int temperatureLevel){
  unsigned long startTime = millis();
  while(millis() < startTime + 500){
      //Wait 500ms
  }
  tft.fillRect(220,22,80,50, TFT_GREEN);
  tft.fillRect(220,105,80,40, TFT_GREEN);
  tft.fillRect(220,178,80,40, TFT_GREEN);
  tft.setFreeFont(NULL);
  //Display moisture
  tft.setTextSize(2);
  if (moistureLevel >= 0 && moistureLevel < 300) {           //Dry - dry
    tft.setTextColor(TFT_RED);
    tft.drawString("Dry",243,40);
    errorSound();
  } else if(moistureLevel >= 300 && moistureLevel < 600) {   //Moist - darkcyan
    analogWrite(WIO_BUZZER, 0);
    tft.setTextColor(TFT_DARKCYAN);
    tft.drawString("Moist",232,40);
  } else if(moistureLevel >= 600 && moistureLevel <= 950){    //Wet - blue
    analogWrite(WIO_BUZZER, 0);
    tft.setTextColor(TFT_BLUE);
    tft.drawString("Wet",243,40);
  } else {
    tft.setTextColor(TFT_BLACK);                              //Error - black (values outside range)
    tft.drawString("ERROR",232,40);
  }
   //Display light
  int range = map(lightLevel, 0, 1300, 0, 10);                //Map light values to a range for percentage
  if(range < 3){
   tft.setTextColor(TFT_RED);
   tft.drawString("Low",245,118);
   errorSound();
  } else if (range > 8){
   tft.setTextColor(TFT_RED);
   tft.drawString("High",237,118);
   errorSound();
  } else if (range > 2 && range < 9){
   analogWrite(WIO_BUZZER, 0);
   tft.setTextColor(TFT_DARKGREEN);
   tft.drawString("Good",237,118);
  } else {
   tft.setTextColor(TFT_BLACK);
   tft.drawString("ERROR",232,118);
  }
  //Display temperature
    if(temperatureLevel >= maxTemp){
     tft.setTextColor(TFT_RED);
    } else if (temperatureLevel > 125 || temperatureLevel < -40) {
      tft.setTextColor(TFT_BLACK);
      tft.drawString("ERROR",232,188);
    } else {
     tft.setTextColor(TFT_DARKGREEN);
    }

    tft.drawNumber(temperatureLevel,240,191);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("C",270,191);
}

void processSend(int moistureLevel, int lightLevel, int temperatureLevel){
  //Moisture
  unsigned long currentMillisMoisture = millis();
  if (currentMillisMoisture - previousMillis >= interval) {
    previousMillis = currentMillisMoisture;
    //Add the value to the queue and calculate the average total value of the queue
    long moistureAverage = addValueToQueueAndCalculateAverage(moistureLevel, moistureQueue, moistureIndex);
  }
  //Light
  unsigned long currentMillisLight = millis();
  if (currentMillisLight - previousMillis >= interval) {
    previousMillis = currentMillisLight;
    //Add the value to the queue and calculate the average total value of the queue
    long lightAverage = addValueToQueueAndCalculateAverage(lightLevel, lightQueue, lightIndex);
  }
  //Temperature
  unsigned long currentMillisTemperature = millis();
  if (currentMillisTemperature - previousMillis >= interval) {
    previousMillis = currentMillisTemperature;
    //Add the value to the queue and calculate the average total value of the queue
    long temperatureAverage = addValueToQueueAndCalculateAverage(temperatureLevel, temperatureQueue, temperatureIndex);
  }
}

void testLight(int lightLevel){
 pixels.clear();
 int range = map(lightLevel, 0, 1300, 0, 10);         //Map light values to a range to activate LEDs
 if(range < 3 || range > 8){
   for(int i = 0; i < range || (i == 0 && range == 0); i++){
    pixels.setPixelColor(i, pixels.Color(255,0,0));   //Red - too low/high
   }
  } else {
   for(int i = 0; i < range; i++){
   pixels.setPixelColor(i, pixels.Color(255,255,0));  //Yellow - sufficient
   }
 }
 pixels.show();
}

void testMoisture(int moistureLevel){
 pixels.clear();
 int range;
  if (moistureLevel >= 0 && moistureLevel < 300) {           //Dry - brown
    range = map(moistureLevel, 0, 299, 0, 3);
    for(int i = 0; i < range || (i == 0 && range == 0); i++){
     pixels.setPixelColor(i, pixels.Color(255,0,0));
    }
  } else if (moistureLevel >= 300 && moistureLevel < 600) {  //Moist - light blue
    range = map(moistureLevel, 300, 599, 3, 7);
    for(int i = 0; i < range; i++){
     pixels.setPixelColor(i, pixels.Color(69,165,217));
    }
  } else {                                                   //Wet - dark blue
    range = map(moistureLevel, 600, 1023, 7, 10);
    for(int i = 0; i < range; i++){
     pixels.setPixelColor(i, pixels.Color(0,0,255));
    }
  }
 pixels.show();
}

void testTemperature(int temp){
  if(temp >= maxTemp){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void errorSound() {
  const unsigned long buzzerBeep = 400;
  const unsigned long shortPause = 300;
  const int buzzerFrequency = 150;

  static unsigned long startTime = 0;
  static int state = 0;

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  switch (state) {
    case 0:
      if (elapsedTime < buzzerBeep && buzzerOn) {
        analogWrite(WIO_BUZZER, buzzerFrequency);
      } else {
        startTime = currentTime;
        state = 1;
      }
      break;
    case 1:
      if (elapsedTime < shortPause) {
        analogWrite(WIO_BUZZER, 0);
      } else {
        startTime = currentTime;
        state = 0;
      }
      break;
  }
}