# DSUL - Disturb State USB Light

[![Build Status](https://travis-ci.org/hymnis/dsul-arduino.svg?branch=master)](https://travis-ci.org/hymnis/dsul-arduino)

The goal of the project is to have a USB connected light, that can be be set to different colors, with adjustable brightness and different modes, which can communicate the users current preference regarding being disturbed.


## Hardware

The hardware used is an Arduino connected to a Neopixel module. The project was developed using an Arduino Nano, but should work on most models as long as the firmware fit and it has enough RAM for the number of LED's used in the module.

Since there are both hardware (current) and software (RAM) limitations to how many LED's/Neopixels can be handled at once by the Arduino, it's important to first check the specifications on the Arduino model used and calculate the maximum number that can be used.


## Firmware

It's possible to adjust the number of LED's used by editing the `NUMPIXELS` variable. Keep in mind how much RAM the Arduino model used has available. With the Neopixel library that this project uses, each LED needs 3 bytes of RAM.

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
- `m`: set display mode (solid or blinking)  
  Message example: `+m000#`

**- messages**

`-` is a request message. For example where the host wants information from the device.

The following requests are supported:

- `!`: full information request  
  Message example: `-!#`

#### Status commands

Status commands are used by both host and device. They can be sent as a singular command or as a terminator to data sequence.

Both host and device can send ping command and then expects the other part to reply with a pong command. If no other command is sent after a minute, a ping is sent to verify a working connection. Until communication is restored the LED(s) will slowly fade.

- `+!`: OK/Pong
- `-!`: Resend/Request data
- `+?`: Unknown/Error
- `-?`: Ping

#### Termination character

All commands and sequences are terminated with a `#`.

This always the last character sent.


## Computer software

The software, daemon/server and client is available at [hymnis/dsul-python](https://github.com/hymnis/dsul-python).
