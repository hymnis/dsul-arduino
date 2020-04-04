# DSUL - Disturb State USB Light

The goal of the project is to have a USB connected light, that can be be set to different colors, with adjustable brightness and different modes, which can communicate the users current preference regarding being disturbed.


## Hardware

The hardware used is an Arduino connected to a Neopixel module. The project was developed using an Arduino Nano, but should work on most models as long as the firmware fit and it has enough RAM for the number of LED's used in the module.

Since there are both hardware (current) and software (RAM) limitations to how many LED's/Neopixels can be handled at once by the Arduino, it's important to first check the specifications on the Arduino model used and calculate the maximum number that can be used.


## Firmware

It's possible to adjust the number of LED's used by editing the `NUMPIXELS` variable. Keep in mind how much RAM the Arduino model used has available. With the Neopixel library that this project uses, each LED needs 3 bytes of RAM.



## Computer software

The software, daemon/server and client is available at [hymnis/dsul-python](https://github.com/hymnis/dsul-python).

