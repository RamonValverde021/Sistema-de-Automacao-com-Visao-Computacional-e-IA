void _ligaEsteira() {
  analogWrite(pin_Motor_Esteira_PWM, potenciaEstPrinc);
  digitalWrite(pin_Motor_Esteira_H, 0);
  digitalWrite(pin_Motor_Esteira_AH, 1);
}

void _desligaEsteira() {
  analogWrite(pin_Motor_Esteira_PWM, 0);
  digitalWrite(pin_Motor_Esteira_H, 0);
  digitalWrite(pin_Motor_Esteira_AH, 0);
}

void _liberaGarrafa() {
  _desligaEsteira();
  _abreCancela();
  _ligaEsteira();
  int sensor = 0;
  while (sensor == 0) {                        // 0 Enquando a garrafa não passa da cancela
    sensor = digitalRead(pin_Sensor_Cancela);  // Verifica se a garrafa já passou pela cancela
  }
  _desligaEsteira();
  _fechaCancela();
  _ligaEsteira();
}