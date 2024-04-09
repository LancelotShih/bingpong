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

int currPos = 1; // use these as conditionals for selecting which if statement to use
int nextPos = 2;
int prevPos = 1;

void gameLoop();
void updateHitTime();
int CPUhitter();

int main() {
    startGraphics();
    initilizePlane();
    drawImgBackground(pingpong);
    drawPlane();
    saveFrame();

    while(1){

        gameLoop();
        score = 0;
    }
}

void gameLoop(){
    hitTime = 50;
    scaleGravity(hitTime-3);
    setUpGame(1, hitTime);
    *(PS2_ptr) = 0xFF;
    currPos = 2;
    nextPos = 0; // default player's first hit will always be to the left
    float netLocation = (OPPONENT_LOC_Z - PLAYER_LOC_Z) / 2 + PLAYER_LOC_Z; // should be like -685
    float netToPlayerLocation = (netLocation - PLAYER_LOC_Z) / 2 + PLAYER_LOC_Z; // should be like -478

    while(1){
		updateFrame();
        if(gameBall.centre[2] > netLocation){
            // only allows an input when the ball is past the net
            PS_2INPUT();
        }
        // main game logic
        if (currPos == 0 || currPos == 1){
            // cpu turn
            flagLeft = 0;
            flagRight = 0;
            if(gameBall.centre[2] < OPPONENT_LOC_Z){
                printf("current position: %d\n", currPos);
                nextPos = CPUhitter();
                bounceBall(hitTime, currPos, nextPos);
                currPos = nextPos;
            }
        } else if (currPos == 2 || currPos == 3){
            // player turn

            bool hitPositive = false;
            // hit it from it's current position to the desired next position
            if((flagLeft == 1 && flagRight == 0 && currPos == 2) || (flagLeft == 0 && flagRight == 1 && currPos == 3)){
                printf("Flag left: %d\n", flagLeft);
                printf("Flag right: %d\n", flagRight);
                printf("current position: %d\n", currPos);
                // check whether to hit it left or right next
                if((gameBall.centre[2] > netLocation) && (gameBall.centre[2] < netToPlayerLocation)){
                    // between net and center of player side's table
                    nextPos = 0;
                } else if ((gameBall.centre[2] > netToPlayerLocation) && (gameBall.centre[2] > PLAYER_LOC_Z)){
                    // between center of player side's table and player
                    nextPos = 1;
                }

                bounceBall(hitTime, currPos, nextPos);
                currPos = nextPos; 
                
                // misc cleanup per hit
                score++;
                wipeScore(1,1,11);
                updateScoreScreen(score);
                updateHitTime();

                // lower the flags after bouncing the ball
                flagLeft = 0;
                flagRight = 0;
                printf("Flag lowered \n");
            }

        }

        

        bool zboundcheck = (gameBall.centre[2] > (PLAYER_LOC_Z + 100))   || (gameBall.centre[2] < (OPPONENT_LOC_Z - 100));
        bool xboundcheck = (gameBall.centre[0] < (PLAYER_LOC_Z/2 - 50))  || (gameBall.centre[0] > (-PLAYER_LOC_Z/2 + 50));
        bool yboundcheck = (gameBall.centre[1] > 200 || (gameBall.centre[1] < (GROUND_Y-150))) ;
        
        if(zboundcheck || xboundcheck || yboundcheck){
            drawImgBackground(tempFrame);
            break;
        }
        
    }



}

void updateHitTime(){
    if(hitTime > 5){
        hitTime = hitTime - 3;
        scaleGravity(hitTime);
    }
}

int CPUhitter(){
    double randNum = (double)rand() / RAND_MAX;

    int CPUhitLocation = (int)randNum + 2;
    return CPUhitLocation;
}