#include <Servo.h>

Servo myservo;  // create Servo object to control a servo

void setup() {
  myservo.attach(9);
  myservo.write(180); // 180 Totalmente avançado - 0 Totalmente recuado
  delay(2000);
}

void loop() {      
  myservo.write(180); // 180 Totalmente avançado
  delay(2000);    
  myservo.write(0);  // 0 Totalmente recuado   
  delay(2000);              
}
