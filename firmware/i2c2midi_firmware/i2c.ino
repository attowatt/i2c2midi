// -------------------------------------------------------------------------------------------
// I2C receive and request events
// -------------------------------------------------------------------------------------------


#ifdef TEENSY3X
  // handler for receiving I2C messages
  void i2cReceiveEvent(size_t count) {
    if(count < MEM_LEN) {
      Wire.read(i2cData, count);
      received = count; // this triggers function in loop   
    }
  }

  // hanlder for receiving I2C request messages
  void i2cRequestEvent(void) {
    opFunctions(true, i2cData); // call the respective OP with isRequest = true
  }
#endif

#ifdef TEENSY41
  // handler for receiving I2C messages
  void i2cReceiveEvent(int count) {
    for(int i=0; i < count; i++) {
      i2cData[i] = Wire.read();  
    }
    received = count;
  }

  // hanlder for receiving I2C request messages
  void i2cRequestEvent() {
    opFunctions(true, i2cData);
  }
#endif

#ifdef ESP32
  // handler for receiving I2C messages
  void i2cReceiveEvent(int count) {
    for(int i=0; i < count; i++) {
      i2cData[i] = Wire.read();  
    }
    received = count;
  }

  // handler for receiving I2C request messages
  void i2cRequestEvent() {
    opFunctions(true, i2cData);
  }
#endif