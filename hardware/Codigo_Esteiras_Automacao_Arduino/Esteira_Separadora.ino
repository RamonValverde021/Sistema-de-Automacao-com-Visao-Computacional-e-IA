int coordenadaAtual = 0;           // Coordenada da esteira separadora
int estado_Atual_Encolder = 0;     // Estado atual do botão
int estado_Anterior_Encolder = 0;  // Estado anterior do botão
int contador = 0;                  // Contagem de cliques do botão
int contagemMovimentacao = 0;      // Flag para contar quantas vezes já chamou a esteira

// Variável para a interrupção saber se deve somar ou subtrair
volatile int direcaoMotor = 1;  // 1 para somar, -1 para subtrair

void _posicaoInicialEsteiraSeparadora() {
  int btn_FimDeCurso = 0;
  // Arranque no motor
  analogWrite(pin_Motor_Separadora_PWM, potenciaEstSep);
  digitalWrite(pin_Motor_Separadora_H, 1);
  digitalWrite(pin_Motor_Separadora_AH, 0);
  delay(75);
  analogWrite(pin_Motor_Separadora_PWM, 150);  // 150
  while (btn_FimDeCurso == 0) {
    btn_FimDeCurso = digitalRead(pin_Chave_Fim_de_Curso_Separadora);
  }
  // Desliga o motor
  _paraMotor();
  // Atualiza a coordena da esteira separadora para 0
  coordenadaAtual = 0;
}

void _posicaoFinalEsteiraSeparadora() {
  _posicaoEsteiraSeparadora(limiteEsteiraSeparadora);
}

void _posicaoEsteiraSeparadora(int novaCoordenada) {
  novaCoordenada = constrain(novaCoordenada, 0, limiteEsteiraSeparadora);
  noInterrupts();
  contador = coordenadaAtual;
  interrupts();
  if (novaCoordenada == coordenadaAtual) return;
  _paraMotor();
  delay(50);
  // Se for mover para frente
  if (novaCoordenada > coordenadaAtual) {
    direcaoMotor = 1;
    digitalWrite(pin_Motor_Separadora_H, 0);
    digitalWrite(pin_Motor_Separadora_AH, 1);

    int copiaContador = coordenadaAtual;
    while (copiaContador < novaCoordenada) {
      noInterrupts();
      copiaContador = contador;
      interrupts();

      // SE ESTIVER PERTO DO ALVO (menos de 15 passos), DIMINUI A VELOCIDADE
      if ((novaCoordenada - copiaContador) <= distanciaFreio) {
        analogWrite(pin_Motor_Separadora_PWM, velocidadeLenta);
      } else {
        analogWrite(pin_Motor_Separadora_PWM, velocidadeMaxima);
      }
      //Serial.print("Coordenada: ");
      //Serial.println(copiaContador);
    }
    coordenadaAtual = copiaContador;

    // Se for mover para trás
  } else if (novaCoordenada < coordenadaAtual) {
    direcaoMotor = -1;
    digitalWrite(pin_Motor_Separadora_H, 1);
    digitalWrite(pin_Motor_Separadora_AH, 0);

    int copiaContador = coordenadaAtual;
    while (copiaContador > novaCoordenada) {
      noInterrupts();
      copiaContador = contador;
      interrupts();

      // SE ESTIVER PERTO DO ALVO (reverso), DIMINUI A VELOCIDADE
      if ((copiaContador - novaCoordenada) <= distanciaFreio) {
        analogWrite(pin_Motor_Separadora_PWM, velocidadeLenta);
      } else {
        analogWrite(pin_Motor_Separadora_PWM, velocidadeMaxima);
      }
      //Serial.print("Coordenada: ");
      //Serial.println(copiaContador);
    }
    coordenadaAtual = copiaContador;
  }

  // ATENÇÃO: Garanta que sua função _paraMotor() use o FREIO ATIVO!
  _paraMotor();

  contagemMovimentacao++;
  if (contagemMovimentacao == 4) {  // 10
    delay(500);
    contagemMovimentacao = 0;
    _posicaoInicialEsteiraSeparadora();
  }
}

// Esta função roda em segundo plano toda vez que o encoder passa por um furo
void _contaEncoder() {
  contador += direcaoMotor;
}

void _paraMotor() {
  // Trava o motor no ponto desejado
  analogWrite(pin_Motor_Separadora_PWM, 255);
  digitalWrite(pin_Motor_Separadora_H, HIGH);
  digitalWrite(pin_Motor_Separadora_AH, HIGH);
  delay(250);
  // Alivia a tensão no motor
  analogWrite(pin_Motor_Separadora_PWM, 0);
  digitalWrite(pin_Motor_Separadora_H, LOW);
  digitalWrite(pin_Motor_Separadora_AH, LOW);
}