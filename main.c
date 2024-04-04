#include "PS2.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "time.h"
#include "graphics.h"


/* GLOBAL VARIABLES */

int score = 0;                // changed by game
int difficulty = 3;
int winScore;
int turnTracker = 0;  // 0 is Player, 1 is CPU
int defaultTime;
int speedupMultiplier = 0.95;

int currPos = 2; // use these as conditionals for selecting which if statement to use
int nextPos = 1;

void gameLoop();

int main() {
    startGraphics();
    initilizePlane();
    drawImgBackground(pingpong);
    drawPlane();
    saveFrame();
    while(1){

        gameLoop();
    }
}

// separate gameloop allows for an easy reset conditional
void gameLoop(){
    scaleGravity(hitTime);
    
    *(PS2_ptr) = 0xFF;
    while(1){
		updateFrame();
        PS_2INPUT();
        if(flagLeft == 1){
            if(gameBall.centre[2] > PLAYER_LOC_Z){
                bounceBall(hitTime, 2, 0);
                printf("######## BALL FINISHED BOUNCING ####### \n");
            } else if(gameBall.centre[2] < OPPONENT_LOC_Z){
                bounceBall(hitTime, 0, 2);
            }
        } else if (flagRight == 1){
            if(gameBall.centre[2] > PLAYER_LOC_Z){
                bounceBall(hitTime, 2, 0);
            } else if(gameBall.centre[2] < OPPONENT_LOC_Z){
                bounceBall(hitTime, 0, 2);
            }
        } 

        flagLeft = 0;
        flagRight = 0;

        if(gameBall.centre[2] > (PLAYER_LOC_Z + 100) || gameBall.centre[2] < OPPONENT_LOC_Z - 100){
            printf("Center: %f \n", gameBall.centre[2]);
            printf("resetting! \n");
            drawImgBackground(tempFrame);
            setUpGame(1, hitTime);
            break;
        }

    }
    return;
}