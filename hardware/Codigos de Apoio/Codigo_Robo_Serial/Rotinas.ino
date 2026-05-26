// Posicao Inicial/Final do Robo
void _posicaoInicial() {
  _Servos('B', 180);
  _Servos('C', 30);
  _Servos('O', 170);
  _Servos('R', 5);
  _Servos('P', 93);
  _Servos('G', 1);
  _Servos('B', 180);
  digitalWrite(luz, HIGH);
  delay(1000);
  digitalWrite(luz, LOW);
}

int pausa_rotina = 500;

void _Rotina_01() {
  // Pegar a garrafa na esteira
  base.write(180);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  cotovelo.write(40);
  delay(pausa_rotina);

  pulso.write(80);
  delay(pausa_rotina);

  ombro.write(100);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Recolher o braço com a garrafa
  cotovelo.write(10);
  delay(pausa_rotina);

  pulso.write(93);
  delay(pausa_rotina);

  ombro.write(180);
  delay(pausa_rotina);

  cotovelo.write(30);
  delay(pausa_rotina);

  // Leva a garrfa até o descarte liquido
  base.write(75);
  delay(pausa_rotina);

  ombro.write(120);
  delay(pausa_rotina);

  cotovelo.write(40);
  delay(pausa_rotina);

  rotacao.write(180);
  delay(2000);

  rotacao.write(5);
  delay(pausa_rotina);

  // Leva a garrfa até o descarte Material
  base.write(105);
  delay(pausa_rotina);

  cotovelo.write(20);
  delay(pausa_rotina);

  ombro.write(60);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Posição Inicial
  _posicaoInicial();
}

void _Rotina_02() {
  // Pegar a garrafa na esteira
  base.write(180);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  cotovelo.write(40);
  delay(pausa_rotina);

  pulso.write(80);
  delay(pausa_rotina);

  ombro.write(100);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Recolher o braço com a garrafa
  cotovelo.write(10);
  delay(pausa_rotina);

  pulso.write(93);
  delay(pausa_rotina);

  ombro.write(180);
  delay(pausa_rotina);

  cotovelo.write(30);
  delay(pausa_rotina);

  // Leva a garrfa até o descarte Material
  base.write(105);
  delay(pausa_rotina);

  cotovelo.write(20);
  delay(pausa_rotina);

  ombro.write(60);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Posição Inicial
  _posicaoInicial();
}

// Rotina 01 com suavidade
void _Rotina_03() {
  // Pegar a garrafa na esteira
  _Servos('B', 180);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _OmbroCotoveloPulso(100, 40, 80);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Recolher o braço com a garrafa
  _Servos('C', 10);
  delay(pausa_rotina);

  _OmbroPulso(180, 93);
  delay(pausa_rotina);

  _Servos('C', 30);
  delay(pausa_rotina);

  // Leva a garrfa até o descarte liquido
  _Servos('B', 75);
  delay(pausa_rotina);

  _OmbroCotovelo(120, 40);
  delay(pausa_rotina);

  _Servos('R', 180);
  delay(2000);

  // Leva a garrfa até o descarte Materia
  _BaseRotacao(105, 5);
  delay(pausa_rotina);

  _OmbroCotovelo(60, 20);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Posição Inicial
  _posicaoInicial();
}

// Rotina 02 com suavidade
void _Rotina_04() {
  // Pegar a garrafa na esteira
  _Servos('B', 180);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _OmbroCotoveloPulso(100, 40, 80);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Recolher o braço com a garrafa
  _Servos('C', 10);
  delay(pausa_rotina);

  _OmbroPulso(180, 93);
  delay(pausa_rotina);

  _Servos('C', 30);
  delay(pausa_rotina);

  // Leva a garrfa até o descarte Materia
  _Servos('B', 180);
  delay(pausa_rotina);

  _OmbroCotovelo(60, 20);
  delay(pausa_rotina);

  _Servos('G', 0);
  delay(pausa_rotina);

  _Servos('G', 1);
  delay(pausa_rotina);

  // Posição Inicial
  _posicaoInicial();
}