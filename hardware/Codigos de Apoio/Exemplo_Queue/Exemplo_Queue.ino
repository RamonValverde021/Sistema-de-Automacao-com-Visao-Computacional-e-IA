#include <Queue.h>

// Definindo a estrutura para armazenar os dados de cada garrafa
struct Garrafa {
  int id;          // Identificador único da garrafa
  int tipoCor;     // Ex: 1 para Azul, 2 para Vermelha, etc.
};

// Cria a instância da fila utilizando a estrutura 'Garrafa'
Queue<Garrafa> filaEsteira;

// Pinos dos sensores e atuadores (simulação)
const int pinoSensorPresenca = 2; // Sensor físico de triagem na esteira
const int pinoAtuadorTriagem = 3; // Braço/Pistão de descarte

void setup() {
  Serial.begin(38400);
  pinMode(pinoSensorPresenca, INPUT_PULLUP);
  pinMode(pinoAtuadorTriagem, OUTPUT);
  
  Serial.println("Sistema da Esteira Inicializado com Fila Dinâmica.");
}

void loop() {
  // 1. RECEBENDO DADOS DO SERVIDOR (Visão Computacional via Serial)
  if (Serial.available() > 0) {
    char comando = Serial.read();
    
    // Supondo que o servidor envia '1' ou '2' conforme identifica a garrafa
    if (comando == '1' || comando == '2') {
      Garrafa novaGarrafa;
      novaGarrafa.id = random(100, 999);   // ID simulado para rastreamento
      novaGarrafa.tipoCor = comando - '0'; // Converte o caractere recebido para inteiro
      
      // Adiciona a garrafa ao final da fila (Enqueue)
      filaEsteira.push(novaGarrafa);
      
      Serial.print("Servidor registrou: Garrafa ID ");
      Serial.print(novaGarrafa.id);
      Serial.print(" (Tipo ");
      Serial.print(novaGarrafa.tipoCor);
      Serial.print(") entrou na esteira. Total na fila: ");
      Serial.println(filaEsteira.count()); // Mostra quantas garrafas estão na esteira
    }
  }

  // 2. LÓGICA DE TRIAGEM (Quando a garrafa física atinge o sensor de descarte)
  if (digitalRead(pinoSensorPresenca) == LOW) { // Alinhado com INPUT_PULLUP
    
    // Verifica se existem garrafas registradas na fila através do contador
    if (filaEsteira.count() > 0) {
      
      // Espia os dados da garrafa que está na frente da fila (a mais antiga)
      // Nota: Esta biblioteca retorna um ponteiro para o objeto no método head()
      //Garrafa* garrafaAtual = filaEsteira.head();
      Garrafa garrafaAtual = filaEsteira.peek();

      Serial.print("\n--- Garrafa chegou ao ponto de triagem ---");
      Serial.print(" Processando ID: ");
      Serial.println(garrafaAtual.id);
      
      // Toma a decisão com base no tipo armazenado na fila
      if (garrafaAtual.tipoCor == 1) {
        Serial.println("Ação: Ativando atuador para descarte (Tipo 1).");
        digitalWrite(pinoAtuadorTriagem, HIGH);
        delay(1000); // Janela de tempo para o braço atuar fisicamente
        digitalWrite(pinoAtuadorTriagem, LOW);
      } else {
        Serial.println("Ação: Garrafa Tipo 2 detectada. Deixando passar direto.");
      }
      
      // Remove o elemento da frente da fila (Dequeue) e libera a memória RAM
      filaEsteira.pop();
      Serial.print("Garrafa processada e removida da fila. Restantes: ");
      Serial.println(filaEsteira.count());
      Serial.println();
      
      // Aguarda a garrafa terminar de passar pelo sensor para evitar leituras duplicadas
      while(digitalRead(pinoSensorPresenca) == LOW); 
      
    } else {
      // Se o sensor físico disparar mas o contador for 0, houve erro de sincronia
      Serial.println("Alerta: Objeto detectado na esteira, mas a fila de dados está vazia!");
      delay(500); 
    }
  }
}