# i2c2midi

i2c2midi is a simple DIY 3 hp eurorack module that speaks I2C and MIDI. It's primarily build to be used together with [monome Teletype](https://monome.org/docs/teletype/). It receives I2C messages and converts them to either MIDI notes or MIDI CC messages.

## Details
- DIY eurorack module 
- 3 hp
- Inputs: I2C 3 pin
- Outputs: MIDI TRS (Type A)

## Pictures 

![](pictures/i2c2midi_top.jpg)

![](pictures/i2c2midi_side.jpg)

## Repository
- `firmware` — The Teensy/Arduino code that converts I2C messages to MIDI out messages.
- `hardware` — Everything hardware related
  - Schematic
  - Protoboard layout (with and without components)
  - Fritzing file
  - Important note about power cabling (!)
- `pictures` — Some pictures of the first working prototype.
  
