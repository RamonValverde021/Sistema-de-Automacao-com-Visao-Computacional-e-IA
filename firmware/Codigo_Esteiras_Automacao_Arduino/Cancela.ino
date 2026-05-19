int atraso = 5;
void _abreCancela() {
  for (int c  = 180; c >= 0; c--) { // goes from 180 degrees to 0 degrees
    servoCancela.write(c);              // tell servo to go to position in variable 'pos'
    delay(atraso);                       // waits 15 ms for the servo to reach the position
  }
}

void _fechaCancela() {
  for (int c = 0; c <= 180; c++) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servoCancela.write(c);              // tell servo to go to position in variable 'pos'
    delay(atraso);                       // waits 15 ms for the servo to reach the position
  }
}

