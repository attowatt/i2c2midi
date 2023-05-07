// -------------------------------------------------------------------------------------------
// MIDI misc
// -------------------------------------------------------------------------------------------


// function for sending MIDI Program Change
void sendMidiProgramChange(int channel, int programNumber) { 
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  if (programNumber < 0 || programNumber > 127) return;
  
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
    MIDI.sendProgramChange(programNumber, channel+1);
    #ifdef USB_DEVICE
      usbMIDI.sendProgramChange(programNumber, channel+1);
    #endif
  } else {
    #ifdef MK2
      midiDevice.sendProgramChange(programNumber, channel+1-16);
    #endif
  }
  blinkLED(1);
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Pitch Bend
void sendMidiPitchBend(int channel, int value) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  if (value < -8192) value = -8192;
  else if (value > 8191) value = 8191;

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
    MIDI.sendPitchBend(value, channel+1);
    #ifdef USB_DEVICE
      usbMIDI.sendPitchBend(value, channel+1);
    #endif
  } else {
    #ifdef MK2
      midiDevice.sendPitchBend(value, channel+1-16);
    #endif
  }
  blinkLED(1);
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Aftertouch
void sendMidiAftertouch(int channel, int value) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  if (value < 0)   value = 0;
  if (value > 127) value = 127;

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
    MIDI.sendAfterTouch(value, channel+1);
    #ifdef USB_DEVICE
      usbMIDI.sendAfterTouch(value, channel+1);
    #endif   
  } else {
    #ifdef MK2
      midiDevice.sendAfterTouch(value, channel+1-16);
    #endif
  }        
  blinkLED(1);
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Clock
void sendMidiClock() {
  MIDI.sendRealTime(midi::Clock);
  #ifdef USB_DEVICE
    usbMIDI.sendRealTime(usbMIDI.Clock);
  #endif   
  #ifdef MK2
    midiDevice.sendRealTime(midiDevice.Clock);
  #endif
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Clock Start
void sendMidiClockStart() {
  MIDI.sendRealTime(midi::Start);
  #ifdef USB_DEVICE
    usbMIDI.sendRealTime(usbMIDI.Start);
  #endif   
  #ifdef MK2
    midiDevice.sendRealTime(midiDevice.Start);
  #endif
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Clock Stop
void sendMidiClockStop() {
  MIDI.sendRealTime(midi::Stop);
  #ifdef USB_DEVICE
    usbMIDI.sendRealTime(usbMIDI.Stop);
  #endif  
  #ifdef MK2 
    midiDevice.sendRealTime(midiDevice.Stop);
  #endif
}


// -------------------------------------------------------------------------------------------


// function for sending MIDI Clock Continue
void sendMidiClockContinue() {
  MIDI.sendRealTime(midi::Continue);
  #ifdef USB_DEVICE
    usbMIDI.sendRealTime(usbMIDI.Continue);
  #endif 
  #ifdef MK2  
    midiDevice.sendRealTime(midiDevice.Continue);
  #endif
}


// -------------------------------------------------------------------------------------------


// MIDI panic
void panic() {
  
  sendMidiClockStop();

  // send note offs and reset ratchets and repeats
  for (int j=0; j < 16; j++) {                           
    for (int i=0; i <= 127; i++) {                            
      midiNoteOff(j, i);
    }
  }

  setDefaults();

}


// -------------------------------------------------------------------------------------------


void setDefaults() {

  for (int i = 0; i < channelsOut; i++) {
    channelMute[i] = 0;
    channelSolo[i] = 0;
    currentNoteDuration[i] = 100;
    currentNoteShift[i] = 0;
    currentRepetition[i] = 1;
    currentRatcheting[i] = 1;
    noteUpperLimit[i] = 127;
    noteLowerLimit[i] = 0;
    noteLimitMode[i] = 0;
    for (int j = 0; j < 127; j++) { 
      CCs[i][j][0] = 0;
      CCs[i][j][1] = 0;                    
      CCs[i][j][2] = 0;
    }
  }

  // reset NRPNs
  for (int j = 0; j < maxNRPNs; j++) { 
    NRPNs[j][2] = 0;
    NRPNs[j][3] = 0;
  }
  
  // reset ratchets and repeats 
  for (int j=0; j < channelsOut; j++) {                           
    for (int i=0; i < maxNotes; i++) {                            
      notes[j][i][5] = 0;   // reset ratchet count
      notes[j][i][6] = 0;   // reset repeat count
    }
  }

  // reset scheduled notes
  for (int i = 0; i < maxNotesScheduled; i++) {
    for (int j = 0; j < 5; j++) {
      scheduledNotes[i][j] = 0;
    }
    scheduledNoteCount -= 1;
  }

  // reset chords
  for (int i = 0; i < maxChords; i++) {
    clearChord(i);
    chordReverse[i] = 0;
    chordRotate[i] = 0;       
    chordInversion[i] = 0;    
    chordStrumming[i] = 0;    
    chordShift[i] = 0;        
    chordStretch[i][0] = 0;   
    chordStretch[i][1] = 0;   
    chordReflection[i][0] = 0;
    chordReflection[i][1] = 0;
    currentScaleChord[i] = 0;
    currentScaleLength[i] = 12;
    for (int j = 0; j < 12; j++) {
      currentScale[i][j] = j;
    }
    curveVelocity[i][0] = 0;
    curveVelocity[i][1] = 100;
    curveVelocity[i][2] = 100;
    curveTime[i][0] = 0;
    curveTime[i][1] = 100;
    curveTime[i][2] = 100;
    chordDirection[i] = 0;
    
  }

  // reset USB MIDI in
  latch = true;
  lastChannelIn = 1;
  lastNoteIn = 0;
  lastVelocityIn = 0;
  lastNoteOffIn = 0;
  lastCIn = 0;
  lastCCIn = 0;

  // reset buffer
  bufferRecord = 0;
  bufferLength = 10;
  bufferStartOffset = 0;
  bufferEndOffset = 0;
  bufferDirection = 0;
  bufferSpeed = 100;
  bufferFeedback = 8;
  bufferPitchShift = 0;
  bufferDurationShift = 0;
  bufferVelocityShift = 0;
  bufferNoteOffset = 0;
  bufferVelocityOffset = 0;
  bufferDurationOffset = 0;
  bufferMode = 0;
  bufferReverse = 0;

  

}