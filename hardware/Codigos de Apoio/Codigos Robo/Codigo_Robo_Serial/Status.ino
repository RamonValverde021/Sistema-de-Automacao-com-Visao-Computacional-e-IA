// Exibe na Serial oque está sendo executado no decorrer do codigo
void _DisplaySerial(char index, int coordenada) {
  switch (index) {
    case '0':
      Serial.println("FUNCAO: ENTRADA DE DADOS");
      break;

    case '1':
      Serial.println("AUTOMACAO 01: MINHA ROTINA");
      break;

    case '2':
      Serial.println("AUTOMACAO 02: PEGAR OVO");
      break;

    case '3':
      Serial.println("AUTOMACAO 03: PEGAR NA ESTEIRA");
      break;

    case 'I':
      Serial.println("\n\n     INICIANDO ROBO     ");
      Serial.println("      ARM MARK VI      \n");
      delay(1000);  // 5000
      break;

    case 'B':
      Serial.print("BASE: ");
      Serial.println(coordenada);
      break;

    case 'O':
      Serial.print("OMBRO: ");
      Serial.println(coordenada);
      break;

    case 'C':
      Serial.print("COTOVELO: ");
      Serial.println(coordenada);
      break;

    case 'P':
      Serial.print("PULSO: ");
      Serial.println(coordenada);
      break;

    case 'R':
      Serial.print("ROTACAO: ");
      Serial.println(coordenada);
      break;

    case 'G':
      Serial.print("GARRA: ");
      Serial.println(coordenada);
      break;

    case 'L':
      Serial.print("LUZ: ");
      digitalRead(luz) == 1 ? Serial.println("LIGADA") : Serial.println("DESLIGADA");
      break;

    case 'Z':
      Serial.print("POSICAO INICIAL");
      break;

    default:
      break;
  }
}

unsigned long tempo_Anterior_Status = 0;
unsigned long intervalo_Status = 2000;  // 1000
// Exibe na Serial constantemente a posição dos servos motores
void _Status(int tipo) {
  unsigned long tempo_Atual_Status = millis();
  if (tempo_Atual_Status - tempo_Anterior_Status >= intervalo_Status) {
    tempo_Anterior_Status = tempo_Atual_Status;

    if (tipo == 1) {
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
      Serial.print("Status: ");
      Serial.println(dados);

    } else if (tipo == 2) {
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
      dados += status_garra;
      dados += "/L-";
      digitalRead(luz) == HIGH ? dados += "LIGADA" : dados += "DESLIGADA";
      Bluetooth.print(dados);
    }
  }
}