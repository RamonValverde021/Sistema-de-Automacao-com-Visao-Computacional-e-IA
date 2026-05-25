void _ligaEsteira() {
  analogWrite(pin_Motor_Esteira_PWM, potenciaEstPrinc);
  digitalWrite(pin_Motor_Esteira_H, 0);
  digitalWrite(pin_Motor_Esteira_AH, 1);
}

void _paraEsteira() {
  analogWrite(pin_Motor_Esteira_PWM, 255);
  digitalWrite(pin_Motor_Esteira_H, 1);
  digitalWrite(pin_Motor_Esteira_AH, 1);
  delay(150);
  analogWrite(pin_Motor_Esteira_PWM, 0);
  digitalWrite(pin_Motor_Esteira_H, 0);
  digitalWrite(pin_Motor_Esteira_AH, 0);
}

void _desligaEsteira() {
  analogWrite(pin_Motor_Esteira_PWM, 0);
  digitalWrite(pin_Motor_Esteira_H, 0);
  digitalWrite(pin_Motor_Esteira_AH, 0);
}

bool okRegistro = true;

void _rotinaEsteiraPrincipal() {
  // Começa com a liberação de uma garrafa
  if (okLiberaGarrafa) {
    okRegistro = true;
    _liberaGarrafa();
    okLiberaGarrafa = false;
  }

  /*
  // Modelo para gerar resultados aleatorios de garrafas para fins de teste
  if (okRegistro) {
    // Prototipo de identificação de garrafa e incrementando na Queue
    _registraGarrafa();  // Registra a primeira garrafa na Queue
    okRegistro = false;
  }
  */

  // Verifica se existem garrafas registradas na fila através do contador
  if (filaEsteira.count() > 0) {
    // Espia os dados da garrafa que está na frente da fila (a mais antiga)
    int garrafaAtual = filaEsteira.peek();
    if ((garrafaAtual > 0) && (garrafaAtual < 5)) {  // Se for alguma garrafa com envase correto
      // Posiciona a Esteira Separadora
      if (garrafaAtual == 1) {
        _posicaoEsteiraSeparadora(posicaoCocaCola);
      } else if (garrafaAtual == 2) {
        _posicaoEsteiraSeparadora(posicaoSprite);
      } else if (garrafaAtual == 3) {
        _posicaoEsteiraSeparadora(posicaoFantaLaranja);
      } else if (garrafaAtual == 4) {
        _posicaoEsteiraSeparadora(posicaoFantaUva);
      }

      sensorLimiteEsteira = digitalRead(pin_Sensor_Limite_Esteira);
      if (sensorLimiteEsteira == 1) {
        delay(100);  // Espera 0.1s para a garrafa cair na esteira separadora
        // Remove o elemento da frente da fila e libera a memória RAM
        filaEsteira.pop();
        Serial.print(F("Garrafa processada e removida da fila. Restantes: "));
        Serial.println(filaEsteira.count());
        Serial.println();
        okLiberaGarrafa = true;  // Libera a proxima garrafa na cancela
      }

    } else {  // Então é erro de produção
      _garrafaErro();
      okLiberaGarrafa = true;  // Libera a proxima garrafa na cancela já que esta não chegará até o sensor do final da esteira
    }
  } else {
    Serial.println(F("Fila de dados está vazia!"));
    //_desligaEsteira();
    okLiberaGarrafa = true;
  }
}

void _liberaGarrafa() {
  _desligaEsteira();
  _abreCancela();
  _ligaEsteira();
  int sensorCancela = 0;
  while (sensorCancela == 0) {                        // 0 Enquando a garrafa não passa da cancela
    sensorCancela = digitalRead(pin_Sensor_Cancela);  // Verifica se a garrafa já passou pela cancela, 1
    _recebeComandos();                                // Sempre onde ouver loop fechado, chamar a função de receber dados
  }
  _desligaEsteira();
  _fechaCancela();
  _ligaEsteira();
}




/*========================================= TESTES =========================================*/
// Função para testes, incremento de garrdas aleatórias na Queue
byte numCocaCola = 0;
byte numSprite = 0;
byte numFantaLaranja = 0;
byte numFantaUva = 0;
byte numGarrafasVazias = 0;  // limite 3 (máximo 3, para quando menor que 4)
byte numErrosProducao = 0;   // limite 8 (máximo 8, para quando menor que 9)

void _registraGarrafa() {
  // 1. CHECAGEM ANTES DE GERAR: O lote de produção acabou?
  if (numCocaCola == 4 && numSprite == 4 && numFantaLaranja == 4 && numFantaUva == 4 && numGarrafasVazias == 3 && numErrosProducao == 8) {

    Serial.println(F("======================================================="));
    Serial.println(F("SUCESSO: Todas as garrafas planejadas foram produzidas!"));
    Serial.println(F("======================================================="));

    _desligaEsteira();
    pause = true;  // Avisa a máquina de estados para pausar o loop principal
    return;        // Aborta a função imediatamente
  }

  byte novaGarrafa = 0;

  // 2. LAÇO DE SORTEIO: Só sai daqui quando achar uma garrafa que ainda tem vaga no lote
  while (novaGarrafa == 0) {
    byte numeroRandomico = random(1, 7);  // Gera de 1 a 6

    if ((numeroRandomico == 1) && (numCocaCola < 5)) {
      novaGarrafa = 1;
      numCocaCola++;
    } else if ((numeroRandomico == 2) && (numSprite < 5)) {
      novaGarrafa = 2;
      numSprite++;
    } else if ((numeroRandomico == 3) && (numFantaLaranja < 5)) {
      novaGarrafa = 3;
      numFantaLaranja++;
    } else if ((numeroRandomico == 4) && (numFantaUva < 5)) {
      novaGarrafa = 4;
      numFantaUva++;
    } else if ((numeroRandomico == 5) && (numGarrafasVazias < 4)) {
      novaGarrafa = 5;
      numGarrafasVazias++;
    } else if ((numeroRandomico == 6) && (numErrosProducao < 9)) {
      novaGarrafa = 6;
      numErrosProducao++;
    }
  }

  // 3. REGISTRO NA FILA (Este bloco só executa se uma garrafa válida foi sorteada)
  filaEsteira.push(novaGarrafa);

  Serial.println(F("Servidor registrou nova garrafa"));
  Serial.print(F("Tipo: "));
  if (novaGarrafa == 1) Serial.println(F("Coca-Cola"));
  else if (novaGarrafa == 2) Serial.println(F("Sprite"));
  else if (novaGarrafa == 3) Serial.println(F("Fanta Laranja"));
  else if (novaGarrafa == 4) Serial.println(F("Fanta Uva"));
  else if (novaGarrafa == 5) Serial.println(F("Garrafa Vazia"));
  else if (novaGarrafa == 6) Serial.println(F("Erro de Produção"));

  Serial.print(F("Total na fila: "));
  Serial.println(filaEsteira.count());
  Serial.println();
}

void _garrafaErro() {
  Serial.println(F("Robô entrando execução para descarte de garrafa."));
  int sensorLazer = 0;
  while (sensorLazer == 0) {                          // Equanto  a garrafa não chegar ao sensor do lazer
    sensorLazer = digitalRead(pin_Receptor_Lazer_D);  // Lê o status do sensor do lazer
    _recebeComandos();                                // Sempre onde ouver loop fechado, chamar a função de receber dados
  }
  _paraEsteira();                                     // Para a esteira
  _posicaoInicialEsteiraSeparadora();                 // Aproveita e já faz a manutenção da esteira separadora enquanto o robo não pega a garrafa
  while (sensorLazer == 1) {                          // Equanto  a garrafa não sair do sensor do lazer
    sensorLazer = digitalRead(pin_Receptor_Lazer_D);  // Lê o status do sensor do lazer
    _recebeComandos();                                // Sempre onde ouver loop fechado, chamar a função de receber dados
  }
  // Remove o elemento da frente da fila e libera a memória RAM
  filaEsteira.pop();
  Serial.print(F("Garrafa processada e removida da fila. Restantes: "));
  Serial.println(filaEsteira.count());
  Serial.println();
}

// Função para testes, leitura os sensores. Interromper todo o fluxo do loop e deixar apenas essa função
// --- VARIÁVEIS GLOBAIS DE ESTADO ANTERIOR ---
// Inicializadas com -1 para garantir que o primeiro estado lido sempre seja printado
int ultimoCancela = -1;
int ultimoLazer = -1;
int ultimoFim = -1;

void _leituraSensores() {
  _ligaEsteira();  // Mantém a esteira ligada (conforme seu fluxo)

  // 1. Leitura dos estados atuais
  int sensorCancela = digitalRead(pin_Sensor_Cancela);
  int sensorLazer = digitalRead(pin_Receptor_Lazer_D);
  int sensorFim = digitalRead(pin_Sensor_Limite_Esteira);

  // 2. Variável para controlar se ALGUM sensor mudou
  bool mudouAlgo = false;

  // Verifica Sensor Cancela
  if (sensorCancela != ultimoCancela) {
    Serial.print(F("Sensor Cancela mudou para: "));
    Serial.println(sensorCancela == 1 ? F("INTERROMPIDO (1)") : F("NORMAL (0)"));
    ultimoCancela = sensorCancela;  // Atualiza o histórico
    mudouAlgo = true;
  }

  // Verifica Sensor Lazer
  if (sensorLazer != ultimoLazer) {
    Serial.print(F("Sensor Lazer mudou para: "));
    Serial.println(sensorLazer == 1 ? F("INTERROMPIDO (1)") : F("NORMAL (0)"));
    ultimoLazer = sensorLazer;  // Atualiza o histórico
    mudouAlgo = true;
  }

  // Verifica Sensor Fim Esteira
  if (sensorFim != ultimoFim) {
    Serial.print(F("Sensor Fim Esteira mudou para: "));
    Serial.println(sensorFim == 1 ? F("INTERROMPIDO (1)") : F("NORMAL (0)"));
    ultimoFim = sensorFim;  // Atualiza o histórico
    mudouAlgo = true;
  }

  // Se algo mudou, coloca uma linha divisória para organizar o monitor serial
  if (mudouAlgo) {
    Serial.println(F("-------------------------"));
  }
}