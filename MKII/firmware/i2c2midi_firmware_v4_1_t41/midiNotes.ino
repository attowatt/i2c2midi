// -------------------------------------------------------------------------------------------
// Notes
// -------------------------------------------------------------------------------------------


// function for getting output type (TRS or USB) for channel number
bool isTRS(int channel) {
    if (channel < 0 || channel >= channelsOut) return;
    return channel >= 0 && channel < 16;         // if channel is  0..15 -> true -> TRS
}  


// -------------------------------------------------------------------------------------------


// function for handling MIDI Note Ons
void midiNoteOn(int channel, int noteNumber_, int velocity, int noteDuration) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  int noteNumber = noteNumber_ + currentNoteShift;
  if (noteNumber < 0 || noteNumber > 127) return; 
  if (noteDuration < 0) return;
  if (velocity < 1) {
    sendMidiNoteOff(channel, noteNumber);         // velocity = 0 is treated as Note Off
    return;
  }
  if (velocity > 127) velocity = 127;

  // check if this note is already playing; if yes, send note off first
  for (int i=0; i < maxNotes; i++) {
    if (notes[channel][i][0] == noteNumber && notes[channel][i][3] == 1) {
      #ifdef DEBUG
        Serial.println("Note is already playing"); 
      #endif
      sendMidiNoteOff(channel, notes[channel][i][0]);
      notes[channel][i][3] = 0;
    }
  }
  noteCount[channel] += 1;                                        // count one note up
  currentNote[channel] = noteCount[channel] % maxNotes;           // determine the current note number
  // check if next note number is still playing; if yes, skip to next note number; 
  // if there's no more space available, replace the note
  for (int i=0; i < maxNotes; i++) {                        
    if (notes[channel][currentNote[channel]][3] == 1) {
      noteCount[channel] += 1;                              
      currentNote[channel] = noteCount[channel] % maxNotes;
    }
    else {
      break;
    }
  }

  int newNoteDuration = noteDuration;

  if (currentRepetition > 1 || currentRatcheting > 1) {
    
    newNoteDuration = noteDuration / currentRatcheting * ratchetingLength / 100;
    if (newNoteDuration <= 0) return; 
    notes[channel][currentNote[channel]][5] = currentRepetition * currentRatcheting;  
  }
  
  // store the values for the note in the notes array
  notes[channel][currentNote[channel]][0] = noteNumber;           // note number
  notes[channel][currentNote[channel]][1] = millis();             // note start time
  notes[channel][currentNote[channel]][2] = newNoteDuration;      // note duration
  notes[channel][currentNote[channel]][3] = 1;                    // note is on
  notes[channel][currentNote[channel]][4] = velocity;             // note is on
  sendMidiNoteOn(channel, noteNumber, velocity);  
}


// -------------------------------------------------------------------------------------------


// function for checking note duration
void checkNoteDurations() {
  unsigned long currentTime = millis();                           // get current time
  for (int j=0; j < channelsOut; j++) {                           // go through all channels
    for (int i=0; i < maxNotes; i++) {                            // go through all notes
    
      if (notes[j][i][3] != 0 && notes[j][i][2] != 0) {           // check if note is currently playing and duration is not 0
        if (currentTime - notes[j][i][1] > notes[j][i][2]) {      // if yes, check if the note duration has been reached
          sendMidiNoteOff(j, notes[j][i][0]);                     // if yes, send MIDI Note off
          notes[j][i][3] = 0;                                     // set note on/off to off
        }
      } 

      // note repetition & ratcheting
      if (currentRepetition > 1 || currentRatcheting > 1) {       // if racheting or repetition is set than 1
      if (notes[j][i][5] > 1) {                                   // index 5 is ratchet/rep count, do if higher than 1
          if (currentTime - notes[j][i][1] > notes[j][i][2] * 100 / ratchetingLength) { 
            notes[j][i][1] = millis();                            // note start time
            notes[j][i][3] = 1;                                   // set note is on
            sendMidiNoteOn(j, notes[j][i][0], notes[j][i][4]);    // send same note with same velocity
            notes[j][i][5] -= 1;                                  // update ratchet count
          }
        }
      }  
      
    }
  }   
}


// -------------------------------------------------------------------------------------------


// function for handling MIDI Note Offs 
void midiNoteOff(int channel, int noteNumber) {
  if (channel < 0 || channel >= channelsOut) return;
  if (noteNumber < 0 || noteNumber > 127) return; 
  sendMidiNoteOff(channel, noteNumber);
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Note On
void sendMidiNoteOn(int channel, int noteNumber, int velocity) {
  
  // check min max note setting
  if (noteNumber < noteLowerLimit || noteNumber > noteUpperLimit) {
    return; 
  }
  
  if (isTRS(channel)) {
    MIDI.sendNoteOn(noteNumber, velocity, channel+1);
    #ifdef USB_DEVICE
      usbMIDI.sendNoteOn(noteNumber, velocity, channel+1);
    #endif
  } else {
    #ifdef MK2
      midiDevice.sendNoteOn(noteNumber, velocity, channel+1-16);
    #endif
  }
  blinkLED(1);
  
  #ifdef DEBUG  
    Serial.print("Sending MIDI Note On: ");
    Serial.print(noteNumber); Serial.print(", ");
    Serial.print("Vel: "); Serial.print(velocity); Serial.print(", ");
    Serial.print("Ch: "); Serial.println(channel+1);
  #endif
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Note Off
void sendMidiNoteOff(int channel, int noteNumber) {

  // check min max note setting
  // if (noteNumber < noteLowerLimit || noteNumber > noteUpperLimit) {
  //   return; 
  // }

  if (isTRS(channel)) {
    MIDI.sendNoteOff(noteNumber, 0, channel+1);
    #ifdef USB_DEVICE
      usbMIDI.sendNoteOff(noteNumber, 0, channel+1);
    #endif
  } else {
    #ifdef MK2
      midiDevice.sendNoteOff(noteNumber, 0, channel+1-16);
    #endif
  }
  blinkLED(1);

  #ifdef DEBUG  
    Serial.print("Sending MIDI Note Off: ");
    Serial.print(noteNumber); Serial.print(", ");
    Serial.print("Ch: "); Serial.println(channel+1);
  #endif
}


// -------------------------------------------------------------------------------------------
// Note History


// function for adding a received note number and velocity to the note history 
void addToNoteHistoryIn(int channel, int noteNumber, int velocity) {
  if (channel < 0 || channel >= channelsOut) return;
  if (noteNumber < 0 || noteNumber > 127) return; 
  if (velocity < 0) return;
  if (velocity > 127) velocity = 127;
  for (int i = noteHistoryInLength; i > 0; i--) {
    noteHistoryIn[channel][i][0] = noteHistoryIn[channel][i-1][0];    // shift note number values to the right by 1 index
    noteHistoryIn[channel][i][1] = noteHistoryIn[channel][i-1][1];    // shift note velocity values to the right by 1 index
  }
  noteHistoryIn[channel][0][0] = noteNumber;                          // store new note number at index 0
  noteHistoryIn[channel][0][1] = velocity;                            // store new note velocity at index 0
  #ifdef DEBUG
    printNoteHistory(channel);
  #endif
}


// -------------------------------------------------------------------------------------------


// function for removing a received note number and velocity from the note history
void removeFromNoteHistoryIn(int channel, int noteNumber) {
  if (channel < 0 || channel >= channelsOut) return;
  if (noteNumber < 0 || noteNumber > 127) return; 
  int position = -1;
  for(int j = 0; j < noteHistoryInLength; j++){                       
     if(noteHistoryIn[channel][j][0] == noteNumber){                  // find the position of the note
         position = j;                                                // store the position
         break;                                                       // stop the for loop
     }
  }
  if (position >= 0) {  
    for (int i = position; i < noteHistoryInLength; i++) {            // go through all elements right to the found position
      if (i == 7) {                                                     
        noteHistoryIn[channel][i][0] = 0;                               
        noteHistoryIn[channel][i][1] = 0;
      } else {
        noteHistoryIn[channel][i][0] = noteHistoryIn[channel][i+1][0];  // shift note number values to the left by 1 index
        noteHistoryIn[channel][i][1] = noteHistoryIn[channel][i+1][1];  // shift note velocity values to the left by 1 index
      }
    }
  }
  #ifdef DEBUG
    printNoteHistory(channel);
  #endif
}


// -------------------------------------------------------------------------------------------


// function for latch setting
void setLatch(int value) {
  if (value < 0 || value > 1) return;
  value ? latch = true : latch = false;

  // clear the note history when changing the latch setting
  for (int j = 0; j < channelsIn; j++) {
    for (int i = 0; i < noteHistoryInLength; i++) {
      noteHistoryIn[j][i][0] = 0;
      noteHistoryIn[j][i][1] = 0;
    }
  }

  #ifdef DEBUG
    Serial.println("op_I2M_Q_LATCH");
    Serial.print(" latch: "); Serial.println(value);  
  #endif
}

// -------------------------------------------------------------------------------------------


#ifdef DEBUG
  void printNoteHistory(int channel) {
    for (int i = 0; i < noteHistoryInLength; i++) {
      Serial.print(noteHistoryIn[channel][i][0]);
      Serial.print(",");
    }
    Serial.println(" ");
  }
#endif


// -------------------------------------------------------------------------------------------
// Schedule Notes


void scheduleNote(int channel, int noteNumber, int velocity, int noteDuration, int delay) {
  scheduledNotes[scheduledNoteCount][0] = noteNumber;
  scheduledNotes[scheduledNoteCount][1] = millis() + delay;
  scheduledNotes[scheduledNoteCount][2] = noteDuration;
  scheduledNotes[scheduledNoteCount][3] = velocity;
  scheduledNotes[scheduledNoteCount][4] = channel;
  #ifdef DEBUG  
    Serial.print("Scheduling note: "); Serial.println(scheduledNotes[scheduledNoteCount][0]);
  #endif
  scheduledNoteCount = (scheduledNoteCount + 1) % maxNotesScheduled;
}


// -------------------------------------------------------------------------------------------


void checkScheduledNotes() {
  unsigned long currentTime = millis();                   // get current time
  for (int i = 0; i < maxNotesScheduled; i++) {           // go through all scheduled notes
    if (scheduledNotes[i][1]) {                           // check if the entry is not empty
      if (currentTime > scheduledNotes[i][1]) {           // check if the scheduled time has been reached
        midiNoteOn(scheduledNotes[i][4], scheduledNotes[i][0], scheduledNotes[i][3], scheduledNotes[i][2]);
        #ifdef DEBUG  
          Serial.print("Playing scheduled note: "); Serial.println(scheduledNotes[i][0]);
        #endif
        for (int j = 0; j < 5; j++) {
          scheduledNotes[i][j] = 0;
        }
        scheduledNoteCount -= 1;
      }
    } 
  }
}