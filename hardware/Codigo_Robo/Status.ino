// Exibe na Serial oque está sendo executado no decorrer do codigo
void _DisplaySerial(char index, int coordenada) {
  switch (index) {
    case '0':
      Serial.println(F("FUNCAO: ENTRADA DE DADOS"));
      break;

    case '1':
      Serial.println(F("EXECUTANDO ROTINA 01:"));
      break;

    case '2':
      Serial.println(F("EXECUTANDO ROTINA 02:"));
      break;

    case 'I':
      Serial.println('\n');
      Serial.println(F("     INICIANDO ROBO     "));
      Serial.println(F("      ARM MARK VI      \n"));
      delay(500);  // 5000
      break;

    case 'L':
      Serial.print(F("LUZ: "));
      digitalRead(luz) == 1 ? Serial.println(F("LIGADA")) : Serial.println(F("DESLIGADA"));
      break;

    case 'Z':
      Serial.print(F("POSICAO INICIAL"));
      break;

    default:
      break;
  }
}