// DSUL - Disturb State USB Light : Arduino Firmware
#include "dsul_patterns.h"
#include "dsul_timer.h"

// Version
#define MAJOR 0
#define MINOR 3
#define PATCH 1

// Fixed values
#define NEOPIN 3
#define NUMPIXELS 1
#define BRIGHT_MIN 10
#define BRIGHT_MAX 120

void dotComplete();
void heartbeatComplete();

uint32_t show_color = 0;       // default color, 0 = black/off
uint16_t show_brightness = 50; // default brightness (1-255)
uint8_t show_mode = 1;         // default mode, 1 = solid
bool show_dim = false;         // default dime state, false = no dimming
uint8_t input_count = 0;
bool input_state = LOW;
char input_string[13] = "";
bool wait_state = false;
bool color_reset = false;
uint16_t host_timeout = 60; // default timeout, 60 seconds

DsulPatterns Dot(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800, &dotComplete);
DsulTimer Heartbeat(1000 * host_timeout, &heartbeatComplete);

// Send OK command
void sendOk() {
  Serial.println(F("+!#"));
  wait_state = false;
  color_reset = true;
  Heartbeat.reset();
}

// Send NOK command
void sendNOK() { Serial.println(F("-!#")); }

// Send a ping command
void sendPing() { Serial.println(F("-?#")); }

// Reset input variables
void resetInput() {
  input_state = LOW;
  input_count = 0;
  memset(input_string, 0, sizeof input_string);
}

// Receive and store incomming serial data
bool receiveInput() {
  bool new_state = LOW;
  int input = Serial.read();

  if (input_state == HIGH) {
    if (input == 35) { // #
      // sequence complete
      new_state = HIGH;
    } else {
      // receiving data
      if (input_count > 13) {
        // received more chars than we should. abort
        sendNOK();
        resetInput();
      } else {
        input_string[input_count] = input;
        input_count += 1;
      }
    }
  } else if ((input == 43 || input == 45) && input_state == LOW) { // + or -
    // receiving command/request
    input_state = HIGH;
    input_string[input_count] = input;
    input_count += 1;
  }

  return new_state;
}

// Handle the input sequence
void handleInput() {
  char type = input_string[0];
  char key = input_string[1];

  if (type == '-') {  // - = 45
    if (key == '!') { // ! = 33
      // information requested
      // respond with version, leds, color, brightness and mode etc.
      char response_v[13];
      char response_ll[6];
      char response_lb[10];
      char response_cc[9];
      char response_cb[6];
      char response_cm[6];
      char response_cd[4];

      sprintf(response_v, "v%03d.%03d.%03d", MAJOR, MINOR, PATCH);
      Serial.print(response_v);
      sprintf(response_ll, "ll%03d", Dot.numPixels());
      Serial.print(response_ll);
      sprintf(response_lb, "lb%03d:%03d", BRIGHT_MIN, BRIGHT_MAX);
      Serial.print(response_lb);
      sprintf(response_cc, "cc%06d", show_color);
      Serial.print(response_cc);
      sprintf(response_cb, "cb%03d", show_brightness);
      Serial.print(response_cb);
      sprintf(response_cm, "cm%03d", show_mode);
      Serial.print(response_cm);
      sprintf(response_cd, "cd%01d", show_dim);
      Serial.print(response_cd);
      Serial.println("#");
    } else if (key == '?') { // ? = 63
      // ping received
      sendOk();
    } else {
      sendNOK(); // unknown key, not OK
    }
  } else if (type == '+') { // + = 43
    if (key == 'l') {
      // set LED color
      // +l rrrgggbbb
      uint16_t r, g, b = 0;
      char color_value[4] = "";

      color_value[0] = input_string[2];
      color_value[1] = input_string[3];
      color_value[2] = input_string[4];
      color_value[3] = '\0';
      r = atoi(color_value);
      r = constrain(r, 0, 255);
      color_value[0] = input_string[5];
      color_value[1] = input_string[6];
      color_value[2] = input_string[7];
      color_value[3] = '\0';
      g = atoi(color_value);
      g = constrain(g, 0, 255);
      color_value[0] = input_string[8];
      color_value[1] = input_string[9];
      color_value[2] = input_string[10];
      color_value[3] = '\0';
      b = atoi(color_value);
      b = constrain(b, 0, 255);

      show_color = Dot.Color(r, g, b);
      Dot.Color1 = show_color;
      sendOk();
    } else if (key == 'b') {
      // set led BRIGHTNESS
      // +b xxx
      char bright_value[4] = "";

      bright_value[0] = input_string[2];
      bright_value[1] = input_string[3];
      bright_value[2] = input_string[4];
      bright_value[3] = '\0';
      show_brightness = atoi(bright_value);
      show_brightness = constrain(show_brightness, BRIGHT_MIN, BRIGHT_MAX);

      Dot.setBrightness(show_brightness);
      sendOk();
    } else if (key == 'm') {
      // set show MODE
      // +m xxx
      char show_value[4] = "";
      show_value[0] = input_string[2];
      show_value[1] = input_string[3];
      show_value[2] = input_string[4];
      show_value[3] = '\0';
      show_mode = atoi(show_value);

      setShowMode();
      sendOk();
    } else if (key == 'd') {
      // set led color DIM
      // +d x
      char dim_value[2] = "";
      dim_value[0] = input_string[2];
      dim_value[1] = '\0';
      show_dim = atoi(dim_value);

      setDimMode();
      sendOk();
    } else {
      sendNOK(); // unknown key, not OK
    }
  } else {
    sendNOK(); // unknown type, not OK
  }

  resetInput();
}

// Set the current mode
void setShowMode() {
  if (show_mode == 0) { // off mode
    Dot.ActivePattern = NONE;
    Dot.Color1 = Dot.Color(0, 0, 0);
  } else if (show_mode == 1) { // solid mode
    Dot.Solid(show_color);
  } else if (show_mode == 2) { // blink mode
    Dot.Blink(show_color, 500);
  } else if (show_mode == 3) { // flash mode
    Dot.Blink(show_color, 250);
  } else if (show_mode == 4) { // pulse mode
    Dot.Pulse(show_color, 20, BRIGHT_MAX);
  }
}

// Set the current dim mode
void setDimMode() {
  if (show_dim) {
    Dot.Dim = true;
  } else {
    Dot.Dim = false;
  }
}

// Run once pattern is complete (if needed)
void dotComplete() {
  // reverse direction for pulse on completion
  if (Dot.ActivePattern == PULSE) {
    Dot.Reverse();
  }
}

// No heartbeat felt, send ping and wait for host to reply
void heartbeatComplete() {
  wait_state = true;
  sendPing();
}

// Prepare to start operations
void setup() {
  Serial.begin(38400);

  Dot.begin();
  Dot.setBrightness(50);
  Dot.Rainbow(45);
  show_color = Dot.Color(0, 0, 0); // set color to black/off as default
}

// Main loop
void loop() {
  bool newState = LOW;

  // update heartbeat timer
  Heartbeat.update();

  // update LEDs
  Dot.Update();

  if (Serial.available() > 0) {
    // get serial input if available
    newState = receiveInput();
  } else if (wait_state) {
    // wait for host connection
    // cycle LED colors while waiting
    Dot.ActivePattern = RAINBOW;
    Dot.Interval = 45;
    Dot.TotalSteps = 255;
  } else {
    if (color_reset == true) {
      // wait is over. continue from last mode and color
      color_reset = false;
      setShowMode();
      setDimMode();
    }
  }

  if (newState == HIGH) {
    // handle serial input
    handleInput();
  }
}
