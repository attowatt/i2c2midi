// -------------------------------------------------------------------------------------------
// LEDs
// -------------------------------------------------------------------------------------------


// function for turning on the LEDs
void blinkLED(int led) {
  if (led == 1) {
    digitalWrite(led1,HIGH);
    lastLEDMillis1 = millis();
  }
  if (led == 2) {
    digitalWrite(led2,HIGH);
    lastLEDMillis2 = millis();
  }
}


// function for turning off the LEDs
void checkLEDs() {
  unsigned long currentMillis = millis();
  int LEDBlinkLength = 10;
  if (currentMillis - lastLEDMillis1 >= LEDBlinkLength) {
    digitalWrite(led1, LOW);
  }
  if (currentMillis - lastLEDMillis2 >= LEDBlinkLength) {
    digitalWrite(led2, LOW);
  }
}