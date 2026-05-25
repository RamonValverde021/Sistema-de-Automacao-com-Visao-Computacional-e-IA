// Bibliotecas
#include <Servo.h>
#include <SoftwareSerial.h>

// Variaveis biblioteca do servo
Servo base;
Servo ombro;
Servo cotovelo;
Servo rotacao;
Servo pulso;
void _Servos(char servo, int posicao);

// Definindo pinos de cada servo motor
const int pinBase = 4;
const int pinOmbro = 5;
const int pinCotovelo = 6;
const int pinRotacao = 7;
const int pinPulso = 8;
const int pinGarraA = 9;
const int pinGarraF = 10;
const int luz = 11;
String status_garra = "FECHADA";

// Entradas e Saidas Bluetooth
const int RxD = 2, TxD = 3;
SoftwareSerial Bluetooth(RxD, TxD);  //RX pino 2, TX pino 3

// Outras Funções
void _DisplaySerial(char index, int coordenada);
void _entradaDados(char *articulacao, int *posicao);
void _posicaoInicial();
void _Status(int tipo);

void setup() {
  // Inicializa as Seriais
  Serial.begin(38400);
  Bluetooth.begin(38400);

  pinMode(TxD, OUTPUT);
  pinMode(RxD, INPUT);

  // Define os pinos de entrada
  pinMode(pinGarraA, OUTPUT);
  pinMode(pinGarraF, OUTPUT);
  pinMode(luz, OUTPUT);
  digitalWrite(pinGarraA, LOW);
  digitalWrite(pinGarraF, LOW);
  digitalWrite(luz, LOW);

  // Inicializar os servos motores
  pinMode(pinBase, INPUT);
  pinMode(pinOmbro, INPUT);
  pinMode(pinCotovelo, INPUT);
  pinMode(pinRotacao, INPUT);
  pinMode(pinPulso, INPUT);
  base.attach(pinBase);
  ombro.attach(pinOmbro);
  cotovelo.attach(pinCotovelo);
  rotacao.attach(pinRotacao);
  pulso.attach(pinPulso);
  base.write(90);
  cotovelo.write(30);
  ombro.write(170);
  rotacao.write(5);
  pulso.write(93);
  _Servos('G', 1);
  delay(1000);

  // Inicializa o display serial
  _DisplaySerial('I', 0);
  // Primeiros comandos
  _entradaDados('x', -1);  // Chamada de função para bloquear a ultima instrução enviada
  _Status(1);
  _Status(2);
  delay(1000);
}

void loop() {
  //_Status();
  _Comandos();
}