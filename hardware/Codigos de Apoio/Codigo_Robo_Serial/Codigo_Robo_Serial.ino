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
void _OmbroCotovelo(int posicao_ombro, int posicao_cotovelo);
void _CotoveloPulso(int posicao_cotovelo, int posicao_pulso);
void _BaseRotacao(int posicao_base, int posicao_rotacao);
void _OmbroCotoveloPulso(int posicao_ombro, int posicao_cotovelo, int posicao_pulso);
void _posicaoInicial();

// Variaveis
bool bluetooth = false;
bool serial = false;
int posicaoAtual = -1;

const int RxD = 2;
const int TxD = 3;
const int pinBase = 4;
const int pinOmbro = 5;
const int pinCotovelo = 6;
const int pinRotacao = 7;
const int pinPulso = 8;
const int pinGarraF = 9;
const int pinGarraA = 10;
const int luz = 11;
String status_garra = "FECHADA";

// Entradas e Saidas Bluetooth
SoftwareSerial Bluetooth(RxD, TxD);  //RX pino 3, TX pino 2

//===== Variáveis de comunicação
const unsigned int TAMANHO_MAX = 256;  // Aumentado para 256 bytes para suportar as chaves/valores no pool do ArduinoJson

// Outras Funções
void _DisplaySerial(char index, int coordenada);
void _entradaDados(char *articulacao, int *posicao);
void _Status(int envio);

// Rotinas
void _Rotina_01();
void _Rotina_02();
void _Rotina_03();
void _Rotina_04();

void setup() {
  // Inicializa as Seriais
  Serial.begin(38400);
  Bluetooth.begin(38400);

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
  base.write(180);
  cotovelo.write(30);
  ombro.write(170);
  rotacao.write(5);
  pulso.write(93);
  _Servos('G', 1);

  // Primeiros comandos
  _entradaDados('x', -1);  // Chamada de função para bloquear a ultima instrução enviada
  //_Status(1);
  delay(500);
  _DisplaySerial('I', 0);
}

void loop() {
  //_Status(2);
  _Comandos();
}