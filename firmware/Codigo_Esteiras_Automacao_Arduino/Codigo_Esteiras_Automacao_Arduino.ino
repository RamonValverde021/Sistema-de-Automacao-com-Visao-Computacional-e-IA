//==================== Inclusão de Bibliotecas =================//
#include <SoftwareSerial.h>
#include <ArduinoJson.h>  // Versão 7.1.0
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//==================== PINOUT ====================//
// Pinagem Digital
#define pin_Chave_Fim_de_Curso_Separadora 14  // A0 - Limite aonde a garrafinha chega antes de cair da esteira
#define pin_Motor_Separadora_H 13
#define pin_Motor_Separadora_AH 12
#define pin_Motor_Separadora_PWM 11
#define pin_Motor_Esteira_PWM 10
#define pin_Motor_Esteira_H 9
#define pin_Motor_Esteira_AH 8
#define pin_Servo_Cancela 7
#define pin_Sensor_Cancela 6
#define pin_Transmissor_Lazer 5      // Diodo Lazer
#define pin_Receptor_Lazer_D 4       // LDR, leitura digital
#define pin_Sensor_Limite_Esteira 3  // Chave fim de curso da esteira separadora
#define pin_Encolder_Motor_Separadora 2
// Pinagem Analogica
#define pin_Receptor_Lazer_A A1  // LDR, leitura analogica

// Variaveis Display 20x04 --------------------
// Configure o endereço do LCD para 0x27 para um Display de 16 caracteres e 2 linhas.
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Variaveis Servo Cancela --------------------
Servo servoCancela;

//==================== VARIÁVEIS GLOBAIS ====================//
const byte potenciaEstPrinc = 50;
const byte potenciaLazer = 10;

//===== Variaveis Esteira Separadora
const int limiteEsteiraSeparadora = 320;
const byte potenciaEstSep = 205;  // 205 = Potencia maxima com precisão
// Definimos a potência máxima e a potência de aproximação lenta
const int velocidadeMaxima = potenciaEstSep; 
const int velocidadeLenta = 125;  // 125 velocidade mínima pro motor não engasgar
const int distanciaFreio = 15;    // 15 (limiteEsteiraSeparadora / 4) - 10

//==================== FLAGS ====================//
const byte posicaoCocaCola = (limiteEsteiraSeparadora / 4) * 1;
const byte posicaoSprite = (limiteEsteiraSeparadora / 4) * 2;
const byte posicaoFantaLaranja = (limiteEsteiraSeparadora / 4) * 3;
const byte posicaoFantaUva = (limiteEsteiraSeparadora / 4) * 4;

//==================== PROTOTIPAÇÃO DE FUNÇÕES ====================//
void _posicaoInicialEsteiraSeparadora();
void _posicaoEsteiraSeparadora(int coordenada);
void trataEncoder();
void _paraMotor();
void _abreCancela();
void _fechaCancela();

//==================== CODIGO PRINCIPAL ====================//
void setup() {
  Serial.begin(115200);
  // Definindo portas
  pinMode(pin_Sensor_Limite_Esteira, INPUT);
  pinMode(pin_Motor_Separadora_H, OUTPUT);
  pinMode(pin_Motor_Separadora_AH, OUTPUT);
  pinMode(pin_Motor_Separadora_PWM, OUTPUT);
  pinMode(pin_Motor_Esteira_PWM, OUTPUT);
  pinMode(pin_Motor_Esteira_H, OUTPUT);
  pinMode(pin_Motor_Esteira_AH, OUTPUT);
  pinMode(pin_Servo_Cancela, OUTPUT);
  pinMode(pin_Sensor_Cancela, INPUT);
  pinMode(pin_Transmissor_Lazer, OUTPUT);
  pinMode(pin_Receptor_Lazer_D, INPUT);
  pinMode(pin_Chave_Fim_de_Curso_Separadora, INPUT);
  pinMode(pin_Encolder_Motor_Separadora, INPUT_PULLUP);
  // Desligando portas
  digitalWrite(pin_Motor_Separadora_H, LOW);
  digitalWrite(pin_Motor_Separadora_AH, LOW);
  analogWrite(pin_Motor_Separadora_PWM, 0);
  analogWrite(pin_Motor_Esteira_PWM, 0);
  digitalWrite(pin_Motor_Esteira_H, LOW);
  digitalWrite(pin_Motor_Esteira_AH, LOW);
  digitalWrite(pin_Servo_Cancela, LOW);
  digitalWrite(pin_Transmissor_Lazer, LOW);
  // Definindo servo
  servoCancela.attach(pin_Servo_Cancela);
  _fechaCancela();
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
  analogWrite(pin_Transmissor_Lazer, potenciaLazer);

  // Configura a interrupção no pino para detectar a subida (RISING) do sinal
  attachInterrupt(digitalPinToInterrupt(pin_Encolder_Motor_Separadora), trataEncoder, RISING);

  // Inicializa funções

  // Teste esteira
  _posicaoInicialEsteiraSeparadora();
  delay(500);
  _posicaoEsteiraSeparadora(posicaoFantaLaranja);
  delay(1000);

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
}

void loop() {
  // put your main code here, to run repeatedly:
}

/*
lcd.setCursor(0, 0);  // Imprime o texto na Coluna 0 e Linha 0
lcd.print("Ola");
lcd.setCursor(0, 1);  // Imprime o texto na Coluna 0 e Linha 1
lcd.print("Mundo");
*/
