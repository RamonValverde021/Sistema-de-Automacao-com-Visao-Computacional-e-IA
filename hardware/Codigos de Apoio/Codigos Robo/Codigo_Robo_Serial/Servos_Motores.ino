int delay_Base = 10; // 10
int delay_Ombro = 4; // 4
int delay_Cotovelo = 4; // 4
int delay_Pulso = 4; // 4
int delay_Rotacao = 10; // 10
int delay_Garra = 150; // 150

// Função para apontar os servos
void _Servos(char servo, int posicao) {
  if (servo == 'B') {
    int base_atual = base.read();
    if (posicao > base_atual) {
      for (int c = base_atual; c < posicao; c++) {
        base.write(c);
        delay(delay_Base);
      }
    } else {
      for (int c = base_atual; c > posicao; c--) {
        base.write(c);
        delay(delay_Base);
      }
    }
    Serial.println("Base concluida!");
  }

  if (servo == 'O') {
    int ombro_atual = ombro.read();
    if (posicao > ombro_atual) {
      for (int c = ombro_atual; c < posicao; c++) {
        ombro.write(c);
        delay(delay_Ombro);
      }
    } else {
      for (int c = ombro_atual; c > posicao; c--) {
        ombro.write(c);
        delay(delay_Ombro);
      }
    }
    Serial.println("Ombro concluido!");
  }

  if (servo == 'C') {
    int cotovelo_atual = cotovelo.read();
    if (posicao > cotovelo_atual) {
      for (int c = cotovelo_atual; c < posicao; c++) {
        cotovelo.write(c);
        delay(delay_Cotovelo);
      }
    } else {
      for (int c = cotovelo_atual; c > posicao; c--) {
        cotovelo.write(c);
        delay(delay_Cotovelo);
      }
    }
    Serial.println("Cotovelo concluido!");
  }

  if (servo == 'P') {
    int pulso_atual = pulso.read();
    if (posicao > pulso_atual) {
      for (int c = pulso_atual; c < posicao; c++) {
        pulso.write(c);
        delay(delay_Pulso);
      }
    } else {
      for (int c = pulso_atual; c > posicao; c--) {
        pulso.write(c);
        delay(delay_Pulso);
      }
    }
    Serial.println("Pulso concluido!");
  }

  if (servo == 'R') {
    int rotacao_atual = rotacao.read();
    if (posicao > rotacao_atual) {
      for (int c = rotacao_atual; c < posicao; c++) {
        rotacao.write(c);
        delay(delay_Rotacao);
      }
    } else {
      for (int c = rotacao_atual; c > posicao; c--) {
        rotacao.write(c);
        delay(delay_Rotacao);
      }
    }
    Serial.println("Rotação concluida!");
  }

  if (servo == 'G') {
    // 1 garra aberta   0 garra fechada
    int potencia = 255;
    if (posicao == 1) {
      // Abre a Garra
      digitalWrite(pinGarraA, LOW);
      analogWrite(pinGarraF, potencia);
      delay(delay_Garra);
      status_garra = "FECHADA";
    } else {
      // Fecha a Garra
      analogWrite(pinGarraA, potencia);
      digitalWrite(pinGarraF, LOW);
      delay(delay_Garra);
      status_garra = "ABERTA";
    }
    // Para o motor
    digitalWrite(pinGarraA, HIGH);
    digitalWrite(pinGarraF, HIGH);
    delay(500);
    digitalWrite(pinGarraA, LOW);
    digitalWrite(pinGarraF, LOW);
    Serial.println("Garra concluida!");
  }
}

// Sequencia de movimentos para posicionar o motor inicialmente
void _posicaoInicial() {
  _Servos('B', 90);
  _Servos('C', 30);
  _Servos('O', 170);
  _Servos('R', 5);
  _Servos('P', 93);
  _Servos('G', 1);
  digitalWrite(luz, HIGH);
  delay(2000);
  digitalWrite(luz, LOW);
}