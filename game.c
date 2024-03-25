#include "PS2.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <time.h>

/* GLOBAL VARIABLES */

int score = 0;                // changed by game
extern bool gameover = true;  // changed by game
extern int flagLeft;          // changed by PS2INPUT and reset by game
extern int flagRight;         // changed by PS2INPUT and reset by game
int difficulty = 3;
int winScore;
int turnTracker = 0;  // 0 is Player, 1 is CPU
int defaultTime;
int speedupMultiplier = 0.95;

int currPos = 2;
int nextPos = 1;


int main() {
    while (gameover == true) {
        startGame();  // waits for start button to be pressed on the keyboard
        difficultySelect();
    }
    while (gameover == false) {
        updateGame();  // updates the game state
        drawGame();    // draws the game state
    }
}

void updateGame() {  // this should be where most of the game logic happens
    // update the game state

    if (turnTracker == 0) {
        // user turn
        int hitTimer = defaultTime * speedupMultiplier;  // in ms
        while (hitTimer != 0) {
            PS_2INPUT();
            if (flagLeft || flagRight) {
                playerHitter();
            }
            sleep(0.001);
            hitTimer = hitTimer - 1;
        }
    } else if (turnTracker == 1) {
        // CPU turn 
        if(winScore != score){
            nextPos = CPUhitter();
        } else if (winScore == score){
            handleGameover();
        }
    }

    // update score
    score = score++;
}

void startGame() {
    PS_2STARTGAME();  // waits for start button to be pressed on the keyboard
    resetGame();
}

void resetGame() {
    score = 0;
    flagLeft = 0;
    flagRight = 0;
    gameover = false;
    difficulty = 0;
}

void difficultySelect() {
    srand(time(NULL));  // Seed the random number generator with current time

    int lowerBound, upperBound;

    if (difficulty == 1) {
        lowerBound = 10;
        upperBound = 15;
        defaultTime = 2000;
    } else if (difficulty == 2) {
        lowerBound = 15;
        upperBound = 25;
        defaultTime = 1800;
    } else if (difficulty == 3) {
        lowerBound = 25;
        upperBound = 35;
        defaultTime = 1500;
    } else {
        printf("Invalid difficulty level\n");
        return;
    }

    winScore = (rand() % (upperBound - lowerBound + 1)) + lowerBound;
}

void handleGameover(){

}

int playerHitter(){
    
}

int CPUhitter(){
    time_t t;

    /* Intializes random number generator */
    srand((unsigned) time(&t));
    int inextPos = (rand() % 2);
    printf("%d\n", inextPos);
    
    return inextPos + 2;
}