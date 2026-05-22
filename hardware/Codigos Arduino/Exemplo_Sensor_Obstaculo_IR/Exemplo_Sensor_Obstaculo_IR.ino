int sensorPin = 8;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop() {
  int leitura = digitalRead(sensorPin);
  if (leitura == 0){
    Serial.println("Objeto Detectado");
  }
  
  delay(500);
}
