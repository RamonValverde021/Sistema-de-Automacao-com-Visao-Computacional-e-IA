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

    // Tratamento para afirmar comunicação com o Servidor Python
    if (_keyJSON(doc.as<JsonObject>(), "id")) {           // Se existe a key "id" e não estiver vaiza no json
      String id = doc["id"];                              // Pega o valor de "id"
      if (id == "servidor") {                             // Se o "id" for igual a "servidor"
        if (_keyJSON(doc.as<JsonObject>(), "conexao")) {  // Se existe a key "conexao" e não estiver vaiza no json
          String conexao = doc["conexao"];                // Pega o valor de "conexao"
          if (conexao == "ouvindo") {                     // Se a "conexao" for igual a "ouvindo"
            conectado = true;                             // Handshake bem-sucedido!
            // Envia uma confirmação em JSON
            StaticJsonDocument<100> docConfirma;
            docConfirma["id"] = "arduino";
            docConfirma["conexao"] = "estabelecida";
            serializeJson(docConfirma, Serial);
            Serial.println();
          }
        }
      }
    }
    

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
