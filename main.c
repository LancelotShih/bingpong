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
int CPUhitter();

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
    hitTime = 50;
    scaleGravity(hitTime-3);
    setUpGame(1, hitTime);
    printf("entered loop \n");
    *(PS2_ptr) = 0xFF;
    int printer = 0;
    float gameBallold = 0;
    while(1){
		updateFrame();
        PS_2INPUT();
        if(flagLeft == 1){
            if(gameBall.centre[2] > PLAYER_LOC_Z){
                nextPos = 0;
                bounceBall(hitTime, 2, nextPos);
                if(hitTime > 5){
                    hitTime = hitTime - 3;
                    scaleGravity(hitTime);
                } 
                printf("######## BALL FINISHED BOUNCING ####### \n");
            } 
            // else if(gameBall.centre[2] < OPPONENT_LOC_Z){
            //     bounceBall(hitTime, 1, 2);
            //     printf("######## BALL HIT BACK BOUNCING ####### \n");
            // }   
        } else if (flagRight == 1){
            if(gameBall.centre[2] > PLAYER_LOC_Z){
                nextPos = 1;
                bounceBall(hitTime, 2, nextPos);
                if(hitTime > 5){
                    hitTime = hitTime - 3;
                    scaleGravity(hitTime);
                }    
            } 
            // else if(gameBall.centre[2] < OPPONENT_LOC_Z){
            //     bounceBall(hitTime, 0, 2);
            // }

        } else if (gameBall.centre[2] < OPPONENT_LOC_Z){
            // int CPUhitLocation = CPUhitter();
            bounceBall(hitTime, nextPos, 2);
        }


        // if(flagLeft == 1 && gameBall.centre[2] > PLAYER_LOC_Z){
        //     bounceBall(hitTime, 2, 0);
        //     if(hitTime > 5){
        //         hitTime = hitTime - 3;
        //         scaleGravity(hitTime);
        //     }
        // } else if (flagRight == 1 && gameBall.centre[2] > PLAYER_LOC_Z){
        //     bounceBall(hitTime, 3, 0);
        //     if(hitTime > 5){
        //         hitTime = hitTime - 3;
        //         scaleGravity(hitTime);
        //     }
        // } else if (gameBall.centre[2] < OPPONENT_LOC_Z){
        //     int CPUhitLocation = CPUhitter();
        //     bounceBall(hitTime, 0, CPUhitLocation);
        // }

        flagLeft = 0;
        flagRight = 0;
        bool zboundcheck = (gameBall.centre[2] > (PLAYER_LOC_Z + 100))   || (gameBall.centre[2] < (OPPONENT_LOC_Z - 100));
        bool xboundcheck = (gameBall.centre[0] < (PLAYER_LOC_Z/2 - 50))  || (gameBall.centre[0] > (-PLAYER_LOC_Z/2 + 50));
        bool yboundcheck = (gameBall.centre[1] > 200 || (gameBall.centre[1] < (GROUND_Y-150))) ;
        
        if(zboundcheck || xboundcheck || yboundcheck){
            printf("reset coords\n");
            printf("x location: %f \n", gameBall.centre[0]);
            printf("y location: %f \n", gameBall.centre[1]);
            printf("z location: %f \n", gameBall.centre[2]);
            printf("resetting! \n");
            drawImgBackground(tempFrame);
            break;
        }
        printer++;
        if(printer == 5){    
            printf("yloc always: %f \n", gameBall.centre[1]);
            printer = 0;
        }
        
        if(gameBall.centre[2] == gameBallold){
            break; // this means something is wrong and ball is no longer moving between the players
        }
        gameBallold = gameBall.centre[2];
    }
    return;
}

int CPUhitter(){
    double randNum = (double)rand() / RAND_MAX;

    int CPUhitLocation = (int)randNum + 2;
    return CPUhitLocation;
}