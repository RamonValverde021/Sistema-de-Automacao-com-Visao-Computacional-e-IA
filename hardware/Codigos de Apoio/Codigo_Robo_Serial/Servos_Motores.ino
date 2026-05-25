int espera = 500;
// Função para apontar os servos
void _Servos(char servo, int posicao) {
  if (servo == 'B') {
    base.write(posicao);
    delay(espera);
    Serial.println("Base concluida!");
  }

  if (servo == 'O') {
    ombro.write(posicao);
    delay(espera);
    Serial.println("Ombro concluido!");
  }

  if (servo == 'C') {
    cotovelo.write(posicao);
    delay(espera);
    Serial.println("Cotovelo concluido!");
  }

  if (servo == 'P') {
    pulso.write(posicao);
    delay(espera);
    Serial.println("Pulso concluido!");
  }

  if (servo == 'R') {
    rotacao.write(posicao);
    delay(espera);
    Serial.println("Rotação concluida!");
  }

  int delay_Garra = 150;  // 150
  if (servo == 'G') {
    // 1 garra aberta   0 garra fechada
    if (posicao == 1) {
      // Abre a Garra
      digitalWrite(pinGarraA, LOW);
      digitalWrite(pinGarraF, HIGH);
      delay(delay_Garra);
      digitalWrite(pinGarraA, LOW);
      digitalWrite(pinGarraF, LOW);
      status_garra = "FECHADA";
    } else {
      // Fecha a Garra
      digitalWrite(pinGarraA, HIGH);
      digitalWrite(pinGarraF, LOW);
      delay(delay_Garra);
      digitalWrite(pinGarraA, LOW);
      digitalWrite(pinGarraF, LOW);
      status_garra = "ABERTA";
    }
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