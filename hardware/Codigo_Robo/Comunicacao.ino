// Função que lê as entradas de dados e armazena os resultados nas variáveis apontadas pelos ponteiros '*' nos parametros (*articulacao e *posicao).
void _recebeComandos() {
  String comando = "";
  if (Serial.available() || Bluetooth.available()) {  // Verifica se tem algo na Serial ou no Bluetooth
    if (Serial.available()) {                         // Se tem algo na Serial bloqueia o Bluetooth
      serial = true;
      bluetooth = false;
    } else if (Bluetooth.available()) {  // Se tem algo no Bluetooth bloqueia a Serial
      serial = false;
      bluetooth = true;
    }
    if (serial == true) {
      if (Serial.available() > 0) {
        comando = Serial.readStringUntil('\n');  // Le novo comando que chegar na Serial
        if (!comando.length() == 0) {            // Se o comando não estiver vazio
          comando.trim();
          _processaComando(comando);
          comando = "";  // Limpa para a próxima mensagem
        }
      }
    } else if (bluetooth == true) {
      if (Bluetooth.available() > 0) {
        comando = Bluetooth.readStringUntil('\n');  // Le novo comando que chegar na Serial
        if (!comando.length() == 0) {               // Se o comando não estiver vazio
          comando.trim();
          _processaComando(comando);
          comando = "";  // Limpa para a próxima mensagem
        }
      }
    }
  }
}

void _processaComando(const String& json) {
  // Certifique-se de que TAMANHO_MAX seja de pelo menos 128 bytes
  StaticJsonDocument<TAMANHO_MAX> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (conectado) {
    String comando = json;
    if (comando.indexOf("ROTINA_01") >= 0) {
      // Reativa os servos
      base.attach(pinBase);
      ombro.attach(pinOmbro);
      cotovelo.attach(pinCotovelo);
      rotacao.attach(pinRotacao);
      pulso.attach(pinPulso);
      delay(100);

      _DisplaySerial('1', 0);
      _Rotina_01();
    } else if (comando.indexOf("ROTINA_02") >= 0) {
      // Reativa os servos
      base.attach(pinBase);
      ombro.attach(pinOmbro);
      cotovelo.attach(pinCotovelo);
      rotacao.attach(pinRotacao);
      pulso.attach(pinPulso);
      delay(100);

      _DisplaySerial('2', 0);
      _Rotina_02();
    }
    //Descansa os servos
    base.detach();
    ombro.detach();
    cotovelo.detach();
    rotacao.detach();
    pulso.detach();
    delay(100);
    return;  // Após concluido as funções retorna para não validar o JSON
  }

  if (error) {
    if (imprimeStatus) {
      Serial.println(F("-------------------------"));
      Serial.println(F("Erro na deserialização"));
      Serial.print(F("Mensagem recebida: "));
      Serial.println(json);
      Serial.print(F("Erro: "));
      Serial.println(error.c_str());
    }
    return;
  }
  if (imprimeStatus) {
    Serial.print(F("JSON recebido: "));
    //serializeJsonPretty(doc, Serial);
    serializeJson(doc, Serial);
    Serial.println();
  }

  // Se chegou aqui, o JSON é válido. Vamos tratar os comandos.
  if (doc.containsKey("id") && doc["id"] == "servidor") {
    // 1. Handshake / Conexão
    if (doc.containsKey("conexao") && doc["conexao"] == "ouvindo") {
      conectado = true;
      StaticJsonDocument<TAMANHO_MAX> docConfirma;
      docConfirma["id"] = "robo";
      docConfirma["conexao"] = "estabelecida";
      serializeJson(docConfirma, Serial);
      Serial.println();
      return;  // Sai da função
    }
  }
}

void _realizarHandshake() {
  Serial.println(F("\n\nComunicação pronta, aguardando comandos...\n"));
  while (!conectado) {
    // Envia um status ao servidor
    Serial.println(F("{\"id\":\"robo\",\"conexao\":\"aguardando\"}"));  // Avisa o Python que está pronto
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