// Posicao Inicial/Final do Robo
void _posicaoInicial() {
  _Servos('B', 90);
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

int pausas2 = 500;
// Rotina 01
void _Rotina_01() {
  // Pegar a garrafa na esteira
  _Servos('G', 0);
  _Servos('B', 180);
  delay(pausas2);
  _OmbroCotoveloPulso(90, 45, 70);
  delay(pausas2);
  _Servos('G', 1);

  // Recolher o braço com a garrafa
  _Servos('C', 10);
  delay(pausas2);
  _OmbroCotoveloPulso(180, 20, 90);
  delay(pausas2);
  _Servos('B', 90);
  delay(pausas2);

  // Leva a garrfa até o descarte liquido
  _BaseOmbroCotovelo(75, 120, 40);
  delay(pausas2);
  _Servos('R', 180);
  delay(2000);
  _BaseRotacao(105, 5);
  delay(pausas2);

  // Leva a garrfa até o descarte Materia
  _Servos('O', 60);
  delay(pausas2);
  _Servos('G', 0);
  delay(pausas2);
  _Servos('G', 1);

  // Posição Inicial
  _posicaoInicial();
}


void _Rotina_02() {
  // Pegar a garrafa na esteira
  _Servos('G', 0);
  _Servos('B', 180);
  delay(pausas2);
  _OmbroCotoveloPulso(90, 45, 70);
  delay(pausas2);
  _Servos('G', 1);

  // Recolher o braço com a garrafa
  _Servos('C', 10);
  delay(pausas2);
  _OmbroCotoveloPulso(180, 20, 90);
  delay(pausas2);
  _Servos('B', 90);
  delay(pausas2);

  // Leva a garrfa até o descarte Material
  _Servos('B', 105);
  delay(pausas2);
  _Servos('O', 60);
  delay(pausas2);
  _Servos('G', 0);
  delay(pausas2);
  _Servos('G', 1);

  // Posição Inicial
  _posicaoInicial();
}





void _Rotina_03() {
  // Pegar a garrafa na esteira
  base.write(178);
  delay(pausas2);

  _Servos('G', 0);
  cotovelo.write(40);
  delay(pausas2);

  pulso.write(80);
  delay(pausas2);

  ombro.write(100);
  delay(pausas2);

  _Servos('G', 1);
  delay(pausas2);

  // Recolher o braço com a garrafa
  cotovelo.write(10);
  delay(pausas2);

  ombro.write(180);
  delay(pausas2);
  cotovelo.write(20);
  delay(pausas2);
  pulso.write(90);
  delay(pausas2);

  // Leva a garrfa até o descarte liquido
  base.write(75);
  delay(pausas2);
  ombro.write(120);
  delay(pausas2);
  cotovelo.write(40);
  delay(pausas2);

  rotacao.write(180);
  delay(2000);

  rotacao.write(5);
  delay(pausas2);
  base.write(105);
  delay(pausas2);

  // Leva a garrfa até o descarte Material
  cotovelo.write(20);
  delay(pausas2);
  ombro.write(60);
  delay(pausas2);

  _Servos('G', 0);
  delay(pausas2);

  _Servos('G', 1);
  delay(pausas2);

  // Posição Inicial
  _posicaoInicial();
}

void _Rotina_04() {
  // Pegar a garrafa na esteira
  base.write(180);
  delay(pausas2);

  _Servos('G', 0);
  cotovelo.write(40);
  delay(pausas2);

  pulso.write(80);
  delay(pausas2);

  ombro.write(100);
  delay(pausas2);

  _Servos('G', 1);
  delay(pausas2);

  // Recolher o braço com a garrafa
  cotovelo.write(10);
  delay(pausas2);

  ombro.write(180);
  delay(pausas2);
  cotovelo.write(20);
  delay(pausas2);
  pulso.write(90);
  delay(pausas2);

  // Leva a garrfa até o descarte Material
  base.write(105);
  delay(pausas2);
  cotovelo.write(20);
  delay(pausas2);
  ombro.write(60);
  delay(pausas2);

  _Servos('G', 0);
  delay(pausas2);

  _Servos('G', 1);

  // Posição Inicial
  _posicaoInicial();
}
