int digitalPin = 8;
int analogPin = A0;

void setup() {
  Serial.begin(9600);
  pinMode(digitalPin, INPUT);
}

void loop() {
  int sensorDigital = digitalRead(digitalPin);
  if (sensorDigital == 0){
    Serial.println("Luz detectada");
  }

  int leitura = analogRead(analogPin);
  int sensorAnalogico = map(leitura, 0, 1023, 255, 0);
  Serial.print("Luminosidade: ");
  Serial.println(sensorAnalogico);

  delay(500);
}
