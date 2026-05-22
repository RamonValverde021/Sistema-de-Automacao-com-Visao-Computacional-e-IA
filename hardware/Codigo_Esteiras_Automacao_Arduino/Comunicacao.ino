void _recebeComandos() {
  static String comando = "";
  comando = Serial.readStringUntil('\n');  // Le novo comando que chegar na Serial
  if (!comando.length() == 0) {            // Se o comando não estiver vazio
    comando.trim();
    Serial.println(comando);  // Imprime o comando
    _processaComando(comando);
    comando = "";  // Limpa para a próxima mensagem
  }
}

void _processaComando(const String& json) {
  StaticJsonDocument<TAMANHO_MAX> doc;                      // Cria um documento JSON para armazenar os dados desserializados
  DeserializationError error = deserializeJson(doc, json);  // Tenta desserializar os dados MsgPack recebidos para o doc
  if (error) {
    Serial.println("-------------------------");
    Serial.println("Erro na deserialização");
    Serial.println(error.c_str());
    return;
  } else {
    Serial.print("JSON recebido: \n");
    serializeJsonPretty(doc, Serial);
    Serial.println();

    /*
    if (keyJSON(doc.as<JsonObject>(), "orelha_direita")) {  // Se existe a key "orelha_direita" e não estiver vaiza no json
      posicaoOrelhaDireita = doc["orelha_direita"];         // Pega o valor de "orelha_direita"
      if (posicaoOrelhaDireita >= 0) {
        movimentaOrelhas("direito", posicaoOrelhaDireita);
      }
    }

    if (keyJSON(doc.as<JsonObject>(), "orelha_esquerda")) {  // Se existe a key "orelha_esquerda" e não estiver vaiza no json
      posicaoOrelhaEsquerda = doc["orelha_esquerda"];        // Pega o valor de "orelha_esquerda"
      if (posicaoOrelhaEsquerda >= 0) {
        movimentaOrelhas("esquerdo", posicaoOrelhaEsquerda);
      }
    }

    if (keyJSON(doc.as<JsonObject>(), "cauda")) {  // Se existe a key "orelha_esquerda" e não estiver vaiza no json
      String cauda = doc["cauda"];                 // Pega o valor de "orelha_esquerda"
      if (cauda == "cima") {
        caudaEmpina();
      } else if (cauda == "direita") {
        caudaMoveDireita();
      } else if (cauda == "esquerda") {
        caudaMoveEsquerda();
      } else if (cauda == "baixo") {
        caudaDesce();
      }
    }


    if (keyJSON(doc.as<JsonObject>(), "modo_operacao")) {  // Se existe a key "modo_operacao" e não estiver vaiza no json
      String modoOperacao = doc["modo_operacao"];          // Pega o valor de "modo_operacao"
      if (modoOperacao == "automatico") {                  // Se o valor da chave for "automatico"
        EEPROM.update(addrModo, 2);                        // Grava na memoria o novo modo de operação
        modo = "automatico";                               // Muda a variavel global de modo de operação para "automatico"
        modoAutomatico();                                  // Inicia a função de modo automatico
      } else if (modoOperacao == "apresentacao") {         // Se o valor da chave for apresentacao
        EEPROM.update(addrModo, 3);                        // Grava na memoria o novo modo de operação
        modo = "apresentacao";                             // Muda a variavel global de modo de operação para "apresentacao"
        modoApresentacao();                                // Inicia a função de modo apresentacao
      } else {                                             // Se o valor da chave não for "automatico" e nem "apresentacao" volta para "normal"
        EEPROM.update(addrModo, 1);                        // Grava na memoria o novo modo de operação
        modo = "normal";                                   // Muda a variavel global de modo de operação para "normal"
      }
    }

    if (keyJSON(doc.as<JsonObject>(), "intervalo_automatico")) {  // Se existe a key "intervalo_automatico" e não estiver vaiza no json
      int valor = doc["intervalo_automatico"];                    // Pega o valor de "intervalo_automatico"
      if (valor > 0 && valor <= 60) {                             // Se o valor do intervalo for maior que 0 e menor igual a 60
        intervaloAutomatico = valor;                              // O intervalo de ação do modo automatico será igual ao valor
        intervaloMs = intervaloAutomatico * 60000UL;              // minutos → ms
        EEPROM.update(addrIntervalo, intervaloAutomatico);        // Grava na memoria o novo valor de intervalo
      }
    }

    if (keyJSON(doc.as<JsonObject>(), "status")) {  // Se existe a key "modo_operacao" e não estiver vaiza no json
      String status = doc["status"];                // Pega o valor de "modo_operacao"
      if (status == "conectado") {
        String statusModo = "{\"modo\":\"" + modo + "\",\"intervalo\":" + intervaloAutomatico + "}";
        Bluetooth.println(statusModo);
      }
    }
    */
  }
}

// Função que retorna se o json contem a chave desejada e se esta vazio
bool _keyJSON(JsonVariantConst obj, String chave) {
  // Verifica se a chave existe e não é nula
  if (!obj.containsKey(chave) || obj[chave].isNull()) return false;  // Verifica se a chave existe no objeto e se o valor dela não é nulo

  JsonVariantConst valor = obj[chave];  // Armazena o valor associado à chave em uma variável

  // String vazia
  if (valor.is<const char*>()) {                // Se o valor for uma string (const char*)
    const char* str = valor.as<const char*>();  // Converte o valor para ponteiro de string
    return str != nullptr && strlen(str) > 0;   // Verifica se a string não é nula e não está vazia
  }
  // Objeto vazio
  if (valor.is<JsonObjectConst>()) {                // Se o valor for um objeto JSON
    return valor.as<JsonObjectConst>().size() > 0;  // Verifica se o objeto tem pelo menos uma chave
  }
  // Array vazio
  if (valor.is<JsonArrayConst>()) {                // Se o valor for um array JSON
    return valor.as<JsonArrayConst>().size() > 0;  // Verifica se o array tem pelo menos um elemento
  }
  // Tudo que não for nulo, string vazia, objeto vazio ou array vazio é considerado válido
  return true;  // Para todos os outros tipos de valor (número, booleano, etc.), considera válido
}



/*
----- Sobre void recebeComandos(); -----

O problema da função anterior é que ela é "gulosa": ela aceita qualquer coisa que o Bluetooth envia e coloca no buffer. 
Se o módulo enviar um caractere nulo (\0) ou um ruído antes do {, seu JSON ficará inválido (ex: \0{"comando":1}).

Como você está trabalhando com a estrutura JSON, a melhor forma de corrigir isso é fazer a função ignorar tudo o que chegar antes da abertura de chaves {.

Por que isso funciona?
Filtro de Entrada: A condição if (c == '{' || buffer.length() > 0) garante que, se o buffer estiver vazio e chegar um caractere de "lixo" 
(como o \0 que o App Inventor às vezes envia), ele será ignorado.

Sincronização: O buffer só começa a crescer quando o primeiro { é detectado.

Resiliência: O uso do .trim() antes de processar ajuda a remover espaços em branco ou caracteres de controle que o chip Jieli costuma anexar ao final da transmissão.

Dica de Desenvolvedor (C++ para Automação)
Como você mencionou que o App Inventor envia um caractere a mais e isso atrapalha o seu JSON, você pode adicionar uma proteção extra de "timeout". 
Se por algum motivo o Bluetooth enviar um { mas nunca enviar o }, o seu buffer ficaria preso para sempre.

Sugestão de melhoria:
if (buffer.length() > 100) buffer = ""; // // Evita estouro de memória se o JSON vier corrompido

No App Inventor
Lembre-se que no seu bloco WriteBytesWithResponse, o ideal é garantir que a string sendo enviada não tenha espaços extras no final. Use o bloco trim do App Inventor na peça de texto antes de passar para a função de envio.
*/
