// ==================================================== PS2.C ================================================


#include "address_map_nios2.h"
#include "stdbool.h"
#include "stdio.h"
#include "PS2.h"


int flagLeft = 0;
int flagRight = 0;
int flagRestart = 0;
int flagDifficulty = 0;
bool gameover = true;
int flagRTXon = 0;
int flagRTXoff = 0;

volatile int *PS2_ptr = (int *)PS2_BASE;
int PS2_data, RVALID;
char byte1 = 0, byte2 = 0, byte3 = 0;

void HEX_PS2(char, char, char);


// int main(void) { // for debugging the PS2 driver
//     *(PS2_ptr) = 0xFF;
//     while(1){
//         PS_2INPUT();
//     }
//     return 0;
// }

void PS_2INPUT(){
    PS2_data = *(PS2_ptr);
    RVALID = PS2_data & 0x8000;  // checks bit 15 to see if RVALID has loaded anything in
    if (RVALID) {     // if RVALID has loaded something in
        byte1 = byte2;
        byte2 = byte3;
        byte3 = PS2_data & 0xFF;
        HEX_PS2(byte1, byte2, byte3);
        flagRaiseCheck(byte1, byte2, byte3); // raises the global variable flags, poll for this during the timing of when the player is prompted to hit
        LED_PS2(flagLeft, flagRight); // just checks if the flag system is working
        // bufferReset();
        // if ((byte2 == (char)0xAA) && (byte3 == (char)0x00)){  // 10101010 and 00000000
        //     *(PS2_ptr) = 0xF4;        // 11111000
        // }
    }
}

void bufferReset(){
    byte1 = 0xFF;
    byte2 = 0xFF;
    byte3 = 0xFF;
}

void PS_2STARTGAME(){
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    char byte1 = 0, byte2 = 0, byte3 = 0;

    *(PS2_ptr) = 0xFF;

    while(1){
        PS2_data = *(PS2_ptr);
        RVALID = PS2_data & 0x8000;
        if (RVALID) {
            byte1 = byte2;
            byte2 = byte3;
            byte3 = PS2_data & 0xFF;
            
        if (byte1 == 0x34){
            gameover = !gameover;
        }

        if ((byte2 == (char)0xAA) &&
            (byte3 == (char)0x00))  // 10101010 and 00000000
            *(PS2_ptr) = 0xF4;        // 11111000
        }
        

    }

}

void HEX_PS2(char b1, char b2, char b3) {
  volatile int *HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
  volatile int *HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

  unsigned char seven_seg_decode_table[] = {
      0x3F,  // 0
      0x06,  // 1
      0x5B,  // 2
      0x4F,  // 3
      0x66,  // 4
      0x6D,  // 5
      0x7D,  // 6
      0x07,  // 7
      0x7F,  // 8
      0x6F,  // 9
      0x77,  // A
      0x7C,  // b
      0x39,  // C
      0x5E,  // d
      0x79,  // E
      0x71   // F
  };

  unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int shift_buffer, nibble;
  unsigned char code;
  int i;

  shift_buffer = (b1 << 16) | (b2 << 8) | b3;
  for (i = 0; i < 6; ++i) {
    nibble = shift_buffer & 0x0000000F;  // character is in rightmost nibble
    code = seven_seg_decode_table[nibble];
    hex_segs[i] = code;
    shift_buffer = shift_buffer >> 4;
  }

  *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
  *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}

void flagRaiseCheck(char b1, char b2, char b3) {
    if (b1 == 0x1C && b2 != 0xF0) {
        flagLeft = 1;  // raised if the left key was pressed
    } else if (b1 == 0x23 && b2 != 0xF0) {
        flagRight = 1;  // raised if the right key was pressed
    } else if (b1 == 0x1B && b2 != 0xF0){
        flagLeft = 0;
        flagRight = 0;
        flagRestart = 0;
    } else if (b1 == 0x2D && b2 != 0xF0){
        flagRestart = 1;
        printf("restart flag raised \n");
    } else if (b1 == 0x16 && b2 != 0xF0){
        flagDifficulty = 1;
        printf("difficulty 1 raised \n");
    } else if (b1 == 0x1E && b2 != 0xF0){
        flagDifficulty = 2;
        printf("difficulty 2 raised \n");
    } else if (b1 == 0x26 && b2 != 0xF0){
        flagDifficulty = 3;
        printf("difficulty 3 raised \n");
    } else if (b1 == 0x25 && b2 != 0xF0){
        flagDifficulty = 4;
        printf("difficulty 4 raised \n");
    } else if (b1 == 0x34 && b2 != 0xF0){
        flagRTXon = 1;
        // flagRTXoff = 0;
        printf("rtx ON\n");
    } else if (b1 == 0x33 && b2 != 0xF0){
        // flagRTXon = 0;
        flagRTXon = 0;
        printf("rtx OFF\n");
    }
}

void LED_PS2(int flagLeft, int flagRight) {
    volatile int *LEDR_ptr = (int *)LED_BASE;
    int ledValue = *LEDR_ptr;
    if (flagLeft == 1 && flagRight == 0 && ledValue == 0x00) { // needs to be fresh and flag is left
        printf("left flag raised \n");
        ledValue = 0x2;  // turns on the left LED
    }
    if (flagRight == 1 && flagLeft == 0 && ledValue == 0x00) { // needs to be fresh and flag is right
        printf("right flag raised \n");
        ledValue = 0x1;  // turns on the right LED
    } 
    if (flagLeft == 0 && flagRight == 0) {
        ledValue = 0x0;  // turns off all LEDs
    }
    *LEDR_ptr = ledValue;
}