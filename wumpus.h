#include <avr/pgmspace.h>
#include "SevSegShift.h"
#include "scale16.h"
#include "songs.h"

void introState(unsigned long);
void startState(unsigned long);
void playState(unsigned long);
void superbatState(unsigned long);
void pitfallState(unsigned long);
void pitfallDropState(unsigned long);
void disturbWumpusState(unsigned long);
void wumpusBumpState(unsigned long);
void wumpusMoveState(unsigned long);
void wumpusEatState(unsigned long);
void arrowStartState(unsigned long);
void arrowFireState(unsigned long);
void youLoseState(unsigned long);
void youWinState(unsigned long);

enum button{north, south, east, west, arrow};

const uint8_t introText[] PROGMEM =
  {0x76, 0x1C, 0x54, 0x70, 0x00, 0x70, 0x74, 0x79, 0x00, 0x3C, 0x1E, 0x1C, 0x33, 0x27, 0x73, 0x1C, 0x6D};

const uint16_t windNorthSouthMask[] PROGMEM =
  {0xFE7E, 0xDDDD, 0xBBBB, 0xEBEB, 0x77F7, 0xFFFF, 0xFFFF};
const uint16_t windEastWestMask[] PROGMEM =
  {0x4FFF, 0x36FF, 0xF9FF, 0x7F7F, 0xFF4F, 0xFF36, 0xFFF9};

const uint16_t wumpusBiteFrames[] PROGMEM = {0x3006, 0x794F, 0x0181, 0x23A3, 0x5454, 0x8808, 0x6773};
const uint16_t wumpusBumpFrames[] PROGMEM = {0x0630, 0x4670, 0x2805, 0x2004};
const uint16_t batFlapFrames[] PROGMEM = {0x2323, 0x4040, 0x0C18, 0x4040};
const uint16_t pitfallFrames[] PROGMEM = {0x0000, 0x4250, 0x390F, 0x0000, 0x4460, 0x390F};
const uint16_t arrowSelectFrames[] PROGMEM = {0x4600, 0x4260, 0x0070, 0x4450};
const uint8_t batNearbyFrames[] PROGMEM = {0x80, 0x80, 0x00, 0x00};

struct room {
  unsigned int wall : 1;
  unsigned int pit : 1;
  unsigned int superbat : 1;
  unsigned int wumpus : 1;
  unsigned int wumpusNearby : 1;
  unsigned int pitNearby : 1;
  unsigned int batsNearby : 1;
};

struct point {
  unsigned int x : 4;
  unsigned int y : 4;
};
