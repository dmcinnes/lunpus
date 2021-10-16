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

const uint8_t minWalls = 6;
const uint8_t maxWalls = 20;
const uint8_t minPits = 2;
const uint8_t maxPits = 6;
const uint8_t minBats = 2;
const uint8_t maxBats = 6;

/**
 * Cave map
 *
 * One nybble per room
 * pit, bats, wumpus, wumpusSnore for each bit
 * pit, bats and wumpus can be in the same room, but wumpusSnore should not be
 * in the same room as the wumpus -- those two bits together will be a wall.
 * 0 == empty
 * xx11 == wall
 */

struct room {
  unsigned int pit : 1;
  unsigned int bats : 1;
  unsigned int wumpus : 1;
  unsigned int snore : 1;
  unsigned int wall : 1;
};

struct room cave[mapWidth][mapHeight];

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

bool isWall(room rm) {
  return (rm.wumpus && rm.snore);
}

bool buttonState(button id) {
  // debounce state
  buttonStates[id] = (buttonStates[id] << 1) | digitalRead(buttonPins[id]) | 0xe000;
  return (buttonStates[id] == 0xf000);
}

void updateCaveDisplay() {
  uint8_t display[2] = {0, 0};
  if (isWall(cave[playerX][playerY - 1])) {
    display[0] |= northWall[0];
    display[1] |= northWall[1];
  }
  if (isWall(cave[playerX][playerY + 1])) {
    display[0] |= southWall[0];
    display[1] |= southWall[1];
  }
  if (isWall(cave[playerX + 1][playerY])) {
    display[0] |= eastWall[0];
    display[1] |= eastWall[1];
  }
  if (isWall(cave[playerX - 1][playerY])) {
    display[0] |= westWall[0];
    display[1] |= westWall[1];
  }

  sevsegshift.setSegments(display);
}

void displayThings() {
  /* if (cave[playerX][playerY] & pit) { */
  /*   sevsegshift.setChars("P"); */
  /* } */
  /* if (cave[playerX][playerY] & bats) { */
  /*   sevsegshift.setChars("b"); */
  /* } */
  /* if (cave[playerX][playerY] & wumpus) { */
  /*   sevsegshift.setChars("uu"); */
  /* } */
}

void setupMap() {
  uint8_t i, j, x, y, count;

  for (i = 0; i < mapWidth; i++) {
    cave[i][0].wumpus = 1;
    cave[i][0].snore = 1;
    cave[i][mapHeight - 1].wumpus = 1;
    cave[i][mapHeight - 1].snore = 1;
  }
  for (j = 0; j < mapHeight; j++) {
    cave[0][j].wumpus = 1;
    cave[0][j].snore = 1;
    cave[mapWidth - 1][j].wumpus = 1;
    cave[mapWidth - 1][j].snore = 1;
  }

  count = minWalls + random(maxWalls - minWalls);

  for (i = 0; i < count; i++) {
    do {
      x = random(mapWidth);
      y = random(mapHeight);
    } while (isWall(cave[x][y]));
    cave[x][y].wumpus = 1;
    cave[x][y].snore = 1;
  }

  count = minPits + random(maxPits - minPits);

  for (i = 0; i < count; i++) {
    do {
      x = random(mapWidth);
      y = random(mapHeight);
    } while (isWall(cave[x][y]));
    cave[x][y].pit = 1;
  }

  count = minBats + random(maxBats - minBats);

  for (i = 0; i < count; i++) {
    do {
      x = random(mapWidth);
      y = random(mapHeight);
    } while (isWall(cave[x][y]));
    cave[x][y].bats = 1;
  }

  do {
    x = random(mapWidth);
    y = random(mapHeight);
  } while (isWall(cave[x][y]));
  cave[x][y].wumpus = 1;
}

void setupPlayer() {
  playerX = 1; // + random(mapWidth - 2);
  playerY = 1; // + random(mapHeight - 2);
}

void setup() {
  randomSeed(analogRead(9));

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

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(7, OUTPUT);

  setupMap();
  setupPlayer();

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
    if (isWall(cave[nextPlayerX][nextPlayerY])) {
      // wall beep
      tone(7, 230);
      delay(100);
      noTone();
    } else {
      playerX = nextPlayerX;
      playerY = nextPlayerY;
      updateCaveDisplay();
      displayThings();
    }
  }

  sevsegshift.refreshDisplay(); // Must run repeatedly
}
