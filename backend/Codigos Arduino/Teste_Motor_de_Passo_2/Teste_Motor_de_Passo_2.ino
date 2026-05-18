/*
  --- ACIONAR MOTOR DE PASSO COM DRIVE ULN2003   ---
  ==================================================
  == BLOG DA ROBOTICA - www.blogdarobotica.com ==
  ==================================================
  Autora: Carol Correia Viana
  E-mail: contato@blogdarobotica.com
  Facebook: facebook.com/blogdarobotica
  Instagram:@blogdarobotica
  YouTube: youtube.com/user/blogdarobotica
  ==================================================
*/

#include <Stepper.h> //Biblioteca do motor
const int passos = 2048; //Número de passos para uma volta
char sentido; //Variável para armazenar o caractere digitado pelo usuário

Stepper myStepper(passos, 8, 10, 9, 11); //Inicia o motor utilizando os pinos 8, 10, 9, 11

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial
}

void loop() {
  if (Serial.available()) { // Se a serial receber algum caractere
    sentido = Serial.read(); // Lê o caractere e guarda na variável sentido
    Serial.println(sentido);// Imprime na serial o conteúdo de sentido
  }

  switch (sentido) {
    case 'A':
      horario(); // Chama a função para acionamento do motor no sentido horário
      break;
    case 'B':
      antihorario(); // Chama a função para acionamento do motor no sentido anti-horário
      break;
  }
}

// Função para acionamento do motor no sentido horário
void horario() { // A
  Serial.println("Girando no sentido Horário!");
  myStepper.setSpeed(10); // Velocidade de 10 rpm
  myStepper.step(passos); // Move 2048 passos, correspondentes a uma volta
}

// Função para acionamento do motor no sentido anti-horário
void antihorario() { // B
  Serial.println("Girando no sentido Anti-horário!");
  myStepper.setSpeed(10); // Velocidade de 10 rpm
  myStepper.step(-passos); // Move 2048 passos, correspondentes a uma volta
}