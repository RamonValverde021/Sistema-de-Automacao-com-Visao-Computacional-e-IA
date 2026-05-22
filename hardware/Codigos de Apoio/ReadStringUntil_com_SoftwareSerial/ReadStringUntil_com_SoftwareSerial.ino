#include <SoftwareSerial.h>

String comando = "";                   // Inicializa e limpa a String comando
const unsigned int TAMANHO_MAX = 512;  // Define um limite de caracteres para evitar realocações constantes de memôria
const int TX_D = 14;
const int RX_D = 15;
SoftwareSerial Micro2(RX_D, TX_D);
int led = 13;  // Define a porta logica do led

void setup() {
  Serial.begin(38400);  // Inicia a Serial
  Micro2.begin(38400);
  Serial.setTimeout(1000);       // diminui o tempo de espera se '\n' não vier
  comando.reserve(TAMANHO_MAX);  // Reserva um espaço de 512 bytes na memôria para o comando
  pinMode(led, OUTPUT);          // Define a porta da led com Saida
  digitalWrite(led, LOW);        // Seta a porta do led com desligada/baixa

  Micro2.println("Ola");
  Micro2.println("Ola");
  Micro2.println("Ola");
}

void loop() {
  comando = "";                              // Apaga o comando anterior
  comando = Micro2.readStringUntil('\n');    // Le novo comando que chegar na Serial
  if (!comando.length() == 0) {              // Se o comando não estiver vazio
    Serial.println(comando);                 // Imprime o comando
    if (comando.indexOf("Lampada") >= 0) {   // Se encontrar no comando uma string "Lampada"
      digitalWrite(led, !digitalRead(led));  // Inverte o estado lógico do led, se antes estava ligado agora fica desligado, vice e versa
    }
  }
}
