// 0th element of the durations is the length of the song

// Hall of the Mountain King by Edvard Grieg
const uint16_t hotmk[] PROGMEM =
  { A2, B2, C3, D3, E3, C3, E3, REST, Dx3, B2, Dx3, REST, D3, Ax2, D3, REST, A2, B2, C3, D3, E3, C3, E3, A3, G3, E3, C3, E3, G3, G3 };

const uint8_t hotmkDurations[] PROGMEM =
  { 30, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 250, 250 };

// Funeral March by Chopin
const uint16_t chopinBlock[] PROGMEM =
  { A0, A0, REST, A0, A0, REST, A0, REST, A0, A0, C1, C1, B0, REST, B0, B0, A0, REST, A0, A0, REST, A0, REST, A0, A0, A0 };

const uint8_t chopinBlockDurations[] PROGMEM =
  { 26, 250, 250, 20, 250, 250, 20, 100, 20, 250, 250, 250, 250, 100, 20, 250, 250, 200, 20, 250, 250, 20, 200, 20, 250, 250, 100 };

const uint16_t bonk[] PROGMEM = { 3000 };
const uint8_t bonkDurations[] PROGMEM = { 1, 200 };

const uint16_t batFlap[] PROGMEM = { B3, C4, G3 };
const uint8_t batFlapDurations[] PROGMEM = { 3, 50, 50, 50 };

const uint16_t batSet[] PROGMEM = { E3, A2 };
const uint8_t batSetDurations[] PROGMEM = { 2, 300, 100 };

const uint16_t snoreUp[] PROGMEM = { 1500, 3000, REPEAT };
const uint8_t snoreUpDurations[] PROGMEM = { 3, 10, 10, 0 };

const uint16_t snoreDown[] PROGMEM = { 3000, 4000, REPEAT };
const uint8_t snoreDownDurations[] PROGMEM = { 3, 10, 10, 0 };

const uint16_t splat[] PROGMEM = { 4000 };
const uint8_t splatDurations[] PROGMEM = { 1, 500 };
