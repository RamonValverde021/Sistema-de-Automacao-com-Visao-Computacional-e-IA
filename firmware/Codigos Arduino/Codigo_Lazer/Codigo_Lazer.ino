int lazer = 10;  // Pino PWM
const byte potenciaLazer = 10;
void setup() {
  pinMode(lazer, OUTPUT);
  analogWrite(lazer, potenciaLazer);
  delay(1000);
}

void loop() {
  for (int c = 0; c < 255; c++) {
    analogWrite(lazer, c);
    delay(5);
  }
  delay(1000);  
  for (int c = 255; c > 0; c--) {
    analogWrite(lazer, c);
    delay(5);
  }
  delay(1000);
}
