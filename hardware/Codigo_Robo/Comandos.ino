// Função que envia as coordenadas para acionar os motores
void _Comandos() {
  char articulacao;
  int posicao;
  const int velocidade = 50;

  // Ao chamar a função, é preciso passar os endereços das variáveis onde deseja armazenar os resultados, isso é feito usando o operador '&' antes das variáveis.
  _entradaDados(&articulacao, &posicao);

  if (posicao != -1) {
    if (articulacao == 'A') {
      if (posicaoAtual != posicao) {
        if (posicao == 1) {
          _DisplaySerial('1', 0);
          _Rotina_01();
        } else if (posicao == 2) {
          _DisplaySerial('2', 0);
          _Rotina_02();
        }
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'B') {
      if (posicaoAtual != posicao) {
        _DisplaySerial(articulacao, posicao);
        _Servos('B', posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'O') {
      if (posicaoAtual != posicao) {
        _DisplaySerial(articulacao, posicao);
        _Servos('O', posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'C') {
      if (posicaoAtual != posicao) {
        _DisplaySerial(articulacao, posicao);
        _Servos('C', posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'P') {
      if (posicaoAtual != posicao) {
        _DisplaySerial(articulacao, posicao);
        _Servos('P', posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'R') {
      if (posicaoAtual != posicao) {
        _DisplaySerial(articulacao, posicao);
        _Servos('R', posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'G') {
      if (posicaoAtual != posicao) {
        _DisplaySerial(articulacao, posicao);
        _Servos('G', posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'L') {
      if (posicaoAtual != posicao) {
        posicao == 1 ? digitalWrite(luz, HIGH) : digitalWrite(luz, LOW);
        _DisplaySerial(articulacao, posicao);
      }
      posicaoAtual = posicao;
    } else if (articulacao == 'Z') {
      _DisplaySerial(articulacao, 0);
      _posicaoInicial();
    }
  }
  _entradaDados('x', -1);
}