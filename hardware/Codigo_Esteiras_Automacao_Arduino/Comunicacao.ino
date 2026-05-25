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
    Serial.println("-------------------------");
    Serial.println("Erro na deserialização");
    Serial.print("Mensagem recebida: ");
    Serial.println(json);
    Serial.print("Erro: ");
    Serial.println(error.c_str());
    return;
  }

  Serial.print("JSON recebido: ");
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
      String controle = doc["controle"];
      if (controle == "iniciar_esteira") {
        Serial.println("Iniciando o Sistema");
        pause = false;
        _ligaEsteira();
      } else if (controle == "pausar_processamento") {
        Serial.println("Pausando o Sistema");
        pause = true;
        _desligaEsteira();  // Para o motor imediatamente
      } else if (controle == "reiniciar_sistema") {
        Serial.println("Reiniciando o Sistema");
        _limpaTerminal();
        delay(250);
        resetFunc();
      }
    }

    // 3. Detecção de Garrafas
    if (doc.containsKey("deteccao_de_item")) {
      String deteccao_de_item = doc["deteccao_de_item"];
      if (deteccao_de_item == "Coca_Cola") {
        Serial.println("Adicionando a fila: Coca-Cola");
      } else if (deteccao_de_item == "Sprite") {
        Serial.println("Adicionando a fila: Sprite");
      } else if (deteccao_de_item == "Fanta_Laranja") {
        Serial.println("Adicionando a fila: Fanta Laranja");
      } else if (deteccao_de_item == "Fanta_Uva") {
        Serial.println("Adicionando a fila: Fanta Uva");
      } else if (deteccao_de_item == "Garrafa_Vazia") {
        Serial.println("Adicionando a fila: Garrafa Vazia");
      } else if (deteccao_de_item == "Envase_Incorreto") {
        Serial.println("Adicionando a fila: Erro de Produção");
      }
    }

  }
}

void _realizarHandshake() {
  Serial.println("\n\nComunicação pronta, aguardando comandos...\n");
  while (!conectado) {
    // Envia um status ao servidor
    String statusModo = "{\"id\":\"arduino\",\"conexao\":\"aguardando\"}";
    Serial.println(statusModo);  // Avisa o Python que está pronto
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