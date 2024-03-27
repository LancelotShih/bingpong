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
int hitTimer;
int speedupMultiplier = 0.95;

int currPos = 2;
int nextPos = 1;

int main() {
    startGraphics();
    while(1){
        PS_2INPUT();
        if(flagLeft == 1){
            for(int i = 0; i < 200; i++){
                startGraphics();
            }
            
        } else if (flagRight == 1){
            for(int i = 0; i < 200; i++){
                startGraphics();
            }
        }
        flagLeft = 0;
        flagRight = 0;
    }
 
}