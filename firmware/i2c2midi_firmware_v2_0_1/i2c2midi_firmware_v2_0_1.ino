// April 23, 2021 
// v2_0_1
// https://github.com/attowatt/i2c2midi

#include <i2c_t3.h>
#include <MIDI.h>

void receiveEvent(size_t count);

// memory
#define MEM_LEN 256
uint8_t databuf[MEM_LEN];
volatile uint8_t received;

// MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// values init
int noteDuration = 500;
unsigned long lastNoteMillis[16];
int lastNote[16] =    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
boolean noteOnQ[16] = { false,false,false,false,
                        false,false,false,false,
                        false,false,false,false,
                        false,false,false,false };
int led1 = 2;
int led2 = 3;
unsigned long lastLEDMillis1 = 0;
unsigned long lastLEDMillis2 = 0;

int animationSpeed = 100;

void setup()
{
  // light up the onboard LED on power on
  pinMode(LED_BUILTIN,OUTPUT); 
  pinMode(led1,OUTPUT); 
  pinMode(led2,OUTPUT); 

  // setup for Slave mode, address (= 66), pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_SLAVE, 0x42, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);

  // data init
  received = 0;
  memset(databuf, 0, sizeof(databuf));

  // register events
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);

  // Start MIDI
  MIDI.begin();

  digitalWrite(led1,HIGH); delay(animationSpeed);
  digitalWrite(led2,HIGH); delay(animationSpeed);
  digitalWrite(led1,LOW); delay(animationSpeed);
  digitalWrite(led2,LOW); delay(animationSpeed);
  digitalWrite(led1,HIGH); delay(animationSpeed);
  digitalWrite(led2,HIGH); delay(animationSpeed);
  digitalWrite(led1,LOW); delay(animationSpeed);
  digitalWrite(led2,LOW); delay(animationSpeed);
  digitalWrite(led1,HIGH); delay(animationSpeed);
  digitalWrite(led2,HIGH); delay(animationSpeed);
  digitalWrite(led1,LOW); delay(animationSpeed);
  digitalWrite(led2,LOW); delay(animationSpeed);
  digitalWrite(led1,HIGH); delay(animationSpeed);
  digitalWrite(led2,HIGH); delay(animationSpeed);
  digitalWrite(led1,LOW); delay(animationSpeed);
  digitalWrite(led2,LOW); delay(animationSpeed);

  // debug
  //Serial.println("started");
}

void loop()
{
  // get current time
  unsigned long currentMillis = millis();
  
  // if there hasn't been a note off yet, send note off after a certain time (noteDuration)
  for (int i = 0; i < 16; i++) {
    if (currentMillis - lastNoteMillis[i] >= noteDuration && noteOnQ[i] == true) {
      MIDI.sendNoteOff(lastNote[i], 0, i+1);
      digitalWrite(led2,LOW);
      noteOnQ[i] = false;
    }
  }
  
  // when I2C message are received
  if(received) {

      digitalWrite(led1,HIGH);
      lastLEDMillis1 = millis();

      // debug
      //Serial.printf("Slave received: '%d'\n", databuf[0]);
      
      // send MIDI NOTES
      // This part is set up to react to I2C messages from Teletype using the Disting EX MIDI OPs.
      // see https://github.com/scanner-darkly/teletype/wiki/DISTING-EX-I2C-SPECIFICATION or the Disting Ex manual.
      // MIDI messages have the command 0x4F == 79
      // The messages also include a status. See https://www.midimountain.com/midi/midi_status.htm
      // Note on messages have status 144-159 for MIDI channels 1-16
      
      if (databuf[0] == 79 && databuf[1] <= 159) {
        for (int i = 0; i < 16; i++) {
          if (databuf[1] == 144 + i) {
            sendMidiNote(databuf[2], databuf[3], i);
          }  
        }
      }
      
      // send MIDI CC
      // Control Change messages have status 176-191 for MIDI channels 1-16
      
      if (databuf[0] == 79 && databuf[1] >= 176) {
        for (int i = 0; i < 16; i++) {
          if (databuf[1] == 176 + i) {
            sendMidiCc(databuf[2], databuf[3], i);
            //Serial.println("EX.M.CC");
            //Serial.printf("Controller: %3d\n", databuf[2]);
            //Serial.printf("Value: %3d\n", databuf[3]);
          }  
        }
      }
      
      // TBD
      // generic parameters (Disting Ex Algorithm Parameters)
      //if (databuf[0] == 70) {
        //Serial.println("EX.P");
        //Serial.printf("Parameter: %3d\n", databuf[1]);
        //int value = (int16_t)(databuf[2] << 8 | databuf[3]);
        //Serial.printf("Value: %3d\n", value);
      //}
      
      received = 0;
      
  }  

  checkLEDs();  
}

// function for receiving I2C messages
void receiveEvent(size_t count) {
  if(count<MEM_LEN) {
    // copy Rx data to databuf
    Wire.read(databuf, count);
    // set received flag to count, this triggers main loop  
    received += count;           
  }
}

// function for sending MIDI note on/off
void sendMidiNote(int note, int velocity, int channel){
  // if there hasn't been a note off yet, send note off now
  if (noteOnQ[channel] == true) {
    MIDI.sendNoteOff(lastNote[channel], 0, channel+1);  
    digitalWrite(led2,LOW);
  }
  lastNoteMillis[channel] = millis();
  lastNote[channel] = note; 
  noteOnQ[channel] = true;
  MIDI.sendNoteOn(note, velocity, channel+1);
  digitalWrite(led2,HIGH);
  lastLEDMillis2 = millis();
}

// function for sending MIDI CCs
void sendMidiCc(int controller, int value, int channel){
  MIDI.sendControlChange(controller, value, channel+1);
  digitalWrite(led2,HIGH);
  lastLEDMillis2 = millis();
}

void checkLEDs() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastLEDMillis1 >= 100) {
    digitalWrite(led1,LOW);
  }
  if (currentMillis - lastLEDMillis2 >= 100) {
    digitalWrite(led2,LOW);
  }
}
