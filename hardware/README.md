# Build Guide


## I 
Get the parts. Here's the [BOM](https://github.com/attowatt/i2c2midi#BOM).

![](../pictures/i2c2midi_v_2_0_build_1.jpg)

## II
Solder the following parts:

- Resistors (R1, R2, R3, R4, R5)
![](../pictures/i2c2midi_v_2_0_build_2.jpg)

- Voltage regulator: Bend the legs before soldering.
- Power header: Please check the orientation – the opening of the header should be on the right, when -12V is on the bottom.
![](../pictures/i2c2midi_v_2_0_build_3.jpg)

- Diodes: Orientation does not matter.
![](../pictures/i2c2midi_v_2_0_build_4.jpg)

- I2C header
- Teensy header: I prefer to solder a few pins with the Teensy board attached (to make sure it will fit later), then remove the Teensy and solder the rest of the pins.
![](../pictures/i2c2midi_v_2_0_build_5.jpg)

Cut the legs of the parts.


## III
- Mount the spacer to the PCB with one of the two M3 screws.
- Place the stereo jack (U1) and leds into their positions, but don't solder them yet.
- Place the panel on top of the stereo jack and spacer. Mount it to the jack with the nut and to the spacer with the second M3 screw.

## IV
- Carefully place the LEDs into position, so they look nice on the panel. Then solder them.
- Solder the stereo jack.

![](../pictures/i2c2midi_v_2_0_build_6.jpg)
![](../pictures/i2c2midi_v_2_0_build_7.jpg)


## V
- Connect the Teensy to your computer with a USB cable.
- Download the firmware and open it with [Teensyduino](https://www.pjrc.com/teensy/td_download.html)
- Under `Tools`, set `Board` to `Teensy 3.2`, `USB Type` to `Serial`, and select the `Port` that shows the Teensy.
- Upload the firmware to your Teensy. 
- If the upload was successfull, i2c2midi lights up with both LEDs turning on and off four times.


![](../pictures/i2c2midi_v_2_0_build_8.jpg)
![](../pictures/i2c2midi_v_2_0_build_9.jpg)

**Disclaimer:** 
After building the module, please test it very carefully in a separate case. This is a DIY module. I am not responsible for any damage to your gear.
