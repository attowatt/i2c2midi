// -------------------------------------------------------------------------------------------
// Chord
// -------------------------------------------------------------------------------------------

// order of chord transformations:
// REVERSE -> ROTATE -> INVERSION -> STRUMMING
// more to come ...



// function for playing chord
void playChord(int channel, int noteNumber, int velocity, int noteDuration, int chordNumber) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return;
  if (noteNumber < 0 || noteNumber > 127) return;
  if (noteDuration < 0) return;
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  
  currentChordLength = chordLength[chordNumber];
  currentChordNoteCount = chordNoteCount[chordNumber];

  // if no notes are defined for the chord, don't do anything
  if (currentChordLength == 0) return;

  int currentReverse = chordReverse[chordNumber];
  int currentRotate = chordRotate[chordNumber];
  int currentInversion = chordInversion[chordNumber];
  int currentStrumming = chordStrumming[chordNumber];

  // notes of the original chord are added to "currentChord" to which transformations are applied
  // this is done in reverse, so notes are played in the order they have been added
  for(int i = currentChordNoteCount-1; i >= 0; i--){
    currentChord[currentChordNoteCount-1-i] = chord[chordNumber][i];
  }

  // Transformation: REVERSE
  if (currentReverse) {
    reverseChord();
  }

  // Transformation: ROTATE
  if (currentRotate != 0) {
    if (currentRotate > 0) rotateChordRight(currentRotate);
    else if (currentRotate < 0) rotateChordLeft(abs(currentRotate)); 
  }

  for (int i = 0; i < currentChordLength; i++) {
    
    // Transformation: INVERSION
    int inversionAddon = 0;
    if (currentInversion >= 0) {           
      inversionAddon = floor((currentInversion + ((currentChordLength-1) - i)) / currentChordLength) * 12;
    } else if (currentInversion < 0) {     
      inversionAddon = floor((currentInversion + (0 - i)) / currentChordLength) * 12;
    }
    
    int chordNote = noteNumber + currentChord[i] + inversionAddon;   

    if (i == 0) {   
      midiNoteOn(channel, chordNote, velocity, noteDuration);
    }
    else {
        // Transformation: STRUMMING
        if (currentStrumming == 0) {
          midiNoteOn(channel, chordNote, velocity, noteDuration);
        } else {
          scheduleNote(channel, chordNote, velocity, noteDuration, currentStrumming * i);
        }
    }
  }
}


// -------------------------------------------------------------------------------------------


// function to remove a note from the chord based on noteNumber
void removeFromChord(int chordNumber, int noteNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  if (noteNumber < 0 || noteNumber > 127) return; 
  int c = chordNumber;
  int position = -1;
  for(int j = 0; j < chordMaxLength; j++){                       
     if(chord[c][j] == noteNumber){                   // find the position of the note
         position = j;                                // store the position
         break;                                       // stop the for loop
     }
  }
  if (position >= 0) {  
    deleteFromChord(chordNumber, position);
  }
}


// -------------------------------------------------------------------------------------------


// function to insert a note into chord at a specific index
void insertIntoChord(int8_t chordNumber, int8_t index, int8_t noteNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  if (index < 0 || index > chordNoteCount[c]) return;
  if (noteNumber < -127 || noteNumber > 127) return;
  
  for (int i = chordMaxLength - 1; i >= index; i--) {
    chord[c][i] = chord[c][i-1];                             // shift note number values to the right by 1 index
  }
  chord[c][index] = noteNumber;                              // store new note number
  
  // set chord length automatically when new note is pushed  
  int newChordNoteCount = chordNoteCount[c] + 1;
  if (newChordNoteCount > 8) newChordNoteCount = 8;
  chordNoteCount[c] = newChordNoteCount;
  chordLength[c] = newChordNoteCount;
  
  #ifdef DEBUG
  Serial.print("chordNumber: "); Serial.println(c);
  Serial.print("chordLength: "); Serial.println(c);
    for (int i = 0; i < chordMaxLength-1; i++) {
      Serial.print(chord[c][i]); Serial.print(",");
    }
    Serial.println(chord[c][chordMaxLength-1]);
  #endif

}


// -------------------------------------------------------------------------------------------


// function to delete a note at specific index from chord
void deleteFromChord(int8_t chordNumber, int8_t index) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  if (index < 0 || index > chordNoteCount[c]) return;
  int position = index;
  
  for (int i = position; i < chordMaxLength; i++) {         // go through all elements right to the found position
    if (i == chordMaxLength) {                                                     
      chord[c][i] = 0;                               
    } else {
      chord[c][i] = chord[c][i+1];                          // shift note number values to the left by 1 index
    }
  }

  // set chord length automatically when a note is removed  
  int newChordNoteCount = chordNoteCount[c] - 1;
  if (newChordNoteCount < 0) newChordNoteCount = 0;
  chordNoteCount[c] = newChordNoteCount;
  chordLength[c] = newChordNoteCount;

  #ifdef DEBUG
    for (int i = 0; i < chordMaxLength-1; i++) {
      Serial.print(chord[c][i]); Serial.print(",");
    }
    Serial.println(chord[c][chordMaxLength-1]);
  #endif
}


// -------------------------------------------------------------------------------------------


// function to set specific index in chord
void setChord(int8_t chordNumber, int8_t index, int8_t noteNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  if (index < 0 || index > chordNoteCount[c]) return;
  if (noteNumber < 0 || noteNumber > 127) return;
  
  chord[c][index] = noteNumber;

  #ifdef DEBUG
    for (int i = 0; i < chordMaxLength-1; i++) {
      Serial.print(chord[c][i]); Serial.print(",");
    }
    Serial.println(chord[c][chordMaxLength-1]);
  #endif
}


// -------------------------------------------------------------------------------------------


// function to clear chord
void clearChord(int chordNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  for (int i = 0; i < chordMaxLength; i++) {
    chord[c][i] = 0;
  }
  chordNoteCount[c] = 0;                           
  chordLength[c] = 0;  
  #ifdef DEBUG
    Serial.println("Chord cleared");
  #endif
}


// -------------------------------------------------------------------------------------------


// function to reverse chord
void reverseChord() {
  int reversedChord[currentChordNoteCount];
  for(int i = currentChordNoteCount-1; i >= 0; i--){
    reversedChord[currentChordNoteCount-1-i] = currentChord[i];
  }
  for (int i = 0; i < currentChordNoteCount; i++) {
    currentChord[i] = reversedChord[i];
  }
  #ifdef DEBUG
    Serial.println("Chord reversed");
    for (int i = 0; i < chordMaxLength-1; i++) {
        Serial.print(currentChord[i]); Serial.print(",");
      }
      Serial.println(currentChord[chordMaxLength-1]);
  #endif
}


// -------------------------------------------------------------------------------------------


// function to rotate chord left
void rotateChordLeft(int amount) {
  if (amount < -127 || amount > 127 || amount == 0) return;
  
  for (int i = 0; i < amount; i++) {
    int firstElement = currentChord[0];
    for (int i = 0; i < currentChordNoteCount-1; i++) {
        currentChord[i] = currentChord[i+1];                        // shift left
      }
    currentChord[currentChordNoteCount-1] = firstElement;
  }
  #ifdef DEBUG
    Serial.println("Chord rotated left");
    for (int i = 0; i < chordMaxLength-1; i++) {
        Serial.print(currentChord[i]); Serial.print(",");
      }
      Serial.println(currentChord[chordMaxLength-1]);
  #endif
}


// function to rotate chord right
void rotateChordRight(int amount) {
  if (amount < -127 || amount > 127 || amount == 0) return;
  for (int i = 0; i < amount; i++) {
    int lastElement = currentChord[currentChordNoteCount-1];
    for (int i = currentChordNoteCount-1; i > 0; i--) {
        currentChord[i] = currentChord[i-1];                        // shift right
      }
    currentChord[0] = lastElement;
  }
  #ifdef DEBUG
    Serial.println("Chord rotated right");
    for (int i = 0; i < chordMaxLength-1; i++) {
        Serial.print(currentChord[i]); Serial.print(",");
      }
      Serial.println(currentChord[chordMaxLength-1]);
  #endif
}


// -------------------------------------------------------------------------------------------


// modulo helper function (that handles negative values correctly)
int mod(int a, int b) {
  int c = a % b;
  return (c < 0) ? c + b : c;
}
