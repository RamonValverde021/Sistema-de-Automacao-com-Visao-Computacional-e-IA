const int delay_Base = 6;
const int delay_Ombro = 6;
const int delay_Cotovelo = 6;
const int delay_Pulso = 6;
const int delay_Rotacao = 3;  // 6
const int delay_Garra = 150;

const int pausas = 250;

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
    delay(pausas);
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
    delay(pausas);
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
    delay(pausas);
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
    delay(pausas);
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
    delay(pausas);
  }

  if (servo == 'G') {
    // 1 garra aberta   0 garra fechada
    int potencia = 255;
    if (posicao == 1) {
      // Abre a Garra
      digitalWrite(pinGarraA, LOW);
      digitalWrite(pinGarraF, HIGH);
      delay(delay_Garra);
    } else {
      // Fecha a Garra
      digitalWrite(pinGarraA, HIGH);
      digitalWrite(pinGarraF, LOW);
      delay(delay_Garra);
      digitalWrite(pinGarraA, LOW);
      digitalWrite(pinGarraF, LOW);
    }
  }
  delay(pausas);
}


//////////////////////// SERVOS SIMULTANEOS ////////////////////////

void _OmbroCotovelo(int posicao_ombro, int posicao_cotovelo) {
  int ombro_atual = 0, cotovelo_atual = 0, o = 0, c = 0;
  while ((posicao_ombro != ombro_atual) || (posicao_cotovelo != cotovelo_atual)) {
    // OMBRO
    ombro_atual = ombro.read();
    o = ombro_atual;
    if (posicao_ombro > ombro_atual) {
      o++;
      ombro.write(o);
    } else if (posicao_ombro < ombro_atual) {
      o--;
      ombro.write(o);
    }
    // COTOVELO
    cotovelo_atual = cotovelo.read();
    c = cotovelo_atual;
    if (posicao_cotovelo > cotovelo_atual) {
      c++;
      cotovelo.write(c);
    } else if (posicao_cotovelo < cotovelo_atual) {
      c--;
      cotovelo.write(c);
    }
    delay(4);
  }
  delay(pausas);
}

void _OmbroPulso(int posicao_ombro, int posicao_pulso) {
  int ombro_atual = 0, pulso_atual = 0, o = 0, p = 0;
  while ((posicao_ombro != ombro_atual) || (posicao_pulso != pulso_atual)) {
    // OMBRO
    ombro_atual = ombro.read();
    o = ombro_atual;
    if (posicao_ombro > ombro_atual) {
      o++;
      ombro.write(o);
    } else if (posicao_ombro < ombro_atual) {
      o--;
      ombro.write(o);
    }
    // PULSO
    pulso_atual = pulso.read();
    p = pulso_atual;
    if (posicao_pulso > pulso_atual) {
      p++;
      pulso.write(p);
    } else if (posicao_pulso < pulso_atual) {
      p--;
      pulso.write(p);
    }
    delay(4);
  }
  delay(pausas);
}

void _CotoveloPulso(int posicao_cotovelo, int posicao_pulso) {
  int cotovelo_atual = 0, pulso_atual = 0, c = 0, p = 0;
  while ((posicao_cotovelo != cotovelo_atual) || (posicao_pulso != pulso_atual)) {
    // COTOVELO
    cotovelo_atual = cotovelo.read();
    c = cotovelo_atual;
    if (posicao_cotovelo > cotovelo_atual) {
      c++;
      cotovelo.write(c);
    } else if (posicao_cotovelo < cotovelo_atual) {
      c--;
      cotovelo.write(c);
    }
    // PULSO
    pulso_atual = pulso.read();
    p = pulso_atual;
    if (posicao_pulso > pulso_atual) {
      p++;
      pulso.write(p);
    } else if (posicao_pulso < pulso_atual) {
      p--;
      pulso.write(p);
    }
    delay(4);
  }
  delay(pausas);
}

void _BaseRotacao(int posicao_base, int posicao_rotacao) {
  int base_atual = 0, rotacao_atual = 0, b = 0, r = 0;
  while ((posicao_base != base_atual) || (posicao_rotacao != rotacao_atual)) {
    // BASE
    base_atual = base.read();
    b = base_atual;
    if (posicao_base > base_atual) {
      b++;
      base.write(b);
    } else if (posicao_base < base_atual) {
      b--;
      base.write(b);
    }
    // ROTACAO
    rotacao_atual = rotacao.read();
    r = rotacao_atual;
    if (posicao_rotacao > rotacao_atual) {
      r++;
      rotacao.write(r);
    } else if (posicao_rotacao < rotacao_atual) {
      r--;
      rotacao.write(r);
    }
    delay(5);
  }
  delay(pausas);
}

void _OmbroCotoveloPulso(int posicao_ombro, int posicao_cotovelo, int posicao_pulso) {
  int ombro_atual = 0, cotovelo_atual = 0, pulso_atual = 0, o = 0, c = 0, p = 0;
  while ((posicao_ombro != ombro_atual) || (posicao_cotovelo != cotovelo_atual) || (posicao_pulso != pulso_atual)) {
    // OMBRO
    ombro_atual = ombro.read();
    o = ombro_atual;
    if (posicao_ombro > ombro_atual) {
      o++;
      ombro.write(o);
    } else if (posicao_ombro < ombro_atual) {
      o--;
      ombro.write(o);
    }
    // COTOVELO
    cotovelo_atual = cotovelo.read();
    c = cotovelo_atual;
    if (posicao_cotovelo > cotovelo_atual) {
      c++;
      cotovelo.write(c);
    } else if (posicao_cotovelo < cotovelo_atual) {
      c--;
      cotovelo.write(c);
    }
    // PULSO
    pulso_atual = pulso.read();
    p = pulso_atual;
    if (posicao_pulso > pulso_atual) {
      p++;
      pulso.write(p);
    } else if (posicao_pulso < pulso_atual) {
      p--;
      pulso.write(p);
    }
    delay(3);
  }
  delay(pausas);
}

void _BaseOmbroCotovelo(int posicao_base, int posicao_ombro, int posicao_cotovelo) {
  int base_atual = 0, ombro_atual = 0, cotovelo_atual = 0, b = 0, o = 0, c = 0;
  while ((posicao_base != base_atual) || (posicao_ombro != ombro_atual) || (posicao_cotovelo != cotovelo_atual)) {
    // BASE
    base_atual = base.read();
    b = base_atual;
    if (posicao_base > base_atual) {
      b++;
      base.write(b);
    } else if (posicao_base < base_atual) {
      b--;
      base.write(b);
    }
    // OMBRO
    ombro_atual = ombro.read();
    o = ombro_atual;
    if (posicao_ombro > ombro_atual) {
      o++;
      ombro.write(o);
    } else if (posicao_ombro < ombro_atual) {
      o--;
      ombro.write(o);
    }
    // COTOVELO
    cotovelo_atual = cotovelo.read();
    c = cotovelo_atual;
    if (posicao_cotovelo > cotovelo_atual) {
      c++;
      cotovelo.write(c);
    } else if (posicao_cotovelo < cotovelo_atual) {
      c--;
      cotovelo.write(c);
    }
    delay(3);
  }
  delay(pausas);
}