//==================== Inclusão de Bibliotecas =================//
#include <ArduinoJson.h>
#include <Servo.h>
#include <SoftwareSerial.h>

//==================== PINOUT ====================//
//===== Portas Digitais
const int RxD = 2;
const int TxD = 3;
const int pinBase = 4;
const int pinGarraF = 6;
const int pinGarraA = 5;
const int pinRotacao = 7;
const int pinPulso = 8;
const int pinOmbro = 9;
const int pinCotovelo = 10;
const int luz = 11;

//==================== VARIÁVEIS GLOBAIS ====================//
bool bluetooth = false;
bool serial = false;
bool imprimeStatus = false;

// Entradas e Saidas Bluetooth
SoftwareSerial Bluetooth(RxD, TxD);  //RX pino 3, TX pino 2

//===== Variáveis de comunicação
const unsigned int TAMANHO_MAX = 128;  // 128 bytes para suportar as chaves/valores no pool do ArduinoJson
bool pause = false;                    // Começa rodando ou pausado, dependendo da sua lógica
bool conectado = false;

//===== Variáveis Servo Cancela
Servo base;
Servo ombro;
Servo cotovelo;
Servo rotacao;
Servo pulso;

//==================== PROTOTIPAÇÃO DE FUNÇÕES ====================//
//===== Funções Servos
void _Servos(char servo, int posicao);
void _OmbroCotovelo(int posicao_ombro, int posicao_cotovelo);
void _OmbroPulso(int posicao_ombro, int posicao_pulso);
void _CotoveloPulso(int posicao_cotovelo, int posicao_pulso);
void _BaseRotacao(int posicao_base, int posicao_rotacao);
void _OmbroCotoveloPulso(int posicao_ombro, int posicao_cotovelo, int posicao_pulso);
void _BaseOmbroCotovelo(int posicao_base, int posicao_ombro, int posicao_cotovelo);
void _posicaoInicial();

//===== Funções de comunicação
void _recebeComandos();
void _processaComando(const String& json);
void _realizarHandshake();
void (*resetFunc)(void) = 0;  // Função para criar o auto reset
void _limpaTerminal();

//===== Funções Rotinas
void _posicaoInicial();
void _Rotina_01();
void _Rotina_02();

//==================== CODIGO PRINCIPAL ====================//
void setup() {
  // Inicializa as Seriais
  Serial.begin(38400);
  Bluetooth.begin(38400);

  // Definindo portas
  pinMode(pinGarraA, OUTPUT);
  pinMode(pinGarraF, OUTPUT);
  pinMode(luz, OUTPUT);
  digitalWrite(luz, LOW);

  // Inicializar os servos motores
  pinMode(pinBase, INPUT);
  pinMode(pinOmbro, INPUT);
  pinMode(pinCotovelo, INPUT);
  pinMode(pinRotacao, INPUT);
  pinMode(pinPulso, INPUT);

  // Ativando Servos
  base.attach(pinBase);
  ombro.attach(pinOmbro);
  cotovelo.attach(pinCotovelo);
  rotacao.attach(pinRotacao);
  pulso.attach(pinPulso);
  delay(100);

  // Posicionando servos na posição inicial
  cotovelo.write(30);
  pulso.write(93);
  ombro.write(180);
  rotacao.write(5);
  base.write(180);
  _Servos('G', 0);
  delay(200);
  _Servos('G', 2);
  delay(200);
  digitalWrite(luz, HIGH);
  delay(1000);
  digitalWrite(luz, LOW);

  //Descansa os servos para não sobrecarregar a Shield de potência
  base.detach();
  ombro.detach();
  cotovelo.detach();
  rotacao.detach();
  pulso.detach();

  // Aguarda a comunicação com o Servidor Python
  _realizarHandshake();  // Executa o Handshake antes de iniciar o loop principal

  delay(500);
  _DisplaySerial('I', 0);
}

void loop() {
  _recebeComandos();
}