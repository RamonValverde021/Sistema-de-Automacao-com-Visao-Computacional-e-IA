// Bibliotecas
#include <Servo.h>

// Variaveis biblioteca do servo
Servo base;
Servo ombro;
Servo cotovelo;
Servo rotacao;
Servo pulso;
void _Servos(char servo, int posicao);
void _BaseOmbroCotovelo(int posicao_base, int posicao_ombro, int posicao_cotovelo);
void _OmbroCotovelo(int posicao_ombro, int posicao_cotovelo);
void _OmbroPulso(int posicao_ombro, int posicao_pulso);
void _CotoveloPulso(int posicao_cotovelo, int posicao_pulso);
void _BaseRotacao(int posicao_base, int posicao_rotacao);
void _OmbroCotoveloPulso(int posicao_ombro, int posicao_cotovelo, int posicao_pulso);
void _posicaoInicial();

// Variaveis
const int pinBase = 4;
const int pinOmbro = 5;
const int pinCotovelo = 6;
const int pinRotacao = 7;
const int pinPulso = 8;
const int pinGarraA = 9;
const int pinGarraF = 10;
const int luz = 11;

// Rotinas
void _Rotina_01();
void _Rotina_02();

void setup() {
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

  _posicaoInicial();
  /*base.write(90);
  cotovelo.write(30);
  ombro.write(170);
  rotacao.write(5);
  pulso.write(93);
  _Servos('G', 1);*/
  delay(2000);
}

void loop() {
  //_Rotina_01(); // Pega a garrafa, descarta o liquido e depois a garrafa
  //_Rotina_02();

  _Rotina_03();

  //Descansa os servos
  base.detach();
  ombro.detach();
  cotovelo.detach();
  rotacao.detach();
  pulso.detach();
  delay(10000);  // Espera 10 segundos

  //Reativa os servos
  base.attach(pinBase);
  ombro.attach(pinOmbro);
  cotovelo.attach(pinCotovelo);
  rotacao.attach(pinRotacao);
  pulso.attach(pinPulso);
}