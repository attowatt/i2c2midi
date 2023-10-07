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
- Looping MIDI buffer / MIDI recorder
- Works with monome crow, too, thanks to [mreid](https://github.com/mreid/crow-i2c2midi)  
- 8 voice polyphony per MIDI channel, 16 channels simultaneously
- 2 HP, 42 mm depth


i2c2midi lines community thread:   
https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/

<br/>

![](/hardware/hardware-MK2/pictures/i2c2midi_MK2_shot.jpg)


<br/><br/>


## Table of contents
[Connections](#connections)  
[Example Scripts](#example-scripts)  
[Teletype OPs](#teletype-ops)   
[Further Reading](#further-reading)   
[Build the module](#build-the-module)  
[Firmware](#firmware)  
[crow Library](#crow-library)  
[Thanks](#thanks)  
[Sources](#sources)  


<br/><br/>


## Connections

### TRS

Use a TRS MIDI adapter (Type A) to connect a standard MIDI DIN cable to the TRS output of i2c2midi.

i2c2midi can be used with up to 16 different external MIDI-enabled devices simultaneously, with each device receiving MIDI messages on one of the 16 available MIDI channels. Just make sure each device is set to a different MIDI channel. Use a MIDI through splitter to connect all devices. Set the MIDI channel to 1..16 for the TRS output, using `I2M.CH`.

Please note: The TRS connection is MIDI *out* only!

### USB 

The USB connection can be used for either [MIDI *out*](#usb-midi-out-host) to connect an additional device (e.g. Teenage Engineering OP-1), or [MIDI *in*](#usb-midi-in-host) to receive MIDI Note or CC messages from an external MIDI controller.

Several MIDI controllers can be connected using a USB hub for MIDI *in*.

i2c2midi acts as a USB host, which makes it possible to connect external USB devices without the need of a computer. However, this also means that a computer (which acts as a host itself) can not be connected via the USB A jack on the front panel. However, it is possible to connect a USB host via the Micro USB jack of the Teensy. [More info about that here](#usb-midi-out-device-to-host).

#### USB MIDI out (Host)

The USB MIDI out connection works like the TRS output, but the 16 available MIDI channels can be addressed separately from TRS. Set the MIDI channel with `I2M.CH` to 17..32 for MIDI channels 1..16 on the USB connection. Please note: Only one USB device can be connected at a time.

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

![](/hardware/hardware-MK2/pictures/i2c2midi_MK2_diagram.png)

<br/><br/>

---

## Example Scripts

#### Play a random note
```
#1 
I2M.CH 1                // set channel to 1
I2M.N + 60 RND 24 127   // play note between 60 and 84
```

#### Play a V/Oct note using `VN`
```
#1
J N.B RRND 1 15         // random note from a scale in V/Oct
K VN J                  // convert the V/Oct value to MIDI note number
I2M.N + 60 K 127        // play note 
```

#### Define and play a chord
```
#1
I2M.C.CLR 1             // clear chord 1
I2M.C.ADD 1 0           // add relative note 0
I2M.C.ADD 1 3           // add relative note 3
I2M.C.ADD 1 7           // add relative note 7

#2
I2M.C.STR 1 100         // set strumming to 100
I2M.C.DIR 1 7           // set play direction to 7: Pingpong
I2M.C 1 60 127          // play chord 1 with rootnote 60 and velocity 127 (= 60,63,67)
```

#### Define a chord using reverse binary
```
#1
I2M.C.B 1 R10010001     // define chord: 1 means add, 0 means don't add = 0,3,7
I2M.C 1 60 127          // play chord 1 with rootnote 60 and velocity 127 (= 60,63,67)
```

#### Play chords stored in pattern 0
```
#1 
I2M.C.B 1 PN.NEXT 0     // define chord with next value in pattern 0
I2M.C 1 60 127          // play chord 1 with rootnote 60 and velocity 127

#Pattern 0
137                     // = R10010001    = 0,3,7
265                     // = R100100001   = 0,3,8
1064                    // = R00010100001 = 3,5,10
1060                    // = R00100100001 = 2,5,10
```

#### Use chord 2 as scale for a chord 1
```
#1
I2M.C.B 1 R100100011    // define chord 1 = 0,3,7,8 (alternatively use "393")
I2M.C.B 2 R10110101101  // define chord 2 = 0,2,3,5,7,8,10 (minor scale) (or "1453")
I2M.C.SC 1 2            // set chord 2 as scale for chord 1

#2
J WRP + J 1 0 7         // increment J and wrap around 0 and 7
I2M.C.STR 1 200         // set strumming for chord 1 to 200 ms
I2M.C.TRP 1 J           // transpose chord 1 by J
I2M.C 1 60 127          // play chord 1 with rootnote 60 and velocity 127
```

#### Send transformed chord notes to Just Friends or ER-301 

To Just Friends:
```
#I 
JF.MODE 1
JF.SHIFT N 0

#1
I2M.C.B 1 R100100011    // define chord 1 = 0,3,7,8
I2M.C.B 2 R10110101101  // define chord 2 = 0,2,3,5,7,8,10 (minor scale)
I2M.C.SC 1 2            // set chord 2 as scale for chord 1

#2
J WRP + J 1 0 4         // increment J and wrap around 4
I2M.C.DIS 1 J 2         // distort chord 1 by J at anchor point 2
I2M.C.VCUR 1 1 40 100   // set a linear (type 1) velocity curve from 40% to 100%

#3
EV 4: $ 2
J WRP + J 1 0 3         // increment J and wrap around chord length
X I2M.C.QN 1 0 J        // get chord note and store in X
Y I2M.C.QV 1 127 J      // get chord note velocity and store in Y
Z SCL 0 127 0 800 Y     // scale velocity from 0..127 to 0..800
JF.NOTE N X VV Z        // play on Just Friends
```

To ER-301:  

```
#3
J WRP + J 1 0 3                 // increment J and wrap around chord length
SC.CV 1 N I2M.C.QN 1 0 J        // send V/Oct to ER-301
SC.CV 2 VV I2M.C.QV 1 127 J     // send velocity to ER-301 (set Gain to 7.88)
SC.TR.P 1                       // send a trigger to ER-301 envelope
```

#### Query CC 1-4 and store values in Pattern 0
```
#1 
L 0 3: PN 0 I I2M.Q.CC + I 1
```

#### Query MIDI notes from controller

Arpeggiator that plays MIDI notes currently held down on a connected MIDI controller.  
CV 1 sends out the V/OCT and CV 2 sends out the velocity. 

```
#I
I2M.Q.LATCH 0           // setting to only store currently played MIDI notes

#M
$ 1

#1
J WRP + J 1 0 7         // counter from 0..7
CV 1 N - I2M.Q.N J 48   // query MIDI note number and subtract 4 octaves
CV 2 VV * 4 I2M.Q.V J   // query MIDI note velocity and scale to VV 0..508
```

---

## Teletype OPs


### Overview

| Type | OP                                             | Alias   | Channel-specific variant [(?)](#channel-specific-op-variants) | Function                    |
| ---- | ---------------------------------------------- | ------- | ----------------------------------------------------- | ----------------------------------- |
| get  | [I2M.CH](#i2mch)                               | I2M.#   |                                                       | Get current Channel                 |
|      | [I2M.CH channel](#i2mch)                       | I2M.#   |                                                       | Set current Channel                 |
| get  | [I2M.TIME](#i2mtime)                           | I2M.T   | [I2M.T# channel](#i2mt)                               | Get note duration                   |
|      | [I2M.TIME duration](#i2mtime)                  | I2M.T   | [I2M.T# channel duration](#i2mt)                      | Set note duration                   |
| get  | [I2M.SHIFT](#i2mshift)                         | I2M.S   | [I2M.S# channel](#i2ms)                               | Get note shift                      |
|      | [I2M.SHIFT semitones](#i2mshift)               | I2M.S   | [I2M.S# channel semitones](#i2ms)                     | Set note shift                      |
|      | [I2M.MIN note mode](#i2mmin)                   |         | [I2M.MIN# channel note mode](#i2mmin-1)               | Set minimum note                    |
|      | [I2M.MAX note mode](#i2mmax)                   |         | [I2M.MAX# channel note mode](#i2mmax-1)               | Set maximum note                    |
| get  | [I2M.REP](#i2mrep)                             |         | [I2M.REP# channel](#i2mrep-1)                         | Get note repetition                 |
|      | [I2M.REP repetitions](#i2mrep)                 |         | [I2M.REP# channel repetitions](#i2mrep-1)             | Set note repetition                 |
| get  | [I2M.RAT](#i2mrat)                             |         | [I2M.RAT# channel](#i2mrat-1)                         | Get note ratcheting                 |
|      | [I2M.RAT ratchets](#i2mrat)                    |         | [I2M.RAT# channel ratchets](#i2mrat-1)                | Set note ratcheting                 |
|      | [I2M.MUTE on/off](#i2mmute)                    |         | [I2M.MUTE# channel on/off](i2mmute-1)                 | Set channel mute state              |
|      | [I2M.SOLO on/off](#i2msolo)                    |         | [I2M.SOLO# channel on/off](i2msolo-2)                 | Set channel solo state              |
|      | [I2M.NOTE note velocity](#i2mnote)             | I2M.N   | [I2M.N# channel note velocity](#i2mn)                 | Send Note On                        |
|      | [I2M.NOTE.O note](#i2mnoteo)                   | I2M.NO  | [I2M.NO# channel note](#i2mno)                        | Send Note Off                       |
|      | [I2M.NT note velocity duration](#i2mnt)        |         | [I2M.NT# channel note velocity dur](#i2mnt-1)         | Send Note On with duration          |
|      | [I2M.CHORD chord rootnote velocity](#i2mchord) | I2M.C   | [I2M.C# channel chord rootnote vel](#i2mc)            | Play chord                          |
|      | [I2M.C.ADD chord note](#i2mcadd)               | I2M.C+  |                                                       | Add note to chord                   |
|      | [I2M.C.RM chord note](#i2mcrm)                 | I2M.C-  |                                                       | Remove note to chord                |
|      | [I2M.C.INS chord index note](#i2mcins)         |         |                                                       | Insert note at index                |
|      | [I2M.C.DEL chord index](#i2mcdel)              |         |                                                       | Delete note at index                |
|      | [I2M.C.SET chord index note](#i2mcset)         |         |                                                       | Set note at index                   |
|      | [I2M.C.B chord revbinary](#i2mcb)              |         |                                                       | Define chord using reverse binary   |
|      | [I2M.C.CLR chord](#i2mcclr)                    |         |                                                       | Clear chord                         |
| get  | [I2M.C.L chord](#i2mcl)                        |         |                                                       | Get chord length                    |
|      | [I2M.C.L chord length](#i2mcl)                 |         |                                                       | Set chord length                    |
|      | [I2M.C.SC chord1 chord2](#i2mcsc)              |         |                                                       | Set chord2 as scale for chord1      |
|      | [I2M.C.REV chord iterations](#i2mcrev)         |         |                                                       | Set chord reversal                  |
|      | [I2M.C.ROT chord iterations](#i2mcrot)         |         |                                                       | Set chord rotation                  |
|      | [I2M.C.TRP chord semitones](#i2mctrp)          |         |                                                       | Set chord transposition             |
|      | [I2M.C.DIS chord iter. anchorpoint](#i2mcdis)  |         |                                                       | Set chord distortion                |
|      | [I2M.C.REF chord iter. anchorpoint](#i2mcref)  |         |                                                       | Set chord reflection                |
|      | [I2M.C.INV chord iteration](#i2mcinv)          |         |                                                       | Set chord inversion                 |
|      | [I2M.C.STR chord ms](#i2mcstr)                 |         |                                                       | Set chord strumming                 |
|      | [I2M.C.VCUR chord curve start end](#i2mcvcur)  | I2M.V~  |                                                       | Set chord velocity curve            |
|      | [I2M.C.TCUR chord curve start end](#i2mctcur)  | I2M.T~  |                                                       | Set chord time curve                |
|      | [I2M.C.DIR chord direction](#i2mcdir)          |         |                                                       | Set chord play direction            |
|      | [I2M.C.QN chord rootnote index](#i2mcqn)       |         |                                                       | Query chord note at index           |
|      | [I2M.C.QV chord velocity index](#i2mcqv)       |         |                                                       | Query chord note velocity at index  |
|      | [I2M.B.R on/off](#i2mbr)                       |         |                                                       | Turn recording to buffer on/off     |
|      | [I2M.B.L length](#i2mbl)                       |         |                                                       | Set buffer length                   |
|      | [I2M.B.START offset](#i2mbstart)               |         |                                                       | Add offset to buffer start          |   
|      | [I2M.B.END offset](#i2mbend)                   |         |                                                       | Add a negative offset to buffer end | 
|      | [I2M.B.DIR direction](#i2mbdir)                |         |                                                       | Set buffer play direction           |    
|      | [I2M.B.SPE percent](#i2mbspe)                  |         |                                                       | Set buffer play speed               |
|      | [I2M.B.FB iterations](#i2mbfb)                 |         |                                                       | Iterations until velocity reaches 0 |
|      | [I2M.B.NSHIFT semitones](#i2mbnshift)          |         |                                                       | Set note shift                      |
|      | [I2M.B.VSHIFT velocity](#i2mbvshift)           |         |                                                       | Set velocity shift                  |
|      | [I2M.B.TSHIFT ms](#i2mbtshift)                 |         |                                                       | Set duration shift                  |
|      | [I2M.B.NOFF semitones](#i2mbnoff)              |         |                                                       | Set note offset                     |
|      | [I2M.B.VOFF velocity](#i2mbvoff)               |         |                                                       | Set velocity offset                 |
|      | [I2M.B.TOFF ms](#i2mbtoff)                     |         |                                                       | Set duration offset                 |
|      | [I2M.B.CLR](#i2mbclr)                          |         |                                                       | Clear the buffer                    |
|      | [I2M.B.MODE mode](#i2mbmode)                   |         |                                                       | Set buffer mode (Free, Tape)        |
|      | [I2M.CC controller value](#i2mcc)              |         | [I2M.CC# channel controller value](#i2mcc-1)          | Send CC                             |
|      | [I2M.CCV controller value](#i2mccv)            |         | [I2M.CCV# channel controller value](#i2mccv-1)        | Send CC with volt value             |
| get  | [I2M.CC.OFF controller](#i2mccoff)             |         | [I2M.CC.OFF# channel controller](#i2mccoff-1)         | Get CC offset                       |
|      | [I2M.CC.OFF controller offset](#i2mccoff)      |         | [I2M.CC.OFF# channel controller offset](#i2mccoff-1)  | Set CC offset                       |
| get  | [I2M.CC.SLEW controller](#i2mccslew)           |         | [I2M.CC.SLEW# channel controller](#i2mccslew-1)       | Get CC slew                         |
|      | [I2M.CC.SLEW controller ms](#i2mccslew)        |         | [I2M.CC.SLEW# channel controller ms](#i2mccslew-1)    | Set CC slew                         |
|      | [I2M.CC.SET controller value](#i2mccset)       |         | [I2M.CC.SET# channel controller value](#i2mccset-1)   | Send CC and ignore slew             |
|      | [I2M.NRPN msb lsb value](#i2mnrpn)             |         | [I2M.NRPN# channel msb lsb value](#i2mnrpn-1)         | Send NRPN                           |
| get  | [I2M.NRPN.OFF msb lsb](#i2mnrpnoff)            |         | [I2M.NRPN.OFF# channel msb lsb](#i2mnrpnoff-1)        | Get NRPN offset                     |
|      | [I2M.NRPN.OFF msb lsb offset](#i2mnrpnoff)     |         | [I2M.NRPN.OFF# channel msb lsb offset](#i2mnrpnoff-1) | Set NRPN offset                     |
| get  | [I2M.NRPN.SLEW msb lsb](#i2mnrpnslew)          |         | [I2M.NRPN.SLEW# channel msb lsb](#i2mnrpnslew-1)      | Get NRPN slew                       |
|      | [I2M.NRPN.SLEW msb lsb ms](#i2mnrpnslew)       |         | [I2M.NRPN.SLEW# channel msb lsb ms](#i2mnrpnslew-1)   | Set NRPN slew                       |
|      | [I2M.NRPN.SET msb lsb value](#i2mnrpnset)      |         | [I2M.NRPN.SET# value msb lsb value](#i2mnrpnset-1)    | Send NRPN and ignore slew           |
|      | [I2M.PRG program](#i2mprg)                     |         |                                                       | Send Program Change                 |
|      | [I2M.PB value](#i2mpb)                         |         |                                                       | Send Pitchbend                      |
|      | [I2M.AT value](#i2mat)                         |         |                                                       | Send Aftertouch                     |
|      | [I2M.CLK](#i2mclk)                             |         |                                                       | Send Clock                          |
|      | [I2M.START](#i2mstart)                         |         |                                                       | Send Clock Start                    |
|      | [I2M.STOP](#i2mstop)                           |         |                                                       | Send Clock Stop                     |
|      | [I2M.CONT](#i2mcont)                           |         |                                                       | Send Clock Continue                 |
| get  | [I2M.Q.CH](#i2mqch)                            |         |                                                       | Get current MIDI in Channel         |
|      | [I2M.Q.CH channel](#i2mqch)                    | I2M.Q.# |                                                       | Set current MIDI in Channel         |
|      | [I2M.Q.LATCH on/off](#i2mqlatch)               |         |                                                       | Turn latch on/off                   |
| get  | [I2M.Q.NOTE index](#i2mqnote)                  | I2M.Q.N |                                                       | Get x-last Note On                  |
| get  | [I2M.Q.VEL index](#i2mqvel)                    | I2M.Q.V |                                                       | Get x-last Velocity                 |
| get  | [I2M.Q.CC controller](#i2mqcc)                 |         |                                                       | Get CC value                        |
| get  | [I2M.Q.LCH](#i2mqlch)                          |         |                                                       | Get last MIDI in Channel            |
| get  | [I2M.Q.LN](#i2mqln)                            |         |                                                       | Get last MIDI in Note On            |
| get  | [I2M.Q.LV](#i2mqlv)                            |         |                                                       | Get last MIDI in Velocity           |
| get  | [I2M.Q.LO](#i2mqlo)                            |         |                                                       | Get last MIDI in Note Off           |
| get  | [I2M.Q.LC](#i2mqlc)                            |         |                                                       | Get last MIDI in Controller         |
| get  | [I2M.Q.LCC](#i2mqlcc)                          |         |                                                       | Get last MIDI in CC Value           |
|      | [I2M.PANIC](#i2mpanic)                         |         |                                                       | Don't panic!                        |


---

#### OP Name
`OP`  
`OP (set)`  
`Alias`    
Description  


---

### MIDI out: Settings
*Settings for outoing MIDI messages*  


#### I2M.CH
`I2M.CH`  
`I2M.CH x`  
`I2M.#`  

Get currently set MIDI channel / Set MIDI channel `x` (1..16 for TRS, 17..32 for USB) for MIDI out. Use MIDI channels 1-16 for TRS output, 17-32 for USB output. Default is `x = 1`.  

#### I2M.TIME
`I2M.TIME`  
`I2M.TIME x`  
`I2M.T`  

Get current note duration / Set note duration of MIDI notes to `x` ms (0..32767) for current channel. Based on note duration, i2c2midi will send a MIDI Note Off message automatically. Set `x = 0` to deactivate automatic Note Off messages. Default is `x = 100`.

#### I2M.T#
`I2M.T# ch`
`I2M.T# ch x`  

Get current note duration / Set note duration of MIDI notes to `x` ms (0..32767) for channel `ch` (0..32). Based on note duration, i2c2midi will send a MIDI Note Off message automatically. Set `x = 0` to deactivate automatic Note Off messages. Default is `x = 100`. Use `ch = 0` to set for all channels.

#### I2M.SHIFT
`I2M.SHIFT`  
`I2M.SHIFT x`  
`I2M.S`  

Get current transposition / Set transposition of MIDI notes to `x` semitones (-127..127) for current channel. Default is `x = 0`.  

#### I2M.S#
`I2M.S# ch`  
`I2M.S# ch x`  

Get current transposition / Set transposition of MIDI notes to `x` semitones (-127..127) for channel `ch` (0..32). Default is `x = 0`. Use `ch = 0` to set for all channels.  

#### I2M.MIN
`I2M.MIN x y`  

Set minimum note number of MIDI notes to `x` (0..127), using mode `y` (0..3), for current channel. Default is `x = 0` and `y = 0`. The following modes are available for notes lower than the minimum: 0) Ignore notes 1) Clamp notes 2) Fold back notes by one octave 3) Fold back notes by multiple octaves.  

#### I2M.MIN#
`I2M.MIN# ch x y`  

Set minimum note number of MIDI notes to `x` (0..127), using mode `y` (0..3), for channel `ch` (0..32). Default is `x = 0` and `y = 0`. The following modes are available for notes lower than the minimum: 0) Ignore notes 1) Clamp notes 2) Fold back notes by one octave 3) Fold back notes by multiple octaves. Use `ch = 0` to set for all channels.  

#### I2M.MAX
`I2M.MAX x y`  

Set maximum note number of MIDI notes to `x` (0..127), using mode `y` (0..3), for current channel. Default is `x = 0` and `y = 0`. The following modes are available for notes higher than the maximum: 0) Ignore notes 1) Clamp notes 2) Fold back notes by one octave 3) Fold back notes by multiple octaves. 

#### I2M.MAX#
`I2M.MAX# ch x y`  

Set maximum note number of MIDI notes to `x` (0..127), using mode `y` (0..3), for channel `ch` (0..32). Default is `x = 0` and `y = 0`. The following modes are available for notes higher than the maximum: 0) Ignore notes 1) Clamp notes 2) Fold back notes by one octave 3) Fold back notes by multiple octaves. Use `ch = 0` to set for all channels.  

#### I2M.REP
`I2M.REP`  
`I2M.REP x`  

Get current repetition / Set repetition of MIDI notes to `x` repetitions (1..127) for current channel. Set `x = 1` for no repetitions. Default is `x = 1`.  

#### I2M.REP#
`I2M.REP# ch`  
`I2M.REP# ch x`  

Get current repetition / Set repetition of MIDI notes to `x` repetitions (1..127) for channel `ch` (0..32). Set `x = 1` for no repetitions. Default is `x = 1`. Use `ch = 0` to set for all channels.  

#### I2M.RAT
`I2M.RAT`  
`I2M.RAT x`  

Get current ratcheting / Set ratcheting of MIDI notes to `x` ratchets (1..127) for current channel. Set `x = 1` for no ratcheting. Default is `x = 1`.  

#### I2M.RAT#
`I2M.RAT# ch`  
`I2M.RAT# ch x`  

Get current ratcheting / Set ratcheting of MIDI notes to `x` ratchets (1..127) for channel `ch` (0..32). Set `x = 1` for no ratcheting. Default is `x = 1`. Use `ch = 0` to set for all channels.  

#### I2M.MUTE
`I2M.MUTE x`  
  
Set mute state of current MIDI channel to `x`. If `x = 1` all outoing MIDI messages on that channel are muted. Default is `x = 0`.  

#### I2M.MUTE#
`I2M.MUTE# ch x`  
  
Set mute state of MIDI channel `ch` to `x`. If `x = 1` all outoing MIDI messages on that channel are muted. Default is `x = 0`. Use `ch = 0` to set for all channels.  

#### I2M.SOLO
`I2M.SOLO x`  

Set solo state of current MIDI channel to `x`. If `x = 1` all outoing MIDI messages on that channel are solod. Default is `x = 0`.  

#### I2M.SOLO#
`I2M.SOLO x`  

Set solo state of MIDI channel `ch` to `x`. If `x = 1` all outoing MIDI messages on that channel are solod. Default is `x = 0`. Use `ch = 0` to set for all channels.  


---

### MIDI out: Notes
*Send an individual MIDI note. Each note consists of a note number (pitch), velocity and duration. i2c2midi will take care of Note Off messages send an automatic Note Off message based on the set note duration. The note duration can be set globally via `I2M.TIME` or individually via `I2M.NT`.*  

#### I2M.NOTE
`I2M.NOTE x y`  
`I2M.N`  

Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127) on current channel. A velocity of `0` will be treated as a MIDI Note Off message.  

#### I2M.N#
`I2M.N# ch x y`    

Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127) on channel `ch` (1..32). A velocity of `0` will be treated as a MIDI Note Off message.

#### I2M.NOTE.O
`I2M.NOTE.O x`  
`I2M.NO`  

Send a manual MIDI Note Off message for note number `x` (0..127) on current channel. This can be used either before i2c2midi sends the automatic Note Off message (to stop the note from playing before its originally planned ending), or in combination with `I2M.TIME` set to `0` (in which case i2c2midi does not send automatic Note Off messages).

#### I2M.NO#
`I2M.NO# ch x`  

Send a manual MIDI Note Off message for note number `x` (0..127) on channel `ch` (1..32). This can be used either before i2c2midi sends the automatic Note Off message (to stop the note from playing before its originally planned ending), or in combination with `I2M.TIME` set to `0` (in which case i2c2midi does not send automatic Note Off messages).  

#### I2M.NT
`I2M.NT x y z`  

Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127) and note duration `z` ms (0..32767) on current channel.   

#### I2M.NT#
`I2M.NT# ch x y z`  

Send MIDI Note On message for note number `x` (0..127) with velocity `y` (1..127) and note duration `z` ms (0..32767) on channel `ch` (1..32).    

---

### MIDI out: Chords
*Play a chord, consisting of multiple MIDI notes. Chords are basically groups of relative note numbers or pitches. One chord consists of up to eight notes defined via `I2M.C.ADD`, `I2M.C.RM`, `I2M.C.INS`, `I2M.C.DEL` or `I2M.C.SET`. The notes are defined relative to the root note which is specified when playing a chord via `I2M.C`. When a chord is played, the notes are sent out as individual MIDI notes in the order they are defined in the chord. If no note has been defined in the chord yet, no note will be played. 8 chords can be defined using their respective index 1..8.*

#### I2M.CHORD
`I2M.CHORD x y z`  
`I2M.C`  

Play chord `x` (1..8) with root note `y` (0..127) and velocity `z` (1..127) on current channel.  

#### I2M.C#
`I2M.C# ch x y z`  

Play chord `x` (1..8) with root note `y` (0..127) and velocity `z` (1..127) on channel `ch` (1..32).  

#### I2M.C.ADD
`I2M.C.ADD x y`  
`I2M.C+`  

Add note `y` (-127..127) to chord `x` (0..8), with `y` relative to the root note specified when playing a chord. E.g. add `0`, `4` and `7` to define a major triad. Or go more experimental, e.g. `-2`, `13`, `2`, `13`. Up to eight chords can be defined, with eight notes each. Use `x = 0` to add the note to all chords.  

#### I2M.C.RM
`I2M.C.RM x y`  
`I2M.C-`  

Remove note `y` (-127..127) from chord `x` (0..8). If the chord contains note `y` multiple times, the latest instance is removed. If the chord does not contain the note the message is simply ignored. Use `x = 0` to remove the note from all chords.  

#### I2M.C.INS
`I2M.C.INS x y z`  

Add note `z` (-127..127) to chord `x` (0..8) at index `y` (0..7), with `z` relative to the root note. Already defined notes at index `y` and higher are pushed to the right. Use `x = 0` to insert the note to all chords.  

#### I2M.C.DEL
`I2M.C.DEL x y`  

Delete note at index `y` (0..7) from chord `x` (0..8). Notes at index `y + 1` and higher are pushed to the left. If `y` is higher than the length of the chord, the message is ignored. Use `x = 0` to delete the note from all chords.  

#### I2M.C.SET
`I2M.C.SET x y z`  

Set note at index `y` (0..7) in chord `x` (0..8) to note `z` (-127..127), replacing what was defined earlier at this index. If `y` is higher than the length of the chord, the message is ignored. Use `x = 0` to set the note in all chords.  

#### I2M.C.B
`I2M.C.B x y`  

Clear and define chord `x` (0..8) using reverse binary notation (`R...`). Use `1` or `0` in order to include or exclude notes from the chord. E.g. use `x = R10001001` for `0,4,7` (major triad) or `x = R1000000100000001` for `0,7,15`. `y` can be a maximum of 16 digit long. Use `x = 0` to clear and define all chords.  

#### I2M.C.CLR
`I2M.C.CLR x`  

Clear chord `x` (0..8). Use `x = 0` to clear all chords.  

#### I2M.C.L
`I2M.C.L x`  
`I2M.C.L x y`  

Get current length / Set length of chord `x` (0..8) to `y` (1..8). The length of a chord changes automatically each time a note is added or removed. Values of `x` higher than number of actual defined notes are ignored. Already defined notes are not affected by setting the chord length, but won't be played if their index is outside of the set chord length. Use `x = 0` to set the length of all chords.  

#### I2M.C.SC
`I2M.C.SC x y`  

Set scale for chord `x` (0..8) based on chord `y` (0..8). Setting a scale for a chord comes in handy when using chord transformations that introduce new notes, like `I2M.C.TRP`, `I2M.C.DIS` or `I2M.C.REF`. Use `y = 0` to remove the scale. Use `x = 0` to set scale for all chords.    

→ [More info on how scales work on i2c2midi](#how-scales-work-on-i2c2midi)

#### I2M.C.REV
`I2M.C.REV x y`  

Set reversal of notes in chord `x` (0..8) to `y`. `y = 0` or an even number means not reversed, `y = 1` or an uneven number  means reversed. E.g. `y = 1` for chord `0,3,7` will lead to `7,3,0`. Default is `y = 0`. Use `x = 0` to set reversal all chords. 

  https://user-images.githubusercontent.com/76960718/170448152-c708c42d-ed18-4411-b0e5-6085a569185b.mp4

#### I2M.C.ROT
`I2M.C.ROT x y`  

Set rotation of notes in chord `x` (0..8) to `y` steps (-127..127). E.g. `y = 1` of chord `0,3,7` will lead to `3,7,0`, `y = 2` will lead to `7,0,3`, `y = -1` will lead to `7,0,3`. Default is `y = 0`. Use `x = 0` to set rotation for all chords.  

  https://user-images.githubusercontent.com/76960718/170448234-86cafa1e-8ecd-4612-9a84-febde5bead3d.mp4

#### I2M.C.TRP
`I2M.C.TRP x y`  

Set transposition of chord `x` (0..8) to `y` (-127..127). Transposition adds `y` to the note number of each note in the chord. Default is `y = 0`. Use `x = 0` to set transposition for all chords. This transformation introduces new notes to the chord – try it in combination with setting a scale.  

  https://user-images.githubusercontent.com/76960718/170448291-4766446c-830e-4b92-a609-74fe5c757047.mp4

#### I2M.C.DIS
`I2M.C.DIS x y z`  

Set distortion of chord `x` (0..8) to width `y` (-127..127) with anchor point `z` (0..16). Distortion adds `y+n` to the note number of each note in the chord. The anchor point influences the direction and amount (`n`) of the transformation. Default is `y = 0`. Use `x = 0` to set distortion for all chords. This transformation introduces new notes to the chord – try it in combination with setting a scale.  

  https://user-images.githubusercontent.com/76960718/170519200-57bbb0fd-a168-498a-873c-f39924235f2d.mp4

#### I2M.C.REF
`I2M.C.REF x y z`  

Set reflection of chord `x` (0..8) to `y` (-127..127) with anchor point `z` (0..16). The anchor point defines at which axis the chord gets reflected. Default is `y = 0`. Use `x = 0` to set distortion for all chords. This transformation introduces new notes to the chord – try it in combination with setting a scale.  

  https://user-images.githubusercontent.com/76960718/170448450-b01a0faa-ed1b-4541-b9ee-6c55721735cb.mp4

#### I2M.C.INV
`I2M.C.INV x y`  

Set inversion of chord `x` (0..8) to `y` (-32..32). Default is `y = 0`. Use `x = 0` to set inversion for all chords.  

  https://user-images.githubusercontent.com/76960718/170448519-62731788-977d-4e5f-a442-cc999a97c141.mp4

#### I2M.C.STR
`I2M.C.STR x y`  

Set strumming of chord `x` (0..8) to `y` ms (0..32767). Strumming plays the notes of a chord arpeggiated, with an interval of `y` ms in between notes. Default is `y = 0`. Use `x = 0` to set strumming for all chords.  

#### I2M.C.VCUR
`I2M.C.VCUR w x y z`  
`I2M.C.V~`  

Set velocity curve for chord `w` (0..8) with curve type `x` (0..5), start value `y`% (0..32767) and end value `z`% (0..32767). This will affect the velocity of the notes in the order they are defined in the chord. Start and end percentages refer to the velocity with which the chord is played via `I2M.C`. Use `x = 0` to turn velocity curve off. The following curves are available: 0) Off 1) Linear 2) Exponential 3) Triangle 4) Square 5) Random. Use `w = 0` to set velocity curve for all chords. Try a random curve with subtle values for a humanizing effect.  

#### I2M.C.TCUR
`I2M.C.TCUR w x y z`  
`I2M.C.T~`  

Set time curve for chord `w` (0..8) with curve type `x` (0..5), start value `y`% (0..32767) and end value `z`% (0..32767). This will affect the time interval between the notes in the order they are defined in the chord. Start and end percentages refer to the current strumming setting of the chord, set via `I2M.C.STR`. Use `x = 0` to turn time curve off. The following curves are available: 0) Off 1) Linear 2) Exponential 3) Triangle 4) Square 5) Random. Use `w = 0` to set time curve for all chords. Try a square curve with similar values to create swing. Try a random curve with subtle values for a humanizing effect.  

#### I2M.C.DIR
`I2M.C.DIR x y`   

Set play direction for chord `x` (0..8) to direction `y` (0..8). This will affect the order in which chord notes are played. Make sure to set strumming via `I2M.C.STR`. The following directions are available: 0) Forward (0,1,2,3,4) 1) Backward (4,3,2,1,0) 2) Inside out (2,1,3,0,4) 3) Outside in (0,4,1,3,2) 4) Random (2,3,1,0,4) 5) Bottom repeat (0,1,0,2,0,3,0,4) 6) Top repeat (0,4,1,4,2,4,3,4) 7) Pingpong (0,1,2,3,4,3,2,1,0) 8) Ping & pong (0,1,2,3,4,4,3,2,1,0). Default is `y = 0`.

#### I2M.C.QN
`I2M.C.QN x y z`   

Get the transformed note number of a chord note for chord `x` (1..8) with root note `y` (0..127) at index `z` (0..7). The reponse is the absolute note number (0..127). Use this OP to send the transformed note number to other devices within eurorack, e.g. via V/OCT to any oscillator or via I2C to I2C-enabled devices like Just Friends or disting EX.

#### I2M.C.QV
`I2M.C.QV x y z`   

Get the transformed note velocity of a chord note for chord `x` (1..8) with root velocity `y` (1..127) at index `z` (0..7). The response is the absolute note velocity (0..127). Use this OP to send the transformed note velocity to other devices within eurorack, e.g. via CV to a VCA or via I2C to I2C-enabled devices like Just Friends or disting EX.

---

### MIDI out: Buffer
*Record MIDI notes into a looping buffer. i2c2midi's MIDI buffer behaves much like a looping tape recorder, recording every MIDI note that leaves i2c2midi into the buffer. When the buffer reaches its end, it starts over and plays back the recorded notes, while new notes are recorded (overdubbed). Notes in the buffer can be modified with each buffer iteration. Change the note pitch via `I2M.B.NSHIFT` and `I2M.B.VSHIFT`, change note velocity via `I2M.B.FB`, `I2M.B.VSHIFT` and `I2M.B.VOFF`, change note duration via `I2M.B.TSHIFT` and `I2M.B.TOFF`. When notes reach a velocity of zero they get deleted from the buffer. However, notes can be held in the buffer indefinitely by setting `I2M.B.FB` to 0. Furthermore, the speed and playback direction of the buffer can be set, as well as the length and a start and end offset. The buffer can work in two modes, set via `I2M.B.MODE`.*   
*Depending on the settings, the buffer can act more like a MIDI looper or more like a pitch-shifting delay for MIDI notes.*  

#### I2M.B.R  
`I2M.B.R x`  

Turn recording of notes into the buffer on or off. `x = 1` is on, `x = 0` is off. If recording is turned on, all outgoing MIDI notes are recorded into the buffer, storing note number, note velocity, note duration and MIDI channel.  

#### I2M.B.L  
`I2M.B.L x`  

Set the length of the buffer to `x` ms (0..32767). Default is `x = 1000`.  

#### I2M.B.START  
`I2M.B.START x`  

Add an offset of `x` ms (0..32767) to the start of the buffer. The offset time is non-distructively added to the start of the looping buffer. E.g. if the buffer length is set to `1000` ms and start offset is set to `200` ms, the buffer will loop the section `200` - `1000` ms, resulting in a looping buffer length of `800` ms. Default is `x = 0`.  

#### I2M.B.END  
`I2M.B.END x`  

Add a negative offset of `x` ms (0..32767) to the end of the buffer. The offset time is non-distructively substracted from the end of the looping buffer. E.g. if the buffer length is set to `1000` ms, start offset is set to `200` ms, and end offset is set to `300` ms, the buffer will loop the section `200` - `700` ms, resulting in a looping buffer length of `500` ms. Default is `x = 0`.  

#### I2M.B.DIR    
`I2M.B.DIR x`  

Set the play direction `x` (0..2) of the buffer. The following directions are available: 0) Forward 1) Backward 2) Pingpong. Keep in mind that changing the direction only affects notes that have been already recorded to the buffer before the change in direction; all notes recorded afterwards are recorded relative to the new direction. Default is `x = 0`.  

#### I2M.B.SPE     
`I2M.B.SPE x`  

Set the playing speed `x` (1..32767) of the buffer. `x = 100` is equivalent to "normal speed", `x = 50` means double the speed, `x = 200` means half the speed, etc. Of course, all values in between can be chosen. Keep in mind that changing the speed only affects notes that have been already recorded to the buffer before the change in speed; all notes recorded afterwards are recorded relative to the new speed. Default is `x = 100`.  

#### I2M.B.FB  
`I2M.B.FB x`  

Set the feedback length `x` (0..255) of the buffer. By default, each recorded note is getting decreased in velocity with each buffer iteration. The feedback value determines, how many buffer iterations a recorded note will "survive" in the buffer, before the decreasing velocity will reach zero (meaning the note is remove from the buffer). Set `x = 0` to turn off the automatic decrease in velocity, keeping notes in the buffer indefinitely. Use this setting in combination with `I2M.B.VSHIFT` or `I2M.B.CLR`. Default is `x = 8`.

#### I2M.B.NSHIFT  
`I2M.B.NSHIFT x`

Set the note shift of recorded notes to `x` semitones (-127..127). With each buffer iteration, this value gets added accumulatively to the original note number. E.g. with a note shift setting of `x = 12`, a recorded note `60` will be played as note `72` during the first buffer iteration, as note `84` during the second iteration, etc. Default is `x = 0`.  

#### I2M.B.VSHIFT    
`I2M.B.VSHIFT x`  

Set the velocity shift of recorded notes to `x` (-127..127). With each buffer iteration, this value gets added accumulatively to the original note velocity. E.g. with a velocity shift setting of `x = -10`, a recorded note with velocity `110` will be played with velocity `100` during the first buffer iteration, with velocity `90` during the second iteration, etc. Default is `x = 0`.  Please note: This setting is the twin sibling of `I2M.B.FB`: While `I2M.B.FB` defines the number of iterations determining the amount of change in velocity per iteration, `I2M.B.VSHIFT` defines the amount of change in velocity per iteration determining the number of iterations.  

#### I2M.B.TSHIFT    
`I2M.B.TSHIFT x`  

Set the note duration shift ("time shift") of recorded notes to `x` ms (-16384..16383). With each buffer iteration, this value gets added accumulatively to the original note duration. E.g. with a duration shift setting of `x = 100`, a recorded note with duration `200` will be played with duration `300` during the first buffer iteration, with duration `400` during the second iteration, etc. Default is `x = 0`.  

#### I2M.B.NOFF     
`I2M.B.NOFF x`  

Set the note offset of recorded notes to `x` semitones (-127..127). This value gets added once to the original note number and is then kept for all buffer iterations. E.g. with a note offset setting of `x = 7`, a recorded note `60` will be played as note `67` for all buffer iterations. Default is `x = 0`.  

#### I2M.B.VOFF    
`I2M.B.VOFF x`  

Set the velocity offset of recorded notes to `x` (-127..127). This value gets added once to the original note velocity and is then kept for all buffer iterations. E.g. with a velocity offset setting of `x = -50`, a recorded note with velocity `120` will be played with velocity `70` for all buffer iterations. Default is `x = 0`.  

#### I2M.B.TOFF    
`I2M.B.TOFF x`  

Set the note duration offset ("time offset") of recorded notes to `x` ms (-16384..16383). This value gets added once to the original note duration and is then kept for all buffer iterations. E.g. with a duration offset setting of `x = -50`, a recorded note with duration `200` will be played with duration `150` for all buffer iterations. Default is `x = 0`.  

#### I2M.B.CLR  
`I2M.B.CLR`  

Clear the buffer, erasing all recorded notes in the buffer.  

#### I2M.B.MODE    
`I2M.B.MODE x`  

Set the buffer mode to `x` (0..1). The buffer can work in two different modes: 1) Digital 2) Tape. In Digital mode, the buffer speed ( set via `I2M.B.SPE`) works independent of note number and note duration: If the buffer speed changes, the note number and note duration of a recorded note stays unaffected. In Tape mode on the other hand, the buffer speed affects the note number and note duration of recorded notes in the buffer, mimicking the behaviour of real tape. If the buffer speed gets doubled, the note number is pitched up by one octave and the note duration gets divided in half. Similarly, if the buffer speed gets divided in half, the note number is pitched down an octave and the note duration gets doubled, etc. Default is `x = 0`.  

---

### MIDI out: CCs
*Send MIDI CCs*

#### I2M.CC
`I2M.CC x y`  

Send MIDI CC message for controller `x` (0..127) with value `y` (0..127) on current channel.  

#### I2M.CC#
`I2M.CC# ch x y`  

Send MIDI CC message for controller `x` (0..127) with value `y` (0..127) on channel `ch` (1..32).  

#### I2M.CCV
`I2M.CCV x y`  

Send MIDI CC message for controller `x` (0..127) with volt value `y` (0..16383, 0..+10V) on current channel.  

#### I2M.CCV#
`I2M.CCV# ch x y`  

Send MIDI CC message for controller `x` (0..127) with volt value `y` (0..16383, 0..+10V) on channel `ch` (1..32).  

#### I2M.CC.OFF
`I2M.CC.OFF x`  
`I2M.CC.OFF x y`  

Get current offset / Set offset of values of controller `x` (0..127) to `y` (-127..127) for current channel. Default is `y = 0`.  

#### I2M.CC.OFF#
`I2M.CC.OFF# ch x`  
`I2M.CC.OFF# ch x y`  

Get current offset / Set offset of values of controller `x` (0..127) to `y` (-127..127) for channel `ch` (1..32). Default is `y = 0`.  

#### I2M.CC.SLEW
`I2M.CC.SLEW x`  
`I2M.CC.SLEW x y`  

Get current slew time / Set slew time for controller `x` (0..127) to `y` ms (0..32767) for current channel. i2c2midi will ramp from the controller's last value to a new value within the given time `x`, sending MIDI CCs at a maximum rate of 30 ms. If the slewing is still ongoing when a new value is set, the slewing uses its current position as the last value. Is 8 CC controller values can be slewed simoultaneously before the oldest currently slewing value is overwritten by the newest. Default is `y = 0`.  

#### I2M.CC.SLEW#
`I2M.CC.SLEW# ch x`  
`I2M.CC.SLEW# ch x y`  

Get current slew time / Set slew time for controller `x` (0..127) to `y` ms (0..32767) for channel `ch` (1..32). i2c2midi will ramp from the controller's last value to a new value within the given time `x`, sending MIDI CCs at a maximum rate of 30 ms. If the slewing is still ongoing when a new value is set, the slewing uses its current position as the last value. Is 8 CC controller values can be slewed simoultaneously before the oldest currently slewing value is overwritten by the newest. Default is `y = 0`.  

#### I2M.CC.SET
`I2M.CC.SET x y`  

Send MIDI CC message for controller `x` (0..127) with value `y` (0..127) on current channel, bypassing any slew settings.  

#### I2M.CC.SET#
`I2M.CC.SET# ch x y`  

Send MIDI CC message for controller `x` (0..127) with value `y` (0..127) on channel `ch` (1..32), bypassing any slew settings.  

---

### MIDI out: NRPN
*Send MIDI NRPN messages*

#### I2M.NRPN
`I2M.NRPN x y z`  

Send MIDI NRPN message (high-res CC) for parameter MSB `x` and LSB `y` with value `y` (0..16383) on current channel.  

#### I2M.NRPN#
`I2M.NRPN# ch x y z`  

Send MIDI NRPN message (high-res CC) for parameter MSB `x` and LSB `y` with value `y` (0..16383) on channel `ch` (1..32).  

#### I2M.NRPN.OFF
`I2M.NRPN.OFF x y`  
`I2M.NRPN.OFF x y z`  

Get current offset / Set offset of values of NRPN messages to `z` (-16384..16383) for current channel. Default is `z = 0`.  

#### I2M.NRPN.OFF#
`I2M.NRPN.OFF# ch x y`  
`I2M.NRPN.OFF# ch x y z`  

Get current offset / Set offset of values of NRPN messages to `z` (-16384..16383) for channel `ch` (1..32). Default is `z = 0`.  

#### I2M.NRPN.SLEW
`I2M.NRPN.SLEW x y`  
`I2M.NRPN.SLEW x y z`  

Get current slew time / Set slew time for NRPN messages to `z` ms (0..32767) for current channel. Default is `z = 0`.  

#### I2M.NRPN.SLEW#
`I2M.NRPN.SLEW# ch x y`  
`I2M.NRPN.SLEW# ch x y z`  

Get current slew time / Set slew time for NRPN messages to `z` ms (0..32767) for channel `ch` (1..32). Default is `z = 0`.  

#### I2M.NRPN.SET
`I2M.NRPN.SET x y z`  

Send MIDI NRPN message for parameter MSB `x` and LSB `y` with value `y` (0..16383) on current channel, bypassing any slew settings.  

#### I2M.NRPN.SET#
`I2M.NRPN.SET# ch x y z`  

Send MIDI NRPN message for parameter MSB `x` and LSB `y` with value `y` (0..16383) on channel `ch` (1..32), bypassing any slew settings.  

---

### MIDI out: Misc
*Send other MIDI messages like Program Change, Pitch Bend and Clock*

#### I2M.PRG
`I2M.PRG x`  

Send MIDI Program Change message for program `x` (0..127)  

#### I2M.PB
`I2M.PB x`  

Send MIDI Pitch Bend message with value `x` (-8192..8191)  

#### I2M.AT
`I2M.AT x`  

Send MIDI After Touch message with value `x` (0..127)  

#### I2M.CLK
`I2M.CLK`  

Send MIDI Clock message, for now: interpreted as quarter note  

#### I2M.START
`I2M.START`  

Send MIDI Clock Start message  

#### I2M.STOP
`I2M.STOP`  

Send MIDI Clock Stop message  

#### I2M.CONT
`I2M.CONT`  

Send MIDI Clock Continue message  

#### I2M.PANIC
`I2M.PANIC`  

Send MIDI Note Off messages for all notes on all channels, and reset note duration, shift, repetition, ratcheting, min/max  

---

### MIDI in: Settings
*Settings for incoming MIDI messages*

#### I2M.Q.CH
`I2M.Q.CH`  
`I2M.Q.CH x`  
`I2M.Q.# x`  

Get currently set MIDI channel / Set MIDI channel `x` (1..16) for MIDI in. Default is `x = 1`.  

#### I2M.Q.LATCH
`I2M.Q.LATCH x`  

Turn on or off latching for MIDI notes received via MIDI in. `x = 0` means Note Off messages are recorded in the note history, so only notes with keys currently held down on the MIDI controller are stored. `x = 1` means Note Off messages are not recorded in the note history, so notes are still stored after releasing the respective key on the MIDI controller. Default is `x = 1`.  

---

### MIDI in: Notes
*Access the note history – the eight last received note numbers and velocities*

#### I2M.Q.NOTE
`I2M.Q.NOTE x`  
`I2M.Q.N`  

Get `x` (0..7) last note number (0..127) received via MIDI in  

#### I2M.Q.VEL
`I2M.Q.VEL x`  
`I2M.Q.V`  

Get `x` (0..7) last note velocity (1..127) received via MIDI in  

--- 

### MIDI in: CCs
*Access the stored data of CC values received via MIDI in*

#### I2M.Q.CC
`I2M.Q.CC x`  

Get current value (0..127) of controller `x` (0..127) received via MIDI in  

---

### MIDI in: Get latest
*Get the respective latest value reveived via MIDI in*

#### I2M.Q.LCH
`I2M.Q.LCH`  

Get the latest channel (1..16) received via MIDI in  

#### I2M.Q.LN
`I2M.Q.LN`  

Get the note number (0..127) of the latest Note On received via MIDI in  

#### I2M.Q.LV
`I2M.Q.LV`  

Get the velocity (1..127) of the latest Note On received via MIDI in  

#### I2M.Q.LO
`I2M.Q.LO`  

Get the note number (0..127) of the latest Note Off received via MIDI in  

#### I2M.Q.LC
`I2M.Q.LC`  

Get the latest controller number (0..127) received via MIDI in  

#### I2M.Q.LCC
`I2M.Q.LCC`  

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


## Further Reading

### How scales work on i2c2midi

Scales in i2c2midi do not work like a quantizer where notes "outside" of the scale are forced in place. Instead, i2c2midi will respect "outside" notes and keep their position in respect to the defined scale. This means, you can use a scale for chord transformations and still intentionally define notes in your chord that are not part of that scale.

Let's say, you define chord 1 with notes `0,4,7,8` and set a major scale `0,2,4,5,7,9,11` via chord 2, note `8` of chord 1 will be "outside" the scale – instead of removing this note or forcing it into another note, i2c2midi will store the nearest note in the scale (`7`) and the respective delta (`+1`). In other words, this note will be treated by i2c2midi as the "5th note in the scale raised by 1 semitone".

Let's break this down step by step:

1) The following three lines of Teletype code will define two chords, and set chord 2 as the scale for chord 1:
   ```
   I2M.C.B 1 R100010011       // define chord 1: 0,4,7,8
   I2M.C.B 2 R101011010101    // define chord 2: 0,2,4,5,7,9,11 (major scale)
   I2M.C.SC 1 2               // set chord 2 as scale for chord 1
   ```

2) Chord 1 has a note that is not part of the scale defined by Chord 2:   
Notes `0` ✅, `4` ✅ and `7` ✅ are part of the scale, Note `8` ❗️ is not:

   | Note Name                     | C | C#| D | D#| E | F | F#| G | G#| A | A# | B  |
   |---                            |---|---|---|---|---|---|---|---|---|---|--- |--- |
   | Chromatic Scale               | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
   | Def. Scale (e.g. Chord 2)  | 0 |   | 2 |   | 4 | 5 |   | 7 |   | 9 |    | 11 |
   | Def. Chord (e.g. Chord 1)  | 0 ✅ |   |   |   | 4 ✅  |  |    | 7 ✅  | 8❗️| | | |
   | How i2c2midi stores the Chord | 0 |   |   |   | 4 |  |    | 7 | 7+1|   |    |    |

3) If notes in a chord are not part of the defined scale, i2c2midi looks for the nearest scale note and stores this note together with the respective delta.  
In this example the note not beeing part of the scale is `8`. The nearest note in the scale would be `7`, and the delta would be `+1`. So, i2c2midi will store `0,4,7,7+1`.  

4) Without any Chord Transformations, Chord 1 will be playing exactly as defined: `0,4,7,8` – the defined scale has no effect yet. But as soon as we use Chord Transformations, each transformation will happen within the defined scale, and the "outside" note `8` will also be transformed within the scale as note `7+1`:
    
   | Transformation | → | Note 1 | Note 2 | Note 3 | Note 4 | → | Resulting Chord |
   |------------|---|--------|--------|--------|--------|---|-----------------|
   | Transpose = 0  |   | 0      | 4      | 7      | 7+1    |   | 0,  4,   7,  8  |
   | Transpose = 1  |   | 2      | 5      | 9      | 9+1    |   | 2,  5,   9,  10 |
   | Transpose = 2  |   | 4      | 7      | 11     | 11+1   |   | 4,  7,   11, 12 |
   | Transpose = 3  |   | 5      | 9      | 0+12   | 0+12+1 |   | 5,  9,   12, 13 |
   | Transpose = 4  |   | 7      | 11     | 2+12   | 2+12+1 |   | 7,  11,  14, 15 |
   | Transpose = 5  |   | 9      | 0+12   | 4+12   | 4+12+1 |   | 9,  12,  16, 17 |
   | Transpose = 6  |   | 11     | 2+12   | 5+12   | 5+12+1 |   | 11, 14,  17, 18 |
   | Transpose = 7  |   | 0+12   | 4+12   | 7+12   | 7+12+1 |   | 12, 16,  19, 20 |
   | Transpose = 8  |   | 2+12   | 5+12   | 9+12   | 9+12+1 |   | 14, 17,  21, 22 |


---


### How curve transformations work

Curve transformations allow you to manipulate the notes of a chord in different ways. Like chord transformations, curve transformations are intentionally designed to encourage experimentation, inviting unexpected and surprising results.

There are two types of curves:
- **Velocity Curve**: Manipulates the velocity of each note in a chord
  
- **Time Curve**: Manipulates the strumming of notes in a chord

Curves are defined by three parameters: A **start value** (%), an **end value** (%), and a **curve type**. 
Start and end values are percentage values, referring to the respective originally set value (velocity for velocity curve, strumming for time curve).
The curve type defines the inpolation of values between the defined start and end values. There are different types available:
  - 1) Linear: Linear from start to end
  - 2) Exponential: Exponential from start to end
  - 3) Triangle: Linear from start to end to start
  - 4) Square: Alternating between start and end
  - 5) Random: Random values between start and end

Here are some examples :

**Example: Chord with 3 notes**
Note duration: 1000 ms; Start: 1%; End: 100%

| Curve Type | Value 1 | Value 2 | Value 3 |
|---|---|---|---|
| 0) Off | 1000 | 1000 | 1000 |
| 1) Linear | 10 | 505 | 1000 |
| 2) Exponential | 10 | 133 | 1000 |
| 3) Triangle | 10 | 1000 | 10 |
| 4) Square | 10 | 1000 | 10 |
| 5) Random | 907 | 393 | 729 |
 

**Example: Chord with 4 notes**
Note duration: 1000 ms; Start: 1%; End: 100%

| Curve Type | Value 1 | Value 2 | Value 3 | Value 4 |
|---|---|---|---|---|
| 0) Off | 1000 | 1000 | 1000 | 1000 |
| 1) Linear | 10 | 340 | 670 | 1000 |
| 2) Exponential | 10 | 46 | 303 | 1000 |
| 3) Triangle | 10 | 505 | 1000 | 505 |
| 4) Alternating | 10 | 1000 | 10 | 1000 |
| 5) Random | 863 | 47 | 677 | 841 |

**Example: Chord with 8 notes**
Note duration: 1000 ms; Start: 1%; End: 100%

| Curve Type | Value 1 | Value 2 | Value 3 | Value 4 | Value 5 | Value 6 | Value 7 | Value 8
|---|---|---|---|---|---|---|---|---|
| 0) Off | 1000 | 1000 | 1000 | 1000 | 1000 | 1000 | 1000 | 1000 | 
| 1) Linear | 10 | 151 | 292 | 434 | 575 | 717 | 858 | 1000 | 
| 2) Exponential | 10 | 12 | 33 | 87 | 194 | 370 | 633 | 1000 | 
| 3) Triangle | 10 | 257 | 505 | 752 | 1000 | 752 | 505 | 257 | 
| 4) Alternating | 10 | 1000 | 10 | 1000 | 10 | 1000 | 10 | 1000 | 
| 5) Random | 468 | 781 | 472 | 34 | 546 | 164 | 301 | 189 |

---



<br/><br/>


## Build the module

i2c2midi comes in two hardware versions: 

- MKI
  - First iteration
  - 4 HP
  - TRS, no USB
  - Works with current firmware
- MKII
  - Second and current iteration
  - 2 HP
  - TRS and USB Host
  - Works with current firmware
 

### Build Guide

- [Here's the build guide for i2c2midi MKII](/hardware/hardware-MK2/BUILD-GUIDE.md)
- Looking for i2c2midi MKI? [Here's all info about MKI](/hardware/hardware-MK1/README.md)


### BOM

Number | Part | Value
--- | --- | ---
1 | T36 | Teensy 3.6 (with pins or without pins), or Teensy 4.1
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
- [Here are the gerber files](/hardware/hardware-MK2/gerber/)
- [Here are some more details about ordering the PCB and Panel from a manufacturer](https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/40950/56) 

### Schematic

![](/hardware/hardware-MK2/i2c2midi_schematic.png)

### MIDI TRS Type A Wiring

Teensy | TRS | MIDI
--- | --- | ---
Teensy Pin 1 | → Tip | → MIDI Pin 5
Teensy 3V3 | → Ring | → MIDI Pin 4
Ground | → Sleeve | → MIDI Pin 2


<br/><br/>

## Firmware

### Firmware Update Teletype

- [Backup your scenes from Teletype](https://monome.org/docs/teletype/manual/#usb-backup)! Your scenes will be gone after the firmware update. 
- Then follow the instructions [detailed over at monome](https://monome.org/docs/modular/update/). 
- When the firmware has been flashed successfully, a message appears on Teletype's screen: `Scenes will be overwritten! Press to confirm; Do not press otherwise!` Please make sure to press the button, before the message disappears (ca. 2-3 seconds). Otherwise the update might not work. This is a special quirk of installing a BETA firmware on Teletype.


### Firmware Update i2c2midi

- Caution: Don't connect the module to Euro power and USB at the same time! 
- Download the latest release [here](https://github.com/attowatt/i2c2midi/releases).
- Unzip the files and open `firmware/i2c2midi_firmware/i2c2midi_firmware.ino` with [Teensyduino](https://www.pjrc.com/teensy/td_download.html).
- Depending on your hardware, change the following settings:
  - For MKI / Teensy 3.2: 
    - Set line 37 to `//#define MK2`
    - Set line 40 to `#define TEENSY3X`
    - Set line 41 to `//#define TEENSY41 `
  - For MKII / Teensy 3.6:
    - Set line 37 to `#define MK2`   
    - Set line 40 to `#define TEENSY3X`
    - Set line 41 to `//#define TEENSY41 `
  - For MKII / Teensy 4.1:
    - Set line 37 to `#define MK2`   
    - Set line 40 to `//#define TEENSY3X`
    - Set line 41 to `#define TEENSY41 `
- Make sure to install all necessary libraries (see list of libraries below). [More info on how to install libraries](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries).
- Connect the Teensy to your computer with a USB cable. Caution: Don't connect the module to Euro power and USB at the same time!
- Under `Tools`:
  - set `Board` to `Teensy 3.2`, `Teensy 3.6` or `Teensy 4.1`
  - set `USB Type` to `Serial`
  - set `CPU Speed` to `96 MHz` for Teensy 3.6 and `150 MHz` for Teensy 4.1
  - select the Port that shows the Teensy
- Upload the firmware to your Teensy via `Sketch` → `Upload`.
- If the upload was successfull, Teensyduino says `Done uploading` and i2c2midi lights up with both LEDs turning on and off four times.

### Changelog

- Version 5.0.1
  - Bugfix for Teensyduino 1.58  
- Version 5.0.0 (works with Teletype Firmware `5.0.0 BETA 1` [Link](https://llllllll.co/t/teletype-5-0-0-beta-testing/62210))
  - New OP `I2M.MUTE` / `I2M.MUTE#`: Get/Set mute state of MIDI channel
  - New OP `I2M.SOLO` / `I2M.SOLO#`: Get/Set solo state of MIDI channel
- Version 4.4.1 (+ Teletype Firmware `I2M BETA 3.1`)
  - Bugfix: Chord 1 could not be set as a scale
- Version 4.4.0 (+ Teletype Firmware `I2M BETA 3.1`)
  - New OP `I2M.C.QN`: Get the transformed chord note at index 
  - New OP `I2M.C.QV`: Get the transformed chord note velocity at index  
- Version 4.3.0 (+ Teletype Firmware `I2M BETA 3`)
  - New looping MIDI buffer / MIDI recorder feature, with new OPs: `I2M.B.R`, `I2M.B.L`, `I2M.B.START`, `I2M.B.END`, `I2M.B.DIR`, `I2M.B.SPE`, `I2M.B.FB`, `I2M.B.NSHIFT`, `I2M.B.VSHIFT`, `I2M.B.TSHIFT`, `I2M.B.NOFF`, `I2M.B.VOFF`, `I2M.B.TOFF`, `I2M.B.CLR`, `I2M.B.MODE`  
  - New OP `I2M.C.DIR`: Set a play direction for a chord
- Version 4.2.0 (+ Teletype Firmware `I2M BETA 2`)
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
- Version 4.1.0
  - Experimental support for Teensy 4.1
- Version 4.0.0 (+ Teletype Firmware `I2M BETA 1`) ([more info](https://llllllll.co/t/i2c2midi-a-diy-module-that-translates-i2c-to-midi/40950/229))
  - Completely rewritten firmware with support for i2c2midi's dedicated Teletype OPs (`I2M`).
  - Updated README.
- Version 3.1.0
  - Added feature flag to use i2c2midi as USB device, using the Teensy Micro USB jack.
  - Fixed a bug where unknown I2C request messages would freeze the I2C bus.
- Version 3.0.0 (MKII)
  - Firmware for i2c2midi hardware MKII, adding USB Host functionality.


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
- teensy4_i2c  
  MIT License – Copyright © 2019-2020 Richard Gemmell  
  https://github.com/Richard-Gemmell/teensy4_i2c  
- Ramp  
  GPL-3.0 License – Sylvain GARNAVAULT - 2016/08/07  
  https://github.com/siteswapjuggler/RAMP
  
  
<br/><br/>

## crow Library

@mreid created a great library that connects i2c2midi to monome crow:  
https://github.com/mreid/crow-i2c2midi

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
