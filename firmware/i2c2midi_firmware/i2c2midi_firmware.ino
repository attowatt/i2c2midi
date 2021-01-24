// -------------------------------------------------------------------------------------------
// Basic Slave
// -------------------------------------------------------------------------------------------
//
// This creates a simple I2C Slave device which will print whatever text string is sent to it.
// It will retain the text string in memory and will send it back to a Master device if 
// requested.  It is intended to pair with a Master device running the basic_master sketch.
//
// This example code is in the public domain.
//
// -------------------------------------------------------------------------------------------

#include <i2c_t3.h>
#include <MIDI.h>

// Function prototypes
void receiveEvent(size_t count);

// Memory
#define MEM_LEN 256
uint8_t databuf[MEM_LEN];
volatile uint8_t received;

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
//const int channel = 1;
//int noteDuration = 500;

unsigned long lastNoteMillis[16][4];
int lastNote[16][4] = {
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 },
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 },
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 },
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 }
};

boolean noteOnQ[16][4] = {
  {false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false},
  {false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false},
  {false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false},
  {false,false,false,false},{false,false,false,false},{false,false,false,false},{false,false,false,false}
};

int noteDuration[16][4] = {
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 },
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 },
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 },
  { 0,0,0,0 }, { 0,0,0,0 },{ 0,0,0,0 }, { 0,0,0,0 }
};

int currentPolyphonyIndex[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };


unsigned long previousMillis = 0;
const long interval = 1000; 
int ledState = LOW; 
const int ledPin =  LED_BUILTIN;

void setup()
{
    pinMode(LED_BUILTIN,OUTPUT); // LED
    

    // Setup for Slave mode, address (=66), pins 18/19, external pullups, 400kHz
    Wire.begin(I2C_SLAVE, 0x42, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);

    // Data init
    received = 0;
    memset(databuf, 0, sizeof(databuf));

    // register events
    Wire.onReceive(receiveEvent);
    Serial.begin(115200);

    MIDI.begin();
    
    Serial.println("started");
}

void loop()
{
  Serial.print(lastNote[0][0]);
  Serial.print(", ");
  Serial.print(lastNote[0][1]);
  Serial.print(", ");
  Serial.print(lastNote[0][2]);
  Serial.print(", ");
  Serial.print(lastNote[0][3]);
  Serial.print(", ");
  Serial.println(currentPolyphonyIndex[0]);
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
    sendMidiNote(random(10), 127, 0);
  }

  

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 4; j++) {  
      if (currentMillis - lastNoteMillis[i][j] >= noteDuration && noteOnQ[i][j] == true) {
        MIDI.sendNoteOff(lastNote[i][j], 0, i+1);
        noteOnQ[i][j] = false;
      }
    }
  }
  
  
  if(received) {
      digitalWrite(LED_BUILTIN,HIGH);
      //Serial.printf("Slave received: '%d'\n", databuf[0]);
      
      // MIDI NOTES
      if (databuf[0] == 79 && databuf[1] < 175) {
        for (int i = 0; i < 16; i++) {
          if (databuf[1] == 144 + i) {
            sendMidiNote(databuf[2], databuf[3], i);
            //Serial.println("EX.M.N");
            //Serial.printf("Note: %3d\n", databuf[2]);
            //Serial.printf("Velocity: %3d\n", databuf[3]);
          }  
        }
      }
      
      // MIDI CC
      if (databuf[0] == 79 && databuf[1] > 175) {
        for (int i = 0; i < 16; i++) {
          if (databuf[1] == 176 + i) {
            sendMidiCc(databuf[2], databuf[3], i);
            //Serial.println("EX.M.CC");
            //Serial.printf("Controller: %3d\n", databuf[2]);
            //Serial.printf("Value: %3d\n", databuf[3]);
          }  
        }
      }
      
      // GENERIC PARAMETER
      if (databuf[0] == 70) {
        //Serial.println("EX.P");
        //Serial.printf("Parameter: %3d\n", databuf[1]);
        //int value = (int16_t)(databuf[2] << 8 | databuf[3]);
        //Serial.printf("Value: %3d\n", value);
      }
      
      received = 0;
      digitalWrite(LED_BUILTIN,LOW);
  }
    
}

void receiveEvent(size_t count) {
  if(count<MEM_LEN) {
    Wire.read(databuf, count);  // copy Rx data to databuf
    received += count;           // set received flag to count, this triggers print in main loop
  }
}

void sendMidiNote(int note, int velocity, int dur, int channel){
  currentPolyphonyIndex[channel] = (currentPolyphonyIndex[channel] + 1) % 4;
  int j = currentPolyphonyIndex[channel];
  
  if (noteOnQ[channel][j] == true) {
    MIDI.sendNoteOff(lastNote[channel][j], 0, channel+1);  
  }
  lastNoteMillis[channel][j] = millis();
  lastNote[channel][j] = note; 
  noteOnQ[channel][j] = true;
  MIDI.sendNoteOn(note, velocity, channel+1);
}

void sendMidiCc(int controller, int value, int channel){
  MIDI.sendControlChange(controller, value, channel+1);
}
