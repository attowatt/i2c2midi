// -------------------------------------------------------------------------------------------
// NRPNs
// -------------------------------------------------------------------------------------------


// function for handling NRPN
void NRPN(int channel, int controller, int value_, bool useRamp) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  
  int offset = 0;
  int slewTime = 0;

  // find the position of the NRPN controller in the NRPN array
  for (int i=0; i < maxNRPNs; i++) {  
    if (NRPNs[i][1] == controller) {
        offset = NRPNs[i][2];   
        slewTime = NRPNs[i][3];
        break;
    } 
  } 

  // keep values in range
  int value = value_ + offset;
  if (value < 0)   value = 0;
  if (value > 16384) value = 16384;
  
  // if the slew time is higher than zero, a ramp is used to slew the value ...
  if (useRamp == true && slewTime != 0) {
    handleRamp(channel, controller, value, slewTime, 1);
  } else {
    sendNRPN(channel, controller, value);
  }

}


// -------------------------------------------------------------------------------------------


// function for sending NRPN
void sendNRPN(int channel, int controller, int value) {
  uint8_t controller_MSB = controller >> 7;
  uint8_t controller_LSB = controller & 0x7F;
  uint8_t value_MSB = value >> 7;
  uint8_t value_LSB = value & 0x7F;

  if (controller_MSB < 0 || controller_MSB > 127) return;
  if (controller_LSB < 0 || controller_LSB > 127) return;
  if (value_MSB < 0 || value_MSB > 127) return;
  if (value_LSB < 0 || value_LSB > 127) return;

  sendMidiCC(channel, 99, controller_MSB);
  sendMidiCC(channel, 98, controller_LSB);
  sendMidiCC(channel,  6, value_MSB);
  sendMidiCC(channel, 38, value_LSB);
}


// -------------------------------------------------------------------------------------------


// function for getting next free slot to store NRPN data
int8_t getNextFreeNRPN(int channel, int controller) {
  // check if controller is already stored in a slot
  for (int i=0; i < maxNRPNs; i++) {                        
    if (NRPNs[i][0] == channel && NRPNs[i][1] == controller) {
        Serial.println("already stored in NRPN array");
        return i;
    }
  }
  // if that did not find anything ...
  nrpnCount += 1;                                                   // count one slot up                
  byte currentNRPN = nrpnCount % maxNRPNs;                         // determine the current slot 
  // check if next slot is occupied; if yes, skip to next slot; 
  // if there's no more space available, replace the slot
  for (int i=0; i < maxNRPNs; i++) {                        
    if (NRPNs[currentNRPN][1] != 0) {
      nrpnCount += 1;
      currentNRPN = nrpnCount % maxNRPNs;
    }
    else {
      break;
    }
  }
  return currentNRPN;
}


// -------------------------------------------------------------------------------------------


int getNRPNvalue(int channel, int controller, int index) {
  for (int i=0; i < maxNRPNs; i++) {                               // go through the list of stored NRPN settings
    if (NRPNs[i][0] == channel && NRPNs[i][1] == controller) {     // check if there's data for requested CH & NRPN controller 
      return NRPNs[i][index];                                      // if yes, return the requested data
    }
  }
  return 0;                                                        // if no, return 0
}