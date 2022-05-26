# i2c2midi

### i2c2midi is a DIY open source 2 HP Teletype Expander that speaks I2C and MIDI.

i2c2midi bridges the gap between [monome Teletype](https://monome.org/docs/teletype/) and external MIDI-enabled devices, using I2C:  
→ It receives I2C messages from Teletype and converts them to MIDI notes, MIDI CC messages and other MIDI messages to control external devices like synths and effects.  
← It receives MIDI messages from external MIDI controllers and stores the values internally, which can be requested at any time by Teletype via I2C.

- Dedicated Teletype OPs: `I2M.x`
- MIDI *out* via TRS
- MIDI *out* and *in* via USB Host
- Automatic MIDI Note Off messages (settable note duration per note)
- Chords and Chord Transformations
- 8 voice polyphony per MIDI channel, 16 channels simultaneously
- 2 HP, 42 mm depth


i2c2midi lines community thread:   
https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/

<br/>

![](https://github.com/attowatt/i2c2midi/blob/main/MKII/hardware/pictures/i2c2midi_MK2_shot.jpg)


<br/><br/>


## Table of contents
[Connections](#connections)  
[Teletype OPs](#teletype-ops)   
[Build the module](#build-the-module)  
[Firmware](#firmware)  
[Thanks](#thanks)  
[Sources](#sources)  


<br/><br/>


## Connections

### TRS

Use a TRS MIDI adapter (Type A) to connect a standard MIDI DIN cable to the TRS output of i2c2midi.

i2c2midi can be used with up to 16 different external MIDI-enabled devices simultaneously, with each device receiving MIDI messages on one of the 16 available MIDI channels. Just make sure each device is set to a different MIDI channel. Use a MIDI through splitter to connect all devices. Set the MIDI channel to 1..16 for the TRS output, using `I2M.CH`.

Please note: The TRS connection is MIDI *out* only!

### USB 

The USB connection can be used for either [MIDI *out*](#usb-midi-out-host) to connect additional devices (e.g. Teenage Engineering OP-1), or [MIDI *in*](#usb-midi-in-host) to receive MIDI Note or CC messages from an external MIDI controller.

Several devices can be connected using a USB hub.

i2c2midi acts as a USB host, which makes it possible to connect external USB devices without the need of a computer. However, this also means that a computer (which acts as a host itself) can not be connected via the USB A jack on the front panel. If you want to connect a USB host (e.g. a computer), you can use the Micro USB jack of the Teensy (only accessible on the back of the module) and enable the `USB_DEVICE` feature flag in the firmware (look for `//#define USB_DEVICE` and remove the `//`).  

#### USB MIDI out (Host)

The USB MIDI out connection works like the TRS output, but the 16 available MIDI channels can be addressed separately from TRS. Set the MIDI channel with `I2M.CH` to 17..32 for MIDI channels 1..16 on the USB connection.

#### USB MIDI in (Host)

If an external MIDI controller is connected via USB, incoming MIDI messages are stored internally in i2c2midi, and can be requested by Teletype at any time:

 - MIDI Note messages: The last eight notes and their velocities are stored in a rotating "note history". Each new received note gets added to the history and pushes the oldest note out. The notes and velocities can be requested with `I2M.Q.N` and `I2M.Q.V`.
 - MIDI CC messages: All values (127 controllers * 16 channels = 2032 values total) are stored and can be requested with `I2M.Q.CC`.

**Please note: Use external power or a powered USB hub!**

If you connect a MIDI device without its own power supply, use a powered USB hub with power supply in between i2c2midi and the MIDI device. This prevents too much current from being drawn from the Teensy and the voltage regulator on the PCB. I would advise to always use a powered USB hub in between – to be on the safe side. 

In case of OP-1, please turn off USB charging by pressing `shift` + `COM`, then `OPT` (T4), then turn the blue encoder to toggle USB charging off.

#### USB MIDI out (Device to Host)

i2c2midi can act as a USB device, too, if you want to connect a device that acts as a USB host (e.g. a computer). For this, you can use the Micro USB jack of the Teensy, which is accessible on the back of the module. Enable the `USB_DEVICE` feature flag in the firmware (look for `//#define USB_DEVICE` and remove the `//`). 
Most importantly: Cut the 5V pads on the Teensy! Otherwise you will damage your Teensy once you connect power from the modular and the host device on the USB port. For more info on why and how to cut the 5V pads, please follow [this link](https://www.pjrc.com/teensy/external_power.html).

**Please note: Don't connect power and USB at the same time!**

Caution: Do not connect power from the modular and the default USB port of the Teensy at the same time, unless you have cut the 5V pads on the Teensy!! Otherwise you will damage your Teensy. 

### LEDs

- The top LED lights up when MIDI messages are coming in.
- The bottom LED lights up when MIDI messages are going out.

<br/>

![](https://github.com/attowatt/i2c2midi/blob/main/MKII/hardware/pictures/i2c2midi_MK2_diagram.png)

<br/><br/>

---

## Teletype OPs

### Overview

| Type | OP                                          | Alias           | Channel-specific variant [(?)](#channel-specific-op-variants)                     | Function                            |
| ---- | ------------------------------------------- | --------------- | -------------------------------------------- | ----------------------------------- |
| get  | I2M.CH                                      | I2M.#           |                                              | Get current Channel                 |
|      | I2M.CH channel                              | I2M.#           |                                              | Set current Channel                 |
| get  | I2M.TIME                                    | I2M.T           | I2M.T# channel                               | Get note duration                   |
|      | I2M.TIME duration                           | I2M.T           | I2M.T# channel duration                      | Set note duration                   |
| get  | I2M.SHIFT                                   | I2M.S           | I2M.S# channel                               | Get note shift                      |
|      | I2M.SHIFT semitones                         | I2M.S           | I2M.S# channel semitones                     | Set note shift                      |
|      | I2M.MIN note mode                           |                 | I2M.MIN# channel note mode                   | Set minimum note                    |
|      | I2M.MAX note mode                           |                 | I2M.MAX# channel note mode                   | Set maximum note                    |
| get  | I2M.REP                                     |                 | I2M.REP# channel                             | Get note repetition                 |
|      | I2M.REP repetitions                         |                 | I2M.REP# channel repetitions                 | Set note repetition                 |
| get  | I2M.RAT                                     |                 | I2M.RAT# channel                             | Get note ratcheting                 |
|      | I2M.RAT ratchets                            |                 | I2M.RAT# channel ratchets                    | Set note ratcheting                 |
|      | I2M.NOTE note velocity                      | I2M.N           | I2M.N# channel note velocity                 | Send Note On                        |
|      | I2M.NOTE.O note                             | I2M.NO          | I2M.NO# channel note                         | Send Note Off                       |
|      | I2M.NT note velocity duration               |                 | I2M.NT# channel note velocity dur            | Send Note On with duration          |
|      | I2M.CHORD chord rootnote velocity           | I2M.C           | I2M.C# channel chord rootnote vel            | Play chord                          |
|      | I2M.C.ADD chord note                        | I2M.C+          |                                              | Add note to chord                   |
|      | I2M.C.RM chord note                         | I2M.C-          |                                              | Remove note to chord                |
|      | I2M.C.INS chord index note                  |                 |                                              | Insert note at index                |
|      | I2M.C.DEL chord index                       |                 |                                              | Delete note at index                |
|      | I2M.C.SET chord index note                  |                 |                                              | Set note at index                   |
|      | I2M.C.B chord revbinary                     |                 |                                              | Define chord using reverse binary   |
|      | I2M.C.CLR chord                             |                 |                                              | Clear chord                         |
| get  | I2M.C.L chord                               |                 |                                              | Get chord length                   |
|      | I2M.C.L chord length                        |                 |                                              | Set chord length                    |
|      | I2M.C.SC chord1 chord2                      |                 |                                              | Set chord2 as scale for chord1  |
|      | I2M.C.REV chord iterations                  |                 |                                              | Set chord reversal                   |
|      | I2M.C.ROT chord iterations                  |                 |                                              | Set chord rotation                  |
|      | I2M.C.TRP chord semitones                   |                 |                                              | Set chord transposition             |
|      | I2M.C.DIS chord iter. anchorpoint           |                 |                                              | Set chord distortion                |
|      | I2M.C.REF chord iter. anchorpoint           |                 |                                              | Set chord reflection                |
|      | I2M.C.INV chord iter.                       |                 |                                              | Set chord inversion                 |
|      | I2M.C.STR chord ms                          |                 |                                              | Set chord strumming                 |
|      | I2M.C.VCUR chord curve start end            | I2M.V~          |                                              | Set chord velocity curve            |
|      | I2M.C.TCUR chord curve start end            | I2M.T~          |                                              | Set chord time curve                |
|      | I2M.CC controller value                     |                 | I2M.CC# channel controller value             | Send CC                             |
|      | I2M.CCV controller value                    |                 | I2M.CCV# channel controller value            | Send CC with volt value             |
| get  | I2M.CC.OFF controller                       |                 | I2M.CC.OFF# channel controller               | Get CC offset                       |
|      | I2M.CC.OFF controller offset                |                 | I2M.CC.OFF# channel controller offset        | Set CC offset                       |
| get  | I2M.CC.SLEW controller                      |                 | I2M.CC.SLEW# channel controller              | Get CC slew                         |
|      | I2M.CC.SLEW controller ms                   |                 | I2M.CC.SLEW# channel controller ms           | Set CC slew                         |
|      | I2M.CC.SET controller value                 |                 | I2M.CC.SET# channel controller value         | Send CC and ignore slew               |
|      | I2M.NRPN msb lsb value                      |                 | I2M.NRPN# channel msb lsb value              | Send NRPN                           |
| get  | I2M.NRPN.OFF msb lsb                        |                 | I2M.NRPN.OFF# channel msb lsb                | Get NRPN offset                     |
|      | I2M.NRPN.OFF msb lsb offset                 |                 | I2M.NRPN.OFF# channel msb lsb offset         | Set NRPN offset                     |
| get  | I2M.NRPN.SLEW msb lsb                       |                 | I2M.NRPN.SLEW# channel msb lsb               | Get NRPN slew                       |
|      | I2M.NRPN.SLEW msb lsb ms                    |                 | I2M.NRPN.SLEW# channel msb lsb ms            | Set NRPN slew                       |
|      | I2M.NRPN.SET msb lsb value                  |                 | I2M.NRPN.SET# value msb lsb value            | Send NRPN and ignore slew             |
|      | I2M.PRG program                             |                 |                                              | Send Program Change                 |
|      | I2M.PB value                                |                 |                                              | Send Pitchbend                      |
|      | I2M.AT value                                |                 |                                              | Send Aftertouch                     |
|      | I2M.CLK                                     |                 |                                              | Send Clock                          |
|      | I2M.START                                   |                 |                                              | Send Clock Start                    |
|      | I2M.STOP                                    |                 |                                              | Send Clock Stop                     |
|      | I2M.CONT                                    |                 |                                              | Send Clock Continue                 |
| get  | I2M.Q.CH                                    |                 |                                              | Get current MIDI in Channel         |
|      | I2M.Q.CH channel                            | I2M.Q.#         |                                              | Set current MIDI in Channel          |
|      | I2M.Q.LATCH on/off                          |                 |                                              | Turn latch on/off                   |
| get  | I2M.Q.NOTE index                            | I2M.Q.N         |                                              | Get x-last Note On                  |
| get  | I2M.Q.VEL index                             | I2M.Q.V         |                                              | Get x-last Velocity                 |
| get  | I2M.Q.CC controller                         |                 |                                              | Get CC value                        |
| get  | I2M.Q.LCH                                   |                 |                                              | Get last MIDI in Channel            |
| get  | I2M.Q.LN                                    |                 |                                              | Get last MIDI in Note On            |
| get  | I2M.Q.LV                                    |                 |                                              | Get last MIDI in Velocity           |
| get  | I2M.Q.LO                                    |                 |                                              | Get last MIDI in Note Off           |
| get  | I2M.Q.LC                                    |                 |                                              | Get last MIDI in Controller         |
| get  | I2M.Q.LCC                                   |                 |                                              | Get last MIDI in CC Value           |
|      | I2M.PANIC                                   |                 |                                              | Don't panic!                                 |

---


- OP  
OP (set)  
Alias  
Description  


---

### MIDI out: Settings
*Settings for outoing MIDI messages*  

- `I2M.CH`  
`I2M.CH x`  
`I2M.#`  
Get currently set MIDI channel / Set MIDI channel `x` (1..16 for TRS, 17..32 for USB) for MIDI out. Use MIDI channels 1-16 for TRS output, 17-32 for USB output. Default is `x = 1`.  

- `I2M.TIME`  
`I2M.TIME x`  
`I2M.T`  
Get current note duration / Set note duration for MIDI notes to `x` ms (0..32767). Based on note duration, i2c2midi will send a MIDI Note Off message automatically. Set `x = 0` to deactivate automatic Note Off messages. Default is `x = 100`. Use `I2M.T#` to add channel parameter [(see below)](#channel-specific-op-variants).

- `I2M.SHIFT`  
`I2M.SHIFT x`  
`I2M.S`  
Get current transposition / Set transposition of MIDI notes to `x` semitones (-127..127). Default is `x = 0`. Use `I2M.S#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.MIN x y`  
Set minimum note number for MIDI notes to `x` (0..127), using mode `y` (0..3). Default is `x = 0` and `y = 0`. The following modes are available for notes lower than the minimum: 0) Ignore notes 1) Clamp notes 2) Fold back notes by one octave 3) Fold back notes by multiple octaves. Use `I2M.MIN#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.MAX x y`  
Set maximum note number for MIDI notes to `x` (0..127), using mode `y` (0..3). Default is `x = 0` and `y = 0`. The following modes are available for notes higher than the maximum: 0) Ignore notes 1) Clamp notes 2) Fold back notes by one octave 3) Fold back notes by multiple octaves. Use `I2M.MAX#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.REP`  
`I2M.REP x`  
Get current repetition / Set repetition of MIDI notes to `x` repetitions (1..127). Set `x = 1` for no repetitions. Default is `x = 1`. Use `I2M.REP#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.RAT`  
`I2M.RAT x`  
Get current ratcheting / Set ratcheting of MIDI notes to `x` ratchets (1..127). Set `x = 1` for no ratcheting. Default is `x = 1`. Use `I2M.RAT#` to add channel parameter [(see below)](#channel-specific-op-variants).  


---

### MIDI out: Notes
*Send MIDI notes*  


- `I2M.NOTE x y`  
`I2M.N`  
Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127). A velocity of `0` will be treated as a MIDI Note Off message. Use `I2M.N#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.NOTE.O x`  
`I2M.NO`  
Send a manual MIDI Note Off message for note number `x` (0..127). This can be used either before i2c2midi sends the automatic Note Off message (to stop the note from playing before its originally planned ending), or in combination with `I2M.TIME` set to `0` (in which case i2c2midi does not send automatic Note Off messages). Use `I2M.NO#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.NT x y z`  
Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127) and note duration `z` ms (0..32767). Use `I2M.NT#` to add channel parameter [(see below)](#channel-specific-op-variants).  


---

### MIDI out: Chords
*Send a chord, consisting of multiple MIDI notes*


- `I2M.CHORD x y z`  
`I2M.C`  
Play chord `x` (1..8) with root note `y` (0..127) and velocity `z` (1..127). A chord consists of up to eight notes defined relative to the root note via `I2M.C.ADD`, `I2M.C.RM`, `I2M.C.INS`, `I2M.C.DEL` or `I2M.C.SET`, which are sent out as MIDI Note On messages in the order they are defined in the chord. If no note has been defined in the chord yet, no note will be played. 8 chords can be defined using their respective index 1..8. Use `I2M.C#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.C.ADD x y`  
`I2M.C+`  
Add note `y` (-127..127) to chord `x` (0..8), with `y` relative to the root note specified when playing a chord. E.g. add `0`, `4` and `7` to define a major triad. Or go more experimental, e.g. `-2`, `13`, `2`, `13`. Up to eight chords can be defined, with eight notes each. Use `x = 0` to add the note to all chords.  

- `I2M.C.RM x y`  
`I2M.C-`  
Remove note `y` (-127..127) from chord `x` (0..8). If the chord contains note `y` multiple times, the latest instance is removed. If the chord does not contain the note the message is simply ignored. Use `x = 0` to remove the note from all chords.  

- `I2M.C.INS x y z`  
Add note `z` (-127..127) to chord `x` (0..8) at index `y` (0..7), with `z` relative to the root note. Already defined notes at index `y` and higher are pushed to the right. Use `x = 0` to insert the note to all chords.  

- `I2M.C.DEL x y`  
Delete note at index `y` (0..7) from chord `x` (0..8). Notes at index `y + 1` and higher are pushed to the left. If `y` is higher than the length of the chord, the message is ignored. Use `x = 0` to delete the note from all chords.  

- `I2M.C.SET x y z`  
Set note at index `y` (0..7) in chord `x` (0..8) to note `z` (-127..127), replacing what was defined earlier at this index. If `y` is higher than the length of the chord, the message is ignored. Use `x = 0` to set the note in all chords.  

- `I2M.C.B x y`  
Clear and define chord `x` (0..8) using reverse binary notation (`R...`). Use `1` or `0` in order to include or exclude notes from the chord. E.g. use `x = R10001001` for `0,4,7` (major triad) or `x = R1000000100000001` for `0,7,15`. `y` can be a maximum of 16 digit long. Use `x = 0` to clear and define all chords.  

- `I2M.C.CLR x`  
Clear chord `x` (0..8). Use `x = 0` to clear all chords.  

- `I2M.C.L x`  
`I2M.C.L x y`  
Get current length / Set length of chord `x` (0..8) to `y` (1..8). The length of a chord changes automatically each time a note is added or removed. Values of `x` higher than number of actual defined notes are ignored. Already defined notes are not affected by setting the chord length, but won't be played if their index is outside of the set chord length. Use `x = 0` to set the length of all chords.  

- `I2M.C.SC x y`  
Set scale for chord `x` (0..8) based on chord `y` (0..8). Setting a scale for a chord comes in handy when using chord transformations that introduce new notes, like `I2M.C.TRP`, `I2M.C.DIS` or `I2M.C.REF`. Use `y = 0` to remove the scale. Use `x = 0` to set scale for all chords.    

- `I2M.C.REV x y`  
Set reversal of notes in chord `x` (0..8) to `y`. `y = 0` or an even number means not reversed, `y = 1` or an uneven number  means reversed. E.g. `y = 1` for chord `0,3,7` will lead to `7,3,0`. Default is `y = 0`. Use `x = 0` to set reversal all chords. 

  https://user-images.githubusercontent.com/76960718/170448152-c708c42d-ed18-4411-b0e5-6085a569185b.mp4

- `I2M.C.ROT x y`  
Set rotation of notes in chord `x` (0..8) to `y` steps (-127..127). E.g. `y = 1` of chord `0,3,7` will lead to `3,7,0`, `y = 2` will lead to `7,0,3`, `y = -1` will lead to `7,0,3`. Default is `y = 0`. Use `x = 0` to set rotation for all chords.  

  https://user-images.githubusercontent.com/76960718/170448234-86cafa1e-8ecd-4612-9a84-febde5bead3d.mp4

- `I2M.C.TRP x y`  
Set transposition of chord `x` (0..8) to `y` (-127..127). Transposition adds `y` to the note number of each note in the chord. Default is `y = 0`. Use `x = 0` to set transposition for all chords. This transformation introduces new notes to the chord – try it in combination with setting a scale.  

  https://user-images.githubusercontent.com/76960718/170448291-4766446c-830e-4b92-a609-74fe5c757047.mp4

- `I2M.C.DIS x y z`  
Set distortion of chord `x` (0..8) to width `y` (-127..127) with anchor point `z` (0..16). Distortion adds `y+n` to the note number of each note in the chord. The anchor point influences the direction and amount (`n`) of the transformation. Default is `y = 0`. Use `x = 0` to set distortion for all chords. This transformation introduces new notes to the chord – try it in combination with setting a scale.  

  https://user-images.githubusercontent.com/76960718/170519200-57bbb0fd-a168-498a-873c-f39924235f2d.mp4

- `I2M.C.REF x y z`  
Set reflection of chord `x` (0..8) to `y` (-127..127) with anchor point `z` (0..16). The anchor point defines at which axis the chord gets reflected. Default is `y = 0`. Use `x = 0` to set distortion for all chords. This transformation introduces new notes to the chord – try it in combination with setting a scale.  

  https://user-images.githubusercontent.com/76960718/170448450-b01a0faa-ed1b-4541-b9ee-6c55721735cb.mp4

- `I2M.C.INV x y`  
Set inversion of chord `x` (0..8) to `y` (-32..32). Default is `y = 0`. Use `x = 0` to set inversion for all chords.  

  https://user-images.githubusercontent.com/76960718/170448519-62731788-977d-4e5f-a442-cc999a97c141.mp4

- `I2M.C.STR x y`  
Set strumming of chord `x` (0..8) to `x` ms (0..32767). Strumming plays the notes of a chord arpeggiated, with an interval of `y` ms in between notes. Default is `y = 0`. Use `x = 0` to set strumming for all chords.  

- `I2M.C.VCUR w x y z`  
`I2M.C.V~`  
Set velocity curve for chord `w` (0..8) with curve type `x` (0..5), start value `y`% (0..32767) and end value `z`% (0..32767). This will affect the velocity of the notes in the order they are defined in the chord. Start and end percentages refer to the velocity with which the chord is played via `I2M.C`. Use `x = 0` to turn velocity curve off. The following curves are available: 0) Off 1) Linear 2) Exponential 3) Triangle 4) Square 5) Random. Use `w = 0` to set velocity curve for all chords. Try a random curve with subtle values for a humanizing effect.  

- `I2M.C.TCUR w x y z`  
`I2M.C.T~`  
Set time curve for chord `w` (0..8) with curve type `x` (0..5), start value `y`% (0..32767) and end value `z`% (0..32767). This will affect the time interval between the notes in the order they are defined in the chord. Start and end percentages refer to the current strumming setting of the chord, set via `I2M.C.STR`. Use `x = 0` to turn time curve off. The following curves are available: 0) Off 1) Linear 2) Exponential 3) Triangle 4) Square 5) Random. Use `w = 0` to set time curve for all chords. Try a square curve with similar values to create swing. Try a random curve with subtle values for a humanizing effect.  


---

### MIDI out: CCs
*Send MIDI CCs*


- `I2M.CC x y`  
Send MIDI CC message for controller `x` (0..127) with value `y` (0..127). Use `I2M.CC#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.CCV x y`  
Send MIDI CC message for controller `x` (0..127) with volt value `y` (0..16383, 0..+10V). Use `I2M.CCV#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.CC.OFF x`  
`I2M.CC.OFF x y`  
Get current offset / Set offset of values of controller `x` (0..127) to `y` (-127..127). Default is `y = 0`. Use `I2M.CC.OFF#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.CC.SLEW x`  
`I2M.CC.SLEW x y`  
Get current slew time for controller `x` / Set slew time for controller `x` (0..127) to `y` ms (0..32767). i2c2midi will ramp from the controller's last value to a new value within the given time `x`, sending MIDI CCs at a maximum rate of 30 ms. If the slewing is still ongoing when a new value is set, the slewing uses its current position as the last value. Is 8 CC controller values can be slewed simoultaneously before the oldest currently slewing value is overwritten by the newest. Default is `y = 0`. Use `I2M.CC.SLEW#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.CC.SET x y`  
Send MIDI CC message for controller `x` (0..127) with value `y` (0..127), bypassing any slew settings. Use `I2M.CC.SET#` to add channel parameter [(see below)](#channel-specific-op-variants).  


---

### MIDI out: NRPN
*Send MIDI NRPN messages*


- `I2M.NRPN x y z`  
Send MIDI NRPN message (high-res CC) for parameter MSB `x` and LSB `y` with value `y` (0..16383). Use `I2M.NRPN#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.NRPN.OFF x y`  
`I2M.NRPN.OFF x y z`  
Get current offset / Set offset of values of NRPN messages to `z` (-16384..16383). Default is z = 0. Use `I2M.NRPN.OFF#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.NRPN.SLEW x y`  
`I2M.NRPN.SLEW x y z`  
Get current slew time / Set slew time for NRPN messages to `z` ms (0..32767). Default is z = 0. Use `I2M.NRPN.SLEW#` to add channel parameter [(see below)](#channel-specific-op-variants).  

- `I2M.NRPN.SET x y z`  
Send MIDI NRPN message for parameter MSB `x` and LSB `y` with value `y` (0..16383), bypassing any slew settings. Use `I2M.NRPN.SET#` to add channel parameter [(see below)](#channel-specific-op-variants).  


---

### MIDI out: Misc
*Send other MIDI messages like Program Change, Pitch Bend and Clock*


- `I2M.PRG x`  
Send MIDI Program Change message for program `x` (0..127)  

- `I2M.PB x`  
Send MIDI Pitch Bend message with value `x` (-8192..8191)  

- `I2M.AT x`  
Send MIDI After Touch message with value `x` (0..127)  

- `I2M.CLK`  
Send MIDI Clock message, for now: interpreted as quarter note  

- `I2M.START`  
Send MIDI Clock Start message  

- `I2M.STOP`  
Send MIDI Clock Stop message  

- `I2M.CONT`  
Send MIDI Clock Continue message  

- `I2M.PANIC`  
Send MIDI Note Off messages for all notes on all channels, and reset note duration, shift, repetition, ratcheting, min/max  


---

### MIDI in: Settings
*Settings for incoming MIDI messages*


- `I2M.Q.CH`  
`I2M.Q.CH x`  
`I2M.Q.# x`  
Get currently set MIDI channel / Set MIDI channel `x` (1..16) for MIDI in. Default is `x = 1`.  

- `I2M.Q.LATCH x`  
Turn on or off latching for MIDI notes received via MIDI in. `x = 0` means Note Off messages are recorded in the note history, so only notes with keys currently held down on the MIDI controller are stored. `x = 1` means Note Off messages are not recorded in the note history, so notes are still stored after releasing the respective key on the MIDI controller. Default is `x = 1`.  


---

### MIDI in: Notes
*Access the note history – the eight last received note numbers and velocities*


- `I2M.Q.NOTE x`  
`I2M.Q.N`  
Get `x` (0..7) last note number (0..127) received via MIDI in  

- `I2M.Q.VEL x`  
`I2M.Q.V`  
Get `x` (0..7) last note velocity (1..127) received via MIDI in  


--- 

### MIDI in: CCs
*Access the stored data of CC values received via MIDI in*


- `I2M.Q.CC x`  
Get current value (0..127) of controller `x` (0..127) received via MIDI in  


---

### MIDI in: Get latest
*Get the respective latest value reveived via MIDI in*


- `I2M.Q.LCH`  
Get the latest channel (1..16) received via MIDI in  

- `I2M.Q.LN`  
Get the note number (0..127) of the latest Note On received via MIDI in  

- `I2M.Q.LV`  
Get the velocity (1..127) of the latest Note On received via MIDI in  

- `I2M.Q.LO`  
Get the note number (0..127) of the latest Note Off received via MIDI in  

- `I2M.Q.LC`  
Get the latest controller number (0..127) received via MIDI in  

- `I2M.Q.LCC`  
Get the latest controller value (0..127) received via MIDI in  

---

### Channel-specific OP variants
*Use the same OP but specify the channel within the OP itself. Simply add `#` to the OP name and the desired channel number as the first argument. 
This way you can easily send information to different channels, without the need to use `I2M.CH`. E.g. send three different CC messages to controller 1 on channels 1, 2 and 3:*

```
I2M.CC# 1 1 10
I2M.CC# 2 1 RND 99
I2M.CC# 3 1 RND 60 120
```

*The following channel-specific OP variants are available:*

- `I2M.T#`
- `I2M.S#`
- `I2M.MIN#`
- `I2M.MAX#`
- `I2M.REP#`
- `I2M.RAT#`
- `I2M.N#`
- `I2M.NO#`
- `I2M.NT#`
- `I2M.C#`
- `I2M.CC#`
- `I2M.CCV#`
- `I2M.CC.OFF#`
- `I2M.CC.SLEW#`
- `I2M.CC.SET#`
- `I2M.NRPN#`
- `I2M.NRPN.OFF#`
- `I2M.NRPN.SLEW#`
- `I2M.NRPN.SET#`




---


<br/><br/>


## Build the module

### Build Guide

- [Here's the build guide for i2c2midi MKII](MKII/hardware//BUILD-GUIDE.md)
- Looking for i2c2midi MKI? [Here's all info about MKI](MKI/README.md)


### BOM

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
- [Here are some more details about ordering the PCB and Panel from a manufacturer](https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/40950/56) 

### Schematic

![](https://github.com/attowatt/i2c2midi/blob/main/MKII/hardware/i2c2midi_schematic.png)

### MIDI TRS Type A Wiring

Teensy | TRS | MIDI
--- | --- | ---
Teensy Pin 1 | → Tip | → MIDI Pin 5
Teensy 3V3 | → Ring | → MIDI Pin 4
Ground | → Sleeve | → MIDI Pin 2


<br/><br/>


## Firmware

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

- Version 4.2 (+ Teletype Firmware `I2M BETA 2`)
  - New channel-specific OP variants for most of the existing OPs
  - New OP `I2M.NT`: Send a note with specific duration
  - New OP `I2M.C.B`: Define chord using reverse binary notation (`R...`)
  - New OP `I2M.C.SC`: Set scale for chord based on another chord
  - New chord transformations:
    - `I2M.C.TRP`: Transpose chord
    - `I2M.C.DIS`: Distort chord based on an anchor point
    - `I2M.C.REF`: Reflect chord based on an anchor point
    - `I2M.C.VCUR`: Apply velocity curve to chord notes
    - `I2M.C.TCUR`: Apply time curve to chord notes
  - New alias for `I2M.CH`: `I2M.#`
  - New alias for `I2M.Q.CH`: `I2M.Q.#`
  - _BREAKING_: New modes for `I2M.MIN` and `I2M.MAX`: Ignore notes, clamp notes, fold back notes by one octave, fold back notes by multiple octaves
  - _BREAKING_: Removed getter OPs for `I2M.MIN`, `I2M.MAX`, `I2M.C.ROT`, `I2M.C.INV`, `I2M.C.STR` because OPs have multiple parameters
  - _BREAKING_: Removed `I2M.NC`, `I2M.NOC`, `I2M.CCC`, `I2M.CCVC`; replaced by channel-specific OP variants
- Version 4.1
  - Experimental support for Teensy 4.1
- Version 4.0 (+ Teletype Firmware `I2M BETA 1`) ([more info](https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/40950/229))
  - Completely rewritten firmware with support for i2c2midi's dedicated Teletype OPs (`I2M`).
  - Updated README.
- Version 3.1 
  - Added feature flag to use i2c2midi as USB device, using the Teensy Micro USB jack.
  - Fixed a bug where unknown I2C request messages would freeze the I2C bus.
- Version 3.0 (MKII)
  - Firmware for i2c2midi hardware MKII, adding USB Host functionality.


<br/><br/>


## Thanks
A huge *Thank You* to the legendary [scanner-darkly](https://github.com/scanner-darkly) who not only implemented i2c2midi's Teletype OPs, but also joined forces with me in designing the feature set and OP concept for MKII.

Also, many thanks to everyone who contributed and helped making this module a reality (in no particular order): [Ansome](https://www.instagram.com/ansomeuk/), [Tom Whitwell](https://twitter.com/tomwhitwell), [Konstantine](https://www.instagram.com/konstantine.f/), [mxmxmx](https://github.com/mxmxmx), [littlescale](https://www.instagram.com/littlescale/), [non.verbal.poetry](https://www.instagram.com/non.verbal.poetry/), [
free_art_ideas](https://www.instagram.com/free_art_ideas/).


<br/><br/>


## Sources

- https://www.pjrc.com/teensy/external_power.html
- https://www.pjrc.com/teensy/td_libs_MIDI.html
- https://github.com/PaulStoffregen/USBHost_t36
- https://github.com/nox771/i2c_t3
- https://github.com/TomWhitwell/MTM-Parts-Library
- https://github.com/monome/teletype/blob/main/docs/ops/i2c.toml
- https://doepfer.de/DIY/a100_diy.htm
