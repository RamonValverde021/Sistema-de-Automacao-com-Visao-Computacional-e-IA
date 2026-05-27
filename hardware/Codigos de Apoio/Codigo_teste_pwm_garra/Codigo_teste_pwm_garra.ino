#include <Servo.h>

Servo ombro;
Servo cotovelo;

const int pinOmbro = 9;
const int pinCotovelo = 10;
const int pinGarraF = 6;
const int pinGarraA = 5;

const int potencia_garra_max = 255;
const int potencia_garra_min = 175;
const int potencia_garra_aperto = 75;

void garra(int posicao);

void setup() {
  pinMode(pinOmbro, INPUT);
  pinMode(pinCotovelo, INPUT);
  ombro.attach(pinOmbro);
  cotovelo.attach(pinCotovelo);

  cotovelo.write(30);
  ombro.write(170);
  delay(2000);

  cotovelo.write(10);
  delay(1000);
  ombro.write(100);
  delay(1000);

  garra(0);
  delay(500);

  garra(1);
  delay(500);

  garra(0);
  delay(500);

  garra(2);
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
}
void garra(int posicao) {
  const int delay_Garra = 150;
  // 1 fechar garra e 0 para abrir
  if (posicao == 1) {
    // Fecha a Garra e mantem apertada
    analogWrite(pinGarraA, 0);
    analogWrite(pinGarraF, potencia_garra_max);
    delay(delay_Garra);
    analogWrite(pinGarraA, 0);
    analogWrite(pinGarraF, potencia_garra_aperto);
  } else if (posicao >= 2) {
    // Fecha a Garra sem manter apertada
    analogWrite(pinGarraA, 0);
    analogWrite(pinGarraF, potencia_garra_max);
    delay(delay_Garra);
    analogWrite(pinGarraA, 0);
    analogWrite(pinGarraF, 0);
  } else {
    // Abre a Garra
    analogWrite(pinGarraA, potencia_garra_min);
    analogWrite(pinGarraF, 0);
    delay(delay_Garra);
    analogWrite(pinGarraA, 0);
    analogWrite(pinGarraF, 0);
  }
}