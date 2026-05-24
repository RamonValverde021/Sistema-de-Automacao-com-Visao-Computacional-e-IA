//==================== Inclusão de Bibliotecas =================//
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <EEPROM.h>

//==================== PINOUT ====================//
//===== Portas Digitais
#define pin_Motor_Separadora_H 13
#define pin_Motor_Separadora_AH 12
#define pin_Motor_Esteira_H 11
#define pin_Motor_Esteira_AH 10
#define pin_Servo_Cancela 9          // Branco - Cabo Preto
#define pin_Sensor_Cancela 8         // Azul - Cabo Preto
#define pin_Sensor_Limite_Esteira 7  // Laranja - Cabo Branco Fino _ Limite aonde a garrafinha chega antes de cair da esteira
#define pin_Motor_Separadora_PWM 6
#define pin_Motor_Esteira_PWM 5
#define pin_Receptor_Lazer_D 4             // Amarelo - Cabo Branco Grosso _ LDR, leitura digital
#define pin_Transmissor_Lazer 3            // Verde - Cabo Branco Grosso _ Diodo Lazer
#define pin_Encolder_Motor_Separadora_D 2  // A1 - Amarelo
//===== Portas Analogicas
#define pin_Encolder_Motor_Separadora_A A0    // Laranja
#define pin_Receptor_Lazer_A A1               // Laranja - Cabo Branco Grosso
#define pin_Chave_Fim_de_Curso_Separadora 16  // A2 - Chave fim de curso da esteira separadora

//==================== VARIÁVEIS GLOBAIS ====================//
const byte potenciaEstPrinc = 68;  // Potencia minima = 65
const byte potenciaLazer = 10;

//===== Variáveis Esteira Separadora
const int limiteEsteiraSeparadora = 221;
const byte potenciaEstSep = 225;  // 205 = Potencia maxima com precisão
// Definimos a potência máxima e a potência de aproximação lenta
const int velocidadeMaxima = potenciaEstSep;
const int velocidadeLenta = 125;  // 125 velocidade mínima pro motor não engasgar
const int distanciaFreio = 15;    // 15 (limiteEsteiraSeparadora / 4) - 10

//===== Variáveis Servo Cancela
Servo servoCancela;

//===== Variáveis de comunicação
bool conectado = false;
const unsigned int TAMANHO_MAX = 512;  // Define um limite de caracteres para evitar realocações constantes de memôria

//===== Variáveis Display
// Configure o endereço do LCD para 0x27 para um Display de 16 caracteres e 2 linhas.
LiquidCrystal_I2C lcd(0x27, 16, 2);

//===== Flags
const byte passo = 61;
const byte posicaoFantaUva = 35;
const byte posicaoFantaLaranja = posicaoFantaUva + (passo * 1);
const byte posicaoSprite = posicaoFantaUva + (passo * 2);
const byte posicaoCocaCola = posicaoFantaUva + (passo * 3);

//==================== PROTOTIPAÇÃO DE FUNÇÕES ====================//
void _ligaEsteira();
void _desligaEsteira();
void _posicaoInicialEsteiraSeparadora();
void _posicaoFinalEsteiraSeparadora();
void _posicaoEsteiraSeparadora(int coordenada);
void _trataEncoder();
void _paraMotor();
void _abreCancela();
void _fechaCancela();

//===== Funções de comunicação
void _recebeComandos();
void _processaComando(const String& json);
bool _keyJSON(JsonVariantConst obj, String chave);
void _realizarHandshake();

//==================== CODIGO PRINCIPAL ====================//
void setup() {
  Serial.begin(38400);
  // Definindo portas
  pinMode(pin_Sensor_Limite_Esteira, INPUT);
  pinMode(pin_Motor_Separadora_H, OUTPUT);
  pinMode(pin_Motor_Separadora_AH, OUTPUT);
  pinMode(pin_Motor_Separadora_PWM, OUTPUT);
  pinMode(pin_Motor_Esteira_PWM, OUTPUT);
  pinMode(pin_Motor_Esteira_H, OUTPUT);
  pinMode(pin_Motor_Esteira_AH, OUTPUT);
  pinMode(pin_Servo_Cancela, INPUT);
  pinMode(pin_Sensor_Cancela, INPUT);
  pinMode(pin_Transmissor_Lazer, OUTPUT);
  pinMode(pin_Receptor_Lazer_D, INPUT);
  pinMode(pin_Chave_Fim_de_Curso_Separadora, INPUT);
  pinMode(pin_Encolder_Motor_Separadora_D, INPUT);
  // Desligando portas
  digitalWrite(pin_Motor_Separadora_H, LOW);
  digitalWrite(pin_Motor_Separadora_AH, LOW);
  analogWrite(pin_Motor_Separadora_PWM, 0);
  analogWrite(pin_Motor_Esteira_PWM, 0);
  digitalWrite(pin_Motor_Esteira_H, LOW);
  digitalWrite(pin_Motor_Esteira_AH, LOW);
  analogWrite(pin_Transmissor_Lazer, potenciaLazer);
  // Configura a interrupção no pino para detectar a subida (RISING) do sinal
  attachInterrupt(digitalPinToInterrupt(pin_Encolder_Motor_Separadora_D), _trataEncoder, RISING);
  servoCancela.attach(pin_Servo_Cancela);
  //_abreCancela();

  /*
  // Inicia o display
  lcd.begin();      // Inicializa o LCD
  lcd.backlight();  // Acenda a luz de fundo
  lcd.noDisplay();
  delay(100);
  lcd.clear();
  lcd.display();
  delay(100);

  // Inicializando Sistema
  Serial.println("   INICIANDO AUTOMACAO  ");
  Serial.println("       VISION BELT      ");
  lcd.setCursor(1, 0);  // Coluna 0 Linha 0
  lcd.print("INICIANDO automacao");
  lcd.setCursor(2, 1);  // Coluna 4 Linha 1
  lcd.print("VISION BELT");
  delay(500);  // 5000
  Serial.println("-----------------------------> Automação Inicializada com Sucesso <-----------------------------");
  lcd.clear();
*/
  // Executa o Handshake antes de iniciar o loop principal
  _realizarHandshake();

  

  // Inicializa funções
  //_ligaEsteira();
  //_testesSeparadora();
}

void loop() {
  // put your main code here, to run repeatedly:
  _recebeComandos();
}

/*
lcd.setCursor(0, 0);  // Imprime o texto na Coluna 0 e Linha 0
lcd.print("Ola");
lcd.setCursor(0, 1);  // Imprime o texto na Coluna 0 e Linha 1
lcd.print("Mundo");
*/


void _testesSeparadora() {
  int espera = 500;

  // Teste esteira
  _posicaoInicialEsteiraSeparadora();
  delay(500);
  _posicaoEsteiraSeparadora(posicaoFantaUva);
  delay(espera);
  _posicaoEsteiraSeparadora(posicaoFantaLaranja);
  delay(espera);
  _posicaoEsteiraSeparadora(posicaoSprite);
  delay(espera);
  _posicaoEsteiraSeparadora(posicaoCocaCola);
  delay(espera);

  _posicaoEsteiraSeparadora(posicaoCocaCola);
  delay(espera);
  _posicaoEsteiraSeparadora(posicaoSprite);
  delay(espera);
  _posicaoEsteiraSeparadora(posicaoFantaLaranja);
  delay(espera);
  _posicaoEsteiraSeparadora(posicaoFantaUva);
  delay(espera);

  _posicaoFinalEsteiraSeparadora();





  /*
  _posicaoEsteiraSeparadora(posicaoCocaCola);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoFantaUva);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoFantaLaranja);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoSprite);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoCocaCola);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoSprite);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoFantaUva);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoFantaLaranja);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoSprite);
  delay(500);
  _posicaoEsteiraSeparadora(posicaoCocaCola);
  delay(500);

  _posicaoEsteiraSeparadora(posicaoFantaLaranja);
  delay(500);
  */
}