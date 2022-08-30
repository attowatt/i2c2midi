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
    case 10:                   op_I2M_MIN(data);           break;
    case 12:                   op_I2M_MAX(data);           break;
    
    // MIDI out: Notes
    case 20:                   op_I2M_NOTE(data);          break;
    case 21:                   op_I2M_NOTE_O(data);        break;
    case 22:                   op_I2M_PANIC(data);         break;
    case 23:                   op_I2M_NT(data);            break;
    
    // MIDI out: Chord
    case 30:                   op_I2M_C(data);             break;
    case 31:                   op_I2M_C_ADD(data);         break;
    case 32:                   op_I2M_C_RM(data);          break;
    case 33:                   op_I2M_C_CLR(data);         break;
    case 34:    if (isRequest) op_I2M_C_L_get(data);       break;
    case 35:                   op_I2M_C_L_set(data);       break;
    case 37:                   op_I2M_C_INV(data);         break;
    case 39:                   op_I2M_C_REV(data);         break;  
    case 156:                  op_I2M_C_ROT(data);         break;
    case 151:                  op_I2M_C_STR(data);         break;
    case 152:                  op_I2M_C_INS(data);         break;
    case 153:                  op_I2M_C_DEL(data);         break;
    case 154:                  op_I2M_C_SET(data);         break;
    case 158:                  op_I2M_C_SC(data);          break;
    case 159:                  op_I2M_C_B(data);           break;
    case 160:                  op_I2M_C_TRP(data);         break;
    case 161:                  op_I2M_C_DIS(data);         break;
    case 162:                  op_I2M_C_REF(data);         break;
    case 163:                  op_I2M_C_VCUR(data);        break;
    case 164:                  op_I2M_C_TCUR(data);        break;
    case 165:                  op_I2M_C_DIR(data);         break;
//  coming soon
//  case 166:   if (isRequest) op_I2M_C_QN(data);          break;
//  case 167:   if (isRequest) op_I2M_C_QV(data);          break;
    
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
    
    // MIDI out: Buffer
    case 180:                  op_I2M_B_R(data);           break;
    case 181:                  op_I2M_B_L(data);           break;
    case 182:                  op_I2M_B_START(data);       break;
    case 183:                  op_I2M_B_END(data);         break;
    case 184:                  op_I2M_B_DIR(data);         break;
    case 185:                  op_I2M_B_SPE(data);         break; 
    case 186:                  op_I2M_B_FB(data);          break; 
    case 187:                  op_I2M_B_NSHIFT(data);      break; 
    case 188:                  op_I2M_B_VSHIFT(data);      break; 
    case 189:                  op_I2M_B_TSHIFT(data);      break; 
    case 190:                  op_I2M_B_NOFF(data);        break; 
    case 191:                  op_I2M_B_VOFF(data);        break; 
    case 192:                  op_I2M_B_TOFF(data);        break; 
    case 193:                  op_I2M_B_CLEAR(data);       break; 
    case 194:                  op_I2M_B_MODE(data);        break; 
    
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

    // for development
    case 255:                  op_I2M_TEST(data);          break;

  }
}


// -------------------------------------------------------------------------------------------
// MIDI out: Settings


void op_I2M_TIME_get(uint8_t data[]) {
  const int8_t channel = data[1] - 1;
  if (channel < 0 || channel >= channelsOut) return;
  const uint8_t response_MSB = currentNoteDuration[channel] >> 8;
  const uint8_t response_LSB = currentNoteDuration[channel] & 0xff;
  Wire.write(response_MSB);
  Wire.write(response_LSB);
}


void op_I2M_TIME_set(uint8_t data[]) { 
  const int8_t channel = data[1];
  const uint8_t value_MSB = data[2];
  const uint8_t value_LSB = data[3];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (channel < 0 || channel > channelsOut) return;
  if (value < 0) return;
  if (channel > 0) {
    currentNoteDuration[channel - 1] = value;
  } 
  else if (channel == 0) {
    for (int i = 0; i < channelsOut; i++) {
      currentNoteDuration[i] = value;
    }
  }
}


void op_I2M_SHIFT_get(uint8_t data[]) {
  const int8_t channel = data[1] - 1;
  if (channel < 0 || channel >= channelsOut) return;
  if (currentNoteShift[channel] < -127 || currentNoteShift[channel] > 127) {
    Wire.write(0);  
  } else {
    Wire.write(currentNoteShift[channel]);
  }
}


void op_I2M_SHIFT_set(uint8_t data[]) {
  const int8_t channel = data[1];
  int8_t value = data[2];
  if (channel < 0 || channel > channelsOut) return;
  if (value < -127) value = -127;
  else if (value > 127) value = 127;
  if (channel > 0) {
    currentNoteShift[channel - 1] = value;
  }
  else if (channel == 0) {
    for (int i = 0; i < channelsOut; i++) {
      currentNoteShift[i] = value;
    }
  }
}


void op_I2M_REP_get(uint8_t data[]) {
  const int8_t channel = data[1] - 1;
  if (channel < 0 || channel >= channelsOut) return;
  const byte response = currentRepetition[channel] - 1; // -1 because internally 1 means "play note once" 
  if (response < 0 || response > 127) {
    Wire.write(255);
    Wire.write(255);
  } else {
    Wire.write(0);
    Wire.write(response);
  }
}


void op_I2M_REP_set(uint8_t data[]) {
  const int8_t channel = data[1];
  if (channel < 0 || channel > channelsOut) return;
  const uint8_t value_MSB = data[2];
  const uint8_t value_LSB = data[3];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value < 0) value = 0;
  else if (value > 127) value = 127;
  if (channel > 0) {
    currentRepetition[channel - 1] = value + 1; // +1 because internally 1 means "play note once"    
  } 
  else if (channel == 0) {
    for (int i = 0; i < channelsOut; i++) {
      currentRepetition[i] = value + 1;
    }
  }
}


void op_I2M_RAT_get(uint8_t data[]) {
  const int8_t channel = data[1] - 1;
  if (channel < 0 || channel >= channelsOut) return;
  const byte response = currentRatcheting[channel] - 1; // -1 because internally 1 means "play note once" 
  if (response < 0 || response > 127) {
    Wire.write(255);
    Wire.write(255);
  } else {
    Wire.write(0);
    Wire.write(response);
  }
}


void op_I2M_RAT_set(uint8_t data[]) {
  const int8_t channel = data[1];
  if (channel < 0 || channel > channelsOut) return;
  const uint8_t value_MSB = data[2];
  const uint8_t value_LSB = data[3];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value < 0) value = 0;
  else if (value > 127) value = 127;
  if (channel > 0) {
    currentRatcheting[channel - 1] = value + 1; // +1 because internally 1 means "play note once"
  } 
  else if (channel == 0) {
    for (int i = 0; i < channelsOut; i++) {
      currentRatcheting[i] = value + 1;
    }
  }
}


void op_I2M_MIN(uint8_t data[]) {
  const int8_t channel = data[1];
  int8_t value = data[2];
  int8_t mode = data[3];
  if (channel < 0 || channel > channelsOut) return;
  if (value < 0) value = 0;
  if (mode < 0 || mode > 3) return;
  else if (value > 127) value = 127;
  if (channel > 0) {
    noteLimitMode[channel - 1] = mode;
    noteLowerLimit[channel - 1] = value;
  } 
  else if (channel == 0) {
    for (int i = 0; i < channelsOut; i++) {
      noteLimitMode[i] = mode;
      noteLowerLimit[i] = value;
    }
  }
}


void op_I2M_MAX(uint8_t data[]) {
  const int8_t channel = data[1];
  int8_t value = data[2];
  int8_t mode = data[3];
  if (channel < 0 || channel > channelsOut) return;
  if (value < 0) value = 0;
  if (mode < 0 || mode > 3) return;
  else if (value > 127) value = 127;
  if (channel > 0) {
    noteLimitMode[channel - 1] = mode;
    noteUpperLimit[channel - 1] = value;
  } 
  else if (channel == 0) {
    for (int i = 0; i < channelsOut; i++) {
      noteLimitMode[i] = mode;
      noteUpperLimit[i] = value;
    }
  }
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
  midiNoteOn(channel, noteNumber, velocity, currentNoteDuration[channel], 1, 99, 0);
}


void op_I2M_NOTE_O(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t noteNumber = data[2];
  midiNoteOff(channel, noteNumber);
}


void op_I2M_NT(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t noteNumber = data[2];
  const int8_t velocity = data[3];
  const uint8_t duration_MSB = data[4];
  const uint8_t duration_LSB = data[5];
  const int16_t duration = (duration_MSB << 8 ) | duration_LSB;
  if (duration < 0) return;
  midiNoteOn(channel, noteNumber, velocity, duration, 1, 99, 0); 
}


// -------------------------------------------------------------------------------------------
// MIDI out: Chord


void op_I2M_C(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t chordNumber = data[2] - 1;
  const int8_t noteNumber = data[3];
  const int8_t velocity = data[4];
  playChord(channel, noteNumber, velocity, currentNoteDuration[channel], chordNumber, false, false, 0);
}


void op_I2M_C_ADD(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t noteNumber = data[2];  
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
}


void op_I2M_C_SC(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t scaleChordNumber = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (scaleChordNumber < 0 || scaleChordNumber > maxChords) return;
  
  const int start = chordNumber ? chordNumber - 1 : 0;
  const int end   = chordNumber ? chordNumber : maxChords;

  for (int chord = start; chord < end; chord++) {
    // set scale based on a chord
    if (scaleChordNumber > 0) {
      currentScaleChord[chord] = scaleChordNumber - 1;
      setCurrentScale(chord);
    } 
    // reset scale to 12 tone chromatic
    else if (scaleChordNumber == 0) {
      for (int i = 0; i < maxChords; i++) {
        currentScaleChord[i] = 0;
        currentScaleLength[i] = 12;
        for (int j = 0; j < 12; j++) {
          currentScale[i][j] = j;
        }
      }
    }
  }
}


void op_I2M_C_B(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const uint16_t binary = (data[2] << 8 ) | (data[3] & 0xff);
  if (chordNumber < 0 || chordNumber > maxChords) return;
  const int start = chordNumber ? chordNumber - 1 : 0;
  const int end   = chordNumber ? chordNumber : maxChords;
  for (int chord = start; chord < end; chord++) {
    clearChord(chord);
    for (int i = 0; i < 16; i++) {
      if (binary & (1 << i)) insertIntoChord(chord, 0, i);
    }
  }
}


void op_I2M_C_INV(uint8_t data[]) {
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
}


void op_I2M_C_REV(uint8_t data[]) {
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
}


void op_I2M_C_ROT(uint8_t data[]) {
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
}


void op_I2M_C_TRP(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t transpose = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (chordNumber > 0) {
    chordShift[chordNumber - 1] = transpose;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordShift[i] = transpose;
    }
  } 
}


void op_I2M_C_DIS(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t value = data[2];
  const int8_t point = data[3];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (chordNumber > 0) {
    chordStretch[chordNumber - 1][0] = value;
    chordStretch[chordNumber - 1][1] = point;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordStretch[i][0] = value;
      chordStretch[i][1] = point;
    }
  }  
}

void op_I2M_C_REF(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t value = data[2];
  const int8_t point = data[3];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (chordNumber > 0) {
    chordReflection[chordNumber - 1][0] = value;
    chordReflection[chordNumber - 1][1] = point;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordReflection[i][0] = value;
      chordReflection[i][1] = point;
    }
  } 
}


void op_I2M_C_STR(uint8_t data[]) { 
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
}


void op_I2M_C_INS(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t index = chordNoteCount[chordNumber - 1] - data[2];
  const int8_t noteNumber = data[3];
  if (chordNumber > 0) {
    insertIntoChord(chordNumber - 1, index, noteNumber);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      insertIntoChord(i, chordNoteCount[i] - data[2], noteNumber);
    }
  } 
}


void op_I2M_C_DEL(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t index = chordNoteCount[chordNumber] - 1 - data[2];
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
  if (chordNumber > 0) {
    setChord(chordNumber - 1, index, noteNumber);
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      setChord(i, index, noteNumber);
    }
  } 
}


void op_I2M_C_VCUR(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t curve = data[2];
  int16_t start = (data[3] << 8) + data[4];
  int16_t end = (data[5] << 8) + data[6];
  if (start < 0 || end < 0 ) return;
  if (chordNumber < 0 || chordNumber > maxChords) return;
  
  if (chordNumber > 0) {
    curveVelocity[chordNumber-1][0] = curve;
    curveVelocity[chordNumber-1][1] = start;
    curveVelocity[chordNumber-1][2] = end;  
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      curveVelocity[i][0] = curve;
      curveVelocity[i][1] = start;
      curveVelocity[i][2] = end; 
    }
  } 
}


void op_I2M_C_TCUR(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const int8_t curve = data[2];
  int16_t start = (data[3] << 8) + data[4];
  int16_t end = (data[5] << 8) + data[6];
  if (start < 0 || end < 0 ) return;
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (chordNumber > 0) {
    curveTime[chordNumber-1][0] = curve;
    curveTime[chordNumber-1][1] = start;
    curveTime[chordNumber-1][2] = end;  
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      curveTime[i][0] = curve;
      curveTime[i][1] = start;
      curveTime[i][2] = end; 
    }
  } 
}


void op_I2M_C_DIR(uint8_t data[]) {
  const int8_t chordNumber = data[1];
  const byte value = data[2];
  if (chordNumber < 0 || chordNumber > maxChords) return;
  if (value < 0 || value > 8) return;
  if (chordNumber > 0) {
    chordDirection[chordNumber - 1] = value;
  } 
  else if (chordNumber == 0) {
    for (int i = 0; i < maxChords; i++) {
      chordDirection[i] = value;  
    }
  }  
}

//  coming soon
// void op_I2M_C_QN(uint8_t data[]) {
//   const int8_t channel = 0;
//   const int8_t chordNumber = data[1] - 1;
//   const int8_t noteNumber = data[2];
//   const int8_t velocity = data[3];
//   const int8_t index = data[4];
//   bool getNote = true;
//   bool getVelocity = false;
  
//   Serial.print("chordNumber: "); Serial.println(chordNumber);
//   Serial.print("noteNumber: "); Serial.println(noteNumber);
//   Serial.print("velocity: "); Serial.println(velocity);
//   Serial.print("index: "); Serial.println(index);

//   const int16_t response = playChord(channel, noteNumber, velocity, currentNoteDuration[channel], chordNumber, getNote, getVelocity, index);
//   if (chordNumber < 0 || chordNumber >= maxChords || response < -127 || response > 127) {
//     Wire.write(-1);
//   } else {
//     const uint8_t response_MSB = response >> 8;
//     const uint8_t response_LSB = response & 0xff;
//     Wire.write(response_MSB);
//     Wire.write(response_LSB);
//   }
// }


// void op_I2M_C_QV(uint8_t data[]) {
//   const int8_t channel = 0;
//   const int8_t chordNumber = data[1] - 1;
//   const int8_t noteNumber = data[2];
//   const int8_t velocity = data[3];
//   const int8_t index = data[4];
//   bool getNote = false;
//   bool getVelocity = true;
//   const int8_t response = playChord(channel, noteNumber, velocity, currentNoteDuration[channel], chordNumber, getNote, getVelocity, index);
//   if (chordNumber < 0 || chordNumber >= maxChords || response < -127 || response > 127) {
//     Wire.write(-1);
//   } else {
//     const uint8_t response_MSB = response >> 8;
//     const uint8_t response_LSB = response & 0xff;
//     Wire.write(response_MSB);
//     Wire.write(response_LSB);
//   }
// }



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
}


void op_I2M_CC_OFF_set(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t controller = data[2];
  const uint8_t value_MSB = data[3];
  const uint8_t value_LSB = data[4];
  if (channel < 0 || channel >= channelsOut) return;
  if (controller < 0 || controller > 127) return;
  int16_t offset = (value_MSB << 8) + value_LSB;
  if (offset < -127 * 129) offset = -127 * 129;
  else if (offset > 127 * 129) offset = 127 * 129;
  CCs[channel][controller][2] = offset;       // store the offset in CCs array (index 2 = offset)
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
}


void op_I2M_CC_SET(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t controller = data[2];
  const uint8_t value_MSB = data[3];
  const uint8_t value_LSB = data[4];
  if (value_MSB < 0 || value_MSB > 127) return;
  if (value_LSB < 0 || value_LSB > 127) return;
  const int value = (value_MSB << 7) + value_LSB;
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
  NRPN(channel, controller, value, false);
}


// -------------------------------------------------------------------------------------------
// MIDI out: Misc


void op_I2M_PRG(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t program = data[2];
  sendMidiProgramChange(channel, program);
}


void op_I2M_PB(uint8_t data[]) {
  const int8_t channel = data[1];
  const uint8_t value_MSB = data[2];
  const uint8_t value_LSB = data[3];
  int16_t value = (value_MSB << 8) | value_LSB;
  sendMidiPitchBend(channel, value);
}


void op_I2M_AT(uint8_t data[]) {
  const int8_t channel = data[1];
  const int8_t value = data[2];
  sendMidiAftertouch(channel, value);
}


void op_I2M_CLK(uint8_t data[]) {
  sendMidiClock();
}


void op_I2M_START(uint8_t data[]) {
  sendMidiClockStart();
}


void op_I2M_STOP(uint8_t data[]) {
  sendMidiClockStop();
}


void op_I2M_CONT(uint8_t data[]) {
  sendMidiClockContinue(); 
}


// -------------------------------------------------------------------------------------------
// MIDI out: Buffer


void op_I2M_B_R(uint8_t data[]) {
  const uint8_t value = data[1];
  bufferRecord = value; // 0 = off, 1 = on
}


void op_I2M_B_L(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  bufferLength = value; // buffer length in milliseconds
}


void op_I2M_B_START(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value > bufferLength - bufferEndOffset) bufferStartOffset = bufferLength - bufferEndOffset - 1;
    if (value < 0) bufferStartOffset = 0;
    else bufferStartOffset = value;
}


void op_I2M_B_END(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  if (value > bufferLength - bufferStartOffset) bufferEndOffset = bufferLength - bufferStartOffset - 1;
    if (value < 0) bufferEndOffset = 0;
    else bufferEndOffset = value;
}


void op_I2M_B_DIR(uint8_t data[]) {
  const uint8_t value = data[1];
  bufferDirection = value;                          // 0 = forward, 1 = backward, 2 = ping pong
    if (value == 0 || value == 1) reverseBuffer(value);
}


void op_I2M_B_SPE(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  bufferSpeed = value;           // 100 = normal speed, 50 = double speed, 200 = half speed
}


void op_I2M_B_FB(uint8_t data[]) {
  const uint8_t value = data[1];
  bufferFeedback = value;        // number of rounds the note plays before velocity reaches 0
}


void op_I2M_B_NSHIFT(uint8_t data[]) {
  const int8_t value = data[1];
  bufferPitchShift = value;
}


void op_I2M_B_VSHIFT(uint8_t data[]) {
  const int8_t value = data[1];
  bufferVelocityShift = value;
}


void op_I2M_B_TSHIFT(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  bufferDurationShift = value;
}


void op_I2M_B_NOFF(uint8_t data[]) {
  const int8_t value = data[1];
  bufferNoteOffset = value;
}


void op_I2M_B_VOFF(uint8_t data[]) {
  const int8_t value = data[1];
  bufferVelocityOffset = value;
}


void op_I2M_B_TOFF(uint8_t data[]) {
  const uint8_t value_MSB = data[1];
  const uint8_t value_LSB = data[2];
  int16_t value = (value_MSB << 8 ) | value_LSB;
  bufferDurationOffset = value;
}


void op_I2M_B_CLEAR(uint8_t data[]) {
  clearBuffer();
}


void op_I2M_B_MODE(uint8_t data[]) {
  const int8_t value = data[1];
  bufferMode = value;
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
}


void op_I2M_Q_LN(uint8_t data[]) {
  const int8_t response = lastNoteIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
}


void op_I2M_Q_LV(uint8_t data[]) {
  const int8_t response = lastVelocityIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
}


void op_I2M_Q_LO(uint8_t data[]) {
  const int8_t response = lastNoteOffIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
}


void op_I2M_Q_LC(uint8_t data[]) {
  const int8_t response = lastCIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
}


void op_I2M_Q_LCC(uint8_t data[]) {
  const int8_t response = lastCCIn;
  if (response < 0 || response > 127) {
    Wire.write(-1);
  } else {
    Wire.write(response);
  }
}



// -------------------------------------------------------------------------------------------
// Development


void op_I2M_TEST(uint8_t data[]) {
  //Serial.println("TESTING");
  // const int8_t value1 = data[1];
  // const uint8_t value_MSB = data[2];
  // const uint8_t value_LSB = data[3];
  // const int16_t value2 = (int16_t)(value_MSB << 8 ) | (value_LSB & 0xff);

  Serial.print("data1: "); Serial.println(data[1]);
  Serial.print("data2: "); Serial.println(data[2]);
  Serial.print("data3: "); Serial.println(data[3]);

  const int8_t channel = 7;
  const int8_t chordNumber = 0;
  const int8_t noteNumber = 60;
  const int8_t velocity = data[1];
  const int8_t index = data[3];
  bool getNote = true;
  bool getVelocity = false;
  const int8_t response = playChord(channel, noteNumber, velocity, currentNoteDuration[channel], chordNumber, getNote, getVelocity, index);
  Serial.print("response: "); Serial.println(response);
}