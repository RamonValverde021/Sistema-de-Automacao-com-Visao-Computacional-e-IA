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

void _rotinaEsteiraPrincipal() {
  // Começa com a liberação de uma garrafa
  if (okLiberaGarrafa) {
    _liberaGarrafa();
    okLiberaGarrafa = false;
  }
  // Prototipo de identificação de garrafa e incrementando na Queue
  _registraGarrafa();  // Registra nava garra na Queue

  // Verifica se existem garrafas registradas na fila através do contador
  if (filaEsteira.count() > 0) {
    // Espia os dados da garrafa que está na frente da fila (a mais antiga)
    Garrafa garrafaAtual = filaEsteira.peek();
    if ((garrafaAtual.tipo > 0) && (garrafaAtual.tipo < 5)) {  // Se for alguma garrafa correta
      // Posiciona a Esteira Separadora
      if (garrafaAtual.tipo == 1) {
        _posicaoEsteiraSeparadora(posicaoCocaCola);
      } else if (garrafaAtual.tipo == 2) {
        _posicaoEsteiraSeparadora(posicaoSprite);
      } else if (garrafaAtual.tipo == 3) {
        _posicaoEsteiraSeparadora(posicaoFantaLaranja);
      } else if (garrafaAtual.tipo == 4) {
        _posicaoEsteiraSeparadora(posicaoFantaUva);
      }

      sensorLimiteEsteira = digitalRead(pin_Sensor_Limite_Esteira);
      if (sensorLimiteEsteira == 1) {
        delay(100);              // Espera 0.1s para a garrafa cair na esteira separadora
        okLiberaGarrafa = true;  // Libera a proxima garrafa na cancela
        // Remove o elemento da frente da fila e libera a memória RAM
        filaEsteira.pop();
        Serial.print("Garrafa processada e removida da fila. Restantes: ");
        Serial.println(filaEsteira.count());
        Serial.println();
      }

    } else {  // Então é erro de produção
      _garrafaErro();
    }
  } else {
    // Se o sensor físico disparar mas o contador for 0, houve erro de sincronia
    Serial.println("Fila de dados está vazia!");
  }
}

void _liberaGarrafa() {
  _desligaEsteira();
  _abreCancela();
  _ligaEsteira();
  int sensor = 0;
  while (sensor == 0) {                        // 0 Enquando a garrafa não passa da cancela
    sensor = digitalRead(pin_Sensor_Cancela);  // Verifica se a garrafa já passou pela cancela
  }
  _desligaEsteira();
  _fechaCancela();
  _ligaEsteira();
}

void _registraGarrafa() {
  delay(200);  // Pequeno atrasdo para andar a esteira e fingir que indeficou nova garrafa
  Garrafa novaGarrafa;
  novaGarrafa.id = ids_garrafa;     // ID simulado para rastreamento
  ids_garrafa++;                    // Incrementa para a proxima garrafa receber um ID diferente
  novaGarrafa.tipo = random(1, 7);  // Tipo de garrafa simulado em um valor randomico entre 1 a 6
  // Adiciona a garrafa ao final da fila
  filaEsteira.push(novaGarrafa);

  Serial.println("Servidor registrou nova garrafa");
  Serial.print("Garrafa ID: ");
  Serial.println(novaGarrafa.id);
  Serial.print(" (Tipo: ");
  if (novaGarrafa.tipo == 1) {
    Serial.println("Coca-Cola");
  } else if (novaGarrafa.tipo == 2) {
    Serial.println("Sprite");
  } else if (novaGarrafa.tipo == 3) {
    Serial.println("Fanta Laranja");
  } else if (novaGarrafa.tipo == 4) {
    Serial.println("Fanta Uva");
  } else if (novaGarrafa.tipo == 5) {
    Serial.println("Garrafa Vazia");
  } else {
    Serial.println("Erro de Produção");
  }
  Serial.print("Total na fila: ");
  Serial.println(filaEsteira.count());  // Mostra quantas garrafas estão na esteira
}

void _garrafaErro() {
  Serial.println("Robô entrando execução para descarte de garrafa.");
  int sensorLazer = 0;
  while (sensorLazer == 0) {                          // Equanto  a garrafa não chegar ao sensor do lazer
    sensorLazer = digitalRead(pin_Receptor_Lazer_D);  // Lê o status do sensor do lazer
  }
  _paraEsteira();                                     // Para a esteira
  _posicaoInicialEsteiraSeparadora();                 // Aproveita e já faz a manutenção da esteira separadora enquanto o robo não pega a garrafa
  while (sensorLazer == 1) {                          // Equanto  a garrafa não sair do sensor do lazer
    sensorLazer = digitalRead(pin_Receptor_Lazer_D);  // Lê o status do sensor do lazer
  }
  _ligaEsteira();  // Liga a esteira de volta
}