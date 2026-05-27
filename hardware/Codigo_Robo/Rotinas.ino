int pausa_rotina = 150;

// Posicao Inicial/Final do Robo
void _posicaoInicial() {
  _Servos('G', 2);
  delay(200);
  _OmbroCotoveloPulso(180, 30, 93);
  delay(pausa_rotina);
  _BaseRotacao(180, 5);
  delay(pausa_rotina);
  digitalWrite(luz, HIGH);
  delay(1000);
  digitalWrite(luz, LOW);
}

void _Rotina_01() {
  // Pegar a garrafa na esteira
  _Servos('B', 180);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _CotoveloPulso(40, 80);
  delay(pausa_rotina);

  _Servos('O', 100);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Recolher o braço com a garrafa
  _Servos('C', 10);
  delay(pausa_rotina);

  _Servos('R', 80);
  delay(pausa_rotina);

  _Servos('P', 93);
  delay(pausa_rotina);

  _Servos('O', 180);
  delay(pausa_rotina);

  // Leva a garrfa até o descarte liquido
  _Servos('B', 75);
  delay(pausa_rotina);

  _OmbroCotovelo(120, 40);
  delay(pausa_rotina);
  _Servos('G', 2);

  _Servos('R', 180);
  delay(2000);

  _Servos('R', 5);
  delay(pausa_rotina);
  _Servos('G', 1);

  // Leva a garrfa até o descarte Materia
  _Servos('B', 105);
  delay(pausa_rotina);

  _OmbroCotovelo(60, 10);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _Servos('G', 2);
  delay(pausa_rotina);

  // Posição Inicial
  _posicaoInicial();
}

void _Rotina_02() {
  // Pegar a garrafa na esteira
  _Servos('B', 180);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _CotoveloPulso(40, 80);
  delay(pausa_rotina);

  _Servos('O', 100);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Recolher o braço com a garrafa
  _Servos('C', 10);
  delay(pausa_rotina);

  _Servos('R', 80);
  delay(pausa_rotina);

  _Servos('P', 93);
  delay(pausa_rotina);

  _Servos('O', 180);

  // Leva a garrfa até o descarte Materia
  _Servos('B', 105);
  delay(pausa_rotina);

  _OmbroCotovelo(60, 10);
  delay(pausa_rotina);

  _Servos('R', 5);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _Servos('G', 2);
  delay(pausa_rotina);

  // Posição Inicial
  _posicaoInicial();
}