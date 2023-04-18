const int moisturePin = A0;
const int temperaturePin = A1;

void setup(){
  pinMode(moisturePin, INPUT);
  pinMode(temperaturePin, INPUT);
  pinMode(WIO_LIGHT, INPUT);
  Serial.begin(9600);
}

void loop(){
  int moistureLevel = analogRead(moisturePin);
  int temperatureLevel = analogRead(temperaturePin);
  int lightLevel = analogRead(WIO_LIGHT);
}