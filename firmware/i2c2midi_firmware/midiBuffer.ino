// -------------------------------------------------------------------------------------------
// MIDI Buffer
// -------------------------------------------------------------------------------------------


void recordToBuffer(int channel, int noteNumber, int velocity, int startTime, int noteDuration, int chordNumber, int noteIndex) {
  
  int c = bufferCount % maxBuffer;          // get slot in the buffer to store note data

  buffer[c][0] = channel;
  buffer[c][1] = noteNumber;
  buffer[c][2] = velocity;                  // new velocity, gets changed per iteration
  buffer[c][3] = bufferFrame;               // store position of note in the buffer (bufferframe)
  buffer[c][4] = noteDuration;
  buffer[c][5] = bufferRoundCount;          // store the unique buffer round, so it is played once per round
  buffer[c][6] = velocity;                  // original velocity, do not change
  buffer[c][7] = chordNumber;               // the chord number, if the note came from a chord
  buffer[c][8] = noteIndex;                 // the original note index in the chord scale, do not change
  buffer[c][9] = noteIndex;                 // new note index, gets changed per iteration 
  bufferCount += 1;                         // advance to the next slot in the buffer
} 



void checkBuffer() {
  
  // fixed offset, used if bufferMode = 1 (pitch/duration offset linked to speed)
  float fixedOffset1 = 1; // used when bufferSpeed is faster
  float fixedOffset2 = 1; // used when bufferSpeed is slower       
  if (bufferMode == 1) { 
    if (bufferSpeed <= 100) { // faster
      fixedOffset1 = 100. / bufferSpeed;
      fixedOffset2 = 1;
    }
    else { // slower
      fixedOffset1 = 1;
      fixedOffset2 = bufferSpeed / 100.;
    }
  }
  
  if (bufferElapsedMicros >= static_cast<long unsigned int>(bufferSpeed * 10)) {  // default bufferSpeed of 100 * 10 = 1000, so 1 bufferframe per millisecond
  
    if (!bufferReverse) {
      bufferFrame += 1;  
      if (bufferFrame >= static_cast<long unsigned int>(bufferLength - bufferEndOffset)) {  // when reaching the end of the buffer...
        if (bufferDirection == 2) {                         // ...if direction 2 = ping pong: reverse the direction
          reverseBuffer(1);                                 
        } else {
          bufferFrame = 0 + bufferStartOffset;              // ...otherwise go back to start
          bufferRoundCount += 1; 
          clearBufferNotes(0 + bufferStartOffset); 
        }
      }
    } 
    else {
      bufferFrame -= 1;
      if (bufferFrame <= static_cast<long unsigned int>(0 + bufferStartOffset)) {           // when reaching the start of the buffer...
        if (bufferDirection == 2) {                         // ...if direction 2 = ping pong: re-reverse the direction
          reverseBuffer(0);                                 
        } else {
          bufferFrame = bufferLength - bufferEndOffset;     // ...otherwise go back to end
          bufferRoundCount += 1;  
          clearBufferNotes(bufferLength - bufferEndOffset); // clear notes, because buffer length might have changed 
        }
      }
    } 
  bufferElapsedMicros = bufferElapsedMicros - bufferSpeed * 10;  // reset the timer
  }

  for (int i = 0; i < maxBuffer; i++) {                     // go through the MIDI buffer
    if (buffer[i][2]) {                                     // check if there is a note to be played
      if (bufferRoundCount != buffer[i][5]) {               // check if note has been played already in this round
        
        if ( (!bufferReverse && bufferFrame > buffer[i][3] ) ||
             ( bufferReverse && bufferFrame < buffer[i][3] ) ) {
          
          // calculate new velocity
          int newVelocity1 = buffer[i][2] - (buffer[i][6] / bufferFeedback); // add shift via I2M.B.FB
          if (newVelocity1 < 0) newVelocity1 = 0;                            // keep in bounds
          int newVelocity2 = newVelocity1 + bufferVelocityShift;             // add shift via I2M.B.VSHIFT
          if (newVelocity2 < 0) newVelocity2 = 0;                            // keep in bounds
          buffer[i][2] = newVelocity2;                                       // save for next round
          int newVelocity3 = newVelocity2 + bufferVelocityOffset;            // add offset
          if (newVelocity3 < 0) newVelocity3 = 0;                            // keep in bounds
          
          // calculate new duration
          int newDuration1 = buffer[i][4] + bufferDurationShift;             // add shift
          if (newDuration1 < 0) newDuration1 = 0;                            // keep in bounds
          buffer[i][4] = newDuration1;                                       // save for next round
          int newDuration2 = (newDuration1 + bufferDurationOffset) / fixedOffset1 * fixedOffset2; // add offset
          if (newDuration2 < 0) newDuration2 = 0;                            // keep in bounds
        
          // calculate new note
          byte newNote1;
          byte newNote2;
          byte newIndex1;
          byte newIndex2;
          if (buffer[i][7] == 99) {
            newNote1 = buffer[i][1] + bufferPitchShift;                    // add shift
            if (newNote1 < 0) newNote1 = 0;                                    // keep in bounds
            buffer[i][1] = newNote1;                                           // save for next round
            newNote2 = newNote1 + bufferNoteOffset + (12 * fixedOffset1) - (12 * fixedOffset2); // add offset
            if (newNote2 < 0) newNote2 = 0;                                    // keep in bounds
          }
          else {
            const byte index_ = buffer[i][8]; // the original index   
            const byte note_ = buffer[i][1];  // the original note
            newIndex1 = buffer[i][9] + bufferPitchShift;
            buffer[i][9] = newIndex1;
            newIndex2 = newIndex1 + bufferNoteOffset + (7 * fixedOffset1) - (7 * fixedOffset2); // add offset
            newNote2 = note_ + getNoteFromIndexBuffer(buffer[i][7], newIndex2) - getNoteFromIndexBuffer(buffer[i][7], index_);
            if (newNote2 < 0) newNote2 = 0; 
          }
          // if any of the values reached out of bound, clear the entry
          if (newVelocity3 <= 0 || newDuration2 <= 0 || newNote2 <= 0 || newNote2 > 127) {
            buffer[i][2] = 0;                                                
          }
          // otherwise play the note
          else {  
            buffer[i][5] = bufferRoundCount;  // store round so note is note is not played again in this round
            midiNoteOn(buffer[i][0], newNote2, newVelocity3, newDuration2, 0, 99, 0);
          } 
        }
      }
    }
  }

}


void clearBuffer() {
  for (int i = 0; i < maxBuffer; i++) {
    buffer[i][2] = 0;
  }
}


void reverseBuffer(bool reverse) {
  bufferOffset = bufferFrame;
  bufferReverse = reverse; 
  bufferRoundCount += 1;      

  clearBufferNotes(bufferOffset);
}



void clearBufferNotes(unsigned long position) {
  for (int i = 0; i < maxBuffer; i++) {
    if (buffer[i][2]) {    
      if ( ( bufferReverse && buffer[i][3] > position ) ||     // for reverse, exclude notes after offset
           (!bufferReverse && buffer[i][3] < position ) ) {    // for forward, exclude notes before offset
        buffer[i][5] = bufferRoundCount;
      }         
    }
  }
}