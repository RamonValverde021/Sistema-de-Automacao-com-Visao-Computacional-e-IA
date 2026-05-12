int digitalPin = 4;

void setup() {
  Serial.begin(9600);
  pinMode(digitalPin, INPUT);
}

void loop() {
  int sensorDigital = digitalRead(digitalPin);
  if (sensorDigital == 1){
    Serial.println("Objeto Detectado");
  }
  delay(500);
}
