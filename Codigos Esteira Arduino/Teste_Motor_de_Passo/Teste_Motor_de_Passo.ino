#include <Stepper.h>

const int stepsPerRevolution = 2048;

//Inicializa a biblioteca utilizando as portas de 8 a 11 para ligação ao motor
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  //Determina a velocidade inicial do motor
  myStepper.setSpeed(10);
}

void loop() {
  //Gira o motor no sentido horário a 90 graus
  for (int i = 0; i <= 3; i++) {
    myStepper.step(-512);
    delay(2000);
  }

  //Gira o motor no sentido anti-horário a 120 graus
  for (int i = 0; i <= 2; i++) {
    myStepper.step(682);
    delay(2000);
  }

  //Gira o motor no sentido horário, aumentando a
  //velocidade gradativamente
  for (int i = 10; i <= 60; i = i + 10) {
    myStepper.setSpeed(i);
    myStepper.step(40 * i);
  }
  delay(2000);
}