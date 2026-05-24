int atraso = 8; // 5
void _abreCancela() {
  for (int c = 180; c >= 0; c--) {  // Vai de 180 graus para 0 graus em incrementos de 1 grau
    servoCancela.write(c);          // Diga ao servo para ir para a posição na variável 'c'.
    delay(atraso);                  // Aguarda 5 ms para que o servo alcance a posição.
  }
}

void _fechaCancela() {
  for (int c = 0; c <= 180; c++) {  //Vai de 0 graus para 180 graus em incrementos de 1 grau
    servoCancela.write(c);          // Diga ao servo para ir para a posição na variável 'c'.
    delay(atraso);                  // Aguarda 5 ms para que o servo alcance a posição.
  }
}
