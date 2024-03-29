#ifndef graphics_h
#define graphics_h
#include <stdbool.h>

extern int pixel_buffer_start; // global variable
extern short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
extern short int Buffer2[240][512];

extern const int SCREENX;
extern const int SCREENY;
extern const int GRAVITY;
extern const int PLAYER_LOC_Z;
extern const int GROUND_Y;
extern const int OPPONENT_LOC_Z;
extern short int origin[3];
extern struct ball gameBall;
extern volatile int * pixel_ctrl_ptr;

struct plane{
	int length;
	int width; 
	int point1[3];
	int point2[3];//x,y,z
	int point3[3];
	int point4[3];
	short int colour;
};


struct ball{
	short int radius;
	float centre[3];
	short int screenLoc[2];
	short int colour;
	short int pastScreenLoc[2];
	float velocity[3];
	
};

extern bool correctHit;

extern int position[3];//gloabal position variable

void startGraphics();
short int colour_packing(short int R, short int G,short  int B);
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_line(int x0, int y0, int x1, int y1,short int line_colour);
void clear_screen();
//void colour_unpack();
//float hitBall(struct ball ball, int origin[3], float viewDir[3]);
//void drawPlane(struct plane, int origin[3]);
//void drawBall(struct ball ball, int origin[3], int short colour);
void projectPixel(short int colour, float point3D[3], short int origin[3],
short int *x, short int *y);
void eraseSimpleBall();
void simpleDrawBall(short int origin[3]);
void updateLocation();
void setUpGame( short int startPosition, short int hitTime);
void updateFrame();
void bounceBall(short int hitTime, short int startPosition, short int nextPosition);

#endif