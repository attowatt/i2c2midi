#ifdef TEST

// -------------------------------------------------------------------------------------------
// Testing
// -------------------------------------------------------------------------------------------


unsigned long lastTEST;  
int TESTinterval = 500;

void TESTFunction() {  
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastTEST >= TESTinterval) {
    //blinkLED(1);
    i2cReceiveEventTEST(20, 0, random(50,70), 120, 0, 0);
    lastTEST = millis();
  }
}


void i2cReceiveEventTEST(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f) {
  i2cData[0] = a; // OP
  i2cData[1] = b; // channel
  i2cData[2] = c; // value 1
  i2cData[3] = d; // value 2
  i2cData[4] = e; // value 3
  i2cData[5] = f; // value 4
  received += 1;           
}


#endif