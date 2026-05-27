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
      if (Serial.available() > 0) {
        comando = Serial.readStringUntil('\n');  // Le novo comando que chegar na Serial
        if (!comando.length() == 0) {            // Se o comando não estiver vazio
          comando.trim();
        }
      }
    } else if (bluetooth == true) {
      if (Bluetooth.available() > 0) {
        comando = Bluetooth.readStringUntil('\n');  // Le novo comando que chegar na Serial
        if (!comando.length() == 0) {               // Se o comando não estiver vazio
          comando.trim();
        }
      }
    }

    Serial.println(comando);

    if (comando == "A1") {
      // Reativa os servos
      base.attach(pinBase);
      ombro.attach(pinOmbro);
      cotovelo.attach(pinCotovelo);
      rotacao.attach(pinRotacao);
      pulso.attach(pinPulso);
      delay(100);

      _DisplaySerial('1', 0);
      _Rotina_01();
    } else if (comando == "A2") {
      // Reativa os servos
      base.attach(pinBase);
      ombro.attach(pinOmbro);
      cotovelo.attach(pinCotovelo);
      rotacao.attach(pinRotacao);
      pulso.attach(pinPulso);
      delay(100);

      _DisplaySerial('1', 0);
      _Rotina_02();
    }
      //Descansa os servos
      base.detach();
      ombro.detach();
      cotovelo.detach();
      rotacao.detach();
      pulso.detach();

/*
    String letras;
    String numeros;
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
    if (numeros.length() != NULL) {
      // Converta a string de números para inteiro
      int numeroConvertido = numeros.toInt();
      // Converte o caractere em maiúsculo
      char caractereMaiusculo = toupper(comando[0]);

      // Definindo limites de articulação dos motores
      switch (caractereMaiusculo) {
        case 'A':
          numeroConvertido = constrain(numeroConvertido, 1, 3);
          break;
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
        default:
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
      _Status(1);
      _Status(2);
    }
    */
  }
 
}