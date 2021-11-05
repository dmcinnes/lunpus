#include <avr/pgmspace.h>
#include "SevSegShift.h"
#include "scale16.h"
#include "songs.h"

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

const uint16_t windNorthSouthMask[] PROGMEM =
  {0xFE7E, 0xDDDD, 0xBBBB, 0xEBEB, 0x77F7};
const uint16_t windEastWestMask[] PROGMEM =
  {0x4FFF, 0x36FF, 0xF9FF, 0x7F7F, 0xFF4F, 0xFF36, 0xFFF9};

const uint16_t wumpusBiteFrames[] PROGMEM = {0x0181, 0x23A3, 0x5454, 0x8808};

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

unsigned long nextNoteTime = 0;
uint8_t currentNote = 0;
uint16_t *currentSong = &hotmk[0];

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
  uint16_t *maskSegments;
  uint8_t totalMaskSegments, selection;
  if ((cave[playerX][playerY - 1].wall + cave[playerX][playerY + 1].wall) <
      (cave[playerX + 1][playerY].wall + cave[playerX - 1][playerY].wall)) {
    maskSegments = &windNorthSouthMask[0];
    totalMaskSegments = sizeof(windNorthSouthMask) / 2;
    selection = playerX % 2;
  } else {
    maskSegments = &windEastWestMask[0];
    totalMaskSegments = sizeof(windEastWestMask) / 2;
    selection = playerY % 2;
  }
  if (selection) {
    displayPitNearbyUp(timer, maskSegments, totalMaskSegments);
  } else {
    displayPitNearbyDown(timer, maskSegments, totalMaskSegments);
  }
}

void displayPitNearbyUp(unsigned long timer, uint16_t *maskSegments, uint8_t maskSegmentCount) {
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
  uint16_t word = pgm_read_word(maskSegments + windOffset);
  displaySegments[0] &= (uint8_t)word;
  displaySegments[1] &= (uint8_t)(word >> 8);
  sevsegshift.setSegments(displaySegments);
  nextAction = timer + 75;
  windOffset++;
}

void displayPitNearbyDown(unsigned long timer, uint16_t *maskSegments, uint8_t maskSegmentCount) {
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
  uint16_t word = pgm_read_word(maskSegments + windOffset);
  displaySegments[0] &= (uint8_t)word;
  displaySegments[1] &= (uint8_t)(word >> 8);
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

void displayWumpusBite(unsigned long timer) {
  static unsigned long nextAction = 0;
  static uint8_t wumpusFrameOffset = 0;
  if (nextAction < timer) {
    if (wumpusFrameOffset >= sizeof(wumpusBiteFrames) / 2) {
      wumpusFrameOffset = 0;
    }
    uint8_t displaySegments[2];
    uint16_t word = pgm_read_word(&wumpusBiteFrames[0] + wumpusFrameOffset);
    displaySegments[0] = (uint8_t)(word >> 8);
    displaySegments[1] = (uint8_t)word;
    sevsegshift.setSegments(displaySegments);
    wumpusFrameOffset++;
    nextAction = timer + 150;
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

void playNote(uint16_t note) {
  if (note == REST) {
    DDRA &= ~(1 << DDA6); // disable output pin
    return;
  }
  TCNT1 = 0;            // reset the counter
  OCR1A = note;         // set pitch
  DDRA |= (1 << DDA6);  // enable output pin
}

void updateAudio(unsigned long timer) {
  if (nextNoteTime == 0) {
    return;
  }
  if (timer < nextNoteTime) {
    return;
  }
  if (currentNote == pgm_read_word(currentSong)) {
    stopSong();
    return;
  }
  uint32_t dword = pgm_read_dword(currentSong + currentNote * 2 + 1);
  playNote(dword); // take the lower word for the note
  nextNoteTime = (unsigned long)(uint16_t(dword >> 16)) + timer;
  currentNote++;
}

void playSong(uint16_t newSong[]) {
  currentNote = 0;
  nextNoteTime = 1;
  currentSong = &newSong[0];
}

void stopSong() {
  nextNoteTime = 0;
  playNote(REST);
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

  playSong(hotmk);
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
      playSong(hotmk);
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

  updateAudio(timer);

  sevsegshift.refreshDisplay(); // Must run repeatedly
}
