// Definição dos pinos conectados ao ULN2003
const int pinoIN1 = 8;
const int pinoIN2 = 9;
const int pinoIN3 = 10;
const int pinoIN4 = 11;

// Tempo de espera entre os passos (controla a velocidade)
// Menor valor = mais rápido. Se for baixo demais, o motor trava.
int intervalo = 4; 
int voltaCompleta = 600; // 300 passos é a volta completa da caixa de redução

// Matriz com a sequência de ativação (Passo Cheio)
const bool sequencia[4][4] = {
  {HIGH, LOW,  LOW,  HIGH},
  {HIGH, HIGH, LOW,  LOW },
  {LOW,  HIGH, HIGH, LOW },
  {LOW,  LOW,  HIGH, HIGH}
};

void setup() {
  pinMode(pinoIN1, OUTPUT);
  pinMode(pinoIN2, OUTPUT);
  pinMode(pinoIN3, OUTPUT);
  pinMode(pinoIN4, OUTPUT);
}

// Função para aplicar um passo específico da matriz
void aplicarPasso(int linha) {
  digitalWrite(pinoIN1, sequencia[linha][0]);
  digitalWrite(pinoIN2, sequencia[linha][1]);
  digitalWrite(pinoIN3, sequencia[linha][2]);
  digitalWrite(pinoIN4, sequencia[linha][3]);
}

void moverMotor(int passos, int sentido) {
  // sentido 1 = horário, sentido -1 = anti-horário
  for (int i = 0; i < passos; i++) {
    for (int j = 0; j < 4; j++) {
      int passoAtual = (sentido == 1) ? j : (3 - j);
      aplicarPasso(passoAtual);
      delay(intervalo);
    }
  }
}

void desligarBobinas() {
  digitalWrite(pinoIN1, LOW);
  digitalWrite(pinoIN2, LOW);
  digitalWrite(pinoIN3, LOW);
  digitalWrite(pinoIN4, LOW);
}

void loop() {
  // Move 100 passos para um lado
  moverMotor(voltaCompleta, 1);
  
  // Desliga para não esquentar o driver enquanto parado
  desligarBobinas(); 
  delay(1000);

  // Move 100 passos para o outro lado
  moverMotor(voltaCompleta, -1);
  
  desligarBobinas();
  delay(1000);
}