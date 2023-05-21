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
using namespace std;

//Pin definitions for rgb stick and sensors
#define DHTPIN A1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define PIN            BCM3
#define NUMPIXELS      10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
const int moisturePin = A0;
const int temperaturePin = A1;
const int ledPin = A2;
const int ledBulbPin = A3;

bool isTestLight = true;
bool buzzerOn = true;        //Initialize the boolean variable as true, to track if the buzzer is on

//variable for data history
const int queueSize = 336; // maximum size of the queue
int lightQueue[queueSize]; // array to hold the queue
int moistureQueue[queueSize]; // array to hold the moisture queue
int temperatureQueue[queueSize]; // array to hold the queue
int lightIndex = 0; // current index of the queue
int moistureIndex = 0; // current index of the moisture queue
int temperatureIndex = 0; // current index of the moisture queue
int interval = 1800000; //Interval between adding values to the queue (30 mins) in seconds
int addToQueueTimer = 0; //Seconds

//Variables for plant species
int maxTemp = 30;
int moistureLowThreshold = 3;
int moistureHighThreshold = 7;
int lightLowThreshold = 3;
int lightHighThreshold = 7;

//Variable to store the currently selected preset
const int arraySize = 5;
int currentPreset = 2;
int currentIndex = 2;
uint16_t primaryColor[arraySize] = {TFT_YELLOW, TFT_CYAN, TFT_GREEN, tft.color565(0, 255, 255), tft.color565(165, 42, 42)};
uint16_t secondaryColor[arraySize] = {TFT_ORANGE, TFT_BLUE, TFT_DARKGREEN, TFT_OLIVE, TFT_MAROON};

//Variables to keep track of mode setup and connectivity status
bool onlineMode = false;
bool showStartingScreen = true;
bool modeIsSetup = false;
bool wifiIsConnected = false;
bool brokerIsConnected = false;

//Ipv4 address provided by router and running broker port
IPAddress brokerIp(0, 0, 0, 0);
int port = 0;

//Variables for wio/pc mqtt topics and wio mqtt client name
char clientName[] = "WIO_TERMINAL";
char pubTopic[] = "sensordata";
char subTopic[] = "commands";

//Variables for wio wifi and wio mqtt client
WiFiClient* wioClient = nullptr;
PubSubClient* mqttClient = nullptr;

//Variables for text spacing vertically
int Y_Cord_Start_Pos = 0;
int text_Y_Margin_Offset = 0;

//Map of commandkey-names and their associated integer values
unordered_map<string, int> commands = {
  {"pub5", 1},
  {"pub60", 2},
  {"pub300", 3},
  {"pub1800", 4},
  {"stoppub", 5},
  {"timeScedOn", 6},
  {"timeScedOff", 7},
  {"automaticOn", 8},
  {"automaticOff", 9},
  {"activeOff", 10},
  {"activeOn", 11},
};

//Variables for mqtt publish frequency
boolean doPub = false;
boolean timeToPub = true;
int timeSincePub = 0; //Seconds
int pubFrequencySec = 5; //Seconds

//Variables for time-of-the-day tracking for led light bulb manual mode
string localTime = ""; //HHmm
string ledScedStartTime = ""; //HHmm
string ledScedEndTime = ""; //HHmm

//Variables for sensor readings
int moistureLevel = 0;
int temperatureLevel = 0;
int lightLevel = 0;

bool automaticMode = true;   //Switch between automatic and manual mode
bool isScedOnMode = false;    //Switch between light on manual mode and light off manual mode
bool inactive = false;        //Toggle lock mode
bool lockMode = false;        //Screen lock tracker

void displayLCDmessage(char* message, uint16_t textColor, const GFXfont* font, boolean centerAlign, boolean clearPrevLCD, int Y_Cord_Start_Pos = Y_Cord_Start_Pos);

void setup(){

  for (int i = 0; i < queueSize; i++){
    lightQueue[i] = 0;
    temperatureQueue[i] = 0;
    moistureQueue[i] = 0;
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
  //Set the threshold values based on the current preset
  currentPreset = currentIndex;
  switch (currentPreset) {
    case 0:
      // Desert
      maxTemp = 50;
      moistureLowThreshold = 1;
      moistureHighThreshold = 2;
      lightLowThreshold = 6;
      lightHighThreshold = 9;
      break;

    case 1:
      // Tundra
      maxTemp = 12;
      moistureLowThreshold = 4;
      moistureHighThreshold = 6;
      lightLowThreshold = 3;
      lightHighThreshold = 7;
      break;

    case 2:
      // Standard/Grassland
      maxTemp = 30;
      moistureLowThreshold = 3;
      moistureHighThreshold = 8;
      lightLowThreshold = 3;
      lightHighThreshold = 8;
      break;

    case 3:
      // Tropical
      maxTemp = 35;
      moistureLowThreshold = 6;
      moistureHighThreshold = 9;
      lightLowThreshold = 5;
      lightHighThreshold = 9;
      break;

    case 4:
      // Taiga
      maxTemp = 25;
      moistureLowThreshold = 5;
      moistureHighThreshold = 8;
      lightLowThreshold = 3;
      lightHighThreshold = 6;
      break;
  }

  tft.setFreeFont(NULL);
  tft.fillScreen(primaryColor[currentIndex]);
  tft.fillRect(10,10,300,220, secondaryColor[currentIndex]);
  tft.fillRect(10,83,300,10, primaryColor[currentIndex]);
  tft.fillRect(10,156,300,10, primaryColor[currentIndex]);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.drawString("Moisture",40,37);
  tft.drawString("Light",40,115);
  tft.drawString("Temp",40,187);
}

//Function to add a value to a queue and calculate the average value
long addValueToQueueAndCalculateAverage(int value, int* queue, int& index) {
  //Add the value to the queue

  for(int i=0;i<335;i++)queue[i]=queue[i+1];

  queue[335] = value;

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

void drawLockScreen() {

  if(lockMode != inactive){
    lockMode = inactive;
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
    tft.setTextColor(TFT_RED);
    tft.setCursor(125, 200);
    tft.print("Locked");
  }
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
      onlineMode = true;
      return;
    }
    if (digitalRead(WIO_5S_PRESS) == LOW && selectedMode == 2){
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

  displayLCDmessage("(Re) Connecting To WiFi...", tft.color565(0, 0, 0), FSSB9, true, true, 50);
  displayLCDmessage("(Connection Keeps Failing? Fix:", tft.color565(0, 0, 0), FSS9, true, false);
  displayLCDmessage("1. WiFi Turned Off", tft.color565(0, 0, 0), FSS9, true, false);
  displayLCDmessage("2. Missing Code WiFi Config", tft.color565(0, 0, 0), FSS9, true, false);
  displayLCDmessage("3. Faulty AP Point Config)", tft.color565(0, 0, 0), FSS9, true, false);
  displayLCDmessage("Alt 2 & 3. Needs Manual Wio Restart", TFT_BLACK, FSS9, true, false);
  displayLCDmessage("After Reconfiguration", TFT_BLACK, FSS9, true, false);
  delay(1500);

  WiFi.begin(SSID, PASS);

  if (WiFi.status() != WL_CONNECTED){

    displayLCDmessage("Failed To Connect To WiFi", tft.color565(0, 0, 0), FSSB9, true, true, 60);
    displayLCDmessage("Retry:", tft.color565(0, 0, 0), FSS9, true, false);
    displayLCDmessage("Press (Top Left Button)", tft.color565(0, 0, 0), FSS9, true, false);
    displayLCDmessage("Return Home:", tft.color565(0, 0, 0), FSS9, true, false);
    displayLCDmessage("Press (Top Right Button)", tft.color565(0, 0, 0), FSS9, true, false);
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

  displayLCDmessage("WiFi Connected!", tft.color565(0, 0, 0), FSSB9, true, true, 110);
  delay(3000);
  wifiIsConnected = true;
}

void setupMqtt(){

  mqttClient = new PubSubClient(*wioClient);
  mqttClient->setServer(brokerIp, port);
  mqttClient->setCallback(handleSubMessage);
}

void connectMqtt(){

  doPub = false;
  displayLCDmessage("(Re) Connecting to MQTT Broker...", tft.color565(0, 0, 0), FSSB9, true, true, 60);
  displayLCDmessage("(Connection Keeps Failing? Fix:", TFT_BLACK, FSS9, true, false);
  displayLCDmessage("1. Broker Is Not Running)", TFT_BLACK, FSS9, true, false);
  displayLCDmessage("(Screen Static For T>10s? Fix:", TFT_BLACK, FSS9, true, false);
  displayLCDmessage("2. Faulty BrokerIP &&/|| WiFi Loss)", TFT_BLACK, FSS9, true, false);
  displayLCDmessage("Alt 2. Needs Manual Wio Restart", TFT_BLACK, FSS9, true, false);
  displayLCDmessage("After Reconfiguration", TFT_BLACK, FSS9, true, false);
  delay(4000);

  if (mqttClient->connect(clientName)){
    displayLCDmessage("Connected To MQTT Broker!", tft.color565(0, 0, 0), FSSB9, true, true, 110);
    brokerIsConnected = true;
    subscribeMqtt();
    delay (3000);
  } else {
    displayLCDmessage("Failed To Connect To MQTT Broker", tft.color565(0, 0, 0), FSSB9, true, true, 70);
    displayLCDmessage("Retry:", tft.color565(0, 0, 0), FSS9, true, false);
    displayLCDmessage("Press (Top Left Button)", tft.color565(0, 0, 0), FSS9, true, false);
    displayLCDmessage("Return Home:", tft.color565(0, 0, 0), FSS9, true, false);
    displayLCDmessage("Press (Top Right Button)", tft.color565(0, 0, 0), FSS9, true, false);
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

void publishMqtt(){

  for (int i = 0; i < 21; i++){
    string packet = string(1, ((char) 97 + i));
    for (int j = i * 16; j < (i + 1) * 16; j++){
      int mappedLight = map(lightQueue[j], 0, 1300, 0, 100);
      int mappedMoisture = 0;
      if (moistureQueue[j] >= 0 && moistureQueue[j] < 300) {
        mappedMoisture = map(moistureQueue[j], 0, 299, 0, 30);
      } else if (moistureQueue[j] >= 300 && moistureQueue[j] < 600) {
        mappedMoisture = map(moistureQueue[j], 300, 599, 30, 70);
      } else {
        mappedMoisture = map(moistureQueue[j], 600, 1023, 70, 100);
      }
      packet += to_string(mappedLight) + "," + to_string(temperatureQueue[j]) + "," + to_string(mappedMoisture) + ",";
    }

    const char* charpacket = packet.c_str();
    mqttClient->publish(pubTopic, charpacket);
  }
}

void subscribeMqtt(){

  mqttClient->subscribe(subTopic);
}

void displayLCDmessage(char* message, uint16_t textColor, const GFXfont* font, boolean centerAlign, boolean clearPrevLCD, int Y_Cord_Start_Pos){

  ::Y_Cord_Start_Pos = Y_Cord_Start_Pos;

  if (clearPrevLCD){
    tft.fillScreen(tft.color565(180,238,180));
    text_Y_Margin_Offset = 0;
  }

  tft.setTextColor(textColor);
  tft.setFreeFont(font);
  tft.drawString(message, centerAlign ? 160 - (tft.textWidth(message) / 2) : 0, Y_Cord_Start_Pos + text_Y_Margin_Offset);
  text_Y_Margin_Offset += tft.fontHeight();
}

void handleSubMessage(char* topic, byte* payload, unsigned int length){

  if (isdigit((char) payload[0])){
    localTime = "";
    for (int i = 0; i < length; i++){
      localTime += (char) payload[i];
    }
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
            ledScedStartTime = msg;
            isSecondEncounter = false;
          } else {
            ledScedEndTime = msg;
          }
        }
        msg.clear();
        continue;
      }

      msg += symbol;
    }

    int commandVal = commands.at(commandKey);

    if (commandVal < 5){
      timeSincePub = 0;
      doPub = true;
    }

    switch(commandVal){

      case 1:
      pubFrequencySec = 5;
      break;

      case 2:
      pubFrequencySec = 60;
      break;

      case 3:
      pubFrequencySec = 300;
      break;

      case 4:
      pubFrequencySec = 1800;
      break;

      case 5:
      doPub = false;
      break;

      case 6:
      isScedOnMode = true;
      break;

      case 7:
      isScedOnMode = false;
      break;

      case 8:
      automaticMode = true;
      break;

      case 9:
      automaticMode = false;
      break;

      case 10:
      inactive = true;
      modeIsSetup = false;
      break;

      case 11:
      inactive = false;
      modeIsSetup = false;
      break;
    }
  }
}

void modes() {

  //Automatic mode
  if(automaticMode){

    int range = map(lightLevel, 0, 1300, 0, 10);
    if(range < lightLowThreshold){
      digitalWrite(ledBulbPin, HIGH); //Turn on LED bulb
    } else {
      digitalWrite(ledBulbPin, LOW); //Turn off LED bulb
    }

  } else {
    
    //Light on manual mode
    if(isScedOnMode){

      if (localTime >= ledScedStartTime && localTime < ledScedEndTime){
        digitalWrite(ledBulbPin, HIGH); //Turn on LED bulb
      } else {
        digitalWrite(ledBulbPin, LOW); //Turn off LED bulb
      }

    } else {
      //Light off manual mode
      if (localTime >= ledScedStartTime && localTime < ledScedEndTime){
        digitalWrite(ledBulbPin, LOW); //Turn off LED bulb
      } else {
        digitalWrite(ledBulbPin, HIGH); //Turn on LED bulb
      }
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

  if(!inactive){

    moistureLevel = analogRead(moisturePin);
    temperatureLevel = dht.readTemperature();
    lightLevel = analogRead(WIO_LIGHT);

    if (isTestLight) {
      testLight();
    } else {
      testMoisture();
    }
    
    testTemperature();

    if (digitalRead(BUTTON_3) == LOW) {
      delay(200);
      isTestLight = !isTestLight; //Toggle to change mode of RGB stick
      delay(200);
    }

    modes();

    if (digitalRead(BUTTON_2) == LOW) {
      buzzerOn = !buzzerOn;
      delay(100);
    }

    //Read the current switch position and update the current preset accordingly
    readSwitchPosition();
    if(currentIndex != currentPreset){
      setupDataDisplay();
    }

    //Displays Values
    drawScreen();
    checkAddToQueue();

    if(onlineMode){
      if(doPub){
        if (timeSincePub == pubFrequencySec){
          timeToPub = true;
          timeSincePub = 0;
        }

        if(timeToPub){
          publishMqtt();
          timeToPub = false;
        }
      }

      timeSincePub++;
      mqttClient->loop();
    }

  } else {

    if(inactive && onlineMode){

    mqttClient->loop();
    }

    drawLockScreen();
  }
}

void drawScreen(){

  unsigned long startTime = millis();
  
  while(millis() < startTime + 500){
      //Wait 500ms before updating values on wio terminal sensor data display
  }

  tft.fillRect(220,22,80,50, primaryColor[currentIndex]);
  tft.fillRect(220,105,80,40, primaryColor[currentIndex]);
  tft.fillRect(220,178,80,40, primaryColor[currentIndex]);
  tft.setFreeFont(NULL);

  //Moisture
  tft.setTextSize(2);
  if (moistureLevel >= 0 && moistureLevel < moistureLowThreshold){           //Dry - red
    tft.setTextColor(TFT_RED);
    tft.drawString("Dry",243,40);
    errorSound();
  } else if(moistureLevel >= moistureLowThreshold && moistureLevel < moistureHighThreshold) {   //Moist - darkcyan
    analogWrite(WIO_BUZZER, 0);
    tft.setTextColor(TFT_DARKCYAN);
    tft.drawString("Moist",232,40);
  } else if(moistureLevel >= moistureHighThreshold && moistureLevel <= 950){    //Wet - blue
    analogWrite(WIO_BUZZER, 0);
    tft.setTextColor(TFT_BLUE);
    tft.drawString("Wet",243,40);
  } else {
    tft.setTextColor(TFT_BLACK);                              //Error - black (values outside range)
    tft.drawString("ERROR",232,40);
  }

  //Light
  int range = map(lightLevel, 0, 1300, 0, 10);                //Map light values to a range of 0-10

  if(range < lightLowThreshold ){
    tft.setTextColor(TFT_RED);
    tft.drawString("Low",245,118);
    errorSound();
  } else if (range > lightHighThreshold ){
    tft.setTextColor(TFT_RED);
    tft.drawString("High",237,118);
    errorSound();
  } else if (range > lightLowThreshold  && range < lightHighThreshold ){
    analogWrite(WIO_BUZZER, 0);
    tft.setTextColor(TFT_DARKGREEN);
    tft.drawString("Good",237,118);
  } else {
    tft.setTextColor(TFT_BLACK);
    tft.drawString("ERROR",232,118);
  }

  //Temperature
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

void readSwitchPosition() {

  if (digitalRead(WIO_5S_RIGHT) == LOW) {
    if(currentIndex + 1 > 4){
      return;
    }
    currentIndex = currentIndex + 1;
  } else if (digitalRead(WIO_5S_LEFT) == LOW) {
    if(currentIndex - 1 < 0){
      return;
    }
    currentIndex = currentIndex - 1;
  }
}

void checkAddToQueue(){

  addToQueueTimer++;
  if (addToQueueTimer >= interval) {
    addToQueueTimer = 0;
    //Add the value to the queue and calculate the average total value of the queue
    int moistureAverage = addValueToQueueAndCalculateAverage(moistureLevel, moistureQueue, moistureIndex);
    //Add the value to the queue and calculate the average total value of the queue
    int lightAverage = addValueToQueueAndCalculateAverage(lightLevel, lightQueue, lightIndex);
    //Add the value to the queue and calculate the average total value of the queue
    int temperatureAverage = addValueToQueueAndCalculateAverage(temperatureLevel, temperatureQueue, temperatureIndex);
  }
}

void testLight(){

  pixels.clear();
  int range = map(lightLevel, 0, 1300, 0, 10);         //Map light values to a range to activate LEDs
  if(range < lightLowThreshold || range > lightHighThreshold ){
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

void testMoisture(){

  pixels.clear();
  int range;
  if (moistureLevel >= 0 && moistureLevel < moistureLowThreshold) {           //Dry - brown
    range = map(moistureLevel, 0, 299, 0, 3);
    for(int i = 0; i < range || (i == 0 && range == 0); i++){
      pixels.setPixelColor(i, pixels.Color(255,0,0));
    }
  } else if (moistureLevel >= moistureLowThreshold && moistureLevel < moistureHighThreshold) {  //Moist - light blue
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

void testTemperature(){

  if(temperatureLevel >= maxTemp){
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void errorSound(){

  const unsigned long buzzerBeep = 400;
  const unsigned long shortPause = 300;
  const int buzzerFrequency = 150;

  static unsigned long startTime = 0;
  static int state = 0;

  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  switch (state){
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