#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "SevSegShift.h"
#include "scale16.h"
#include "songs.h"

void playNote(uint16_t);
void playSong(uint16_t);
void playWind(uint8_t);
void playSong(uint16_t[], uint8_t[]);
void stopSong();

void introState(unsigned long);
void startState(unsigned long);
void playState(unsigned long);
void superbatState(unsigned long);
void pitfallState(unsigned long);
void disturbWumpusState(unsigned long);
void wumpusBumpState(unsigned long);
void wumpusMoveState(unsigned long);
void wumpusEatState(unsigned long);
void wumpusEatAnimationState(unsigned long);
void arrowStartState(unsigned long);
void arrowFireState(unsigned long);
void youLoseState(unsigned long);
void youWinState(unsigned long);

enum button{north, south, east, west, arrow};

uint8_t introText[] EEMEM =
  {19, 0x00, 0x00, 0x76, 0x1C, 0x54, 0x70, 0x00, 0x70, 0x74, 0x79, 0x00, 0x38, 0x1C, 0x54, 0x73, 0x1C, 0x6D, 0x00, 0x00};

uint16_t windNorthSouthMask[] EEMEM =
  {0xFE7E, 0xDDDD, 0xBBBB, 0xEBEB, 0x77F7, 0xFFFF, 0xFFFF};
uint16_t windEastWestMask[] EEMEM =
  {0x4FFF, 0x36FF, 0xF9FF, 0x7F7F, 0xFF4F, 0xFF36, 0xFFF9};

uint8_t wumpusBiteFrames[] EEMEM = {15, 0x30, 0x06, 0x79, 0x4F, 0x01, 0x81, 0x23, 0xA3, 0x54, 0x54, 0x88, 0x08, 0x67, 0x73};
uint8_t wumpusBumpFrames[] EEMEM = {9, 0x06, 0x30, 0x46, 0x70, 0x28, 0x05, 0x20, 0x04};
uint8_t batFlapFrames[] EEMEM = {8, 0x23, 0x23, 0x40, 0x40, 0x0C, 0x18, 0x40, 0x40};
uint8_t pitfallFrames[] EEMEM = {12, 0x00, 0x00, 0x42, 0x50, 0x39, 0x0F, 0x00, 0x00, 0x44, 0x60, 0x39, 0x0F};

uint8_t arrowSelectDigits[] EEMEM = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D};
uint8_t arrowSelectFrames[] EEMEM = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};

uint8_t batNearbyFrames[] EEMEM = {0x80, 0x80, 0x00, 0x00};

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
