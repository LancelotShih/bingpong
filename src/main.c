#include "audio.h"
//interupt handler, nios 2 control reg macros, and reset code are from 
//DE1-soc Nios II manual

/////////////audio interupt
#define NIOS2_READ_STATUS(dest) \
  do {                          \
    dest = __builtin_rdctl(0);  \
  } while (0)
#define NIOS2_WRITE_STATUS(src) \
  do {                          \
    __builtin_wrctl(0, src);    \
  } while (0)
#define NIOS2_READ_ESTATUS(dest) \
  do {                           \
    dest = __builtin_rdctl(1);   \
  } while (0)
#define NIOS2_READ_BSTATUS(dest) \
  do {                           \
    dest = __builtin_rdctl(2);   \
  } while (0)
#define NIOS2_READ_IENABLE(dest) \
  do {                           \
    dest = __builtin_rdctl(3);   \
  } while (0)
#define NIOS2_WRITE_IENABLE(src) \
  do {                           \
    __builtin_wrctl(3, src);     \
  } while (0)
#define NIOS2_READ_IPENDING(dest) \
  do {                            \
    dest = __builtin_rdctl(4);    \
  } while (0)
#define NIOS2_READ_CPUID(dest) \
  do {                         \
    dest = __builtin_rdctl(5); \
  } while (0)

void interrupt_handler(void);
void audio_ISR(void);
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
/*******************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we
 * allow the linker program to locate this code at the proper reset vector
 * address. This code just calls the main program.
 ******************************************************************************/
{
  asm(".set noat");       /* Instruct the assembler NOT to use reg at
                         (r1) as * a temp register for performing optimizations */
  asm(".set nobreak");    /* Suppresses a warning message that says that *
                         some debuggers corrupt regs bt (r25) and ba
                         * (r30)
                         */
  asm("movia r2, main");  // Call the C language main program
  asm("jmp r2");
}
/* The assembly language code below handles CPU exception processing. This
 * code should not be modified; instead, the C language code in the function
 * interrupt_handler() can be modified as needed for a given application.
 */

void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name
 * ".exceptions" we allow the linker program to locate this code at the proper
 * exceptions vector address.
 * This code calls the interrupt handler and later returns from the exception.
 ******************************************************************************/
{
  asm("subi sp, sp, 128");
  asm("stw et, 96(sp)");
  asm("rdctl et, ctl4");
  asm("beq et, r0, SKIP_EA_DEC");  // Interrupt is not external
  asm("subi ea, ea, 4");           /* Must decrement ea by one instruction
                                    * for external interupts, so that the
                                    * interrupted instruction will be run */
  asm("SKIP_EA_DEC:");
  asm("stw r1, 4(sp)");  // Save all registers asm("stw r2, 8(sp)");
  asm("stw r2, 8(sp)");
  asm("stw r3, 12(sp)");
  asm("stw r4,  16(sp)");
  asm("stw r5,  20(sp)");
  asm("stw r6,  24(sp)");
  asm("stw r7,  28(sp)");
  asm("stw r8,  32(sp)");
  asm("stw r9,  36(sp)");
  asm("stw r10, 40(sp)");
  asm("stw r11, 44(sp)");
  asm("stw r12, 48(sp)");
  asm("stw r13, 52(sp)");
  asm("stw r14, 56(sp)");
  asm("stw r15, 60(sp)");
  asm("stw r16, 64(sp)");
  asm("stw r17, 68(sp)");
  asm("stw r18, 72(sp)");
  asm("stw r19, 76(sp)");
  asm("stw r20, 80(sp)");
  asm("stw r21, 84(sp)");
  asm("stw r22, 88(sp)");
  asm("stw r23, 92(sp)");
  
  asm("stw r25, 100(sp)");
  asm("stw r26, 104(sp)");  // r25=bt
  // skip r27 because it is sp, and there is no point in saving this
  asm("stw r28, 112(sp)");
  asm("stw r29, 116(sp)");
  asm("stw r30, 120(sp)");
  asm("stw r31, 124(sp)");
  asm("addi   fp,  sp, 128");
  asm("call interrupt_handler");  // Call the C language interrupt handler
  asm("ldw r1, 4(sp)");           // Restore all registers r2, 8(sp)
  asm("ldw r2, 8(sp)");
  asm("ldw r3, 12(sp) ");
  asm("ldw r4, 16(sp) ");
  asm("ldw r5,  20(sp)");
  asm("ldw  r6,  24(sp)");
  asm("ldw  r7,  28(sp)");
  asm("ldw  r8,  32(sp)");
  asm("ldw  r9,  36(sp)");
  asm("ldw  r10, 40(sp)");
  asm("ldw  r11, 44(sp)");
  asm("ldw  r12, 48(sp)");
  asm("ldw  r13, 52(sp)");
  asm("ldw  r14, 56(sp)");
  asm("ldw  r15, 60(sp)");
  asm("ldw  r16, 64(sp)");
  asm("ldw  r17, 68(sp)");
  asm("ldw  r18, 72(sp)");
  asm("ldw  r19, 76(sp)");
  asm("ldw  r20, 80(sp)");
  asm("ldw  r21, 84(sp)");
  asm("ldw  r22, 88(sp)");
  asm("ldw  r23, 92(sp)");
  asm("ldw  r24, 96(sp)");
  asm("ldw  r25, 100(sp)");
  asm("ldw  r26, 104(sp)");
  asm("ldw  r28, 112(sp)");
  asm("ldw  r29, 116(sp)");
  asm("ldw  r30, 120(sp)");
  asm("ldw  r31, 124(sp)");
  asm("addi   sp,  sp, 128");
  asm("eret");
}

//Interrupt Service Routine
void interrupt_handler(void) { 
    //printf("got to interupt handler");
    int ipending;
    NIOS2_READ_IPENDING(ipending);
    // if (ipending & 0x1) // interval timer is interrupt level 0 
    // {
    //     interval_timer_ISR();
    // }
    // if (ipending & 0x2) // pushbuttons are interrupt level 1 
    // {
    //     pushbutton_ISR();
    // }
    if (ipending & 0x40){
        audio_ISR();
    }
        // else, ignore the interrupt
    return; 
}
struct audio_t {
  volatile unsigned int control;
  volatile unsigned char rarc;
  volatile unsigned char ralc;
  volatile unsigned char wsrc;
  volatile unsigned char wslc;
  volatile unsigned int ldata;
  volatile unsigned int rdata;
};

struct audio_t *const audiop = ((struct audio_t *)0xff203040);
int songIndex =0;//global
const int output_length = 229651;////////////remebeer these consts
void audio_ISR(void){
    //printf("interupt!\n");
    //printf("%d ",audiop->control);
	//printf("%d\n", 0b1000000000);
	
    while((audiop->control & 0b1000000000)!=0){
        audiop->ldata = output[songIndex];
        //printf("outsong");
        audiop->rdata = output[songIndex];
        songIndex++;
        if(songIndex>=output_length){
            songIndex =0;
        }
    }

}


// int main(void) {
//     audiop -> control = 0x2; //enable we to generate interupts
//     NIOS2_WRITE_IENABLE(0x40);
//      NIOS2_WRITE_STATUS( 1 );
 
//   // audio_playback_mono(output, output_length);
//   int e =0;
//   while (1) {
//     e+= 2;
//     // audio_playback_mono(output, output_length);
//   }
// }

// ==================================================== MAIN.C ================================================
// This project was created by Lancelot Shih and Laura Lin

#include "address_map_nios2.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "PS2.h"
#include "graphics.h"
//#include "audio.h"



/* GLOBAL VARIABLES */

int score = 0;                // changed by game
int CPUfailScore = 999999;
int minhitTime;

int currPos = 1; // use these as conditionals for selecting which if statement to use
int nextPos = 2;
int prevPos = 1;

void gameLoop();
void updateHitTime();
int CPUhitter();

int main() {
        audiop -> control = 0x2; //enable we to generate interupts
    NIOS2_WRITE_IENABLE(0x40);
     NIOS2_WRITE_STATUS( 1 );
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
                minhitTime = 60;
                CPUfailScore = 6;
                printf("flag set 1 success\n");
                flagDifficulty = 0;
                break;
            } else if (flagDifficulty == 2){
                minhitTime = 55;
                CPUfailScore = 11;
                printf("flag set 2 success\n");
                flagDifficulty = 0;
                break;
            } else if (flagDifficulty == 3){
                minhitTime = 50;
                CPUfailScore = 16;
                printf("flag set 3 success\n");
                flagDifficulty = 0;
                break;
            } else if (flagDifficulty == 4){
                minhitTime = 50;
                CPUfailScore = 99;
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
    hitTime = minhitTime;
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
                flagDifficulty = 0;
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