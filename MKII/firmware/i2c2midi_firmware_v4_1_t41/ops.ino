// -------------------------------------------------------------------------------------------
// OPs
// -------------------------------------------------------------------------------------------


 void opFunctions(bool isRequest, uint8_t data[]) {
  uint8_t op = data[0];
  switch (op) {

    // MIDI out: Settings
    case 1:     if (isRequest) op_I2M_TIME_get(data);      break;
    case 2:                    op_I2M_TIME_set(data);      break;
    case 3:     if (isRequest) op_I2M_SHIFT_get(data);     break;
    case 4:                    op_I2M_SHIFT_set(data);     break;
    case 5:     if (isRequest) op_I2M_REP_get(data);       break;
    case 6:                    op_I2M_REP_set(data);       break;
    case 7:     if (isRequest) op_I2M_RAT_get(data);       break;
    case 8:                    op_I2M_RAT_set(data);       break;
    case 9:     if (isRequest) op_I2M_MIN_get(data);       break;
    case 10:                   op_I2M_MIN_set(data);       break;
    case 11:    if (isRequest) op_I2M_MAX_get(data);       break;
    case 12:                   op_I2M_MAX_set(data);       break;
    
    // MIDI out: Notes
    case 20:                   op_I2M_NOTE(data);          break;
    case 21:                   op_I2M_NOTE_O(data);        break;
    case 22:                   op_I2M_PANIC(data);         break;
    
    // MIDI out: Chord
    case 30:                   op_I2M_C(data);             break;
    case 31:                   op_I2M_C_ADD(data);         break;
    case 32:                   op_I2M_C_RM(data);          break;
    case 33:                   op_I2M_C_CLR(data);         break;
    case 34:    if (isRequest) op_I2M_C_L_get(data);       break;
    case 35:                   op_I2M_C_L_set(data);       break;
    case 36:    if (isRequest) op_I2M_C_INV_get(data);     break;
    case 37:                   op_I2M_C_INV_set(data);     break;
    case 38:    if (isRequest) op_I2M_C_REV_get(data);     break;
    case 39:                   op_I2M_C_REV_set(data);     break;  
    case 155:   if (isRequest) op_I2M_C_ROT_get(data);     break;
    case 156:                  op_I2M_C_ROT_set(data);     break;
    case 150:   if (isRequest) op_I2M_C_STR_get(data);     break;
    case 151:                  op_I2M_C_STR_set(data);     break;
    case 152:                  op_I2M_C_INS(data);         break;
    case 153:                  op_I2M_C_DEL(data);         break;
    case 154:                  op_I2M_C_SET(data);         break;
    
    // MIDI out: CC
    case 40:                   op_I2M_CC(data);            break;
    case 41:    if (isRequest) op_I2M_CC_OFF_get(data);    break;
    case 42:                   op_I2M_CC_OFF_set(data);    break;
    case 43:    if (isRequest) op_I2M_CC_SLEW_get(data);   break;
    case 44:                   op_I2M_CC_SLEW_set(data);   break;
    case 45:                   op_I2M_CC_SET(data);        break;
    
    // MIDI out: NRPN
    case 50:                   op_I2M_NRPN(data);          break;
    case 51:    if (isRequest) op_I2M_NRPN_OFF_get(data);  break;
    case 52:                   op_I2M_NRPN_OFF_set(data);  break;
    case 53:    if (isRequest) op_I2M_NRPN_SLEW_get(data); break;
    case 54:                   op_I2M_NRPN_SLEW_set(data); break;
    case 55:                   op_I2M_NRPN_SET(data);      break;
    
    // MIDI out: misc
    case 60:                   op_I2M_PRG(data);           break;
    case 61:                   op_I2M_PB(data);            break;
    case 62:                   op_I2M_AT(data);            break;
    case 63:                   op_I2M_CLK(data);           break;
    case 64:                   op_I2M_START(data);         break;
    case 65:                   op_I2M_STOP(data);          break;
    case 66:                   op_I2M_CONT(data);          break;
    
    // ----------------------------------------------------------
    
    // MIDI in: Settings
    case 100:                  op_I2M_Q_LATCH(data);       break;
    
    // MIDI in: Notes
    case 110:   if (isRequest) op_I2M_Q_NOTE(data);        break;
    case 111:   if (isRequest) op_I2M_Q_VEL(data);         break;
    
    // MIDI in: CCs
    case 120:   if (isRequest) op_I2M_Q_CC(data);          break;
    
    // MIDI in: Get latest 
    case 130:   if (isRequest) op_I2M_Q_LCH(data);         break;
    case 131:   if (isRequest) op_I2M_Q_LN(data);          break;
    case 132:   if (isRequest) op_I2M_Q_LV(data);          break;
    case 133:   if (isRequest) op_I2M_Q_LO(data);          break;
    case 134:   if (isRequest) op_I2M_Q_LC(data);          break;
    case 135:   if (isRequest) op_I2M_Q_LCC(data);         break;

  }
}


// -------------------------------------------------------------------------------------------
// MIDI out: Settings


void op_I2M_TIME_get(uint8_t data[]) {
  const uint8_t response_MSB = currentNoteDuration >> 8;
  const uint8_t response_LSB = currentNoteDuration & 0xff;
  Wire.write(response_MSB);
  Wire.write(response_LSB);
  #ifdef DEBUG
    Serial.println("op_I2M_TIME_get");
    Serial.print(" note duration: "); Serial.println(currentNoteDuration);
  #endif  
}


void op_I2M_TIME_set(uint8_t data[]) { 
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value < 0) return;
  currentNoteDuration = value;
  #ifdef DEBUG
    Serial.println("op_I2M_TIME_set");
    Serial.print(" note duration: "); Serial.println(currentNoteDuration);
  #endif  
}


void op_I2M_SHIFT_get(uint8_t data[]) {
  if (currentNoteShift < -127 || currentNoteShift > 127) {
    Wire.write(0);  
  } else {
    Wire.write(currentNoteShift);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_SHIFT_get");
    Serial.print(" note shift: "); Serial.println(currentNoteShift);
  #endif
}


void op_I2M_SHIFT_set(uint8_t data[]) {
  int8_t value = data[1];
  if (value < -127) value = -127;
  else if (value > 127) value = 127;
  currentNoteShift = value;
  #ifdef DEBUG
    Serial.println("op_I2M_SHIFT_get");
    Serial.print(" note shift: "); Serial.println(currentNoteShift);
  #endif
}


void op_I2M_REP_get(uint8_t data[]) {
  const byte response = currentRepetition - 1; // -1 because internally 1 means "play note once" 
  if (response < 0 || response > 127) {
    Wire.write(255);
    Wire.write(255);
  } else {
    Wire.write(0);
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_REP_get");
    Serial.print(" note repetition: "); Serial.println(response);
  #endif
}


void op_I2M_REP_set(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value < 0) value = 0;
  else if (value > 127) value = 127;
  currentRepetition = value + 1; // +1 because internally 1 means "play note once"    
  #ifdef DEBUG
    Serial.println("op_I2M_REP_set");
    Serial.print(" note repetition: "); Serial.println(currentRepetition);
  #endif
}


void op_I2M_RAT_get(uint8_t data[]) {
  const byte response = currentRatcheting - 1; // -1 because internally 1 means "play note once" 
  if (response < 0 || response > 127) {
    Wire.write(255);
    Wire.write(255);
  } else {
    Wire.write(0);
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_RAT_get");
    Serial.print(" note ratcheting: "); Serial.println(response);
  #endif
}


void op_I2M_RAT_set(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value < 0) value = 0;
  else if (value > 127) value = 127;
  currentRatcheting = value + 1; // +1 because internally 1 means "play note once"
  #ifdef DEBUG
    Serial.println("op_I2M_RAT_set");
    Serial.print(" note ratcheting: "); Serial.println(currentRatcheting);
  #endif
}


void op_I2M_MIN_get(uint8_t data[]) {
  Wire.write(noteLowerLimit); 
}


void op_I2M_MIN_set(uint8_t data[]) {
  int8_t value = data[1];
  if (value < 0) value = 0;
  else if (value > 127) value = 127;
  noteLowerLimit = value;
}


void op_I2M_MAX_get(uint8_t data[]) {
  Wire.write(noteUpperLimit); 
}


void op_I2M_MAX_set(uint8_t data[]) {
  int8_t value = data[1];
  if (value < 0) value = 0;
  else if (value > 127) value = 127;
  noteUpperLimit = value;
}


// -------------------------------------------------------------------------------------------
// MIDI out: Notes


void op_I2M_PANIC(uint8_t data[]) {
  panic();
}


void op_I2M_NOTE(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t noteNumber = data[2];
  const int8_t velocity = data[3];
  #ifdef DEBUG
    Serial.println("op_I2M_NOTE"); 
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" noteNumber: "); Serial.print(noteNumber);
    Serial.print(" velocity: "); Serial.println(velocity);
  #endif
  midiNoteOn(channel, noteNumber, velocity, currentNoteDuration);
}


void op_I2M_NOTE_O(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t noteNumber = data[2];
  #ifdef DEBUG
    Serial.println("op_I2M_NOTE_O");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" noteNumber: "); Serial.println(noteNumber);
  #endif
  midiNoteOff(channel, noteNumber);
}


// -------------------------------------------------------------------------------------------
// MIDI out: Chord


void op_I2M_C(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t chordNumber = data[2] - 1;
  const int8_t noteNumber = data[3];
  const int8_t velocity = data[4];
  #ifdef DEBUG
    Serial.println("op_I2M_C");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" root note: "); Serial.print(noteNumber);
    Serial.print(" velocity: "); Serial.println(velocity);
  #endif
  playChord(channel, noteNumber, velocity, currentNoteDuration, chordNumber);
}


void op_I2M_C_ADD(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t noteNumber = data[2]; 
  #ifdef DEBUG
    Serial.println("op_I2M_C_ADD");
  #endif
  
  if (chordNumber > 0) {
    insertIntoChord(chordNumber - 1, 0, noteNumber);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      insertIntoChord(i, 0, noteNumber);
    }
  }  

}


void op_I2M_C_RM(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t noteNumber = data[2];
  #ifdef DEBUG
    Serial.println("op_I2M_C_RM");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" note: "); Serial.println(noteNumber);
  #endif
  
  if (chordNumber > 0) {
    removeFromChord(chordNumber - 1, noteNumber);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      removeFromChord(i, noteNumber);
    }
  }  

}


void op_I2M_C_CLR(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  #ifdef DEBUG
    Serial.println("op_I2M_C_CLR");
    Serial.print(" chordNumber: "); Serial.println(chordNumber);
  #endif
  
  if (chordNumber > 0) {
    clearChord(chordNumber - 1);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      clearChord(i);
    }
  }  
  
}


void op_I2M_C_L_get(uint8_t data[]) {
  const int8_t chordNumber = data[1] - 1;
  const byte response = chordLength[chordNumber];
  if (chordNumber < 0 || chordNumber >= maxChords || response < 1 || response > 8) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_C_L_get");
    Serial.print(" chord length: "); Serial.println(response);
  #endif
}


void op_I2M_C_L_set(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  int8_t value = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (value < 1 || value > 8) return;
  if (chordNumber > 0) {
    // user defined chord length must not be longer than notes in the chord
    if (value > chordNoteCount[chordNumber-1]) {
      value = chordNoteCount[chordNumber-1];
    }
    chordLength[chordNumber - 1] = value;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      if (value < chordNoteCount[i]) {
        chordLength[i] = value;
      }
    }
  }   
  #ifdef DEBUG
    Serial.println("op_I2M_C_L_set");
    Serial.print(" chord lengthh: "); Serial.println(chordLength[chordNumber-1]);
  #endif
}


void op_I2M_C_INV_get(uint8_t data[]) {
  const int8_t chordNumber = data[1] - 1;
  const int response = chordInversion[chordNumber];
  if (chordNumber < 0 || chordNumber >= maxChords || response < -32 || response > 32) {
    Wire.write(0);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_C_INV_get");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord inversion: "); Serial.println(response);
  #endif
}


void op_I2M_C_INV_set(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  int8_t value = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (value < -32 || value > 32) return;
  
  if (chordNumber > 0) {
    chordInversion[chordNumber - 1] = value;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordInversion[i] = value;
    }
  } 
  
  #ifdef DEBUG
    Serial.println("op_I2M_C_INV_set");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord inversion: "); Serial.println(chordInversion[chordNumber]);
  #endif  
}


void op_I2M_C_REV_get(uint8_t data[]) {
  const int8_t chordNumber = data[1] - 1;
  const int response = chordReverse[chordNumber];
  if (chordNumber < 0 || chordNumber >= maxChords) {
    Wire.write(0);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_C_REV_get");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord reverse: "); Serial.println(response);
  #endif
}
void op_I2M_C_REV_set(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  int8_t value = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  bool valueBool = mod(value, 2);

  if (chordNumber > 0) {
    chordReverse[chordNumber - 1] = valueBool;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordReverse[i] = valueBool;
    }
  } 
  
  #ifdef DEBUG
    Serial.println("op_I2M_C_REV_set");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord reverse: "); Serial.println(valueBool);
  #endif
}


void op_I2M_C_ROT_get(uint8_t data[]) {
  const int8_t chordNumber = data[1] - 1;
  const int response = chordRotate[chordNumber];
  if (chordNumber < 0 || chordNumber >= maxChords) {
    Wire.write(0);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_C_ROT_get");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord rotation: "); Serial.println(response);
  #endif
}
void op_I2M_C_ROT_set(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  int8_t value = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (value < -8 || value > 8) return;

  if (chordNumber > 0) {
    chordRotate[chordNumber - 1] = value;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordRotate[i] = value;
    }
  } 
    
  #ifdef DEBUG
    Serial.println("op_I2M_C_ROT_set");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord rotation: "); Serial.println(value);
  #endif  
}


void op_I2M_C_STR_get(uint8_t data[]) {
  const int8_t chordNumber = data[1] - 1;
  if (chordNumber < 0 || chordNumber >= maxChords) {
    Wire.write(-1);
    Wire.write(-1);
  } else {
    const uint8_t response_MSB = chordStrumming[chordNumber] >> 8;
    const uint8_t response_LSB = chordStrumming[chordNumber] & 0xff;
    Wire.write(response_MSB);
    Wire.write(response_LSB);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_C_STR_get");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord strumming: "); Serial.println(chordStrumming[chordNumber]);
  #endif  
}


void op_I2M_C_STR_set(uint8_t data[]) { 
  const int8_t chordNumber = data[1];
  const uint8_t value_MSB = data[2];
  const uint8_t value_LSB = data[3];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  const int16_t value = (value_MSB << 8 ) | (value_LSB & 0xff);
  if (value < 0) return;

  if (chordNumber > 0) {
    chordStrumming[chordNumber - 1] = value;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordStrumming[i] = value;  
    }
  } 
  
  #ifdef DEBUG
    Serial.println("op_I2M_C_STR_set");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" chord strumming: "); Serial.println(chordStrumming[chordNumber]);
  #endif  
}


void op_I2M_C_INS(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t index = chordNoteCount[chordNumber] - data[2];
  const int8_t noteNumber = data[3];
  #ifdef DEBUG
    Serial.println("op_I2M_C_INS");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" index: "); Serial.print(index);
    Serial.print(" noteNumber: "); Serial.println(noteNumber);
  #endif  
  
  if (chordNumber > 0) {
    insertIntoChord(chordNumber - 1, index, noteNumber);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      insertIntoChord(i, index, noteNumber);
    }
  } 

}


void op_I2M_C_DEL(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t index = chordNoteCount[chordNumber] - 1 - data[2];
  #ifdef DEBUG
    Serial.println("op_I2M_C_DEL");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" index: "); Serial.println(index);
  #endif  
  
  if (chordNumber > 0) {
    deleteFromChord(chordNumber - 1, index);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      deleteFromChord(i, index);
    }
  } 

}


void op_I2M_C_SET(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t index = chordNoteCount[chordNumber] - 1 - data[2];
  const int8_t noteNumber = data[3];
  #ifdef DEBUG
    Serial.println("op_I2M_C_SET");
    Serial.print(" chordNumber: "); Serial.print(chordNumber);
    Serial.print(" index: "); Serial.print(index);
    Serial.print(" noteNumber: "); Serial.println(noteNumber);
  #endif  
  
  if (chordNumber > 0) {
    setChord(chordNumber - 1, index, noteNumber);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      setChord(i, index, noteNumber);
    }
  }
  
}


// -------------------------------------------------------------------------------------------
// MIDI out: CC


void op_I2M_CC(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t controller = data[2];
  const uint8_t value_MSB = data[3];
  const uint8_t value_LSB = data[4];
  if (value_MSB < 0 || value_MSB > 127) return;
  if (value_LSB < 0 || value_LSB > 127) return;
  const int value = (value_MSB << 7) + value_LSB;
  #ifdef DEBUG
    Serial.println("op_I2M_CC");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" value: "); Serial.print(value);
    Serial.print(" value scaled down: "); Serial.println(scaleDown(value));
  #endif
  midiCC(channel, controller, value, true);
}


void op_I2M_CC_OFF_get(uint8_t data[]) {
  int8_t channel = data[1];
  int8_t controller = data[2];
  if (channel < 0 || channel >= channelsOut) channel = 0;
  if (controller < 0 || controller > 127) controller = 0;
  const int16_t response = CCs[channel][controller][2];
  Wire.write(response >> 8);
  Wire.write(response & 0xff);
  #ifdef DEBUG
    Serial.println("op_I2M_CC_OFF_get");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" offset: "); Serial.println(response);
  #endif
}


void op_I2M_CC_OFF_set(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t controller = data[2];
  const uint8_t value_MSB = data[3];
  const uint8_t value_LSB = data[4];
  if (channel < 0 || channel >= channelsOut) return;
  if (controller < 0 || controller > 127) return;
  int16_t offset = (value_MSB << 8) + value_LSB;
  if (offset < -127 * 129)
    offset = -127 * 129;
  else if (offset > 127 * 129)
    offset = 127 * 129;
  CCs[channel][controller][2] = offset;       // store the offset in CCs array (index 2 = offset)
  #ifdef DEBUG
    Serial.println("op_I2M_CC_OFF_set");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" offset: "); Serial.println(offset);
  #endif
}


void op_I2M_CC_SLEW_get(uint8_t data[]) {
  int8_t channel = data[1];
  int8_t controller = data[2];
  if (channel < 0 || channel >= channelsOut) channel = 0;
  if (controller < 0 || controller > 127) controller = 0;
  const int16_t response = CCs[channel][controller][1];
  const int8_t response_MSB = response >> 8;
  const int8_t response_LSB = response & 0xff;
  Wire.write(response_MSB);
  Wire.write(response_LSB);
  #ifdef DEBUG
    Serial.println("op_I2M_CC_SLEW_get");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" slew time: "); Serial.println(response);
  #endif
}


void op_I2M_CC_SLEW_set(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t controller = data[2];
  const uint8_t value_MSB = data[3];
  const uint8_t value_LSB = data[4];
  if (channel < 0 || channel >= channelsOut) return;
  if (controller < 0 || controller > 127) return;
  const int16_t value = (int16_t)(value_MSB << 8 ) | (value_LSB & 0xff);
  if (value < 0) return;
  CCs[channel][controller][1] = value;     // store the slew time in CCs array (index 1 = slew time)
  #ifdef DEBUG
    Serial.println("op_I2M_CC_SLEW_set");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" slew time: "); Serial.println(value);
  #endif
}


void op_I2M_CC_SET(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t controller = data[2];
  const uint8_t value_MSB = data[3];
  const uint8_t value_LSB = data[4];
  if (value_MSB < 0 || value_MSB > 127) return;
  if (value_LSB < 0 || value_LSB > 127) return;
  const int value = (value_MSB << 7) + value_LSB;
  #ifdef DEBUG
    Serial.println("op_I2M_CC_SET");
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" value: "); Serial.print(value);
    Serial.print(" value scaled down: "); Serial.println(scaleDown(value));
  #endif
  midiCC(channel, controller, value, false);

}


// -------------------------------------------------------------------------------------------
// MIDI out: NRPN


void op_I2M_NRPN(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t controller_MSB = data[2];
  const uint8_t controller_LSB = data[3];
  const uint8_t value_MSB = data[4];
  const uint8_t value_LSB = data[5];
  if (controller_MSB < 0 || controller_MSB > 127) return;
  if (controller_LSB < 0 || controller_LSB > 127) return;
  const int controller = (controller_MSB << 7) + controller_LSB;
  const int value = (value_MSB << 7) + value_LSB;
  #ifdef DEBUG
    Serial.println("op_I2M_NRPN");
    Serial.print(" channel: "); Serial.print(channel+1);
    Serial.print(" MSB: "); Serial.print(controller_MSB);
    Serial.print(" LSB: "); Serial.print(controller_LSB);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" value: "); Serial.println(value);
  #endif
  NRPN(channel, controller, value, true);
}


void op_I2M_NRPN_OFF_get(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t controller_MSB = data[2];
  const uint8_t controller_LSB = data[3];
  const int controller = (controller_MSB << 7) + controller_LSB;
  int response;
  if (channel < 0 || channel >= channelsOut || controller_MSB < 0 || controller_MSB > 127 || controller_LSB < 0 || controller_LSB > 127)
    response = 0;
  else
    response = getNRPNvalue(channel, controller, 2);   // index 2 is offset
  Wire.write(response >> 8);
  Wire.write(response & 0xFF);
  #ifdef DEBUG
    Serial.println("op_I2M_NRPN_OFF_get");
    Serial.print(" channel: "); Serial.print(channel+1);
    Serial.print(" MSB: "); Serial.print(controller_MSB);
    Serial.print(" LSB: "); Serial.print(controller_LSB);
    Serial.print(" offset: "); Serial.println(response);
  #endif
}


void op_I2M_NRPN_OFF_set(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t controller_MSB = data[2];
  const uint8_t controller_LSB = data[3];
  const uint8_t offset_MSB = data[4];
  const uint8_t offset_LSB = data[5];
  if (channel < 0 || channel >= channelsOut) return;
  if (controller_MSB < 0 || controller_MSB > 127) return;
  if (controller_LSB < 0 || controller_LSB > 127) return;
  const int controller = (controller_MSB << 7) + controller_LSB;
  int16_t offset = (offset_MSB << 8) | offset_LSB;  
  if (offset < -16384)
    offset = -16384;
  else if (offset > 16384)
    offset = 16384;
  // store data in the NRPNs array
  const int8_t position = getNextFreeNRPN(channel, controller);
  NRPNs[position][0] = channel;
  NRPNs[position][1] = controller;
  NRPNs[position][2] = offset;
  #ifdef DEBUG
    Serial.println("op_I2M_NRPN_OFF_set");
    Serial.print(" slot: "); Serial.print(position);
    Serial.print(" channel: "); Serial.print(channel+1);
    Serial.print(" MSB: "); Serial.print(controller_MSB);
    Serial.print(" LSB: "); Serial.print(controller_LSB);
    Serial.print(" offset: "); Serial.println(offset);
  #endif
}


void op_I2M_NRPN_SLEW_get(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t controller_MSB = data[2];
  const uint8_t controller_LSB = data[3];
  const int controller = (controller_MSB << 7) + controller_LSB;
  int response;
  if (channel < 0 || channel >= channelsOut || controller_MSB < 0 || controller_MSB > 127 || controller_LSB < 0 || controller_LSB > 127)
    response = 0;
  else
   response = getNRPNvalue(channel, controller, 3); // index 3 is slew
  const uint8_t response_MSB = response >> 8;
  const uint8_t response_LSB = response & 0xFF;
  Wire.write(response_MSB);
  Wire.write(response_LSB);
  #ifdef DEBUG
    Serial.println("op_I2M_NRPN_SLEW_get");
    Serial.print(" channel: "); Serial.print(channel+1);
    Serial.print(" MSB: "); Serial.print(controller_MSB);
    Serial.print(" LSB: "); Serial.print(controller_LSB);
    Serial.print(" slew: "); Serial.println(response);
  #endif 
}


void op_I2M_NRPN_SLEW_set(uint8_t data[]) {
  const int8_t channel = data[1];
  if (channel < 0 || channel >= channelsOut) return;
  const uint8_t controller_MSB = data[2];
  const uint8_t controller_LSB = data[3];
  const uint8_t slew_MSB = data[4];
  const uint8_t slew_LSB = data[5];
  const int controller = (controller_MSB << 7) + controller_LSB;
  const int16_t slew = (int16_t)(slew_MSB << 8 ) | (slew_LSB & 0xff);
  if (controller < 0 || controller > 16383) return;
  if (slew < 0) return;
  // store data in the NRPNs array
  const int8_t position = getNextFreeNRPN(channel, controller);
  NRPNs[position][0] = channel;
  NRPNs[position][1] = controller;
  NRPNs[position][3] = slew;
  #ifdef DEBUG
    Serial.println("op_I2M_NRPN_SLEW_set");
    Serial.print(" slot: "); Serial.print(position);
    Serial.print(" channel: "); Serial.print(channel+1);
    Serial.print(" MSB: "); Serial.print(controller_MSB);
    Serial.print(" LSB: "); Serial.print(controller_LSB);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" slew: "); Serial.println(slew);
  #endif
}


void op_I2M_NRPN_SET(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t controller_MSB = data[2];
  const uint8_t controller_LSB = data[3];
  const uint8_t value_MSB = data[4];
  const uint8_t value_LSB = data[5];
  if (controller_MSB < 0 || controller_MSB > 127) return;
  if (controller_LSB < 0 || controller_LSB > 127) return;
  const int controller = (controller_MSB << 7) + controller_LSB;
  const int value = (value_MSB << 7) + value_LSB;
  #ifdef DEBUG
    Serial.println("op_I2M_NRPN_SET");
    Serial.print(" channel: "); Serial.print(channel+1);
    Serial.print(" MSB: "); Serial.print(controller_MSB);
    Serial.print(" LSB: "); Serial.print(controller_LSB);
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" value: "); Serial.println(value);
  #endif
  NRPN(channel, controller, value, false);
}


// -------------------------------------------------------------------------------------------
// MIDI out: Misc


void op_I2M_PRG(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t program = data[2];
  #ifdef DEBUG
    Serial.println("op_I2M_PRG"); 
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" program: "); Serial.println(program);
  #endif
  sendMidiProgramChange(channel, program);
}


void op_I2M_PB(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t value_MSB = data[2];
  const uint8_t value_LSB = data[3];
  int16_t value = (value_MSB << 8) | value_LSB;
  #ifdef DEBUG
    Serial.println("op_I2M_PB"); 
    Serial.print(" value: "); Serial.println(value);
  #endif
  sendMidiPitchBend(channel, value);
}


void op_I2M_AT(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t value = data[2];
  #ifdef DEBUG
    Serial.println("op_I2M_AT"); 
    Serial.print(" channel: "); Serial.print(channel);
    Serial.print(" value: "); Serial.println(value);
  #endif
  sendMidiAftertouch(channel, value);
}


void op_I2M_CLK(uint8_t data[]) {
  sendMidiClock();
  #ifdef DEBUG
    Serial.println("op_I2M_CLK");
  #endif
}


void op_I2M_START(uint8_t data[]) {
  sendMidiClockStart();
  #ifdef DEBUG
    Serial.println("op_I2M_START");
  #endif
}


void op_I2M_STOP(uint8_t data[]) {
  sendMidiClockStop();
  #ifdef DEBUG
    Serial.println("op_I2M_STOP");
  #endif
}


void op_I2M_CONT(uint8_t data[]) {
  sendMidiClockContinue(); 
  #ifdef DEBUG
    Serial.println("op_I2M_CONT");
  #endif
}


// -------------------------------------------------------------------------------------------
// MIDI in: Settings


void op_I2M_Q_LATCH(uint8_t data[]) {
  const int8_t value = data[1];
  setLatch(value);
}


// -------------------------------------------------------------------------------------------
// MIDI in: Notes


void op_I2M_Q_NOTE(uint8_t data[]) {
  int8_t channel = data[1];
  int8_t n = data[2] % noteHistoryInLength;
  const int8_t noteNumber = noteHistoryIn[channel][n][0];        // array index 0 = note number
  if (noteNumber < 0 || noteNumber > 127) {
    Wire.write(0);
  } else {
    Wire.write(noteNumber);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_NOTE");
    Serial.print(" channel: "); Serial.print(channel);  
    Serial.print(" index: "); Serial.print(n);
    Serial.print(" note number: "); Serial.println(noteNumber);  
  #endif
}


void op_I2M_Q_VEL(uint8_t data[]) {
  int8_t channel = data[1];
  int8_t n = data[2] % noteHistoryInLength;
  const int8_t velocity = noteHistoryIn[channel][n][1];          // array index 0 = note number
  if (velocity < 1 || velocity > 127) {
    Wire.write(0);
  } else {
    Wire.write(velocity);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_VEL");
    Serial.print(" channel: "); Serial.print(channel);  
    Serial.print(" index: "); Serial.print(n);
    Serial.print(" velocity: "); Serial.println(velocity);  
  #endif
}


// -------------------------------------------------------------------------------------------
// MIDI in: CCs


void op_I2M_Q_CC(uint8_t data[]) {
  int8_t channel = data[1];
  const int8_t controller = data[2];
  const int8_t value = CCsIn[channel][controller];
  if (value < 0 || value > 127) {
    Wire.write(-1);
  } else {
    Wire.write(value);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_CC");
    Serial.print(" channel: "); Serial.print(channel);  
    Serial.print(" controller: "); Serial.print(controller);
    Serial.print(" value: "); Serial.println(value);  
  #endif
}


// -------------------------------------------------------------------------------------------
// MIDI in: Get latest ...


void op_I2M_Q_LCH(uint8_t data[]) {
  const int8_t response = lastChannelIn;
  if (response < 0 || response > 15) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_LCH");
    Serial.print(" last channel: "); Serial.println(response);  
  #endif
}


void op_I2M_Q_LN(uint8_t data[]) {
  const int8_t response = lastNoteIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_LN");
    Serial.print(" last note: "); Serial.println(response);  
  #endif
}


void op_I2M_Q_LV(uint8_t data[]) {
  const int8_t response = lastVelocityIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_LV");
    Serial.print(" last velocity: "); Serial.println(response);  
  #endif
}


void op_I2M_Q_LO(uint8_t data[]) {
  const int8_t response = lastNoteOffIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_LO");
    Serial.print(" last note off: "); Serial.println(response);  
  #endif
}


void op_I2M_Q_LC(uint8_t data[]) {
  const int8_t response = lastCIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_LC");
    Serial.print(" last controller: "); Serial.println(response);  
  #endif
}


void op_I2M_Q_LCC(uint8_t data[]) {
  const int8_t response = lastCCIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
  #ifdef DEBUG
    Serial.println("op_I2M_Q_LCC");
    Serial.print(" last controller value: "); Serial.println(response);  
  #endif
}
