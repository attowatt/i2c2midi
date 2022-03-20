// -------------------------------------------------------------------------------------------
// I2C receive and request events
// -------------------------------------------------------------------------------------------


// handler for receiving I2C messages
void i2cReceiveEvent(size_t count) {
  if(count < MEM_LEN) {
    Wire.read(i2cData, count);
    received = count; // this triggers function in loop   
  }
}


// -------------------------------------------------------------------------------------------


// hanlder for receiving I2C request messages
void i2cRequestEvent(void) {
  #ifdef DEBUG
    Serial.println("I2C Request received");
  #endif
  opFunctions(true, i2cData); // call the respective OP with isRequest = true
}
