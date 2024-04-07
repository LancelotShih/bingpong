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
    hitTime = 20;
    scaleGravity(hitTime-3);
    setUpGame(1, hitTime);
    *(PS2_ptr) = 0xFF;
    currPos = 2;
    nextPos = 0; // default player's first hit will always be to the left
    float netLocation = -600;
    float netToPlayerLocation = -385; // should be like -478

    while(1){
		updateFrame();
        // printf("gameBall.centre[2]: %f\n", gameBall.centre[2]);
        // main game logic
        flagLeft = 0;
        flagRight = 0;
        PS_2INPUT();
        // printf("Flag left: %d\n", flagLeft);
        // printf("Flag right: %d\n", flagRight);
        if (currPos == 0 || currPos == 1){
            // cpu turn
            if(gameBall.centre[2] < OPPONENT_LOC_Z){ // doesn't trigger until it reaches CPU
                printf("current position: %d\n", currPos);
                nextPos = CPUhitter();
                printf("next position: %d\n", nextPos);
                bounceBall(hitTime, currPos, nextPos);
                bufferReset();
                currPos = nextPos;
            }
        } else if (currPos == 2 || currPos == 3){
            // player turn
            bool hitPositive = false;
            // doesn't trigger unless correct flag combination is raised with position
            // hit it from it's current position to the desired next position
            if(((flagLeft == 1 && flagRight == 0 && currPos == 2) || (flagLeft == 0 && flagRight == 1 && currPos == 3)) && gameBall.centre[2] > netLocation){
                printf("current position: %d\n", currPos);
                // check whether to hit it left or right next
                // printf("gameBall HIT location: %f\n", gameBall.centre[2]);
                if((gameBall.centre[2] <= netToPlayerLocation) && gameBall.centre[2] > netLocation){
                    // between net and center of player side's table
                    nextPos = 0;
                    printf("next pos is 0\n");
                } else {
                    // between center of player side's table and player
                    nextPos = 1;
                    printf("next pos is 1\n");
                }
                printf("next pos: %d\n", nextPos);
                bounceBall(hitTime, currPos, nextPos);
                currPos = nextPos;
            
                // misc cleanup per hit
                bufferReset();
                score++;
                // wipeScore(1,1,11);
                updateScoreScreen(score);
                updateHitTime();

                // lower the flags after bouncing the ball
                // printf("Flag lowered \n");
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
    if(hitTime > 10){
        hitTime = hitTime - 3;
        scaleGravity(hitTime);
    }
}

int CPUhitter(){
    // Static variable to hold the state of the seed
    static unsigned int seed = 0;

    // Use XOR operation on the seed with a prime number
    seed ^= 0x5A8E9039;

    // Extract least significant bit of the seed
    int random_bit = seed & 1;

    // Return either 2 or 3 based on the random bit
    return (random_bit == 0) ? 2 : 3;
}