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
int hitTimer;
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
    if (turnTracker == 0){
        // player turn 
        playerHitter();
    } else if (turnTracker == 1){
        // CPU turn 
        if(winScore != score){
            CPUhitter();
        } else if (winScore == score){
            handleGameover();
        }
    }

    score = score++;
}

void startGame() {
    while(1){
        PS_2STARTGAME();  // waits for start button to be pressed on the keyboard
        if(gameover == true){
            break;
        }
    }
    resetGame();
}

void resetGame() {
    score = 0;
    flagLeft = 0;
    flagRight = 0;
    gameover = false;
    difficulty = 1;
}



void difficultySelect() {
    srand(time(NULL));  // Seed the random number generator with current time

    int lowerBound, upperBound;

    if (difficulty == 1) {
        lowerBound = 10;
        upperBound = 15;
        hitTimer = 2000;
    } else if (difficulty == 2) {
        lowerBound = 15;
        upperBound = 25;
        hitTimer = 1800;
    } else if (difficulty == 3) {
        lowerBound = 25;
        upperBound = 35;
        hitTimer = 1500;
    } else {
        printf("Invalid difficulty level\n");
        return;
    }

    winScore = (rand() % (upperBound - lowerBound + 1)) + lowerBound;
}

void handleGameover(){
    
    if(winScore != score){
        printf("you lost");
    } else if(winScore == score){
        printf("you won!");
    }

    resetGame();
}

void playerHitter(){
    // initialize the hit timer for this cycle
    int tempHitTimer = hitTimer; // save for reset later 
    hitTimer = hitTimer * speedupMultiplier;

    while(hitTimer != 0){
        PS_2INPUT;
        if(hitTimer > 750 && hitTimer < 1500 && currPos == 2 && flagLeft == 1) { // early hit left side
            nextPos = 0;
            callgraphics(); // perhaps?
            break;
        } else if (hitTimer < 750 && currPos == 2 && flagLeft == 1){ // late hit left side
            nextPos = 1;
            break;
        } else if(hitTimer > 750 && hitTimer < 1500 && currPos == 3 && flagRight == 1) { // early hit right side
            nextPos = 0;
            break;
        } else if (hitTimer < 750 && currPos == 2 && flagRight == 1){ // late hit right side
            nextPos = 1;
            break;
        } else {
            handleGameover();
            break;
        }
        hitTimer = hitTimer - 1;
        sleep(0.001);
    }
    // reset the hit timer before exiting the playerHitter loop
    hitTimer = tempHitTimer;
    return;

}

void CPUhitter(){
    time_t t;

    /* Intializes random number generator */
    srand((unsigned) time(&t));
    int inextPos = (rand() % 2);
    printf("%d\n", inextPos);
    
    nextPos = inextPos; // generates the nextPos value after calling CPUhitter to be a 2 or a 3
}