#include "SevSegShift.h"
#include "scale16.h"

#define SHIFT_PIN_SHCP 5
#define SHIFT_PIN_STCP 4
#define SHIFT_PIN_DS   3

enum button{north, south, east, west};

// north, south, east, west
const uint8_t buttonPins[4] = {9, 8, 7, 10};

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

const uint8_t maxBrightness = 90;

const uint8_t totalWindSegmentsNorthSouth = 5;
const uint8_t windNorthSouth[totalWindSegmentsNorthSouth * 2] =
  {0xFE, 0x7E, 0xDD, 0xDD, 0xBB, 0xBB, 0xEB, 0xEB, 0x77, 0xF7};
const uint8_t totalWindSegmentsWestEast = 7;
const uint8_t windWestEast[totalWindSegmentsWestEast * 2] =
  {0x4F, 0xFF, 0x36, 0xFF, 0xF9, 0xFF, 0x7F, 0x7F, 0xFF, 0x4F, 0xFF, 0x36, 0xFF, 0xF9};

struct room {
  unsigned int wall : 1;
  unsigned int pit : 1;
  unsigned int bats : 1;
  unsigned int wumpus : 1;
  unsigned int wumpusNearby : 1;
  unsigned int pitNearby : 1;
  unsigned int batsNearby : 1;
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

bool buttonState(button id) {
  // debounce state
  buttonStates[id] = (buttonStates[id] << 1) | digitalRead(buttonPins[id]) | 0xe000;
  return (buttonStates[id] == 0xf000);
}

void updateCaveDisplay() {
  sevsegshift.blank();
  uint8_t display[2] = {0, 0};
  if (cave[playerX][playerY - 1].wall) {
    display[0] |= northWall[0];
    display[1] |= northWall[1];
  }
  if (cave[playerX][playerY + 1].wall) {
    display[0] |= southWall[0];
    display[1] |= southWall[1];
  }
  if (cave[playerX + 1][playerY].wall) {
    display[0] |= eastWall[0];
    display[1] |= eastWall[1];
  }
  if (cave[playerX - 1][playerY].wall) {
    display[0] |= westWall[0];
    display[1] |= westWall[1];
  }

  sevsegshift.setSegments(display);
}

void displayThings() {
  if (cave[playerX][playerY].pit) {
    sevsegshift.setChars("P");
  }
  if (cave[playerX][playerY].bats) {
    sevsegshift.setChars("b");
  }
  if (cave[playerX][playerY].wumpus) {
    sevsegshift.setChars("uu");
  }
}

enum batStates{batStartNorth, batEndNorth, batStartSouth, batEndSouth, batClear, batReset};

void displayBatsNearby(unsigned long timer) {
  if (!cave[playerX][playerY].batsNearby) {
    return;
  }

  static unsigned long nextAction = 0;
  if (nextAction < timer) {
    static bool batPos;
    static batStates batState = batStartNorth;
    uint8_t segments[2];
    sevsegshift.getSegments(segments);

    switch(batState) {
      case batStartNorth :
        segments[0] |= 0x80;
        batState = batEndNorth;
        nextAction = timer + 50 + random(100);
        break;
      case batStartSouth :
        segments[1] |= 0x80;
        batState = batEndSouth;
        nextAction = timer + 50 + random(100);
        break;
      case batEndNorth :
        segments[0] ^= 0x80;
        segments[1] |= 0x80;
        batState = batClear;
        nextAction = timer + 100 + random(100);
        break;
      case batEndSouth :
        segments[0] |= 0x80;
        segments[1] ^= 0x80;
        batState = batClear;
        nextAction = timer + 100 + random(100);
        break;
      case batClear :
        segments[0] &= 0x7F;
        segments[1] &= 0x7F;
        nextAction = timer + 100;
        batState = batReset;
        break;
      case batReset :
        batState = (random(2)) ? batStartNorth : batStartSouth;
        nextAction = timer + 500 + random(500);
        updateCaveDisplay(); // reset cave view
        return;
    }
    sevsegshift.setSegments(segments);
  }
}

void displayPitNearby(unsigned long timer) {
  if (!cave[playerX][playerY].pitNearby) {
    return;
  }
  uint8_t *maskSegments;
  uint8_t totalMaskSegments, selection;
  if ((cave[playerX][playerY - 1].wall + cave[playerX][playerY + 1].wall) <
      (cave[playerX + 1][playerY].wall + cave[playerX - 1][playerY].wall)) {
    maskSegments = windNorthSouth;
    totalMaskSegments = totalWindSegmentsNorthSouth;
    selection = playerX % 2;
  } else {
    maskSegments = windWestEast;
    totalMaskSegments = totalWindSegmentsWestEast;
    selection = playerY % 2;
  }
  if (selection) {
    displayPitNearbyUp(timer, maskSegments, totalMaskSegments);
  } else {
    displayPitNearbyDown(timer, maskSegments, totalMaskSegments);
  }
}

void displayPitNearbyUp(unsigned long timer, uint8_t *maskSegments, uint8_t maskSegmentCount) {
  static unsigned long nextAction = 0;
  static uint8_t windOffset = 0;
  if (nextAction > timer) {
    return;
  }
  updateCaveDisplay();
  if (windOffset == maskSegmentCount) {
    windOffset = 0;
    nextAction = timer + 400 + random(600);
    return;
  }
  uint8_t displaySegments[2];
  sevsegshift.getSegments(displaySegments);
  displaySegments[0] &= maskSegments[windOffset * 2];
  displaySegments[1] &= maskSegments[windOffset * 2 + 1];
  sevsegshift.setSegments(displaySegments);
  nextAction = timer + 75;
  windOffset++;
}

void displayPitNearbyDown(unsigned long timer, uint8_t *maskSegments, uint8_t maskSegmentCount) {
  static unsigned long nextAction = 0;
  static uint8_t windOffset = 0;
  if (nextAction > timer) {
    return;
  }
  updateCaveDisplay();
  if (windOffset == 0xFF) { // wrapped around
    windOffset = maskSegmentCount - 1;
    nextAction = timer + 400 + random(600);
    return;
  }
  uint8_t displaySegments[2];
  sevsegshift.getSegments(displaySegments);
  displaySegments[0] &= maskSegments[windOffset * 2];
  displaySegments[1] &= maskSegments[windOffset * 2 + 1];
  sevsegshift.setSegments(displaySegments);
  nextAction = timer + 75;
  windOffset--;
}

void displayWumpusNearby(unsigned long timer) {
  if (!cave[playerX][playerY].wumpusNearby) {
    return;
  }

  static unsigned long nextAction = 0;
  if (nextAction < timer) {
    static int16_t currentBrightness;
    static bool direction = true;
    if (direction) {
      currentBrightness += 1;
      nextAction = timer + 10;
      if (currentBrightness >= maxBrightness) {
        direction = false;
        nextAction += 2000;
      }
    } else {
      currentBrightness -= 1;
      nextAction = timer + 10;
      if (currentBrightness <= 0) {
        direction = true;
        nextAction += 2000;
      }
    }
    sevsegshift.setBrightness(currentBrightness);
  }
}

void setupMap() {
  uint8_t i, j, x, y, count;

  for (i = 0; i < mapWidth; i++) {
    cave[i][0].wall = 1;
    cave[i][mapHeight - 1].wall = 1;
  }
  for (j = 0; j < mapHeight; j++) {
    cave[0][j].wall = 1;
    cave[mapWidth - 1][j].wall = 1;
  }

  count = minWalls + random(maxWalls - minWalls);

  for (i = 0; i < count; i++) {
    do {
      x = random(mapWidth);
      y = random(mapHeight);
    } while (cave[x][y].wall);
    cave[x][y].wall = 1;
  }

  count = minPits + random(maxPits - minPits);

  for (i = 0; i < count; i++) {
    do {
      x = random(mapWidth);
      y = random(mapHeight);
    } while (cave[x][y].wall);
    cave[x][y].pit = 1;
    cave[x - 1][y - 1].pitNearby = 1;
    cave[x - 1][y    ].pitNearby = 1;
    cave[x - 1][y + 1].pitNearby = 1;
    cave[x    ][y - 1].pitNearby = 1;
    cave[x    ][y + 1].pitNearby = 1;
    cave[x + 1][y - 1].pitNearby = 1;
    cave[x + 1][y    ].pitNearby = 1;
    cave[x + 1][y + 1].pitNearby = 1;
  }

  count = minBats + random(maxBats - minBats);

  for (i = 0; i < count; i++) {
    do {
      x = random(mapWidth);
      y = random(mapHeight);
    } while (cave[x][y].wall);
    cave[x][y].bats = 1;
    cave[x - 1][y - 1].batsNearby = 1;
    cave[x - 1][y    ].batsNearby = 1;
    cave[x - 1][y + 1].batsNearby = 1;
    cave[x    ][y - 1].batsNearby = 1;
    cave[x    ][y + 1].batsNearby = 1;
    cave[x + 1][y - 1].batsNearby = 1;
    cave[x + 1][y    ].batsNearby = 1;
    cave[x + 1][y + 1].batsNearby = 1;
  }

  do {
    x = random(mapWidth);
    y = random(mapHeight);
  } while (cave[x][y].wall);
  cave[x][y].wumpus = 1;
  cave[x - 1][y - 1].wumpusNearby = 1;
  cave[x - 1][y    ].wumpusNearby = 1;
  cave[x - 1][y + 1].wumpusNearby = 1;
  cave[x    ][y - 1].wumpusNearby = 1;
  cave[x    ][y + 1].wumpusNearby = 1;
  cave[x + 1][y - 1].wumpusNearby = 1;
  cave[x + 1][y    ].wumpusNearby = 1;
  cave[x + 1][y + 1].wumpusNearby = 1;
}

void setupPlayer() {
  playerX = 1; // + random(mapWidth - 2);
  playerY = 1; // + random(mapHeight - 2);
}

void setDefaultBrightness() {
  sevsegshift.setBrightness(maxBrightness);
}

void initSpeaker() {
  // reset the registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);  // CTC: Clear Timer on Compare with OCR1A
  TCCR1A |= (1 << COM1A0); // Toggle OC1A on Compare Match
  TCCR1B |= (1 << CS11);   // CPU clock / 8
}

static inline void playNote(uint16_t period, uint16_t duration) {
  TCNT1 = 0;                                      /* reset the counter */
  OCR1A = period;                                     /* set pitch */
  DDRA |= (1 << DDA6);
  delay(duration);
  DDRA &= ~(1 << DDA6);
}

void setup() {
  randomSeed(analogRead(1));

  initSpeaker();

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

  setDefaultBrightness();

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  setupMap();
  setupPlayer();

  updateCaveDisplay();

  playNote(A2, 200);
  playNote(B2, 200);
  playNote(C3, 200);
  playNote(D3, 200);
  playNote(E3, 200);
  playNote(C3, 200);
  playNote(E3, 200);
  delay(200);
  playNote(Dx3, 200);
  playNote(B2, 200);
  playNote(Dx3, 200);
  delay(200);
  playNote(D3, 200);
  playNote(Ax2, 200);
  playNote(D3, 200);
  delay(200);
  playNote(A2, 200);
  playNote(B2, 200);
  playNote(C3, 200);
  playNote(D3, 200);
  playNote(E3, 200);
  playNote(C3, 200);
  playNote(E3, 200);
  playNote(A3, 200);
  playNote(G3, 200);
  playNote(E3, 200);
  playNote(C3, 200);
  playNote(E3, 200);
  playNote(G3, 500);
}

void loop() {
  static unsigned long timer;
  timer = millis();

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
    if (cave[nextPlayerX][nextPlayerY].wall) {
      // wall beep
      playNote(252, 200);
    } else {
      playerX = nextPlayerX;
      playerY = nextPlayerY;
      setDefaultBrightness();
      updateCaveDisplay();
      displayThings();
    }
  }

  displayBatsNearby(timer);
  displayPitNearby(timer);
  displayWumpusNearby(timer);

  sevsegshift.refreshDisplay(); // Must run repeatedly
}
