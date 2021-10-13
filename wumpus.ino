#include "SevSegShift.h"

#define SHIFT_PIN_SHCP 5
#define SHIFT_PIN_STCP 4
#define SHIFT_PIN_DS   3

enum button{north, south, east, west};

// north, south, east, west
const uint8_t buttonPins[4] = {9, 8, 6, 10};

const uint8_t northWall[2] = {0x01, 0x81};
const uint8_t southWall[2] = {0x88, 0x08};
const uint8_t eastWall[2]  = {0x00, 0x06};
const uint8_t westWall[2]  = {0x30, 0x00};

uint16_t buttonStates[4] = {};

const uint8_t mapWidth = 12;
const uint8_t mapHeight = 12;

uint8_t cave[mapWidth][mapHeight];

const uint8_t wall        = 1 << 0;
const uint8_t pit         = 1 << 1;
const uint8_t bats        = 1 << 2;
const uint8_t wumpus      = 1 << 3;
const uint8_t pitWind     = 1 << 4;
const uint8_t batsNearby  = 1 << 5;
const uint8_t wumpusSmell = 1 << 6;

uint8_t playerX, playerY;

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

bool buttonState(button id) {
  // debounce state
  buttonStates[id] = (buttonStates[id] << 1) | digitalRead(buttonPins[id]) | 0xe000;
  return (buttonStates[id] == 0xf000);
}

void updateCaveDisplay() {
  uint8_t display[2] = {0, 0};
  if (cave[playerX - 1][playerY] & wall) {
    display[0] |= northWall[0];
    display[1] |= northWall[1];
  }
  if (cave[playerX + 1][playerY] & wall) {
    display[0] |= southWall[0];
    display[1] |= southWall[1];
  }
  if (cave[playerX][playerY + 1] & wall) {
    display[0] |= eastWall[0];
    display[1] |= eastWall[1];
  }
  if (cave[playerX][playerY - 1] & wall) {
    display[0] |= westWall[0];
    display[1] |= westWall[1];
  }
  sevsegshift.setSegments(display);
}

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

  uint8_t i, j;

  for (i = 0; i < mapWidth; i++) {
    for (j = 0; j < mapHeight; j++) {
      cave[i][j] = 0;
    }
    cave[i][0] = wall;
    cave[i][mapHeight - 1] = wall;
  }
  for (j = 0; j < mapHeight; j++) {
    cave[0][j] = wall;
    cave[mapHeight - 1][j] = wall;
  }

  playerX = 1;
  playerY = 1;

  for (i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(7, OUTPUT);

  /*
  for (int i = 0; i < 10; i++) {
    tone(7, 31);
    delay(100);
    tone(7, 40);
    delay(100);
  }
  for (int i = 0; i < 10; i++) {
    tone(7, 50);
    delay(50);
    tone(7, 48);
    delay(100);
  }
  for (int i = 0; i < 10; i++) {
    tone(7, 31);
    delay(100);
    tone(7, 40);
    delay(100);
  }
  */

  /* tone(7, 174); */
  /* delay(300); */
  /* tone(7, 208); */
  /* delay(300); */
  /* tone(7, 220); */
  /* delay(300); */
  /* tone(7, 247); */
  /* delay(300); */
  /* tone(7, 277); */
  /* delay(300); */
  /* tone(7, 220); */
  /* delay(300); */
  /* tone(7, 277); */
  /* delay(700); */
  /*  */
  /* tone(7, 261); */
  /* delay(300); */
  /* tone(7, 208); */
  /* delay(300); */
  /* tone(7, 261); */
  /* delay(700); */
  /*  */
  /* tone(7, 247); */
  /* delay(300); */
  /* tone(7, 196); */
  /* delay(300); */
  /* tone(7, 247); */
  /* delay(700); */
  /* noTone(); */

  updateCaveDisplay();

  tone(7, 247);
  delay(300);
  noTone();
}

void loop() {
  /* static unsigned long timer = millis(); */

  uint8_t nextPlayerX = playerX;
  uint8_t nextPlayerY = playerY;

  if (buttonState(north)) {
    nextPlayerY--;
  }
  if (buttonState(south)) {
    nextPlayerY++;
  }
  if (buttonState(east)) {
    nextPlayerX++;
  }
  if (buttonState(west)) {
    nextPlayerX--;
  }

  if (nextPlayerX != playerX || nextPlayerY != playerY) {
    if (cave[nextPlayerX][nextPlayerY] & wall) {
      // wall beep
      tone(7, 230);
      delay(100);
      noTone();
    } else {
      playerX = nextPlayerX;
      playerY = nextPlayerY;
      updateCaveDisplay();
    }
  }

  sevsegshift.refreshDisplay(); // Must run repeatedly
}
