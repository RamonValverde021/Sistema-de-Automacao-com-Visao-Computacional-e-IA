void _abreCancela() {
  myservo.write(180);  // 180 Totalmente avançado
}

void _fechaCancela() {
  myservo.write(0);  // 0 Totalmente recuado
}