// -------------------------------------------------------------------------------------------
// Notes
// -------------------------------------------------------------------------------------------


// function for getting output type (TRS or USB) for channel number
bool isTRS(int channel) {
    if (channel < 0 || channel >= channelsOut) return 1;
    return channel >= 0 && channel < 16;         // if channel is  0..15 -> true -> TRS
}  


// -------------------------------------------------------------------------------------------


// function for handling MIDI Note Ons
void midiNoteOn(int channel, int noteNumber_, int velocity, int noteDuration, bool toBuffer, int chordNumber, int noteIndex) {
  
  // check channel mute
  if (channelMute[channel] == 1 && channelSolo[channel] != 1) return;
  
  // check channel solo
  int solo = 0;
  for(int i = 0; i < channelsOut; i++) 
  {
    solo = solo + channelSolo[i];
  }
  if (solo > 0 && channelSolo[channel] != 1) return; 

  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  int noteNumberUnlimited = noteNumber_ + currentNoteShift[channel];
  byte noteNumber = getLimitedNote(channel, noteNumberUnlimited);
  
  if (noteNumber < 0 || noteNumber > 127) return; 
  if (noteDuration < 1) return;
  if (velocity < 1) {
    sendMidiNoteOff(channel, noteNumber);         // velocity = 0 is treated as Note Off
    return;
  }
  if (velocity > 127) velocity = 127;

  // check if this note is already playing; if yes, send note off first
  for (int i=0; i < maxNotes; i++) {
    if (notes[channel][i][0] == noteNumber && notes[channel][i][3] == 1) {
      sendMidiNoteOff(channel, notes[channel][i][0]);
      notes[channel][i][3] = 0;
    }
  }
  noteCount[channel] += 1;                                        // count one note up
  currentNote[channel] = noteCount[channel] % maxNotes;           // determine the current note slot
  // check if next note slot is free; if no, skip to next note slot; 
  for (int i=0; i < maxNotes; i++) {                        
    if (notes[channel][currentNote[channel]][3] == 1) {           // slot not free ...
      noteCount[channel] += 1;                                    // ... so skip to the next    
      currentNote[channel] = noteCount[channel] % maxNotes;
    }
    else {
      break; // slot is free, we can stop the for loop
    }
  }
  // if no slot was empty, and the note in the oldest slot is still playing, send a note off
  if (notes[channel][currentNote[channel]][3] == 1) {
    sendMidiNoteOff(channel, notes[channel][currentNote[channel]][0]);
  }

  int newNoteDuration = noteDuration;

  if (currentRepetition[channel] > 1 || currentRatcheting[channel] > 1) {
    newNoteDuration = noteDuration / currentRatcheting[channel] * ratchetingLength / 100;
    if (newNoteDuration <= 0) return; 
    notes[channel][currentNote[channel]][5] = currentRepetition[channel] * currentRatcheting[channel];  
  }
  
  // store the values for the note in the notes array
  notes[channel][currentNote[channel]][0] = noteNumber;           // note number
  notes[channel][currentNote[channel]][1] = millis();             // note start time
  notes[channel][currentNote[channel]][2] = newNoteDuration;      // note duration
  notes[channel][currentNote[channel]][3] = 1;                    // note is on
  notes[channel][currentNote[channel]][4] = velocity;             // note is on
  
  sendMidiNoteOn(channel, noteNumber, velocity);  

  // record outoing notes to MIDI buffer:
  // only "inital" notes are recorded to the buffer;
  // notes that are triggered from the buffer itself are not recorded to the buffer
  if (toBuffer) {     
    // notes are only recorded, if recording is on
    if (bufferRecord) {
      recordToBuffer(channel, noteNumber - currentNoteShift[channel], velocity, millis(), newNoteDuration, chordNumber, noteIndex);
    }
  }

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
      if (currentRepetition[j] > 1 || currentRatcheting[j] > 1) {       // if racheting or repetition is set than 1
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
}


// -------------------------------------------------------------------------------------------


// function for removing a received note number and velocity from the note history
void removeFromNoteHistoryIn(int channel, int noteNumber) {
  if (channel < 0 || channel >= channelsOut) return;
  if (noteNumber < 0 || noteNumber > 127) return; 
  int position = -1;
  for(int j = 0; j < noteHistoryInLength; j++){                       
     if(noteHistoryIn[channel][j][0] == noteNumber){            // find the position of the note
         position = j;                                          // store the position
         break;                                                 // stop the for loop
     }
  }
  if (position >= 0) {  
    for (int i = position; i < noteHistoryInLength; i++) {      // go through all elements right to the found position
      if (i == 7) {                                                     
        noteHistoryIn[channel][i][0] = 0;                               
        noteHistoryIn[channel][i][1] = 0;
      } else {
        noteHistoryIn[channel][i][0] = noteHistoryIn[channel][i+1][0];  // shift note number values to the left by 1 index
        noteHistoryIn[channel][i][1] = noteHistoryIn[channel][i+1][1];  // shift note velocity values to the left by 1 index
      }
    }
  }
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


void scheduleNote(int channel, int noteNumber, int velocity, int noteDuration, int delay, int chordNumber, int noteIndex) {
  scheduledNotes[scheduledNoteCount][0] = noteNumber;
  scheduledNotes[scheduledNoteCount][1] = millis() + delay;
  scheduledNotes[scheduledNoteCount][2] = noteDuration;
  scheduledNotes[scheduledNoteCount][3] = velocity;
  scheduledNotes[scheduledNoteCount][4] = channel;
  scheduledNotes[scheduledNoteCount][5] = chordNumber;
  scheduledNotes[scheduledNoteCount][6] = noteIndex;
  scheduledNoteCount = (scheduledNoteCount + 1) % maxNotesScheduled;
}


// -------------------------------------------------------------------------------------------


void checkScheduledNotes() {
  unsigned long currentTime = millis();                   // get current time
  for (int i = 0; i < maxNotesScheduled; i++) {           // go through all scheduled notes
    if (scheduledNotes[i][1]) {                           // check if the entry is not empty
      if (currentTime > scheduledNotes[i][1]) {           // check if the scheduled time has been reached
        midiNoteOn(scheduledNotes[i][4], scheduledNotes[i][0], scheduledNotes[i][3], scheduledNotes[i][2], 1, scheduledNotes[i][5], scheduledNotes[i][6]);
        for (int j = 0; j < 5; j++) {
          scheduledNotes[i][j] = 0;
        }
        scheduledNoteCount -= 1;
      }
    } 
  }
}


// -------------------------------------------------------------------------------------------


// function for different ways of handling min max note limit
byte getLimitedNote(int channel, int noteNumber) {
   
  byte lowerLimit = noteLowerLimit[channel];
  byte upperLimit = noteUpperLimit[channel];
  byte mode = noteLimitMode[channel];       

  // mode 0: ignore notes
  if (mode == 0) {
    if (noteNumber < lowerLimit || noteNumber > upperLimit) return -1;
    else return noteNumber;
  }
  
  // mode 1: clamp notes
  else if (mode == 1) {
    if (noteNumber < lowerLimit) return lowerLimit;
    if (noteNumber > upperLimit) return upperLimit;
    else return noteNumber;
  }

  // mode 2: fold back notes by 1 octave
  else if (mode == 2) {
    if (noteNumber < lowerLimit) {
      return lowerLimit + 12 - mod(lowerLimit - noteNumber, 12);
    }
    else if (noteNumber > upperLimit) {
      return upperLimit - 12 + mod(noteNumber - upperLimit, 12);
    }
    else {
      return noteNumber;
    }
  }

  // mode 3: fold back notes by multiple octaves
  else if (mode == 3) {
    if (noteNumber < lowerLimit) {
      const int octave = ((lowerLimit - noteNumber) / 12 ) + 1;
      const int finalNote = lowerLimit + (octave * 12) - mod(lowerLimit - noteNumber, 12);
      return finalNote;
    }
    else if (noteNumber > upperLimit) {
      const int octave = ((noteNumber - upperLimit) / 12) + 1;
      const int finalNote = upperLimit - (octave * 12) + mod(noteNumber - upperLimit, 12);
      return finalNote;
      
    }
    else {
      return noteNumber;
    }
  } else {
    return noteNumber;
  }
   
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Note On
void sendMidiNoteOn(int channel, int noteNumber, int velocity) {

  if (isTRS(channel)) {
    MIDI.sendNoteOn(noteNumber, velocity, channel+1);
    blinkLED(1);
    #ifdef USB_DEVICE
      usbMIDI.sendNoteOn(noteNumber, velocity, channel+1);
    #endif
  } else {
    #ifdef MK2
      #ifdef MULTIPLEUSBOUT
        for (int i = 0; i < 2; i++) {
          if (* midiDeviceList[i] && channel == i+16) {
            midiDeviceList[i]->sendNoteOn(noteNumber, velocity, i+1);  
          }
        }
      #else
        midiDevice.sendNoteOn(noteNumber, velocity, channel+1-16);
      #endif
    #endif
  }
  
  
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

  if (isTRS(channel)) {
    MIDI.sendNoteOff(noteNumber, 0, channel+1);
    blinkLED(1);
    #ifdef USB_DEVICE
      usbMIDI.sendNoteOff(noteNumber, 0, channel+1);
    #endif
  } else {
    #ifdef MK2
      #ifdef MULTIPLEUSBOUT
        for (int i = 0; i < 2; i++) {
          if (* midiDeviceList[i] && channel == i+16) {
            midiDeviceList[i]->sendNoteOff(noteNumber, 0, i+1);  
          }
        }
      #else
        midiDevice.sendNoteOff(noteNumber, 0, channel+1-16);
      #endif
    #endif
  }
  

  #ifdef DEBUG  
    Serial.print("Sending MIDI Note Off: ");
    Serial.print(noteNumber); Serial.print(", ");
    Serial.print("Ch: "); Serial.println(channel+1);
  #endif
}