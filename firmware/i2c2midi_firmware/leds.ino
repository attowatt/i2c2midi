// -------------------------------------------------------------------------------------------
// LEDs
// -------------------------------------------------------------------------------------------


// function for turning on the LEDs
void blinkLED(int led) {
  if (led == 1) {
    //digitalWrite(led1,HIGH);
    digitalWrite(RGB_BUILTIN, HIGH);
    lastLEDMillis1 = millis();
    ledOn1 = true;
  }
  if (led == 2) {
    digitalWrite(led2,HIGH);
    lastLEDMillis2 = millis();
    ledOn2 = true;
  }
}


// function for turning off the LEDs
void checkLEDs() {
  if (!ledOn1) return;
  unsigned long currentMillis = millis();
  uint8_t LEDBlinkLength = 10;
  if (currentMillis - lastLEDMillis1 >= LEDBlinkLength) {
    //digitalWrite(led1, LOW);
    digitalWrite(RGB_BUILTIN, LOW);
    ledOn1 = false;
  }
  if (currentMillis - lastLEDMillis2 >= LEDBlinkLength) {
    digitalWrite(led2, LOW);
    ledOn2 = false;
  }
}