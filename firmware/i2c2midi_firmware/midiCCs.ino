// -------------------------------------------------------------------------------------------
// MIDI CCs
// -------------------------------------------------------------------------------------------


// function for handling MIDI CCs
void midiCC(int channel, int controller, int value_, bool useRamp) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  if (controller < 0 || controller > 127) return;
  
  int offset = CCs[channel][controller][2];          
  int value = value_ + offset;
  if (value < 0) value = 0;
  if (value > 16383) value = 16383;
  
  int slewTime = CCs[channel][controller][1];             // get the set slew time for controller

  // if ramp is allowed and slew time higher than zero: use a ramp
  if (useRamp == true && slewTime != 0) {
    handleRamp(channel, controller, value, slewTime, 0);
  } 
  // if ramp is not allowed and slew time higher than zero: 
  // CC.SET, use a ramp with slew time = 1 to set it to new value immediately but using a ramp
  else if (useRamp == false && slewTime != 0) {
    handleRamp(channel, controller, value, 1, 0);
  }
  // if slewTime is zero, don't use a ramp
  else if (slewTime == 0) {
    sendMidiCC(channel, controller, scaleDown(value)); 
  }
  CCs[channel][controller][0] = value;                    // store the new CC value in the CC array
}


// -------------------------------------------------------------------------------------------


// function for handling ramps
void handleRamp(int channel, int controller, int value, int slewTime, bool type) {
  int oldValue = CCs[channel][controller][0];             // get the old CC value temporarily
  int rampToUse = 0;                                      // set to zero, because no ramp is yet chosen
    // check if this controller is already assigned to a ramp, if yes: use the same ramp, if no: get a new ramp to use
    for (int i = 0; i < maxRamps; i++) {
      if (rampsAssignedCCs[i][0] == channel && rampsAssignedCCs[i][1] == controller) {
        rampToUse = i;                                    // use the ramp to which this controller is already assigned
        break;
      }
    }
    if (!rampToUse) {                                     // if there's not yet a ramp assigned ...
        rampToUse = getNextFreeRamp();                    // check which new ramp to use
        rampsAssignedCCs[rampToUse][0] = channel;         // assign channel to this ramp
        rampsAssignedCCs[rampToUse][1] = controller;      // assign controller to this ramp
        rampsAssignedCCs[rampToUse][2] = type;            // Type: 0 = CC, 1 = NRPN
        myRamps[rampToUse].go(oldValue, 1);               // set the new ramp to the old value before using it
    }
    // start the ramp
    myRamps[rampToUse].go(value, slewTime);
}


// -------------------------------------------------------------------------------------------


// function for updating ramps
void updateRamps() {
  for (int i = 0; i < maxRamps; i++) {                                  // go through all ramps
    if (myRamps[i].isRunning()) {                                       // check if the ramp is running
      int currentValue = myRamps[i].update();                           // update the ramp
      //int currentValueScaled = scaleDown(currentValue);               // scale the value down from 14 bit to 0..127
      if (scaleDown(currentValue) != lastRampValues[i]) {               // if the updated ramp value is different...
        int channel = rampsAssignedCCs[i][0];                           // get the channel of the ramp
        int controller = rampsAssignedCCs[i][1];                        // get the controller of the ramp
        int type = rampsAssignedCCs[i][2];                              // get the type: 0 = CC, 1 = NRPN
        if (type == 1) {
          sendNRPN(channel, controller, currentValue);                  // send NRPN
        } else {
          sendMidiCC(channel, controller, scaleDown(currentValue));     // send MIDI CC with new ramp value
        }
      }
      lastRampValues[i] = scaleDown(currentValue);                      // store the new value for comparison
    }  
  }
}


// -------------------------------------------------------------------------------------------


// function for getting next free ramp to use
int getNextFreeRamp() {
  rampCount += 1;                                         // count one ramp up                           
  currentRamp = rampCount % maxRamps;                     // determine the current ramp number
  // check if next ramp number is still running; if yes, skip to next ramp number; 
  // if there's no more space available, replace the ramp
  for (int i = 0; i < maxRamps; i++) {                        
    if (myRamps[currentRamp].isRunning()) {
      rampCount += 1;
      currentRamp = rampCount % maxRamps;
    }
    else {
      break;
    }
  }
  return currentRamp;
}


// -------------------------------------------------------------------------------------------


// functions to scale up/down between 0..127 and 14 bit range
int scaleUp (int value) {
  return value * 129;                     // 129 == 16383 / 127
}
int scaleDown (int value) {
  int result = (value << 1) / 129;        // multiply by 2 so we can round below
  return (result >> 1) + (result & 1);    // divide by 2 and round
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI CC
void sendMidiCC(int channel, int controller, int value) {

  // check channel mute
  if (channelMute[channel] == 1 && channelSolo[channel] != 1) return;
  
  // check channel solo
  int solo = 0;
  for(int i = 0; i < channelsOut; i++) 
  {
    solo = solo + channelSolo[i];
  }
  if (solo > 0 && channelSolo[channel] != 1) return; 


  if (isTRS(channel)) {
    MIDI.sendControlChange(controller, value, channel+1);
    #ifdef USB_DEVICE
      usbMIDI.sendControlChange(controller, value, channel+1);
    #endif
  } else {
    #ifdef MK2
      #ifdef MULTIPLEUSBOUT
        for (int i = 0; i < 2; i++) {
          if (* midiDeviceList[i] && channel == i+16) {
            midiDeviceList[i]->sendControlChange(controller, value, i+1);  
          }
        }
      #else
        midiDevice.sendControlChange(controller, value, channel+1-16);
      #endif
    #endif
  }
  blinkLED(1); 
  #ifdef DEBUG  
    Serial.print("Sending MIDI CC: ");
    Serial.print(controller); Serial.print(", ");
    Serial.print("Ch: "); Serial.print(channel+1); Serial.print(", ");
    Serial.print("Val: "); Serial.println(value); 
  #endif 
}