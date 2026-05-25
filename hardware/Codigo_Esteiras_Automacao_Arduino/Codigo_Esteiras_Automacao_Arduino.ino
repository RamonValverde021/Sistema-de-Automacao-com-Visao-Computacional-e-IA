//==================== Inclusão de Bibliotecas =================//
#include <ArduinoJson.h>
#include <Servo.h>
#include <Queue.h>  // Para funcionar a biblioteca tem que ser a do arquivo .zip, não pode atualizar

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
const byte potenciaEstPrinc = 90;  // 85 Potencia minima = 65
const byte potenciaLazer = 175;

//===== Variáveis Esteira Separadora
const int limiteEsteiraSeparadora = 221;
const byte potenciaEstSep = 235;  // 225 = Potencia maxima com precisão
// Definimos a potência máxima e a potência de aproximação lenta
const int velocidadeMaxima = potenciaEstSep;
const int velocidadeLenta = 135;  // 125 velocidade mínima pro motor não engasgar
const int distanciaFreio = 15;    // 15 (limiteEsteiraSeparadora / 4) - 10

//===== Variáveis Esteira Principal
int sensorLimiteEsteira = 0;

//===== Variáveis Servo Cancela
Servo servoCancela;

//===== Variáveis de comunicação
const unsigned int TAMANHO_MAX = 256;  // Aumentado para 256 bytes para suportar as chaves/valores no pool do ArduinoJson
bool pause = false;                    // Começa rodando ou pausado, dependendo da sua lógica
bool conectado = true;

//===== Variáveis Queue
Queue<int> filaEsteira = Queue<int>(27);  // Queue of max 256 int

//===== Flags
const byte passo = 61;
const byte posicaoFantaUva = 35;
const byte posicaoFantaLaranja = posicaoFantaUva + (passo * 1);
const byte posicaoSprite = posicaoFantaUva + (passo * 2);
const byte posicaoCocaCola = posicaoFantaUva + (passo * 3);
bool okLiberaGarrafa = true;
bool garrafaTriada = false;

//==================== PROTOTIPAÇÃO DE FUNÇÕES ====================//
//===== Funções Esteira Separadora
void _posicaoInicialEsteiraSeparadora();
void _posicaoFinalEsteiraSeparadora();
void _posicaoEsteiraSeparadora(int coordenada);
void _contaEncoder();
void _paraMotor();

//===== Funções Esteira Principal
void _rotinaEsteiraPrincipal();
void _ligaEsteira();
void _desligaEsteira();
void _paraEsteira();
void _liberaGarrafa();
void _abreCancela();
void _fechaCancela();
void _registraGarrafa();
void _garrafaErro();

//===== Funções de comunicação
void _recebeComandos();
void _processaComando(const String& json);
void _realizarHandshake();
void (*resetFunc)(void) = 0;  // Função para criar o auto reset
void _limpaTerminal();

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
  attachInterrupt(digitalPinToInterrupt(pin_Encolder_Motor_Separadora_D), _contaEncoder, RISING);
  servoCancela.attach(pin_Servo_Cancela);

  // Inicializando Sistema
  Serial.println('\n');
  Serial.println(F("======================== INICIANDO AUTOMACAO ========================"));
  Serial.println(F("============================ CORE VISION ============================"));
  Serial.println();
  // Posicionando esteira para inciar operações
  _fechaCancela();                     // Garante que a cencela está fechada
  _posicaoInicialEsteiraSeparadora();  // Posiciona a Esteira Separadora para o ponto inicial
  _ligaEsteira();                      // Ativa a esteira
  delay(2000);                         // Espera a esteira normalizar a posição das garrafinhas
  Serial.println(F("________________ AUTOMAÇÃO INICIALIZADA COM SUCESSO ________________"));

  // Inicializa funções
  //_realizarHandshake(); // Executa o Handshake antes de iniciar o loop principal
}

void loop() {
  // A leitura da serial roda CONSTANTEMENTE, esteja o sistema pausado ou não
  _recebeComandos();
  // A lógica da esteira só roda se NÃO estiver pausado
  if (!pause && conectado) {
    _rotinaEsteiraPrincipal();
  }
}