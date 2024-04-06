#ifndef ps2_h
#define ps2_h
#include "stdbool.h"

extern bool gameover;  // changed by game
extern int flagLeft;          // changed by PS2INPUT and reset by game
extern int flagRight;         // changed by PS2INPUT and reset by game

extern volatile int *PS2_ptr;
extern int PS2_data, RVALID;
extern char byte1, byte2, byte3;

void HEX_PS2(char b1, char b2, char b3);

void PS_2INPUT();

void flagRaiseCheck(char b1, char b2, char b3);

void LED_PS2(int flagLeft, int flagRight);

#endif