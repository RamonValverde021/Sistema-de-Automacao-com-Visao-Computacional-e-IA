int digitalPin = 4;

void setup() {
  Serial.begin(9600);
  pinMode(digitalPin, INPUT);
}

void loop() {
  int sensorDigital = digitalRead(digitalPin);
  if (sensorDigital == 1){
    Serial.println("Objeto Detectado");
  }
  delay(500);
}



/*
void _posicaoEsteiraSeparadora(int novaCoordenada) {
  // Defino um range limite entre 0 a 21
  novaCoordenada = constrain(novaCoordenada, 0, limiteEsteiraSeparadora); //21
  // Atualiza o contador com a posição atual da esteira
  contador = coordenadaAtual;
  // Eleva a potencia de trabalho no motor
  analogWrite(pin_Motor_Separadora_PWM, potenciaEstSep);
  // Se a nova coordenada for maior que a atual
  if (novaCoordenada > coordenadaAtual) {  // 21 > 0
    // Trago as divisorias
    digitalWrite(pin_Motor_Separadora_H, 0);
    digitalWrite(pin_Motor_Separadora_AH, 1);
    while (contador < novaCoordenada) {                                        // 0 < 21
      int estado_Atual_Encolder = digitalRead(pin_Encolder_Motor_Separadora);  // Lê o estado atual do botão
      if (estado_Atual_Encolder != estado_Anterior_Encolder) {                 // Verifica se houve mudança de estado (pressionado ou liberado)
        if (estado_Atual_Encolder == HIGH) {
          contador++;  // Incrementa o contador de cliques
          //delay(5);
          //Serial.print("Conatagem de Pulsos: ");
          //Serial.println(contador);  // Exibe o número de cliques no monitor serial
        }
      }
      estado_Anterior_Encolder = estado_Atual_Encolder;  // Atualiza o estado do botão
    }
    // Atualiza a coordena atual da esteira separadora
    coordenadaAtual = contador;
    // Se a nova coordenada for menor que a atual
  } else if (novaCoordenada < coordenadaAtual) {  // 15 < 21
    // Levo as divisorias
    digitalWrite(pin_Motor_Separadora_H, 1);
    digitalWrite(pin_Motor_Separadora_AH, 0);
    while (contador > novaCoordenada) {                                        // 21 > 15
      int estado_Atual_Encolder = digitalRead(pin_Encolder_Motor_Separadora);  // Lê o estado atual do botão
      if (estado_Atual_Encolder != estado_Anterior_Encolder) {                 // Verifica se houve mudança de estado (pressionado ou liberado)
        if (estado_Atual_Encolder == HIGH) {
          contador--;  // Incrementa o contador de cliques
          //delay(5);
          //Serial.print("Conatagem de Pulsos: ");
          //Serial.println(contador);  // Exibe o número de cliques no monitor serial
        }
      }
      estado_Anterior_Encolder = estado_Atual_Encolder;  // Atualiza o estado do botão
    }
    // Atualiza a coordena atual da esteira separadora
    coordenadaAtual = contador;
  } else {  // Se já estão no lugar
  }
  _paraMotor();

  // Realiza a manutenção das coordenadas
  contagemMovimentacao++;
  if (contagemMovimentacao == 20) { //5
    delay(500);
    contagemMovimentacao = 0;
    _posicaoInicialEsteiraSeparadora();
  }
}
*/