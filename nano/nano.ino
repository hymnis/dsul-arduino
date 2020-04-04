// DSUL - Disturb State USB Light : Arduino Nano
#include <Adafruit_NeoPixel.h>

#define MAJOR       0
#define MINOR       1
#define PATCH       0

#define NEOPIN      3
#define NUMPIXELS   1  // NUMPIXELS * 3 = bytes of RAM used
#define BRIGHT_MIN  10
#define BRIGHT_MAX  120

Adafruit_NeoPixel *leds;
int NEOFORMAT = NEO_GRB + NEO_KHZ800;
uint16_t show_brightness = 50;  // default brightness (1-255)
uint16_t r, g, b = 0;  // default color, 0 = black/off
uint8_t show_mode = 1; // default mode, 1 = solid
uint8_t input_count = 0;
bool input_state = LOW;
char input_string[10] = "";
int counter = 0;


// perform actual LED state update
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<leds->numPixels(); i++) {
    leds->setPixelColor(i, c);
  }
  leds->show();
}

// handle the input sequence
void handleInput() {
  char type = input_string[0];
  char key = input_string[1];

  if (type == '-') {  // - = 45
    if (key == '!') {  // ! = 33
      // information requested
      // respond with color, brightness, mode and version
      char response_c[10];
      char response_b[4];
      char response_m[4];
      char response_v[10];

      sprintf(response_c, "c%03d%03d%03d", r, g, b);
      Serial.print(response_c);
      sprintf(response_b, "b%03d", show_brightness);
      Serial.print(response_b);
      sprintf(response_m, "m%03d", show_mode);
      Serial.print(response_m);
      sprintf(response_v, "v%03d.%03d.%03d", MAJOR, MINOR, PATCH);
      Serial.print(response_v);
      Serial.print("+!#");
    } else if (key == '?') {  // ? = 63
      // ping received
      Serial.print("+!#");
    }
  } else if (type == '+') {  // + = 43
    if (key == 'l') {
      // set LED color
      // l rrrgggbbb
      char color_value[3] = "";
      color_value[0] = input_string[2];
      color_value[1] = input_string[3];
      color_value[2] = input_string[4];
      r = atoi(color_value);
      color_value[0] = input_string[5];
      color_value[1] = input_string[6];
      color_value[2] = input_string[7];
      g = atoi(color_value);
      color_value[0] = input_string[8];
      color_value[1] = input_string[9];
      color_value[2] = input_string[10];
      b = atoi(color_value);
  
      colorWipe(leds->Color(r, g, b));
      Serial.print("+!#");
    } else if (key == 'b') {
      // set led BRIGHTNESS
      // b xxx
      char bright_value[3] = "";
      bright_value[0] = input_string[2];
      bright_value[1] = input_string[3];
      bright_value[2] = input_string[4];
      show_brightness = atoi(bright_value);
  
      if (show_brightness < BRIGHT_MIN)
        show_brightness = BRIGHT_MIN;
      if (show_brightness > BRIGHT_MAX)
        show_brightness = BRIGHT_MAX;
  
      leds->setBrightness(show_brightness);
      leds->show();
      Serial.print("+!#");
    } else if (key == 'm') {
      // set show MODE
      // m xxx
      // set the mode here...
      Serial.print("+!#");
    }
  }

  resetInput();
}

// reset input variables
void resetInput() {
  input_state = LOW;
  input_count = 0;
  memset(input_string, 0, sizeof input_string);
}

void setup() {
  Serial.begin(9600);
  leds = new Adafruit_NeoPixel(NUMPIXELS, NEOPIN, NEOFORMAT);
  leds->begin();
  leds->clear();
  leds->setBrightness(show_brightness);
  colorWipe(leds->Color(r, g, b));
}

void loop() {
  bool newState = LOW;
  counter += 1;

  if (Serial.available() > 0) {
    int input = Serial.read();

    // DEBUG
    // Serial.print("> ");
    // Serial.println(input, DEC);

    if (input_state == HIGH) {
      if (input == 35) {  // #
        // sequence complete
        newState = HIGH;
      } else {
        // receiving data
        if (input_count > 10) {
          input_state = LOW;
          Serial.println("-!#");
          resetInput(); 
        } else {
          input_string[input_count] = input;
          input_count += 1;
        }
      }
    } else if ((input == 43 || input == 45) && input_state == LOW) {  // +
      // receiving command/request
      input_state = HIGH;
      input_string[input_count] = input;
      input_count += 1;
    }
  }

  // handle input
  if (newState == HIGH) {
    handleInput();
  }
}
