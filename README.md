# DSUL - Disturb State USB Light

[![Build Status](https://github.com/hymnis/dsul-arduino/actions/workflows/build.yml/badge.svg)](https://github.com/hymnis/dsul-arduino/actions)
[![License MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

The goal of the project is to have a USB connected light, that can be be set to different colors, with adjustable brightness and different modes, which can communicate the users current preference regarding being disturbed.


## Hardware

The hardware used is an Arduino connected to a NeoPixel module. The project was developed using an Arduino Nano (compatible) device, but should work on many Arduino models as long as the firmware fit, it has enough RAM for the number of LED's used in the module and support the basic libraries.

Automated builds using [Travis-CI](https://travis-ci.com/) are made for the following platforms:

- Uno
- Leonardo
- Mega 2560
- Zero
- ESP8266

Since there are both hardware (current) and software (RAM) limitations to how many LED's/NeoPixel can be handled at once by the Arduino, it's important to first check the specifications on the Arduino model used and calculate the maximum number that can be used.


## Firmware

It's possible to adjust the number of LED's used by editing the `NUMPIXELS` variable. Keep in mind how much RAM the Arduino model used has available. With the NeoPixel library that this project uses, each LED needs 3 bytes of RAM. `NEOPIN` holds the pin number for where the NeoPixel module has been connected.

### Dependencies

This FW requires the Adafruit NeoPixel library. The current version is tested with version 1.7.0.

### Serial communication

The serial interface allows for commands and data to be sent in both directions. The message structure is the same for both incoming and outgoing messages but some commands are exclusive for the host or device. For example; only the host will send a set LED message.

#### Connection settings

- Baudrate: 38400
- Byte size: 8
- Parity: None
- Stopbits: 1
- Timeout: None
- XonXoff: 0
- RtsCts: 0

#### Message types

Messages are split into two types: `+` or `-`.

This is always the first character sent.

**+ messages**

`+` is an action message, For example where the host wants to change a state (perform an action) on the device.

The following actions are supported:

- `l`: set LED color
  Message example: `+l000111222#`
- `b`: set brightness
  Message example: `+b000#`
- `m`: set display mode (solid, blinking etc.)
  Message example: `+m000#`
- `d`: set dim mode (dimmed or not)
  Message example: `+d0#`

**- messages**

`-` is a request message. For example where the host wants information from the device.

The following requests are supported:

- `!`: full information request
  Message example: `-!#`

#### Status commands

Status commands are used by both host and device. They can be sent as a singular command or as a terminator to data sequence.

Both host and device can send ping command and then expects the other part to reply with a pong command. If no other command is sent after a minute, a ping is sent to verify a working connection. Until communication is restored the LED(s) will slowly cycle through the colors.

- `+!`: OK/Pong
- `-!`: Resend/Request data
- `+?`: Unknown/Error
- `-?`: Ping

#### Termination character

All commands and sequences are terminated with a `#`.

This is always the last character sent.

### Modes

Currently there are five user settable modes and one extra mode for when disconnected.
The `off` mode has the same effect as sending the color `black`, i.e. turn the LED off.
If there's no connection to the DSUL daemon (on the computer side), the LED will slowly cycle through all colors over and over.

- 0/`off`: Turns off LED
- 1/`solid`: Turns on LED and keeps consistently on
- 2/`blink`: Blinks the LED
- 3/`flash`: Blinks the LED faster (flashes)
- 4/`pulse`: Pulses the LED from min to max brightness

## Computer software

The software, daemon/server and client is available at [hymnis/dsul-python](https://github.com/hymnis/dsul-python) or [hymnis/dsul-go](https://github.com/hymnis/dsul-go).

## Acknowledgements

- Parts of the project (like `DsulTimer` and `DsulPatterns`) are inspired by an excellent article series of Bill Earl.
   Check out the articles at:
   - [https://learn.adafruit.com/multi-tasking-the-arduino-part-1](https://learn.adafruit.com/multi-tasking-the-arduino-part-1)
   - [https://learn.adafruit.com/multi-tasking-the-arduino-part-2](https://learn.adafruit.com/multi-tasking-the-arduino-part-2)
   - [https://learn.adafruit.com/multi-tasking-the-arduino-part-3](https://learn.adafruit.com/multi-tasking-the-arduino-part-3)
