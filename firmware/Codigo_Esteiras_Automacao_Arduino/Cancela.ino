void _abreCancela() {
  servoCancela.write(180);  // 180 Totalmente avançado
}

void _fechaCancela() {
  servoCancela.write(0);  // 0 Totalmente recuado
}