#include <ArduinoJson.h>

bool conectado = false;

void setup() {
  Serial.begin(38400); // Mesma velocidade para USB ou Bluetooth
  while (!Serial) { continue; } // Aguarda inicialização
  
  // Executa o Handshake antes de iniciar o loop principal
  realizarHandshake();
}

void loop() {
  // Verifica se há dados vindos do Python
  if (Serial.available() > 0) {
    String dadosRecebidos = Serial.readStringUntil('\n');
    dadosRecebidos.trim();

    // Aloca memória para deserializar o JSON recebido
    StaticJsonDocument<200> docRecebido;
    DeserializationError erro = deserializeJson(docRecebido, dadosRecebidos);

    if (!erro) {
      // 1. Lendo os dados enviados pelo Python
      const char* comando = docRecebido["comando"];
      int valorLed = docRecebido["led"];

      // Exemplo de ação baseada no JSON do Python
      if (valorLed == 1) {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }

      // 2. Criando a Resposta em JSON para o Python
      StaticJsonDocument<200> docResposta;
      docResposta["status"] = "sucesso";
      docResposta["eco_comando"] = comando;
      // Simulando a leitura de um sensor analógico (A0)
      docResposta["sensor_A0"] = analogRead(A0); 

      // Envia o JSON de volta pela serial terminando com \n
      serializeJson(docResposta, Serial);
      Serial.println(); 
    }
  }
}

void realizarHandshake() {
  while (!conectado) {
    Serial.println("PING"); // Avisa o Python que está pronto
    
    if (Serial.available() > 0) {
      String resposta = Serial.readStringUntil('\n');
      resposta.trim();
      
      if (resposta == "PONG") {
        conectado = true; // Handshake bem-sucedido!
        
        // Envia uma confirmação em JSON
        StaticJsonDocument<100> docConfirma;
        docConfirma["conexao"] = "estabelecida";
        serializeJson(docConfirma, Serial);
        Serial.println();
      }
    }
    delay(1000); // Aguarda antes de tentar o PING novamente
  }
}