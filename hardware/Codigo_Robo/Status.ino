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

    case '3':
      Serial.println(F("EXECUTANDO ROTINA 03:"));
      break;

    case '4':
      Serial.println(F("EXECUTANDO ROTINA 04:"));
      break;

    case 'I':
      Serial.println('\n');
      Serial.println(F("     INICIANDO ROBO     "));
      Serial.println(F("      ARM MARK VI      \n"));
      delay(500);  // 5000
      break;

    case 'B':
      Serial.print(F("BASE: "));
      Serial.println(coordenada);
      break;

    case 'O':
      Serial.print(F("OMBRO: "));
      Serial.println(coordenada);
      break;

    case 'C':
      Serial.print(F("COTOVELO: "));
      Serial.println(coordenada);
      break;

    case 'P':
      Serial.print(F("PULSO: "));
      Serial.println(coordenada);
      break;

    case 'R':
      Serial.print(F("ROTACAO: "));
      Serial.println(coordenada);
      break;

    case 'G':
      Serial.print(F("GARRA: "));
      Serial.println(coordenada);
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

unsigned long tempo_Anterior_Status = 0;
unsigned long intervalo_Status = 1000;  // 1000
// Exibe na Serial constantemente a posição dos servos motores
void _Status(int envio) {
  unsigned long tempo_Atual_Status = millis();
  if (tempo_Atual_Status - tempo_Anterior_Status >= intervalo_Status) {
    tempo_Anterior_Status = tempo_Atual_Status;

    if (envio == 1) {
      String dados = "";
      dados += "B = ";
      dados += base.read();
      dados += " - O = ";
      dados += ombro.read();
      dados += " - C = ";
      dados += cotovelo.read();
      dados += " - P = ";
      dados += pulso.read();
      dados += " - R = ";
      dados += rotacao.read();
      dados += " - G = ";
      dados += status_garra;
      dados += " - L = ";
      digitalRead(luz) == HIGH ? dados += "Ligada" : dados += "Desligada";

      Serial.print(F("Status: "));
      Serial.println(dados);
    } else if (envio == 2) {
      String dados = "";
      dados += "B-";
      dados += base.read();
      dados += "/O-";
      dados += ombro.read();
      dados += "/C-";
      dados += cotovelo.read();
      dados += "/P-";
      dados += pulso.read();
      dados += "/R-";
      dados += rotacao.read();
      dados += "/G-";
      status_garra == 0 ? dados += "0" : dados += "1";
      dados += "/L-";
      digitalRead(luz) == HIGH ? dados += "LIGADA" : dados += "DESLIGADA";
      Bluetooth.print(dados);
    }
  }
}