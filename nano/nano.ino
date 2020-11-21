// DSUL - Disturb State USB Light : Arduino Nano
#include <Adafruit_NeoPixel.h>

#define MAJOR       0
#define MINOR       1
#define PATCH       2

#define NEOPIN      3
#define NUMPIXELS   1  // NUMPIXELS * 3 = bytes of RAM used
#define BRIGHT_MIN  10
#define BRIGHT_MAX  120

Adafruit_NeoPixel *leds;
int NEOFORMAT = NEO_GRB + NEO_KHZ800;
uint16_t show_brightness = 50;  // default brightness (1-255)
uint16_t i = 0;
uint16_t r = 0;
uint16_t g = 64;
uint16_t b = 0;
uint32_t current_color[NUMPIXELS + 1];
uint8_t show_mode = 1; // default mode, 1 = solid
uint8_t input_count = 0;
bool input_state = LOW;
char input_string[13] = "";
uint8_t current_time = 0;
bool wait_state = false;
bool color_reset = false;
uint16_t host_timeout = 60; // default timeout, 60 seconds


// set color for given LED
void setLed(byte index, uint32_t color) {
  index = index % leds->numPixels();
  leds->setPixelColor(index, color);
}

// perform actual LED state update for specific LED
void colorWipe(int index, uint32_t color) {
  if (index == 0) {
    // set color for all LEDs
    for (uint16_t i = 0; i < leds->numPixels(); i++) {
      if (i > 0) { current_color[i] = 0; } // clear individual values
      setLed(i, color);
    }
  } else {
    // set color for one LED
    setLed(index - 1, color);
  }
  leds->show();
}

// fade through the colors
void colorFade() {
  if (r == 64) {
    r = 63;
    g = 1;
    b = 0;
  } else if (r > 0 && g < 64 && b == 0) {
    r--;
    g++;
  } else if (g == 64) {
    g = 63;
    b = 1;
    r = 0;
  } else if (g > 0 && b < 64 && r == 0) {
    g--;
    b++;
  } else if (b == 64) {
    b = 63;
    r = 1;
    g = 0;
  } else if (b > 0 && r < 64 && g == 0) {
    b--;
    r++;
  }
  colorWipe(0, leds->Color(r, g, b));

  delay(70);
}

// send OK command
void sendOk() {
  Serial.println("+!#");
  wait_state = false;
  color_reset = true;
  current_time = 0;
}

// send NOK command
void sendNOK() {
  Serial.println("-!#");
}

// send a ping command
void sendPing() {
  Serial.println("-?#");
}

// reset input variables
void resetInput() {
  input_state = LOW;
  input_count = 0;
  memset(input_string, 0, sizeof input_string);
}

// receive and store incomming serial data
bool receiveInput() {
  bool new_state = LOW;
  int input = Serial.read();

  if (input_state == HIGH) {
    if (input == 35) {  // #
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
  } else if ((input == 43 || input == 45) && input_state == LOW) {  // + or -
    // receiving command/request
    input_state = HIGH;
    input_string[input_count] = input;
    input_count += 1;
  }

  return new_state;
}

// handle the input sequence
void handleInput() {
  char type = input_string[0];
  char key = input_string[1];

  if (type == '-') {  // - = 45
    if (key == '!') {  // ! = 33
      // information requested
      // respond with version, leds, color, brightness and mode
      char response_v[12];
      char response_ll[5];
      char response_lb[9];
      char response_cc[7];
      char response_cb[5];
      char response_cm[5];

      sprintf(response_v, "v%03d.%03d.%03d", MAJOR, MINOR, PATCH);
      Serial.print(response_v);
      sprintf(response_ll, "ll%03d", leds->numPixels());
      Serial.print(response_ll);
      sprintf(response_lb, "lb%03d:%03d", BRIGHT_MIN, BRIGHT_MAX);
      Serial.print(response_lb);
      for (int n = 0; n <= NUMPIXELS; n++) {
        sprintf(response_cc, "cc%03d:%s", n, current_color[n]);
        Serial.print(response_cc);
      }
      sprintf(response_cb, "cb%03d", show_brightness);
      Serial.print(response_cb);
      sprintf(response_cm, "cm%03d", show_mode);
      Serial.print(response_cm);
      Serial.print("#");
    } else if (key == '?') {  // ? = 63
      // ping received
      sendOk();
    } else {
      sendNOK(); // unknown key, not OK
    }
  } else if (type == '+') {  // + = 43
    if (key == 'l') {
      // set LED color
      // +l iii rrrgggbbb
      char index_value[4] = "";
      char color_value[4] = "";
      index_value[0] = input_string[2];
      index_value[1] = input_string[3];
      index_value[2] = input_string[4];
      index_value[3] = '\0';
      i = atoi(index_value);
      i = constrain(i, 0, NUMPIXELS);
      color_value[0] = input_string[5];
      color_value[1] = input_string[6];
      color_value[2] = input_string[7];
      color_value[3] = '\0';
      r = atoi(color_value);
      r = constrain(r, 0, 255);
      color_value[0] = input_string[8];
      color_value[1] = input_string[9];
      color_value[2] = input_string[10];
      color_value[3] = '\0';
      g = atoi(color_value);
      g = constrain(g, 0, 255);
      color_value[0] = input_string[11];
      color_value[1] = input_string[12];
      color_value[2] = input_string[13];
      color_value[3] = '\0';
      b = atoi(color_value);
      b = constrain(b, 0, 255);

      current_color[i] = leds->Color(r, g, b);
      colorWipe(i, current_color[i]);
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

      leds->setBrightness(show_brightness);
      leds->show();
      sendOk();
    } else if (key == 'm') {
      // set show MODE
      // +m xxx
      uint8_t old_value = show_mode;
      char show_value[4] = "";
      show_value[0] = input_string[2];
      show_value[1] = input_string[3];
      show_value[2] = input_string[4];
      show_value[3] = '\0';
      show_mode = atoi(show_value);

      if(show_mode > 3 || show_mode < 1) {
        show_mode = old_value;
      }

      leds->setBrightness(show_brightness);
      leds->show();
      sendOk();
    } else {
      sendNOK(); // unknown key, not OK
    }
  } else {
    sendNOK(); // unknown type, not OK
  }

  resetInput();
}

// set up serial communication and LED(s)
void setup() {
  Serial.begin(38400);
  leds = new Adafruit_NeoPixel(NUMPIXELS, NEOPIN, NEOFORMAT);
  leds->begin();
  leds->clear();
  leds->setBrightness(show_brightness);

  current_color[0] = leds->Color(0, 0, 0); // default color, 000 = black (off)
  colorWipe(0, current_color[0]);
  wait_state = true;
}

// run the main loop (communication and state changes)
void loop() {
  bool newState = LOW;

  if (Serial.available() > 0) {
    // get serial input if available
    newState = receiveInput();
  } else if (wait_state) {
    // wait for host connection
    // fade LEDs while waiting
    colorFade();
  } else {
    // set LEDs to proper state
    uint16_t final_delay = 1000;

    if (color_reset == true) {
      color_reset = false;
      colorWipe(0, current_color[0]);
    }

    if (show_mode == 2) {
      // blink
      if ((current_time % 2) == 0) {
        leds->setBrightness(show_brightness);
      } else {
        leds->setBrightness(1);
      }
    } else if (show_mode == 3) {
      // flash
      leds->setBrightness(1);
      leds->show();
      delay(250);
      leds->setBrightness(show_brightness);
      leds->show();
      delay(250);
      leds->setBrightness(1);
      leds->show();
      delay(250);
      leds->setBrightness(show_brightness);
      final_delay = 250;
    }
    leds->show();

    current_time++;

    if (current_time > host_timeout) {
      // it's past timeout! prepare for fade and send ping to host
      current_time = 0;
      wait_state = true;
      show_mode = 1;
      leds->setBrightness(50);
      r = 0;
      g = 64;
      b = 0;
      sendPing();
    }

    delay(final_delay);
  }

  if (newState == HIGH) {
    // handle serial input if any
    handleInput();
  }
}
