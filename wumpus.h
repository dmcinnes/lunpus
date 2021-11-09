#include <avr/pgmspace.h>
#include "SevSegShift.h"
#include "scale16.h"
#include "songs.h"

void introState(unsigned long);
void startState(unsigned long);
void playState(unsigned long);
void superbatState(unsigned long);
void pitfallState(unsigned long);
void wumpusEatState(unsigned long);
void arrowStartState(unsigned long);
void arrowFireState(unsigned long);
void youLoseState(unsigned long);
void youWinState(unsigned long);

enum button{north, south, east, west};

const uint16_t windNorthSouthMask[] PROGMEM =
  {0xFE7E, 0xDDDD, 0xBBBB, 0xEBEB, 0x77F7};
const uint16_t windEastWestMask[] PROGMEM =
  {0x4FFF, 0x36FF, 0xF9FF, 0x7F7F, 0xFF4F, 0xFF36, 0xFFF9};

const uint16_t wumpusBiteFrames[] PROGMEM = {0x0181, 0x23A3, 0x5454, 0x8808};
const uint16_t batFlapFrames[] PROGMEM = {0x2323, 0x4040, 0x0C18, 0x4040};

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

const uint16_t titleFrames[] PROGMEM =
  {0x0006, 0x0076, 0x0634, 0x761C};
