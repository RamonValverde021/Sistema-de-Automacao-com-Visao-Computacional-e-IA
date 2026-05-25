bool bluetooth = false;
bool serial = false;

// Função que lê as entradas de dados e armazena os resultados nas variáveis apontadas pelos ponteiros '*' nos parametros (*articulacao e *posicao).
void _entradaDados(char *articulacao, int *posicao) {
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
      while (Serial.available()) {      // Enquanto houver bytes disponíveis para leitura, ...
        char caracter = Serial.read();  // recebe o byte como caractere
        comando += caracter;
        delay(10);
      }
    } else if (bluetooth == true) {
      while (Bluetooth.available()) {
        char caracter = Bluetooth.read();
        comando += caracter;
        delay(10);
      }
    }

    Serial.println(comando);

    String letras = "";
    String numeros = "";
    // Iterar através da string de entrada
    for (char caractere : comando) {
      if (isalpha(caractere)) {
        // Se for uma letra, adicione à string de letras
        letras += caractere;
      } else if (isdigit(caractere)) {
        // Se for um dígito, adicione à string de números
        numeros += caractere;
      }
      // Se não for uma letra nem um dígito, ignore
    }

    // Verifica se ouve alguma entrada de coordenada
    if (numeros.length() != 0) {
      // Converta a string de números para inteiro
      int numeroConvertido = numeros.toInt();
      // Converte o caractere em maiúsculo
      char caractereMaiusculo = toupper(comando[0]);

      // Definindo limites de articulação dos motores
      switch (caractereMaiusculo) {
        case 'B':
          numeroConvertido = constrain(numeroConvertido, 0, 180);
          break;
        case 'O':
          numeroConvertido = constrain(numeroConvertido, 0, 180);
          break;
        case 'C':
          numeroConvertido = constrain(numeroConvertido, 0, 180);
          break;
        case 'P':
          numeroConvertido = constrain(numeroConvertido, 0, 180);
          break;
        case 'R':
          numeroConvertido = constrain(numeroConvertido, 0, 180);
          break;
        case 'G':
          numeroConvertido = constrain(numeroConvertido, 0, 1);  // Fecha ou Abre
          break;
        case 'L':
          numeroConvertido = constrain(numeroConvertido, 0, 1);  // Desliga ou Liga
          break;
        case 'Z':
          numeroConvertido = constrain(numeroConvertido, 0, 1);  // Posiçãoinical, qualquer valor é valido
          break;
        default:
          numeroConvertido = -1;
          Serial.println("Entrada Invalida!");
          break;
      }

      *posicao = numeroConvertido;
      *articulacao = caractereMaiusculo;
    } else {
      *posicao = -1;
      *articulacao = 'v';  // v de voide de vazio
    }

    // Quando o aplicativo solicita um status de posição do robo
    if (comando.indexOf("Status") >= 0) {
      serial ? _Status(1) : _Status(2);
    }
  }
}