#include "SevSegShift.h"

#define SHIFT_PIN_SHCP 5
#define SHIFT_PIN_STCP 4
#define SHIFT_PIN_DS   3

const uint8_t northWall[2] = {0x01, 0x81};
const uint8_t southWall[2] = {0x88, 0x08};
const uint8_t eastWall[2]  = {0x00, 0x06};
const uint8_t westWall[2]  = {0x30, 0x00};

SevSegShift sevsegshift(
                  SHIFT_PIN_DS,
                  SHIFT_PIN_SHCP,
                  SHIFT_PIN_STCP,
                  1, /* number of shift registers there is only 1 Shiftregister
                        used for all Segments (digits are on Controller)
                        default value = 2 (see SevSegShift example)
                        */
                  true /* Digits are connected to Arduino directly
                          default value = false (see SevSegShift example)
                        */
                );

void setup() {
  byte numDigits = 2;
  byte digitPins[] = {1, 2}; // These are the PINS of the ** Arduino **
  byte segmentPins[] = {6, 7, 3, 2, 1, 5, 0, 4}; // these are the PINs of the ** Shift register **
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected

  sevsegshift.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevsegshift.setBrightness(90);

  tone(7, 440);
  delay(150);
  tone(7, 494);
  delay(150);
  tone(7, 523);
  delay(300);
  noTone();
}

void loop() {
  static unsigned long timer = millis();
  static int deciSeconds = 0;
  static int wall = 0;

  if (millis() - timer >= 100) {
    timer += 100;
    deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond

    if (deciSeconds % 10 == 0) {
      wall++;
      switch(wall) {
        case 1:
          sevsegshift.setSegments(northWall);
          break;
        case 2:
          sevsegshift.setSegments(eastWall);
          break;
        case 3:
          sevsegshift.setSegments(southWall);
          break;
        case 4:
          sevsegshift.setSegments(westWall);
          wall = 0;
      }
    }

    if (deciSeconds == 100) { // Reset to 0 after counting for 1000 seconds.
      deciSeconds=0;
    }
    /* sevsegshift.setNumber(deciSeconds, 1); */
  }

  sevsegshift.refreshDisplay(); // Must run repeatedly
}
