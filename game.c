#include "stdio.h"
#include "stdbool.h"
#include "PS2.h"

/* GLOBAL VARIABLES */

int score = 0; // changed by game
bool gameover = false; // changed by game
extern int flagLeft; // changed by PS2INPUT and reset by game
extern int flagRight; // changed by PS2INPUT and reset by game


int main() {
    startGame();  // waits for start button to be pressed on the keyboard
    while(gameover == false) {
        updateGame();  // updates the game state
        drawGame();  // draws the game state
    }
}


void updateGame() {
    // update the game state
    PS_2INPUT();
    // check for game over
    // update score
}