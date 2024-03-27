#ifndef ps2_h
#define ps2_h

extern bool gameover = true;  // changed by game
extern int flagLeft;          // changed by PS2INPUT and reset by game
extern int flagRight;         // changed by PS2INPUT and reset by game

void HEX_PS2(char b1, char b2, char b3);

void PS_2INPUT();

void flagRaiseCheck(char b1, char b2, char b3);

void LED_PS2(int flagLeft, int flagRight);

#endif