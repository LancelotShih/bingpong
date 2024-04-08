// ==================================================== MAIN.C ================================================

#include "address_map_nios2.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "PS2.h"
#include "graphics.h"
#include "audio.h"



/* GLOBAL VARIABLES */

int score = 0;                // changed by game
int CPUfailScore = 999999;

int currPos = 1; // use these as conditionals for selecting which if statement to use
int nextPos = 2;
int prevPos = 1;

void gameLoop();
void updateHitTime();
int CPUhitter();

int main() {
    startGraphics();
    initilizePlane();
    // initializeAudioInterupts();
    
    while(1){
        while(1){
            startScreen();
            flagDifficulty = 0;
            flagRestart = 0;
            PS_2INPUT();
            // switch (flagDifficulty){
            //     case 1:
            //         hitTime = 60;
            //         CPUfailScore = 5;
            //         break;
            //     case 2:
            //         hitTime = 55;
            //         CPUfailScore = 10;
            //         break;
            //     case 3:
            //         hitTime = 50;
            //         CPUfailScore = 15;
            //         break;
            //     case 4:
            //         hitTime = 50;
            //         CPUfailScore = 999999;
            //         break;
            //     default:
            //         hitTime = 50;
            //         CPUfailScore = 999999;
            // }
            if(flagDifficulty == 1){
                hitTime = 60;
                CPUfailScore = 6;
                printf("flag set 1 success\n");
                flagDifficulty = 0;
                break;
            } else if (flagDifficulty == 2){
                hitTime = 55;
                CPUfailScore = 11;
                printf("flag set 2 success\n");
                flagDifficulty = 0;
                break;
            } else if (flagDifficulty == 3){
                hitTime == 50;
                CPUfailScore == 16;
                printf("flag set 3 success\n");
                flagDifficulty = 0;
                break;
            } else if (flagDifficulty == 4){
                hitTime == 50;
                CPUfailScore == 999;
                printf("flag set 4 success\n");
                flagDifficulty = 0;
                break;
            }
        }

        drawImgBackground(pingpong);
        drawPlane();
        saveFrame();

        while(1){
            gameLoop();
            if(flagRestart == 1){
                score = 0;
                break;
            }
            if(score >= CPUfailScore){
                score = 0;
                break;
            }
            score = 0;
        }

        while(1){
            endScreen();
            flagRestart = 0;
            PS_2INPUT();
            if(flagRestart == 1){
                flagRestart = 0;
                break;
            }
        }
    }
}

void gameLoop(){
    scaleGravity(hitTime-2);
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
            if(gameBall.centre[2] < OPPONENT_LOC_Z && score < CPUfailScore){ // doesn't trigger until it reaches CPU
                printf("current position: %d\n", currPos);
                nextPos = CPUhitter();
                printf("next position: %d\n", nextPos);
                score++;
                if(score >= CPUfailScore){
                    drawImgBackground(tempFrame);
                    break;
                }
                bounceBall(hitTime, currPos, nextPos);
                bufferReset();
                currPos = nextPos; 
            } else if (flagRestart == 1){
                flagRestart = 0;
                printf("game force restarted \n");
                break;
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