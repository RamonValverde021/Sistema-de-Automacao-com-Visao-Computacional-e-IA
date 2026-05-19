//==================== Inclusão de Bibliotecas =================//
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <PinChangeInterrupt.h>

//==================== PINOUT ====================//
// Pinagem Digital
#define pin_Chave_Fim_de_Curso_Separadora 14  // A0 - Limite aonde a garrafinha chega antes de cair da esteira
#define pin_Led 17
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
#define pin_Sensor_Limite_Esteira 2  // Chave fim de curso da esteira separadora
#define pin_Encolder_Motor_Separadora_D 3
// Pinagem Analogica
#define pin_Receptor_Lazer_A A1  // LDR, leitura analogica






/*
// Pinagem Analogica
#define pin_Receptor_Lazer_A A1  // LDR, leitura analogica

// Pinagem Digital
#define pin_Chave_Fim_de_Curso_Separadora 14  // A0 - Limite aonde a garrafinha chega antes de cair da esteira
#define pin_Encolder_Motor_Separadora_D 15    // A1 - Amarelo
#define pin_Encolder_Motor_Separadora_A 16    // A2 - Laranja
#define pin_Receptor_Lazer_A 17               // A3 - Laranja - Cabo Branco Grosso

#define pin_Led 13                   // 13 Sinalizador
#define pin_Receptor_Lazer_D 12      // Amarelo - Cabo Branco Grosso _ LDR, leitura digital
#define pin_Transmissor_Lazer 11     // Verde - Cabo Branco Grosso _ Diodo Lazer
#define pin_Sensor_Cancela 10        // Azul - Cabo Preto
#define pin_Servo_Cancela 9          // Branco - Cabo Preto
#define pin_Sensor_Limite_Esteira 8  // Laranja - Cabo Branco Fino _ Chave fim de curso da esteira separadora

#define pin_Motor_Separadora_H 7
#define pin_Motor_Separadora_PWM 6
#define pin_Motor_Separadora_AH 5
#define pin_Motor_Esteira_H 4
#define pin_Motor_Esteira_PWM 3
#define pin_Motor_Esteira_AH 2
*/









// Variaveis Display 20x04 --------------------
// Configure o endereço do LCD para 0x27 para um Display de 16 caracteres e 2 linhas.
LiquidCrystal_I2C lcd(0x27, 20, 4);

//==================== VARIÁVEIS GLOBAIS ====================//
const byte potenciaEstPrinc = 60;  // Potencia minima = 60
const byte potenciaLazer = 10;

//===== Variaveis Esteira Separadora
const int limiteEsteiraSeparadora = 221;
const byte potenciaEstSep = 225;  // 205 = Potencia maxima com precisão
// Definimos a potência máxima e a potência de aproximação lenta
const int velocidadeMaxima = potenciaEstSep;
const int velocidadeLenta = 125;  // 125 velocidade mínima pro motor não engasgar
const int distanciaFreio = 15;    // 15 (limiteEsteiraSeparadora / 4) - 10

// Variaveis Servo Cancela --------------------
Servo servoCancela;

//==================== FLAGS ====================//
const byte passo = 62;
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

//==================== CODIGO PRINCIPAL ====================//
void setup() {
  Serial.begin(38400);
  // Definindo portas
  pinMode(pin_Led, OUTPUT);
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
  digitalWrite(pin_Led, LOW);
  digitalWrite(pin_Motor_Separadora_H, LOW);
  digitalWrite(pin_Motor_Separadora_AH, LOW);
  analogWrite(pin_Motor_Separadora_PWM, 0);
  analogWrite(pin_Motor_Esteira_PWM, 0);
  digitalWrite(pin_Motor_Esteira_H, LOW);
  digitalWrite(pin_Motor_Esteira_AH, LOW);
  analogWrite(pin_Transmissor_Lazer, potenciaLazer);


  // =============================================================
  // CONFIGURAÇÃO NATIVA DA INTERRUPÇÃO NA PORTA A1 (PINO 15)
  // =============================================================
  PCICR |= (1 << PCIE1);    // 1. Ativa o grupo de interrupção 1 (Portas A0 a A5)
  PCMSK1 |= (1 << PCINT9);  // 2. Ativa especificamente o pino A1 (PCINT9)
  interrupts();             // 3. Garante que as interrupções globais estão ligadas







  // Configura a interrupção no pino para detectar a subida (RISING) do sinal
  //attachInterrupt(digitalPinToInterrupt(pin_Encolder_Motor_Separadora_D), _trataEncoder, RISING);
  // Permite usar interrupção em quase qualquer pino (ex: pino 4)
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(pin_Encolder_Motor_Separadora_D), _trataEncoder, RISING);
  //servoCancela.attach(pin_Servo_Cancela);

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
  // Inicializa funções
  //_ligaEsteira();
  _testesSeparadora();
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

int espera = 200;
void _testesSeparadora() {


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


// =============================================================
// FUNÇÃO NATIVA DE INTERRUPÇÃO (Roda quando o estado de A1 muda)
// =============================================================
ISR(PCINT1_vect) {
  // Como o PCINT nativo dispara tanto na SUBIDA quanto na DESCIDA do sinal,
  // nós checamos se o pino está em nível ALTO (HIGH) para simular o "RISING"
  if (digitalRead(pin_Receptor_Lazer_A) == HIGH) {
    
    // =========================================================
    // COLOQUE O SEU CÓDIGO AQUI DENTRO (O que você quer que aconteça)
    // =========================================================
    _trataEncoder(); // Exemplo: se for chamar a função do encoder
    
  }
}