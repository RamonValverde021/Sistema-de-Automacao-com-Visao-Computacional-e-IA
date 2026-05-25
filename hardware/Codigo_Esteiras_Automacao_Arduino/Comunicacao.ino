void _recebeComandos() {
  String comando = "";  // Limite de 58 Bytes por vez
  if (Serial.available() > 0) {
    comando = Serial.readStringUntil('\n');  // Le novo comando que chegar na Serial
    if (!comando.length() == 0) {            // Se o comando não estiver vazio
      comando.trim();
      _processaComando(comando);
      comando = "";  // Limpa para a próxima mensagem
    }
  }
}

void _processaComando(const String& json) {
  // Certifique-se de que TAMANHO_MAX seja de pelo menos 128 bytes
  StaticJsonDocument<TAMANHO_MAX> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.println(F("-------------------------"));
    Serial.println(F("Erro na deserialização"));
    Serial.print(F("Mensagem recebida: "));
    Serial.println(json);
    Serial.print(F("Erro: "));
    Serial.println(error.c_str());
    return;
  }

  Serial.print(F("JSON recebido: "));
  //serializeJsonPretty(doc, Serial);
  serializeJson(doc, Serial);
  Serial.println();

  // Se chegou aqui, o JSON é válido. Vamos tratar os comandos.
  if (doc.containsKey("id") && doc["id"] == "servidor") {

    // 1. Handshake / Conexão
    if (doc.containsKey("conexao") && doc["conexao"] == "ouvindo") {
      conectado = true;
      StaticJsonDocument<128> docConfirma;
      docConfirma["id"] = "arduino";
      docConfirma["conexao"] = "estabelecida";
      serializeJson(docConfirma, Serial);
      Serial.println();
      return;  // Sai da função
    }

    // 2. Controle do Sistema
    if (doc.containsKey("controle")) {
      const char* controle = doc["controle"];
      if (strcmp(controle, "iniciar_esteira") == 0) {
        Serial.println(F("Iniciando o Sistema"));
        pause = false;
        _ligaEsteira();
      } else if (strcmp(controle, "pausar_processamento") == 0) {
        Serial.println(F("Pausando o Sistema"));
        pause = true;
        _desligaEsteira();  // Para o motor imediatamente
      } else if (strcmp(controle, "reiniciar_sistema") == 0) {
        Serial.println(F("Reiniciando o Sistema"));
        _limpaTerminal();
        delay(250);
        resetFunc();
      }
    }

    // 3. Detecção de Garrafas
    if (doc.containsKey("deteccao_de_item")) {
      const char* deteccao_de_item = doc["deteccao_de_item"];
      byte novaGarrafa = 0;
      if (strcmp(deteccao_de_item, "Coca_Cola") == 0) {
        novaGarrafa = 1;
        filaEsteira.push(novaGarrafa);
        Serial.println(F("Adicionando a fila: Coca-Cola"));
      } else if (strcmp(deteccao_de_item, "Sprite") == 0) {
        novaGarrafa = 2;
        filaEsteira.push(novaGarrafa);
        Serial.println(F("Adicionando a fila: Sprite"));
      } else if (strcmp(deteccao_de_item, "Fanta_Laranja") == 0) {
        novaGarrafa = 3;
        filaEsteira.push(novaGarrafa);
        Serial.println(F("Adicionando a fila: Fanta Laranja"));
      } else if (strcmp(deteccao_de_item, "Fanta_Uva") == 0) {
        novaGarrafa = 4;
        filaEsteira.push(novaGarrafa);
        Serial.println(F("Adicionando a fila: Fanta Uva"));
      } else if (strcmp(deteccao_de_item, "Garrafa_Vazia") == 0) {
        novaGarrafa = 5;
        filaEsteira.push(novaGarrafa);
        Serial.println(F("Adicionando a fila: Garrafa Vazia"));
      } else if (strcmp(deteccao_de_item, "Envase_Incorreto") == 0) {
        novaGarrafa = 6;
        filaEsteira.push(novaGarrafa);
        Serial.println(F("Adicionando a fila: Erro de Produção"));
      } else {
        novaGarrafa = 0;
      }
      Serial.print(F("Total na fila: "));
      Serial.println(filaEsteira.count());
      Serial.println();
    }
  }
}

void _realizarHandshake() {
  Serial.println(F("\n\nComunicação pronta, aguardando comandos...\n"));
  while (!conectado) {
    // Envia um status ao servidor
    Serial.println(F("{\"id\":\"arduino\",\"conexao\":\"aguardando\"}"));  // Avisa o Python que está pronto
    _recebeComandos();
    delay(1000);  // Aguarda antes de tentar o PING novamente
  }
}

void _limpaTerminal() {
  // Envia o comando ANSI de "Clear Screen" (ESC [ 2 J) e move o cursor para o topo (ESC [ H)
  Serial.write("\x1B[2J\x1B[H");
  // Lê e descarta tudo o que estiver na fila da serial até que não sobre nada
  while (Serial.available() > 0) {
    Serial.read();
  }
}