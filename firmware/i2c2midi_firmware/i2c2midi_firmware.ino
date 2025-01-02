/* 
  -------------------------------------------------------------------------------------------


  I2C2MIDI MK2 
  – Firmware v5_0_2 experimental disting ex support

  https://github.com/attowatt/i2c2midi

  -------------------------------------------------------------------------------------------

  Copyright (c) 2025 attowatt (http://www.attowatt.com)
  
  MIT License
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software 
  without restriction, including without limitation the rights to use, copy, modify, merge, 
  publish, distribute, sublicense, and/or sell copies of the Software, and to permit 
  persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT 
  OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
  OTHER DEALINGS IN THE SOFTWARE.
  
  -------------------------------------------------------------------------------------------
*/ 


// Turn on MK2 features
    #define MK2

// Set Teensy model (Teensy 3.x vs. Teensy 4.1)
    //#define TEENSY3X
    #define TEENSY41

// USB Device
//  i2c2midi can also act as a USB device and send MIDI data over the Teensy Micro USB jack to a host (e.g. a computer).
//  Please note: Do not connect Teensy USB and Euro Power at the same time! Please cut the 5V pads on the Teensy!
//  Select Tools -> USB Type "MIDI" in Teensyduino, and uncomment the next line to turn the MIDI device feature on:
//  #define USB_DEVICE

// Debug Mode
//  Turn on debug mode to plot some debug message to the Serial Monitor
    //#define DEBUG      

// Turn on testing mode
//  Sending: channel 1, random note between  50 and 70, velocity 127
    //#define TEST

// Experimental feature enabling multiple devices for USB MIDI out
    //#define MULTIPLEUSBOUT


// -------------------------------------------------------------------------------------------


#include <MIDI.h>                   
// MIT License – Copyright (c) 2016 Francois Best
// https://github.com/FortySevenEffects/arduino_midi_library
// Used for MIDI communication

#ifdef TEENSY3X
  #include <i2c_t3.h>                 
  // MIT License – Copyright (c) 2013-2018, Brian (nox771 at gmail.com)
  // https://github.com/nox771/i2c_t3
  // Used for I2C communication on Teensy 3.x
#endif
#ifdef TEENSY41
  #include <i2c_driver.h>
  #include <i2c_driver_wire.h>
  // MIT License – Copyright © 2019-2020 Richard Gemmell
  // https://github.com/Richard-Gemmell/teensy4_i2c
  // Used for I2C communication on Teensy 4.1
#endif

#ifdef MK2
  #include <USBHost_t36.h>            
  // MIT License – Copyright 2017 Paul Stoffregen (paul@pjrc.com)
  // https://github.com/PaulStoffregen/USBHost_t36
  // Used for USB MIDI Host
#endif

#include <Ramp.h>
// GPL-3.0 License – Sylvain GARNAVAULT - 2016/08/07
// https://github.com/siteswapjuggler/RAMP
// Used for ramping CC values


// -------------------------------------------------------------------------------------------


// USB Host
//  The front panel USB jack ...
//  ... receives data from MIDI controllers (MIDI in)
//  ... sends out MIDI data to devices (MIDI out)
#ifdef MK2
  USBHost myusb;                                        
  USBHub hub1(myusb);                                   // USB host: MIDI in     
  MIDIDevice_BigBuffer midiDevice(myusb);               // USB host: MIDI out
  #ifdef MULTIPLEUSBOUT
    USBHub hub2(myusb);
    MIDIDevice_BigBuffer midiDevice2(myusb);     
    MIDIDevice_BigBuffer* midiDeviceList[2] = { 0 };
  #endif
#endif

// I2C
#define MEM_LEN 256                 // save I2C data in 8 bit chunks
uint8_t i2cData[MEM_LEN];           // save I2C data in variable "i2cData"
volatile uint8_t received;          // variable to store if there are received I2C messages
void i2cReceiveEvent(size_t count); // function for receiving I2C messages, count = number of bites
void i2cRequestEvent(void);         // function for receiving I2C messages, count = number of bites

// I2C Address
//  i2c2midi acts as a I2C follower and listens to messages on address 0x3F (63).
//  const uint8_t i2cAddress = 0x3F;    // official I2C address for Teletype I2M OPs

// To use Teletype's distingEX OPs, change the address back to 0x42. 
byte i2cAddress = 0x41;           // legacy I2C addresses for Teletype distingEX OPs (EX1: 0x41, EX2: 0x42, EX3: 0x43, EX4: 0x44)


// MIDI TRS
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  

 
// -------------------------------------------------------------------------------------------


// channels
#ifdef MK2
  const byte channelsOut = 32;    // number of MIDI channels OUT (1-16 for TRS out, 17-32 for USB out)
  const byte channelsIn = 16;     // number of MIDI channels IN  (16 for USB in)
#else
  const byte channelsOut = 16;    // number of MIDI channels OUT
  const byte channelsIn = 0;      // number of MIDI channels IN
#endif

// channels
int channelMute[channelsOut];     // mute state for each MIDI out channel
int channelSolo[channelsOut];     // solo state for each MIDI out channel

// notes
const byte maxNotes = 8;
unsigned long notes[channelsOut][maxNotes][7];  
// array to store notes: 
// 0 : note number
// 1 : start time
// 2 : duration
// 3 : currently on/off
// 4 : velocity
// 5 : ratchet count
// 6 : repeat count
int noteCount[channelsOut];                        // total note count, per channel
int currentNote[channelsOut];                      // current note number between 0 and maxNotes, per channel
int currentNoteDuration[channelsOut];              // setting for note duration per channel
int8_t currentNoteShift[channelsOut];              // setting for note shift per channel
byte currentRepetition[channelsOut];               // setting for note repetition per channel
byte currentRatcheting[channelsOut];               // setting for note ratcheting per channel
const int ratchetingLength = 75;                   // in percent: 75 means 75% of original note length for racheted notes
byte noteUpperLimit[channelsOut];                  // setting for highest allowed midi note per channel
byte noteLowerLimit[channelsOut];                  // setting for lowest allowed midi note per channel
byte noteLimitMode[channelsOut];                   // setting for limit mode per channel

// CCs
int CCs[channelsOut][127][3];                    
// array to store CC values: 
// 1 : current value
// 2 : slew time in ms
// 3 : offset

// NRPNs
const byte maxNRPNs = 8;
int NRPNs[maxNRPNs][4];
// array to store NRPN settings
// 0 : channel
// 1 : controller
// 2 : offset
// 3 : slew time
byte nrpnCount = 0;

// Chords
const byte maxChords = 8;                          // number of chords
const byte chordMaxLength = 8;                     // maximum allowed length of chords
int chord[maxChords][chordMaxLength];              // array to store chords
byte chordNoteCount[maxChords];                    // number of added notes per chord
byte chordLength[maxChords];                       // size of chord that should be played
int chordScaled[2][chordMaxLength];                // store current chord w transformations; 0 scaled notes, 1 deltas
byte currentChordLength;                           // length of current chord
byte currentChordNoteCount;                        // number of added notes in current chord
int chordReverse[maxChords];                       // reverse transformation setting per chord
bool chordReversedQ = 0;                           // keeping track of wether the chord is reversed or not
int chordRotate[maxChords];                        // rotate transformation setting per chord
int chordInversion[maxChords];                     // inversion transformation setting per chord
int chordStrumming[maxChords];                     // strumming transformation setting per chord
int chordShift[maxChords];                         // shift transformation setting per chord
int chordStretch[maxChords][2];                    // stretch transformation setting per chord; 0 value, 1 anchor point
int chordReflection[maxChords][2];                 // reflection setting per chord; 0 refl value, 1 refl point
int currentScaleChord[maxChords];                  // storing the chord number which is set as the scale for each chord
int currentScale[maxChords][12];                   // storing the current scale for each chord
int currentScaleLength[maxChords];                 // storing the length of the current scale for each chord
int curveVelocity[maxChords][3];                   // type of curve, start percent, end percent
int curveTime[maxChords][3];                       // type of curve, start percent, end percent 
byte chordDirection[maxChords];                    // play direction for chord
int chordNotePlayCount[maxChords];                 // count how often a note of a chord has been played 
int chordRandomIndices[maxChords][chordMaxLength*2];

// Scheduled notes
const byte maxNotesScheduled = 42;                 // maximum allowed notes for scheduling
unsigned long scheduledNotes[maxNotesScheduled][7];
// array to store scheduled notes: 
// 0 : note number
// 1 : start time scheduled
// 2 : duration
// 3 : velocity
// 4 : channel
// 5 : chord number
// 6 : note index
byte scheduledNoteCount = 0;                       // total count of scheduled notes

// MIDI buffer
const uint8_t maxBuffer = 64;
unsigned long buffer[maxBuffer][10];
// 0 : channel
// 1 : note number
// 2 : velocity
// 3 : start time
// 4 : duration
// 5 : buffer round
// 6 : original velocity
// 7 : chord number
// 8 : note index
// 9 : note index original
// internal values
uint8_t bufferCount = 0;                           // the current splot position in the buffer array
uint8_t bufferRoundCount = 0;                      // keeping track of rounds
int bufferOffset = 0;                              // time when play direction was changed
bool bufferReverse = 0;                            // play direction
unsigned long bufferFrame = 0;
elapsedMicros bufferElapsedMicros;
// parameters set by OPs
byte bufferRecord = 0;                             // if notes should be recorded to buffer or not
int bufferLength = 1000;                           // length of the buffer in ms
int bufferStartOffset = 0;                         // offset added to the start in ms
int bufferEndOffset = 0;                           // negative offset added to the end in ms
byte bufferDirection = 0;                          // direction of buffer, 0 = forward, 1 = backward, 2 = ping pong
int bufferSpeed = 100;                             // buffer speed, 100 = normal speed, 50 = double speed, 200 = half speed
byte bufferFeedback = 8;                           // how many rounds the note should survive (affects note velocity)
int bufferPitchShift = 0;                          // pitch added to note with each round
int bufferDurationShift = 0;                       // duration added to note with each round
int bufferVelocityShift = 0;
int bufferNoteOffset = 0;                          // pitch offset added to all notes, fixed for all rounds
int bufferVelocityOffset = 0;                      // velocity offset added to all notes, fixed for all rounds
int bufferDurationOffset = 0;                      // duration offset added to all notes, fixed for all rounds
byte bufferMode = 0;                               // buffer mode, 0 = "digital", 1 = "tape" (pitch/duration fixed to speed)

// Ramp (slew)
const byte maxRamps = 8;                           // maximum allowed ramps
rampInt* myRamps = new rampInt[maxRamps];          // intialize 8 ramps 
int lastRampValues[maxRamps];                      // store the latest ramp values (used for comparison with new values)
int rampsAssignedCCs[maxRamps][3];                 // which CC was assigned to each ramp: channel, controller, NRPN?
const byte maxRampSpeed = 30;                      // shortest intervall between outgoing MIDI CC messages when ramping
byte rampCount;                                    // total ramp count
byte currentRamp;                                  // current ramp number between 0 and maxRamps

// USB MIDI in
byte CCsIn[channelsIn][127];                       // array to store MIDI CCs values received via USB MIDI in
byte noteHistoryIn[channelsIn][16][2];             // array to store the last 8 received MIDI notes: note number, velocity
const byte noteHistoryInLength = 16;               // length of the note history per channel
bool latch = true;                                 // latch setting for note history 
byte lastChannelIn = 1;                            // the last channel received via MIDI in
byte lastNoteIn = 0;                               // the last note number of Note On message received via MIDI in
byte lastVelocityIn = 0;                           // the last note velocity of Note On messagereceived via MIDI in
byte lastNoteOffIn = 0;                            // the last note number of Note Off message received via MIDI in  
byte lastCIn = 0;                                  // the last controller number received via MIDI in
byte lastCCIn = 0;                                 // the last CC value received via MIDI in

// LEDs
const byte led1 = 3;                               // pin definition for led 1
const byte led2 = 2;                               // pin definition for led 2
unsigned long lastLEDMillis1 = 0;                  // last time LED 1 turned on
unsigned long lastLEDMillis2 = 0;                  // last time LED 2 turned on
const byte animationSpeed = 100;                   // start up animation speed


// -------------------------------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------------------------------


void setup() {

  // LEDs
  pinMode(led1,OUTPUT); 
  pinMode(led2,OUTPUT); 

  // I2C
  #ifdef TEENSY3X
    // setup for slave mode, address, pins 18/19, external pullups, 400kHz
    Wire.begin(I2C_SLAVE, i2cAddress, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);  
  #endif
  #ifdef TEENSY41
    Wire.begin(i2cAddress);        
  #endif
  received = 0;                                    // i2c data initalize
  memset(i2cData, 0, sizeof(i2cData));             // save I2C data in memory
  Wire.onReceive(i2cReceiveEvent);                 // register i2c events
  Wire.onRequest(i2cRequestEvent);

  // Serial
  Serial.begin(115200);

  // MIDI & USB MIDI
  MIDI.begin();
  #ifdef MK2
    #ifdef MULTIPLEUSBOUT
      midiDeviceList[0] = &midiDevice;
      midiDeviceList[1] = &midiDevice2;
    #endif
    myusb.begin();
  #endif

  // setup up ramps
  for (int i = 0; i < maxRamps; i++) {
    myRamps[i].setGrain(maxRampSpeed);
  }

  // setup up default scales
  for (int i = 0; i < maxChords; i++) {
    currentScaleChord[i] = 0;
    currentScaleLength[i] = 12;
    for (int j = 0; j < 12; j++) {
      currentScale[i][j] = j;
    }
  }

  // setup default note duration & note shift
  for (int i = 0; i < channelsOut; i++) {
    currentNoteDuration[i] = 100;
    currentNoteShift[i] = 0;
    currentRepetition[i] = 1;
    currentRatcheting[i] = 1;
    noteUpperLimit[i] = 127;
    noteLowerLimit[i] = 0;
    noteLowerLimit[i] = 0;
    noteLimitMode[i] = 0;
    channelMute[i] = 0;
    channelSolo[i] = 0;
  }

  // start up animation
  for (int i = 0; i < 4; i++) {
    digitalWrite(led2,HIGH); delay(animationSpeed);
    digitalWrite(led1,HIGH); delay(animationSpeed);
    digitalWrite(led2,LOW);  delay(animationSpeed);
    digitalWrite(led1,LOW);  delay(animationSpeed);
  }

  #ifdef DEBUG
    Serial.println("started");
  #endif

}


// -------------------------------------------------------------------------------------------
// LOOP
// -------------------------------------------------------------------------------------------


void loop() {

  // I2C   
  if(received) {
    #ifdef DEBUG
      Serial.print(received); 
      Serial.print(" bytes received: ");
      for (int i = 0; i < received; i++) {
        Serial.print(i2cData[i]);
        Serial.print(" ");
      }
      Serial.println("");
    #endif

    opFunctions(false, i2cData);                   // call the respective OP with isRequest = false
    received = 0;                                  // reset back to 0
  }

  #ifdef MK2
    // USB MIDI
    if (midiDevice.read()) {  
    
      uint8_t type =       midiDevice.getType();
      uint8_t channel =    midiDevice.getChannel();
      uint8_t data1 =      midiDevice.getData1();
      uint8_t data2 =      midiDevice.getData2();

      //const uint8_t *sys = midiDevice.getSysExArray();  // not used at the moment

      #ifdef DEBUG
        Serial.print("USB received");
        Serial.print("Type: "); Serial.println(type);
        Serial.print("channel: "); Serial.println(channel);
        Serial.print("data1: "); Serial.println(data1);
        Serial.print("data2: "); Serial.println(data2);
      #endif

      if (channel < 1 || channel > 16) return;

      // NOTE ON (status 144-159)
      if (type >= 144 && type <= 159) {
        int noteNumber = data1;
        if (noteNumber < 0 || noteNumber > 127) return;
        int velocity = data2;
        if (velocity < 0) {                                         // velocity = 0 is treated as Note Off
          if (!latch) {
            removeFromNoteHistoryIn(channel-1, noteNumber);         
          }
        };
        if (velocity > 127) velocity = 127;
        addToNoteHistoryIn(channel-1, noteNumber, velocity);        // store the note number to the history
        lastNoteIn = noteNumber;                                    // store note number as last received
        lastVelocityIn = velocity;
      }

      // NOTE OFF (status 128-143)
      else if (type >= 128 && type <= 143) {
        int noteNumber = data1;
        if (noteNumber < 0 || noteNumber > 127) return;
        if (!latch) {
          removeFromNoteHistoryIn(channel-1, noteNumber);           // remove the note number from the history if latch = 0 
        }
        lastNoteOffIn = noteNumber;                                 // store note number as last received
      }
      
      // CC (status 176-191)
      else if (type >= 176 && type <= 191) {
        int controller = data1;
        if (controller < 0 || controller > 127) return;
        int value = data2;
        if (value < 0)   value = 0;
        if (value > 127) value = 127;
        CCsIn[channel-1][controller] = value;                       // store CC value 
        lastCIn = controller;                                       // store controller number as last received
        lastCCIn = value;                                           // store CC value as last received
      }
      
      lastChannelIn = channel;                                      // store the channel as last used channel

      blinkLED(2);

    }
  #endif

  checkNoteDurations();             // check if there are notes to turn off
  checkScheduledNotes();            // check scheduled notes
  checkBuffer();
  updateRamps();                    // update all ramps for MIDI CC 
  checkLEDs();                      // check if the LEDs should be turned off
  
  #ifdef TEST
    TESTFunction();                 // function for testing
  #endif

}