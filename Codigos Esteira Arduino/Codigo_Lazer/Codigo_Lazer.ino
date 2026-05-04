int lazer = 10; // Pino PWM
void setup() {
  pinMode(lazer, OUTPUT);
}

void loop() {
  analogWrite(lazer, 75);  
  delay(1000);                     
  analogWrite(lazer, 0);   
  delay(1000);               
}
