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