# i2c2midi

### i2c2midi is a DIY open source 2 hp eurorack module that speaks I2C and MIDI.

i2c2midi bridges the gap between [monome Teletype](https://monome.org/docs/teletype/) and external MIDI-enabled devices, using I2C:  
It receives I2C messages from Teletype and converts them to MIDI notes, MIDI CC messages and other MIDI messages to control external devices like synths and effects.  
It receives MIDI messages from external MIDI controllers and stores the values internally, which can be requested at any time by Teletype via I2C.

- Dedicated Teletype OPs: `I2M`
- MIDI *out* via TRS
- MIDI *out* and *in* via USB Host
- automatic MIDI Note Off messages (settable note duration per note)
- 8 voice polyphony per MIDI channel
- 16 MIDI channels simultaneously
- 2 HP, 42 mm depth


i2c2midi lines community thread:   
https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/

Please get the Teletype Beta firmware here:  
https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/40950/229

![](https://github.com/attowatt/i2c2midi/blob/main/MKII/hardware/pictures/i2c2midi_MK2_shot.jpg)


## Table of content
[Connections](#connections)  
[Teletype OPs](#teletype-ops)   
[Schematic](#schematic)  
[BOM](#bom)  
[Build Guide](#build-guide)   
[Firmware](#firmware)  
[Thanks](#thanks)  
[Sources](#sources)  




## Connections

### I2C

i2c2midi acts as an I2C follower device and listens to address `0x3F` (63) on the bus.

### TRS

Use a TRS MIDI adapter (Type A) to connect a standard MIDI DIN cable to the TRS output of i2c2midi.

i2c2midi can be used with up to 16 different external MIDI-enabled devices simultaneously, with each device receiving MIDI messages on one of the 16 available MIDI channels. Just make sure each device is set to a different MIDI channel. Use a MIDI through splitter to connect all devices. Set the MIDI channel with `I2M.CH` to 1..16 for the TRS output.

Please note: The TRS connection is MIDI *out* only!

![](https://github.com/attowatt/i2c2midi/blob/main/MKII/hardware/pictures/i2c2midi_MK2_diagram.png)


### USB 

The USB connection can be used for either MIDI *out* to connect additional devices (e.g. Teenage Engineering OP-1), or MIDI *in* to receive MIDI Note or CC messages from an external MIDI controller.

Several devices can be connected using a USB hub.

i2c2midi acts as a USB host, which makes it possible to connect external USB devices without the need of a computer. However, this also means that a computer (which acts as a host itself) can not be connected via the USB A jack on the front panel. If you want to connect a USB host, you can use the Micro USB jack of the Teensy (on the back of the module) and enable the `USB_DEVICE` feature flag in the firmware. 

#### USB MIDI out

The USB MIDI out connection works like the TRS output, but the 16 available MIDI channels can be addressed separately from TRS. Set the MIDI channel with `I2M.CH` to 17..32 for MIDI channels 1..16 on the USB connection.

#### USB MIDI in

If an external MIDI controller is connected via USB, incoming MIDI messages are stored internally in i2c2midi, and can be requested by Teletype at any time:

 - MIDI Note messages: The last eight notes and their velocities are stored in a rotating "note history". Each new received note gets added to the history and pushes the oldest note out. The notes and velocities can be requested with `I2M.Q.N` and `I2M.Q.V`.
 - MIDI CC messages: All values (127 controllers * 16 channels = 2032 values total) are stored and can be requested with `I2M.Q.CC`.

#### Please note: Use external power or a powered USB hub!

If you connect a MIDI device without its own power supply, use a powered USB hub with power supply in between i2c2midi and the MIDI device. This prevents too much current from being drawn from the Teensy. In case of OP-1, please turn off USB charging by pressing `shift` + `COM`, then `OPT` (T4), then turn the blue encoder to toggle USB charging off.

#### Please note: Connecting the Teensy to your computer

Caution: Do not connect power from the modular and the default USB port of the Teensy at the same time, unless you have separated the 5V pads on the Teensy!! Otherwise you will damage your Teensy. For more info, please follow [this link](https://www.pjrc.com/teensy/external_power.html).



### LEDs

- The top LED lights up when MIDI messages are coming in.
- The bottom LED lights up when MIDI messages are going out.



## Teletype OPs


### MIDI out: Settings
*Settings for outoing MIDI messages*  


- `I2M.CH` / `I2M.CH x`  
Get currently set MIDI channel / Set MIDI channel `x` (1..16 for TRS, 17..32 for USB) for MIDI out. Use MIDI channels 1-16 for TRS output, 17-32 for USB output. Default is `x = 1`.

- `I2M.TIME` / `I2M.TIME x`  
`I2M.T`  
Get current note duration / Set note duration for MIDI notes to `x` ms (0..32767). Based on note duration, i2c2midi will send a MIDI Note Off message automatically. Set to `0` to deactivate automatic Note Off messages. Default is `x = 100`.

- `I2M.SHIFT` / `I2M.SHIFT x`  
`I2M.S`  
Get current transposition / Set transposition of MIDI notes to `x` semitones (-127..127). Default is `x = 0`. 

- `I2M.MIN` / `I2M.MIN x`  
Get current minimum note number / Set minimum note number for MIDI notes to `x` (0..127). Default is `x = 0`.

- `I2M.MAX` / `I2M.MAX x`  
Get current maximum note number / Set maximum note number for MIDI notes to `x` (0..127). Default is `x = 127`.   

- `I2M.REP` / `I2M.REP x`  
Get current repetition / Set repetition of MIDI notes to `x` repetitions (1..127). Set this to `1` for no repetitions. Default is `x = 1`.

- `I2M.RAT` / `I2M.RAT x`  
Get current ratcheting / Set ratcheting of MIDI notes to `x` ratchets (1..127). Set this to `1` for no ratcheting. Default is `x = 1`. 

---

### MIDI out: Notes
*Send MIDI notes*  


- `I2M.NOTE x y`    
`I2M.N`  
Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127). A velocity of `0` will be treated as a MIDI Note off message. 

- `I2M.NOTE.O x`  
`I2M.NO`  
Send a manual MIDI Note Off message for note number `x` (0..127). This can be used either before i2c2midi sends the automatic Note Off message (to stop the note from playing before its originally planned ending), or in combination with `I2M.TIME` set to `0` (in which case i2c2midi does not send automatic Note Off messages). 

- `I2M.NC x y z`  
Send MIDI Note On message to channel `x` (1..16 for TRS, 17..32 for USB) for note number `y` (0..127) with velocity `z` (1..127) 

- `I2M.NOC x y`  
Send MIDI Note Off message to channel `x` (1..16 for TRS, 17..32 for USB) for note number `y` (0..127) 

---

### MIDI out: Chords
*Send a chord, consisting of multiple MIDI notes*


- `I2M.CHORD x y z`  
`I2M.C`  
Send multiple MIDI Note On messages for chord `x` (1..8) with root note `x` (0..127) and velocity `z` (1..127). A chord consists of up to eight notes defined relative to the root note via `I2M.C.ADD`, `I2M.C.RM`, `I2M.C.INS`, `I2M.C.DEL` or `I2M.C.SET`. If no note has been added to the respective chord yet, no note will be played. 8 chords can be defined using its respective index 1..8. 

- `I2M.C.ADD x y`  
`I2M.C+`  
Add note `y` (-127..127) to chord `x` (0..8), with `y` relative to the root note specified when playing a chord. E.g. add `0`, `4` and `7` to create a major triad. Or go more experimental, e.g. `-2`, `13`, `2`, `13`. Up to eight chords can be defined, with eight notes each. Use `x = 0` to add the note to all chords. 

- `I2M.C.RM x y`  
`I2M.C-`    
Remove note `y` (-127..127) from chord `x` (0..8). If the chord contains note `y` multiple times, the latest instance is removed. Use `x = 0` to remove the note from all chords.

- `I2M.C.INS x y z`  
Add note `z` (-127..127) to chord `x` (0..8) at index `y` (0..7), with `z` relative to the root note. Already defined notes at index `y` and higher are pushed to the right. Use `x = 0` to insert the note to all chords. 

- `I2M.C.DEL x y`  
Delete note at index `y` (0..7) from chord `x` (0..8). Notes at index `y + 1` and higher are pushed to the left. Use `x = 0` to delete the note from all chords.  

- `I2M.C.SET x y z`  
Set note at index `y` (0..7) in chord `x` (0..8) to note `z` (-127..127), replacing what was defined earlier at this index. Use `x = 0` to set the note in all chords. 

- `I2M.C.CLR x`  
Clear chord `x` (0..8). use `x = 0` to clear all chords.

- `I2M.C.L x` / `I2M.C.L x y`  
Get current length / Set length of chord `x` (0..8) to `y` (1..8). Values of `x` higher than number of actual defined notes are ignored. Already defined notes are not affected by setting the chord length, but won't be played if their index is outside of the set chord length. Use `x = 0` to set the length of all chords.

- `I2M.C.REV x`  
Get current reverse / Set reverse of notes in chord `x` (0..8) to `y`. `y = 0` or an even number means not reversed, `y = 1` or an uneven number  means reversed. E.g. reverse set to `1` for chord `0,3,7` will lead to `7,3,0`. Default is `y = 0`. Use `x = 0` to reverse all chords.

- `I2M.C.ROT x` / `I2M.C.ROT x y`  
Get current rotation / Set rotation of notes in chord `x` (0..8) to `y` steps (-127..127). E.g. rotation set to `1` of chord `0,3,7` will lead to `3,7,0`. Default is `y = 0`. Use `x = 0` to set rotation for all chords.

- `I2M.C.INV x` / `I2M.C.INV x y`  
Get current inversion / Set inversion of chord `x` (0..8) to `y` (-32..32). Default is `y = 0`. Use `x = 0` to set inversion for all chords. 

- `I2M.C.STR x` / `I2M.C.STR x y`  
Get current strumming / Set strumming of chord `x` (0..8) to `x` ms (0..32767). Strumming plays the notes of a chord arpeggiated, with `x` ms in between notes. Default is `y = 0`. Use `x = 0` to set strumming for all chords.

---

### MIDI out: CCs
*Send MIDI CCs*


- `I2M.CC x y`  
Send MIDI CC message for controller `x` (0..127) with value `y` (0..127).  

- `I2M.CCC x y z`  
Send MIDI CC message to channel `x` (1..16 for TRS, 17..32 for USB) for controller `y` (0..127) with value `z` (0..127).   

- `I2M.CCV x y`  
Send MIDI CC message for controller `x` (0..127) with volt value `y` (0..16383, 0..+10V).   

- `I2M.CCVC x y z`  
Send MIDI CC message to channel `x` (1..16 for TRS, 17..32 for USB) for controller `x` (0..127) with volt value `z` (0..16383, 0..+10V).   

- `I2M.CC.OFF x` / `I2M.CC.OFF x y`  
Get current offset / Set offset of values of controller `x` (0..127) to `y` (-127..127). Default is `x = 0`.  

- `I2M.CC.SLEW x` / `I2M.CC.SLEW x y`  
Get current slew time for controller `x` / Set slew time for controller `x` (0..127) to `y` ms (0..32767). i2c2midi will ramp from the controller's last value to the new value within the given time x, sending MIDI CCs at a maximum rate of 30 ms; 8 CC controller values can be slewed simoultaneously before the "oldest" currently slewing value is overwritten by the newest. Default is `x = 0`.      

- `I2M.CC.SET x y`  
Send MIDI CC message for controller `x` (0..127) with value `y` (0..127), bypassing any slew settings.  

---

### MIDI out: NRPN
*Send MIDI NRPN messages*


- `I2M.NRPN x y z`  
Send MIDI NRPN message (high-res CC) for parameter MSB `x` and LSB `y` with value `y` (0..16383).  

- `I2M.NRPN.OFF x y` / `I2M.NRPN.OFF x y z`  
Get current offset / Set offset of values of NRPN messages to `z` (-16384..16383). Default is z = 0.  

- `I2M.NRPN.SLEW x y` / `I2M.NRPN.SLEW x y z`  
Get current slew time / Set slew time for NRPN messages to `z` ms (0..32767). Default is z = 0.   

- `I2M.NRPN.SET x y z`  
Send MIDI NRPN message (high-res CC) for parameter MSB `x` and LSB `y` with value `y` (0..16383), bypassing any slew settings.  

---

### MIDI out: Misc
*Send other MIDI messages like Program Change, Pitch Bend and Clock*


- `I2M.PRG x`  
Send MIDI Program Change message for program `x` (0..127).  

- `I2M.PB x`  
Send MIDI Pitch Bend message with value `x` (-8192..8191).  

- `I2M.AT x`  
Send MIDI After Touch message with value `x` (0..127).  

- `I2M.CLK`  
Send MIDI Clock message, interpreted as quarter note.  

- `I2M.START`  
Send MIDI Clock Start message.  

- `I2M.STOP`  
Send MIDI Clock Stop message.  

- `I2M.CONT`  
Send MIDI Clock Continue message.  

- `I2M.PANIC`  
Send MIDI Note Off messages for all notes on all channels, and reset note duration, shift, repetition, ratcheting, min/max.  

---

### MIDI in: Settings
*Settings for incoming MIDI messages*


- `I2M.Q.CH` / `I2M.Q.CH x`  
Get currently set MIDI channel / Set MIDI channel `x` (1..16) for MIDI in. Default is `x = 1`.  

- `I2M.Q.LATCH x`  
Turn on or off "latching" for MIDI notes received via MIDI in. `x = 0` means Note Off messages are recorded in the note history, so only notes with keys currently held down on the MIDI controller are stored. `x = 1` means Note Off messages are not recorded in the note history, so notes are still stored after releasing the respective key on the MIDI controller. Default is `x = 1`.   

---

### MIDI in: Notes
*Access the note history – the eight last received note numbers and velocities*


- `I2M.Q.N x`  
Get `x` (0..7) last note number (0..127) received via MIDI in.  

- `I2M.Q.V x`  
Get `x` (0..7) last note velocity (0..127) received via MIDI in.

---

### MIDI in: CCs
*Access the stored data of CC values received via MIDI in*


- `I2M.Q.CC x`  
Get current value (0..127) of controller `x` (0..127) received via MIDI in.  

---

### MIDI in: Get latest
*Get the respective latest value reveived via MIDI in*


- `I2M.Q.LCH`  
Get the latest channel (1..16) received via MIDI in.  

- `I2M.Q.LN`  
Get the note number (0..127) of the latest Note On received via MIDI in.  

- `I2M.Q.LV`  
Get the latest velocity (1..127) received via MIDI in.  

- `I2M.Q.LO`  
Get the note number (0..127) of the latest Note Off received via MIDI in.  

- `I2M.Q.LC`  
Get the latest controller number (0..127) received via MIDI in.  

- `I2M.Q.LCC`  
Get the latest controller value (0..127) received via MIDI in.  

---


## Schematic

![](https://github.com/attowatt/i2c2midi/blob/main/MKII/hardware/i2c2midi_schematic.png)

**MIDI TRS Type A Wiring**

Teensy | TRS | MIDI
--- | --- | ---
Teensy Pin 1 | → Tip | → MIDI Pin 5
Teensy 3V3 | → Ring | → MIDI Pin 4
Ground | → Sleeve | → MIDI Pin 2


## BOM

Number | Part | Value
--- | --- | ---
1 | T36 | Teensy 3.6 (with pins or without pins)
2 | C1 | 10 uF capacitor
3 | C2 | 10 uF capacitor
4 | D1 | LED 3 mm
5 | D2 | LED 3 mm
6 | IC1 | LM1117 5V regulator
7 | R1 | 47 Ω resistor
8 | R2 | 47 Ω resistor
9 | R3 | 2.2k Ω resistor
10 | R4 | 2.2k Ω resistor
11 | R5 | 220 Ω resistor
12 | R6 | 220 Ω resistor
13 | TRS | Green Thonkiconn Stereo 3.5mm Audio Jack (PJ366ST)
14 | USB | USB Jack Type A vertical
15 | POW | 2x5 male pin header angled
16 | I2C | 2x3 male pin header angled
17 |  | 1x10 male pin header angled
18 |  | 10 mm M3 Hex standoff
19 |  | 10 mm M3 screw
20 |  | 10 mm M3 screw
21 | | Nut for Thonkiconn
22 | | 1x5 male pin header (for USB host)
23 | | 1x24 male pin header (if Teensy has no pins)
24 | | 1x24 male pin header (if Teensy has no pins)


### PCB and Panel
- [Here are the gerber files](MKII/hardware/gerber/)

- Here are some [more details](https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/40950/56) about ordering the PCB and Pancel from a manufacturer.


## Build Guide

- [Here's the build guide](MKII/hardware//BUILD-GUIDE.md)


## Firmware

### MKI vs. MKII

The new firmware (v4.0+) does work for both MKI and MKII modules. 

However, the MKI hardware does not support MIDI in. If you use firmware v4.0 or higher for MKI, please comment out `#define MK2` in the main `i2c2midi_firmware_v4_0.ino` file (line 35).

### Libraries used

- Arduino MIDI Library  
  MIT License – Copyright (c) 2016 Francois Best  
  https://github.com/FortySevenEffects/arduino_midi_library
- I2C_t3  
  MIT License – Copyright (c) 2013-2018, Brian (nox771 at gmail.com)  
  https://github.com/nox771/i2c_t3
- USBHost_t36  
  MIT License – Copyright 2017 Paul Stoffregen (paul@pjrc.com)  
  https://github.com/PaulStoffregen/USBHost_t36
- Ramp  
  GPL-3.0 License – Sylvain GARNAVAULT - 2016/08/07  
  https://github.com/siteswapjuggler/RAMP



### Changelog

- Version 4.1
  - Experimental support for Teensy 4.1
  - Fixing some things related to compile warning messages
- Version 4.0
  - Completely rewritten firmware with support for i2c2midi's dedicated Teletype OPs (`I2M`).
  - Updated README.
- Version 3.1
  - Added feature flag to use i2c2midi as USB device, using the Teensy Micro USB jack.
  - Fixed a bug where unknown I2C request messages would freeze the I2C bus.
- Version 3.0 (MKII)
  - Firmware for i2c2midi hardware MKII, adding USB Host functionality.



## Thanks
A huge *Thank You* to the legendary [scanner-darkly](https://github.com/scanner-darkly) who not only implemented i2c2midi's Teletype OPs, but also joined forces with me in designing the feature set and OP concept for MKII.

Also, many thanks to everyone who contributed and helped making this module a reality (in no particular order): [Ansome](https://www.instagram.com/ansomeuk/), [Tom Whitwell](https://twitter.com/tomwhitwell), [Konstantine](https://www.instagram.com/konstantine.f/), [mxmxmx](https://github.com/mxmxmx), [littlescale](https://www.instagram.com/littlescale/), [non.verbal.poetry](https://www.instagram.com/non.verbal.poetry/)


## Sources

- https://www.pjrc.com/teensy/external_power.html
- https://www.pjrc.com/teensy/td_libs_MIDI.html
- https://github.com/PaulStoffregen/USBHost_t36
- https://github.com/nox771/i2c_t3
- https://github.com/TomWhitwell/MTM-Parts-Library
- https://github.com/monome/teletype/blob/main/docs/ops/i2c.toml
