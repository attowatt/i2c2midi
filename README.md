# i2c2midi

**i2c2midi is a simple DIY 4 hp eurorack module that speaks I2C and MIDI.**
It's primarily build to be used together with [monome Teletype](https://monome.org/docs/teletype/). It receives I2C messages from Teletype and converts them to MIDI notes, MIDI CC messages and other MIDI messages to control external devices like synths and effects.

- handles MIDI note off messages automatically
- variable note duration
- 8 voice polyphony per MIDI channel
- 16 MIDI channels simultaneously

![](pictures/i2c2midi_diagram.png)

## Table of content
[Details](#Details)  
[Usage](#usage)  
[About the firmware](#about-the-firmware)  
[Schematic](#schematic)  
[BOM](#BOM)  
[Build Guide](#Build-Guide)  
[Changelog](#Changelog)  
[Thanks](#thanks)  



## Details
- DIY eurorack module (early stage)
- 4 hp
- Inputs: I2C 3 pin
- Outputs: MIDI TRS (Type A)
- Based on Teensy 3.2

![](pictures/i2c2midi_v_2_0_side.jpg)
 

## Usage
The following commands are currently available:

```
EX 2                      // tells Teletype that following code in the script is meant for i2c2midi
EX2:                      // tells Teletype that following line is meant for i2c2midi 
EX.M.CH channel           // set the MIDI channel
```
```
EX.M.N note velocity      // send MIDI Note
EX.M.CC controller value  // send MIDI CC
EX.M.PB value             // sent MIDI Pitch Bend (-8192 - 8191); affects all notes per channel
EX.M.PRG number           // sent MIDI Program Change
```
```
EX.M.CLK                  // send MIDI clock pulse
EX.M.START                // start MIDI transport start
EX.M.STOP                 // stop MIDI transport stop
EX.M.CONT                 // continue MIDI transport
```
```
EX.P 1 value              // set note duration in milliseconds
EX.P 2 value              // send MIDI Aftertouch value (0 - 127); affects all notes of last used channel
EX.P 99 value             // set I2C address of i2c2midi (65 for EX 1, 66 for EX 2, 67 for EX 3, 68 for EX 4)
```

**Note off messages**

The firmware takes care of Note off messages automatically, depending on the current note duration value (EX.P 1).

**LEDs**

The left LED lights up when I2C messages are incoming.
The right LED lights up when MIDI messages are outgoing.




## About the firmware

The firmware is written specifically for I2C messages sent from [monome Teletype](https://monome.org/docs/teletype/) using the [disting Ex MIDI OPs](https://github.com/scanner-darkly/teletype/wiki/DISTING-EX-INTEGRATION) by [scanner-darkly](https://github.com/scanner-darkly). The i2c2midi module “poses” as a second disting.
Based on that setup, there are a few things to note and hardcoded within the firmware:

- The following addresses are specifically reserved for the disting EX within the Teletype firmware: `0x41`, `0x42`, `0x43`, `0x44` (65, 66, 67, 68). Since the module should act like a second disting EX, it is set up to listen to I2C messages on address `0x42` (66). This could be changed to act as the first (65), third (67) or fourth (68) disting EX instead.

- The I2C messages for the disting Ex are constructed as `<address> <command> <optional bytes according to command>`. See [here](https://github.com/scanner-darkly/teletype/wiki/DISTING-EX-I2C-SPECIFICATION) or [here](https://www.expert-sleepers.co.uk/distingEXfirmwareupdates.html) for further details. 

- The “send MIDI message” command of the disting Ex is `0x4F`(79) and is constructed as `<address> 0x4F <status> <optional data byte 0> <optional data byte 1>`.

- `<status>`refers to the [MIDI status](https://www.midimountain.com/midi/midi_status.htm). MIDI note on messages have the status 144-159 for channels 1-16. MIDI CC messages have the status 176-191 for channels 1-16.

- Sending a MIDI note on (e.g. note 48, velocity 127, channel 1) therefore looks like this: `0x42 0x4F 144 48 127`.

- Sending a MIDI CC (e.g. controller 1, value 60, channel 1) therefore looks like this: `0x42 0x4F 176 1 60`.

- Same applies to MIDI Pitch Bend and Program Change messages.

- There is also a general I2C message used to control parameters of a disting Ex algorithm. This message is hijacked for additional parameters of the i2c2midi module. E.g. parameter 1 is hardcoded for note duration, parameter 2 for Aftertouch, etc.

**USB MIDI**

If you want to to modify the module and use Teensy’s USB port for MIDI out: USB MIDI can be activated in the firmware by uncommenting `#define USB_MIDI` in the source code. All MIDI messages will then be sent to the USB port of the Teensy, as well as the TRS output of the module. Please make sure to select `Tools` → `USB Type: Serial + MIDI` when uploading the firmware to the Teensy.

**Libraries used**

The firmware uses the [i2c_t3 Library](https://github.com/nox771/i2c_t3) for reading I2C and [Arduino MIDI library](https://github.com/FortySevenEffects/arduino_midi_library/) for sending MIDI.




## Schematic

![](hardware/i2c2midi_schematic.png)

**MIDI TRS Type A Wiring:**
Teensy | TRS | MIDI
--- | --- | ---
Teensy Pin 1 | → Tip | → MIDI Pin 5
Teensy 3V3 | → Ring | → MIDI Pin 4
Ground | → Sleeve | → MIDI Pin 2


## BOM

Number | Part | Value
--- | --- | ---
1 | T32 | Teensy 3.2
2 | C1 | 0.33 uF capacitor
3 | C2 | 0.1 uF capacitor
4 | D1 | LED 3 mm
5 | D2 | LED 3 mm
6 | I2C | 3x2 pin header 
7 | IC1 | 7805 5V regulator
8 | R1 | 2.2k Ω resistor
9 | R2 | 2.2k Ω resistor
10 | R3 | 47 Ω resistor
11 | R4 | 47 Ω resistor
12 | R5 | 220 Ω resistor
13 | R6 | 220 Ω resistor
14 | POW | 2x5 power header
15 | U1 | Green Thonkiconn Stereo 3.5mm Audio Jack (PJ366ST)
16 | | Nut for Thonkiconn
17 | | 1x14 pin header for Teensy
18 | | 1x14 pin header for Teensy
19 |  | 10 mm M3 Hex standoffs
20 |  | 10 mm M3 screw
21 |  | 10 mm M3 screw


![](pictures/i2c2midi_v_2_0_kit.jpg)

## Build Guide

Please find a Build Guide [here](hardware/README.md).

## Changelog

**Version 2.5**
- added optional USB MIDI support (for one's own modifications involving USB MIDI)

**Version 2.4.4**
- updated firmware
  - added MIDI clock, via EX.M.CLK
  - added MIDI transport start, stop and continue, via EX.M.START, EX.M.STOP and EX.M.CONT
  - I2C address can be set via EX.P 99 

**Version 2.3.0**
- new firmware
  - added 8-voice polyphony for 16 channels
  - note duration can be set in ms via EX.P 1 value 
  - added MIDI Pitch Bend, via EX.M.PB value (min: -8192, max: 8191)
  - added MIDI Program Change, via EX.M.PRG program

**Version 2.0**
- Proper PCB
- Proper panel
- Added two LEDs

**Version 1.1**
- Added a voltage regulator, so the module can be powered from the case with a normal eurorack power ribbon cable.

**Version 1.0**
- Initial version using a protoboard and LEGO 2x16 plate as front panel.



## Thanks

- To [scanner-darkly](https://github.com/scanner-darkly) – for developing the disting EX Teletype OPs and helping out on several occasions during the making of this module.
- To [Ansome](https://www.instagram.com/ansomeuk/) – for helping me out on multiple occasions trying to learn Eagle CAD.


## Sources

- https://www.pjrc.com/teensy/external_power.html
- https://www.pjrc.com/teensy/td_libs_MIDI.html
- https://github.com/nox771/i2c_t3
- https://github.com/TomWhitwell/MTM-Parts-Library
