// Função que lê as entradas de dados e armazena os resultados nas variáveis apontadas pelos ponteiros '*' nos parametros (*articulacao e *posicao).
void _entradaDados() {
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
  }
}