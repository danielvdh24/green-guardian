const int moisturePin = A0;
const int lightPin = A1;
const int temperaturePin = A2;

void setup(){
  Serial.begin(9600);
}

void loop(){
  int lightLevel = analogRead(lightPin);
}