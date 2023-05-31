// -------------------------------------------------------------------------------------------
// Chord
// -------------------------------------------------------------------------------------------

// order of chord transformations:
// REVERSE > ROTATE > TRANSPOSE > DISTORT > REFLECT > INVERT > STRUMMING


// function for playing chord or getting chord note/velocity
int playChord(int channel, int noteNumber, int velocity, int noteDuration, int chordNumber, bool getNote, bool getVelocity, byte index) {
  
  // keep values in range
  if (channel < 0 || channel >= channelsOut) return 0;
  if (noteNumber < 0 || noteNumber > 127) return 0;
  if (noteDuration < 0) return 0;
  if (chordNumber < 0 || chordNumber >= maxChords) return 0;

  currentChordLength = chordLength[chordNumber];
  currentChordNoteCount = chordNoteCount[chordNumber];
  
  // if no notes are defined for the chord, don't do anything
  if (currentChordLength == 0) return 0 ;
  
  // create a scaled chord (chordScaled) based on original chord and currentScale
  createChordScaled(chordNumber);
  
  // apply chord transformations to scaledChord
  applyTransformations(chordNumber);
  
  // calculate new chord length based on play direction
  int currentChordLength2 = calculateChordLengthByDirection(chordNumber);
  
  // randomize the note order if direction is random (4)
  if (chordDirection[chordNumber] == 4) {
    randomizeChordIndices(chordNumber, currentChordLength2);
  }

  int delay = 0;
  
  // if the chord should be played, go through all the chord notes
  int j_start = 0;
  int j_max = currentChordLength2;
  // if note or velocity is requested, calculate note and velocity only for given index
  if (getNote || getVelocity) { 
    j_start = mod(index, currentChordLength2);
    j_max = index + 1;
  }

  // go through the chord notes
  for (int j = j_start; j < j_max; j++) {
    
    // get next note based on play direction
    int i = getNextChordNoteIndex(chordNumber, currentChordLength, currentChordLength2, j);

    const int chordNote = noteNumber + getNoteFromIndex(chordNumber, i);
    
    // Transformation: VELOCITY CURVE
    const int velocityCurved = getCurveValue(curveVelocity[chordNumber][0], j, currentChordLength2, velocity, curveVelocity[chordNumber][1], curveVelocity[chordNumber][2]);
    
    // if note or velocity is requested, return the values and stop the function
    if (getNote) return chordNote;
    if (getVelocity) return velocityCurved;

    if (j == 0) {   
      midiNoteOn(channel, chordNote, velocityCurved, noteDuration, 1, chordNumber, chordScaled[0][i]);
    }
    else {
      // Transformation: STRUMMING
      int currentStrumming = chordStrumming[chordNumber];

      // Transformation: TIME CURVE
      const int timeCurved = getCurveValue(curveTime[chordNumber][0], j, currentChordLength2, currentStrumming, curveTime[chordNumber][1], curveTime[chordNumber][2]) - currentStrumming;
      delay += currentStrumming + timeCurved;
      
      if (currentStrumming == 0) {
        midiNoteOn(channel, chordNote, velocityCurved, noteDuration, 1, chordNumber, chordScaled[0][i]);
      } else {
        scheduleNote(channel, chordNote, velocityCurved, noteDuration, delay, chordNumber, chordScaled[0][i]);
      }
    }
  }
  return 0;
}


// -------------------------------------------------------------------------------------------


// function to remove a note from the chord based on noteNumber
void removeFromChord(int chordNumber, int noteNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  if (noteNumber < -127 || noteNumber > 127) return; 
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


// function to insert a note into chord at a specific index
void insertIntoChord(int8_t chordNumber, int8_t index, int8_t noteNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  if (index < 0 || index > chordNoteCount[c]) return;
  if (noteNumber < -127 || noteNumber > 127) return;
  
  for (int i = chordMaxLength - 1; i > index; i--) {
    chord[c][i] = chord[c][i-1];                             // shift note number values to the right by 1 index
  }
  chord[c][index] = noteNumber;                              // store new note number
  
  // set chord length automatically when new note is pushed  
  int newChordNoteCount = chordNoteCount[c] + 1;
  if (newChordNoteCount > 8) newChordNoteCount = 8;
  chordNoteCount[c] = newChordNoteCount;
  chordLength[c] = newChordNoteCount;
}


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
}


// function to set specific index in chord
void setChord(int8_t chordNumber, int8_t index, int8_t noteNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  if (index < 0 || index > chordNoteCount[c]) return;
  if (noteNumber < 0 || noteNumber > 127) return;
  
  chord[c][index] = noteNumber;
}


// function to clear chord
void clearChord(int chordNumber) {
  if (chordNumber < 0 || chordNumber >= maxChords) return;
  int c = chordNumber;
  for (int i = 0; i < chordMaxLength; i++) {
    chord[c][i] = 0;
  }
  chordNoteCount[c] = 0;                           
  chordLength[c] = 0;  
}


// -------------------------------------------------------------------------------------------


// function to reverse chord
void reverseChord() {
  
  int reversedChord[2][currentChordNoteCount];
  for(int i = currentChordNoteCount-1; i >= 0; i--){
    reversedChord[0][currentChordNoteCount-1-i] = chordScaled[0][i];
    reversedChord[1][currentChordNoteCount-1-i] = chordScaled[1][i];
  }
  for (int i = 0; i < currentChordNoteCount; i++) {
    chordScaled[0][i] = reversedChord[0][i];
    chordScaled[1][i] = reversedChord[1][i];
  }
}


// -------------------------------------------------------------------------------------------


// function to rotate chord left
void rotateChordLeft(int amount) {
  if (amount < -127 || amount > 127 || amount == 0) return;
  
  for (int i = 0; i < amount; i++) {
    int firstElement0 = chordScaled[0][0];
    int firstElement1 = chordScaled[1][0];
    for (int i = 0; i < currentChordNoteCount-1; i++) {
        chordScaled[0][i] = chordScaled[0][i+1]; // shift left
        chordScaled[1][i] = chordScaled[1][i+1]; // shift left
      }
    chordScaled[0][currentChordNoteCount-1] = firstElement0;
    chordScaled[1][currentChordNoteCount-1] = firstElement1;
  }
}


// function to rotate chord right
void rotateChordRight(int amount) {
  if (amount < -127 || amount > 127 || amount == 0) return;
  for (int i = 0; i < amount; i++) {
    int lastElement0 = chordScaled[0][currentChordNoteCount-1];
    int lastElement1 = chordScaled[1][currentChordNoteCount-1];
    for (int i = currentChordNoteCount-1; i > 0; i--) {
        chordScaled[0][i] = chordScaled[0][i-1]; // shift right
        chordScaled[1][i] = chordScaled[1][i-1]; // shift right
      }
    chordScaled[0][0] = lastElement0;
    chordScaled[1][0] = lastElement1;
  }
}


// -------------------------------------------------------------------------------------------


int getStretchedNote(int i, int value, int position, int inbetween) {  
  
  int note = 0;

  if (i <= position) {
      note = chordScaled[0][i] + (value * (i-position-inbetween));
  } else {
      note = chordScaled[0][i] + (value * (i-position));
  }
  return note;
}





// -------------------------------------------------------------------------------------------


// function to calculate notes for chord reflection
int getReflectedNote(int noteIndex, int chordNumber) {  
  int reflectionValue = chordReflection[chordNumber][0];
  int reflectionPoint = chordReflection[chordNumber][1];
  int note;
  int reflectionValue2 = 0;
  if (mod(reflectionValue, 2) != 0 ) {              // if odd number
    chordReversedQ = !chordReversedQ;
    int distance = reflectionPoint - noteIndex;
    note = reflectionPoint + distance;
    if (reflectionValue > 1) {
      reflectionValue2 = reflectionValue - 1;
    } 
  } else {
    note = noteIndex;
    reflectionValue2 = reflectionValue;
  }
  
  int addon = (reflectionPoint + (reflectionPoint - chordScaled[0][currentChordNoteCount-1])) * reflectionValue2;
  int finalNote = note + addon;
  return finalNote;
}


// -------------------------------------------------------------------------------------------


// function to get inversion addon for a chord note
int getInversionAddon(int noteIndex_, int chordNumber) {
  int currentInversion = chordInversion[chordNumber];
  int inversionAddon = 0;
  int noteIndex = noteIndex_;
  int octaveMultiplier = 12;
  
  if (currentScaleChord[chordNumber] != 0) octaveMultiplier = 7;
  if (!chordReversedQ) {
    noteIndex = mod(noteIndex_ + currentInversion, currentChordNoteCount);     
  } else {
    noteIndex = mod(noteIndex_ , currentChordNoteCount);     
  }
  if (currentInversion > 0) {      
    inversionAddon = floor((currentInversion + ((currentChordNoteCount-1) - noteIndex)) / currentChordNoteCount) * octaveMultiplier; 
  } else if (currentInversion < 0) {  
    inversionAddon = floor((currentInversion + (0 - noteIndex)) / currentChordNoteCount) * octaveMultiplier;
  }
  return inversionAddon;
}


// -------------------------------------------------------------------------------------------


// function to set current scale based on a chord
void setCurrentScale(int chordNumber) {
  
  // scale is set in three steps:
  // 1: notes from original chord are stored unchanged in chordScaled
  // 2: chord transformations are applied to chordScaled
  // 3: final notes are calculated from chordScaled and added to currentScale
  const int scaleChordNumber = currentScaleChord[chordNumber];

  if (scaleChordNumber >= 0) {
    
    currentChordLength = chordLength[scaleChordNumber];
    currentChordNoteCount = chordNoteCount[scaleChordNumber];
    currentScaleLength[chordNumber] = chordNoteCount[scaleChordNumber];
    
    // 1: add notes in chord to chordScaled (notes as indexes; deltas are all zero)
    for (int i = 0; i < currentChordNoteCount; i++) {
      chordScaled[0][i] = chord[scaleChordNumber][currentScaleLength[chordNumber] - 1 - i]; // indexes
      chordScaled[1][i] = 0;                                                                // deltas
    }
    
    // 2: apply transformations to chordScaled
    applyTransformations(scaleChordNumber);

    // 3: calculate notes from indexes in chordScaled and add them to currentScale
    for (int i = 0; i < currentScaleLength[chordNumber]; i++) {
      const int scaleLengthChromatic = 12;
      const int noteIndex = chordScaled[0][i];
      int octave = 0;
      if (noteIndex >= 0) {
        octave = (noteIndex / scaleLengthChromatic) * 12;            // calc octave from shifted index
      } else {
        octave = (((noteIndex+1) / scaleLengthChromatic) * 12) - 12;            // calc octave from shifted index
      }
      const int finalNote = noteIndex + octave;              // final note incl octave and delta
      currentScale[chordNumber][i] = finalNote; 
    }
    
  }
}


// -------------------------------------------------------------------------------------------


// function to store the current chord as scale indexes and their deltas
void createChordScaled(int chordNumber) {
  for (int i = 0; i < chordNoteCount[chordNumber]; i++) {
    const int noteChord = chord[chordNumber][i];                    // get the note of the original chord
    const int noteChordIndex = getNearest(chordNumber, mod(noteChord, 12));      // calc the note's nearest index in the scale  
    const int delta = noteChord - currentScale[chordNumber][mod(noteChordIndex, currentScaleLength[chordNumber])];    // calc delta betw orgiginal note and note in scale
    chordScaled[0][i] = noteChordIndex;
    chordScaled[1][i] = delta;
  }
  reverseChord();
}


int getNearest(int chordNumber, int x) {
  for (int i = 0; i < currentScaleLength[chordNumber]; i++) {
    if (x == currentScale[chordNumber][i]) {
      return i;
    }
  }
  for (int i = 0; i < currentScaleLength[chordNumber]-1; i++) {
    if (
        (x > currentScale[chordNumber][i] && x < currentScale[chordNumber][i+1])
        || 
        (x < currentScale[chordNumber][i] && x > currentScale[chordNumber][i+1])
        ) {
      return i;
    }
  }
  return 0;
}


// -------------------------------------------------------------------------------------------


// function to calculate the final note from the current scaled chord and an index
int getNoteFromIndex(int chordNumber, int noteIndex_) {
  const int noteIndex = chordScaled[0][noteIndex_];
  const int delta = chordScaled[1][noteIndex_];
  const int noteScaled = currentScale[chordNumber][mod(noteIndex, currentScaleLength[chordNumber])];    // get the note in scale from shifted index
  int octave = 0;
  if (noteIndex >= 0) {
    octave = (noteIndex / currentScaleLength[chordNumber]) * 12;            // calc octave from shifted index
  } else {
    octave = (((noteIndex+1) / currentScaleLength[chordNumber]) * 12) - 12;            // calc octave from shifted index
  }
  const int finalNote = noteScaled + octave + delta;              // final note incl octave and delta
  
  return finalNote;
}

// function to calculate the final note from the current scaled chord and an index
int getNoteFromIndexBuffer(int chordNumber, int noteIndex) {
  //const int noteIndex = chordScaled[0][noteIndex_];
  //const int delta = chordScaled[1][noteIndex_];
  const int noteScaled = currentScale[chordNumber][mod(noteIndex, currentScaleLength[chordNumber])];    // get the note in scale from shifted index
  int octave = 0;
  if (noteIndex >= 0) {
    octave = (noteIndex / currentScaleLength[chordNumber]) * 12;            // calc octave from shifted index
  } else {
    octave = (((noteIndex+1) / currentScaleLength[chordNumber]) * 12) - 12;            // calc octave from shifted index
  }
  //const int finalNote = noteScaled + octave + delta;              // final note incl octave and delta
  const int finalNote = noteScaled + octave;              // final note incl octave and delta
  
  return finalNote;
}


// -------------------------------------------------------------------------------------------


// modulo helper function (that handles negative values correctly)
int mod(int a, int b) {
  int c = a % b;
  return (c < 0) ? c + b : c;
}


// -------------------------------------------------------------------------------------------


void applyTransformations(int chordNumber) {

  // Transformation: REVERSE
  if (chordReverse[chordNumber]) {
    reverseChord();
    chordReversedQ = 1;
  } else {
    chordReversedQ = 0;
  }

  // Transformation: ROTATE
  int currentRotate = chordRotate[chordNumber];
  if (currentRotate != 0) {
    if (currentRotate > 0) rotateChordRight(currentRotate);
    else if (currentRotate < 0) rotateChordLeft(abs(currentRotate)); 
  }

  // Transformation: STRETCH (distort)
  int stretchValue = chordStretch[chordNumber][0];
  int stretchAnchorPoint = chordStretch[chordNumber][1];
  int stretchPosition = 0;
  bool stretchInbetween = 0;
  // calculate where the anchor point is in the chord
  for (int c = 0; c < currentChordNoteCount; c++) {
    if (chordScaled[0][0] > stretchAnchorPoint) {
      stretchPosition = -1; break;
    }
    if (chordScaled[0][c] == stretchAnchorPoint) {
      stretchPosition = c; break;
    }
    if (chordScaled[0][c] < stretchAnchorPoint && chordScaled[0][c+1] > stretchAnchorPoint) {
      stretchPosition = c; 
      stretchInbetween = 1; break;
    }
    else {
      stretchPosition = currentChordNoteCount;
    }
  }
  // get the note based on stretch value and anchor point position
  for (int i = 0; i < currentChordNoteCount; i++) {
    chordScaled[0][i] = getStretchedNote(i, stretchValue, stretchPosition, stretchInbetween); 
  }
  
  // Transformation: SHIFT (transpose)
  for (int i = 0; i < currentChordNoteCount; i++) {
    const int noteIndex = chordScaled[0][i] + chordShift[chordNumber];   // add shift
    chordScaled[0][i] = noteIndex;
  }
  
  // Transformation: REFLECT
  for (int i = 0; i < currentChordNoteCount; i++) {
    chordScaled[0][i] = getReflectedNote(chordScaled[0][i], chordNumber); 
  }
  
  // Transformation: INVERT
  if (chordInversion[chordNumber] > 0) {
    if (!chordReversedQ) {
      rotateChordLeft(chordInversion[chordNumber]);
    } else {
      rotateChordRight(chordInversion[chordNumber]);  
    }
  } 
  else if (chordInversion[chordNumber] < 0) {
    if (!chordReversedQ) { rotateChordRight(abs(chordInversion[chordNumber]));
    } else {
      rotateChordLeft(abs(chordInversion[chordNumber]));
    }
  }
  for (int i = 0; i < currentChordNoteCount; i++) {
    int inversionAddon = getInversionAddon(i, chordNumber);
    chordScaled[0][i] = chordScaled[0][i] + inversionAddon;
  }

}


// -------------------------------------------------------------------------------------------


int getCurveValue(byte curveType, int step, int numberOfSteps, int originalValue, float percStart, float percEnd){
  float valueStart = (originalValue / 100.) * percStart; 
  float valueEnd = (originalValue / 100.) * percEnd;

  if (curveType == 0) {
    return originalValue;
  }

  // linear
  else if (curveType == 1) {
    return valueStart + ((valueEnd - valueStart) / (numberOfSteps - 1) * step);
  }

  // exponential
  else if (curveType == 2) {
    return valueStart + ((valueEnd - valueStart) * pow((1. / (numberOfSteps - 1)) * step, 3));
  }  

  // triangle
  else if (curveType == 3) {
    int period = numberOfSteps/2;
    return valueStart + (((valueEnd - valueStart) / period) * (period - abs(step % (2 * period) - period)));
  }   

  // alternating
  else if (curveType == 4) {
    if (step % 2) return valueEnd;
    else return valueStart;
  }  

  // random
  else if (curveType == 5) {
    if (valueStart > valueEnd) {
      return random(valueEnd + 1, valueStart);  
    } else {
      return random(valueStart, int(valueEnd) + 1);
    }
  }  
  
  return originalValue;
  
}


// -------------------------------------------------------------------------------------------


byte calculateChordLengthByDirection(int chordNumber) {
  // calculate chord length based on play direction
  int currentChordLength2 = currentChordLength;  // forward, backward, inside out, outside in, random
  if (chordDirection[chordNumber] == 7) currentChordLength2 = (currentChordLength * 2) - 1;    // pingpong
  else if (chordDirection[chordNumber] == 8) currentChordLength2 = (currentChordLength * 2);   // ping & pong
  else if (chordDirection[chordNumber] == 5 || chordDirection[chordNumber] == 6) currentChordLength2 = currentChordLength + (currentChordLength - 2);   // bottom repeat or top repeat
  return currentChordLength2;
}


// -------------------------------------------------------------------------------------------


byte getNextChordNoteIndex(int chordNumber, int chordLength, int chordLength2, int index) {
  int i = 0;  
  int j = index;

  // forward 
  if (chordDirection[chordNumber] == 0) i = j;
  
  // backward
  else if (chordDirection[chordNumber] == 1) i = (chordLength - 1) - j;
  
  // inside out
  else if (chordDirection[chordNumber] == 2) { 
    int start = (chordLength - 1) / 2;
    if (chordLength % 2) {  // odd chord length
      i = start;
      for (int k = 0; k <= j; k++) {
        if (k % 2) i -= k;
        else i += k; 
      }
    }
    else {  // even chord length
      i = start;
      for (int k = 0; k <= j; k++) {
        if (k % 2) i += k;
        else i -= k; 
      }
    }
  }

  // outside in
  else if (chordDirection[chordNumber] == 3) { 
    i = 0;
    if (j != 0) {
      for (int k = 1; k <= j; k++) {
        if (k % 2) i = i + chordLength - k;
        else i = i - chordLength + k;
      }
    }  
  }

  // random
  else if (chordDirection[chordNumber] == 4) { 
    i = chordRandomIndices[chordNumber][j];
  }

  // bottom repeat
  else if (chordDirection[chordNumber] == 5) { 
    if (j % 2) i = j / 2 + 1;
    else i = 0;
  } 
  
  // top repeat
  else if (chordDirection[chordNumber] == 6) { 
    if (j % 2) i = chordLength - 1;
    else i = j - (j / 2);
  }

  // pingpong
  else if (chordDirection[chordNumber] == 7) {
    i = (chordLength - 1) - abs(j - (chordLength - 1));
  }
  // ping & pong
  else if (chordDirection[chordNumber] == 8) {
    if (j < chordLength2 / 2) i = j;
    else i = (chordLength2 - 1) - j;
  }
  
  return i;
}


// -------------------------------------------------------------------------------------------


void randomizeChordIndices(int chordNumber, int chordLength) {

  // fill it with 0,1,2,...
  for(int i = 0; i < chordLength; i++) {
    chordRandomIndices[chordNumber][i] = i;
  }
  
  // shuffle it
  for(int i = 0; i < chordLength; i++) {
    int j = random(i, chordLength);
    int t = chordRandomIndices[chordNumber][i];
    chordRandomIndices[chordNumber][i] = chordRandomIndices[chordNumber][j];
    chordRandomIndices[chordNumber][j] = t;
  }

}