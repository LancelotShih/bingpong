#include <stdlib.h> 
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
 int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

const int SCREENX = 320;
const int SCREENY = 240;
const int GRAVITY = 10;
const int PLAYER_LOC_Z = -250;
const int GROUND_Y = -370;
const int OPPONENT_LOC_Z = -900;
short int origin[3] = {0,100,0};
struct ball gameBall;
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

bool correctHit =false;

int position[3];//gloabal position variable

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
	short int centre[3];
	short int screenLoc[2];
	short int colour;
	short int pastScreenLoc[2];
	float velocity[3];
	
};

short int colour_packing(short int R, short int G,short  int B);
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_line(int x0, int y0, int x1, int y1,short int line_colour);
void clear_screen();
void colour_unpack();
float hitBall(struct ball ball, int origin[3], float viewDir[3]);
void drawPlane(struct plane, int origin[3]);
void drawBall(struct ball ball, int origin[3], int short colour);
void projectPixel(short int colour, short int point3D[3], short int origin[3],
short int *x, short int *y);
void eraseSimpleBall(struct ball gameBall);
void simpleDrawBall(struct ball *gameBall, short int origin[3]);
void updateLocation(struct ball *gameBall);

int main(void)
{
	short int origin[3] = {0,100,0};
    
    // declare other variables(not shown)
	
    // initialize location and direction of rectangles
	gameBall.centre[0] = -260;
	gameBall.centre[1] = GROUND_Y+20;
	gameBall.centre[2] = PLAYER_LOC_Z-10;
	gameBall.colour = colour_packing(31, 0,0);
	gameBall.radius = 10;//no effect atm
	gameBall.pastScreenLoc[0] = 0;//initillise to anything
	gameBall.pastScreenLoc[1] = 0;
	gameBall.velocity[0] = 10;
	gameBall.velocity[1] = 30;
	gameBall.velocity[2] = -70;


    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
		//draw(boxes);

	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	
	//Testing
	simpleDrawBall(&gameBall, origin); 
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	
	/*
    while (1)
    {
		if(gameBall.centre[2] <= OPPONENT_LOC_Z||gameBall.centre[2] >= PLAYER_LOC_Z){
			correctHit = true;
		}
		//erase
        eraseSimpleBall(gameBall);
		//gameBall.centre[0] +=1;
		//gameBall.centre[1] +=1;
		
		
		simpleDrawBall(&gameBall, origin); 
		updateLocation(&gameBall);

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		correctHit = false;
    }
	*/
	

	
}
void bounceBall(short int hitTime, short int startPosition, short int nextPosition){ 
	eraseSimpleBall(gameBall);
	gameBall.velocity[1] = -((gameBall.centre[1]-GROUND_Y)-0.5*GRAVITY*(hitTime*60.0/1000.0)*hitTime*60.0/1000.0)*1000.0/(60.0*hitTime);
	if (startPosition == 0){
		if(nextPosition == 2){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 3){
			gameBall.velocity[0] = -PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		}
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime*(1000.0/60.0);
	}

	else if(startPosition == 2){
		if(nextPosition == 0){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 3){
			gameBall.velocity[0] = -PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		}
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime*(1000.0/60.0);
	}
	else if (startPosition == 1){
		if(nextPosition == 3){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 2){
			gameBall.velocity[0] = PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		}
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime*(1000.0/60.0);
	}
	else if(startPosition == 3){
		if(nextPosition == 1){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 0){
			gameBall.velocity[0] = PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		}
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime*(1000.0/60.0);
	}
	simpleDrawBall(&gameBall, origin); 
	updateLocation(&gameBall);
	wait_for_vsync();
	pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	
}

void updateFrame(){
	eraseSimpleBall(gameBall);
	simpleDrawBall(&gameBall, origin); 
	updateLocation(&gameBall);
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

}

//used for serves -always diagonal
void setUpGame( short int startPosition, short int hitTime){
	eraseSimpleBall(gameBall);
	if (startPosition == 0){
		gameBall.centre[0] = -PLAYER_LOC_Z/2;
		gameBall.velocity[0] = -PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime*(1000.0/60.0);
		gameBall.centre[2] = OPPONENT_LOC_Z+1;

	}
	else if(startPosition == 2){
		gameBall.centre[0] = -PLAYER_LOC_Z/2;
		gameBall.velocity[0] = -PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime*(1000.0/60.0);
		gameBall.centre[2] = PLAYER_LOC_Z-1;
	}
	else if (startPosition == 1){
		gameBall.centre[0] = PLAYER_LOC_Z/2;
		gameBall.velocity[0] = PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime*(1000.0/60.0);
		gameBall.centre[2] = OPPONENT_LOC_Z+1;
	}
	else if(startPosition == 3){
		gameBall.centre[0] = PLAYER_LOC_Z/2;
		gameBall.velocity[0] = PLAYER_LOC_Z*(6.0/5.0)/(float)hitTime/(float)(1000.0/60.0);
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime*(1000.0/60.0);
		gameBall.centre[2] = PLAYER_LOC_Z-1;
	}
	
	gameBall.centre[1] = GROUND_Y+100; //100 = height from table 
	gameBall.pastScreenLoc[0] = 0;
	gameBall.pastScreenLoc[1] = 0;
	gameBall.velocity[1] = -((100)-0.5*GRAVITY*(hitTime*60.0/1000.0)*hitTime*60.0/1000.0)*1000.0/(60.0*hitTime);
	simpleDrawBall(&gameBall, origin); 
	updateLocation(&gameBall);

	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	
}

// code for subroutines (not shown)

void plot_pixel(int x, int y, short int line_color)
{
    short int *one_pixel_address;
        
        one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
        *one_pixel_address = line_color;
}

void wait_for_vsync(){
	volatile int * pixel_ctrl_ptr = (int *) 0xff203020; 
	* pixel_ctrl_ptr = 1;
	int status= *(pixel_ctrl_ptr+3);
	while(status &1 ==1){
		status= *(pixel_ctrl_ptr+3);
	}
}

void draw_line(int x0, int y0, int x1, int y1,short int line_colour){
	bool is_steep = abs(y1-y0)>abs(x1-x0);
	int temp;
	if(is_steep){
		temp = x0; 
		x0 = y0;
		y0 = temp;
		temp =x1;
		x1 =y1;
		y1 =temp;
	}
	if(x0>x1){
		temp = x0; 
		x0 = x1;
		x1 = temp;
		temp = y0; 
		y0 = y1;
		y1 = temp;
	}
	int deltax = x1-x0;
	int deltay = abs(y1-y0);
	int error = -(deltax/2);
	int y= y0;
	int y_step;
	if(y0<y1){
		y_step =1;
	}
	else{
		y_step = -1;
	}
	for(int x =x0; x<x1; x++){
		if(is_steep){
			plot_pixel(y,x,line_colour);
		}
		else{
			plot_pixel(x,y,line_colour);
		}
		error = error +deltay;
		if(error>0){
			y=y+y_step;
			error = error -deltax;
		}
	}
}


void clear_screen(){ //basic clearing, draw every pixel
	for(int y=0; y<240; y++){
		for(int x = 0; x<320; x++){
			plot_pixel(x, y, 0);
		}
	}
}


//code for colourpack
short int colour_packing(short int R, short int G,short  int B){
	short int colour;
	R = R&0x1F;
	G = G&0x3F;
	B = B&0x1F;
	colour = B +(G<<5)+(R<<10);
	return colour;
}

void drawPlane(struct plane plane,int origin[3]){
	//projeect 3d point using pov location angle
	//draw_line( x0, int y0, int x1, int y1,short int line_colour);
	
}

float dotProduct(int A[3], int B[3]){
    return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
    
}
//following function only works with camera shifting vertically
void projectPixel(short int colour, short int point3D[3], short int origin[3],
short int *x, short int *y){
    float dz = point3D[2]-origin[2];
    if (dz ==0){
        return;
    }
    float dx = (float)point3D[0]-(float)origin[0];
    float dy = point3D[1]-origin[1];
    *x = ((dx/-dz)*(float)SCREENX/2)+SCREENX/2;
    *y = ((dy/dz)*(float)SCREENY/2)+SCREENY/2-origin[1];
	//*y = ((dy/dz)*(float)SCREENY/2)+SCREENY/2;
	
	printf("x location screen %d, ", *x);
	printf("\n");
	printf("y location screen %d", *y);
	printf("\n");
	if(*y>=SCREENY-1||*y<0||*x<0||*x>=SCREENX-1){
		printf("not drawing");
		return;
	}
    plot_pixel(*x,*y,colour);
	plot_pixel(*x+1,*y,colour);
	plot_pixel(*x+1,*y+1,colour);
	plot_pixel(*x,*y+1,colour);
}
void simpleDrawBall(struct ball *gameBall, short int origin[3]){
	//printf("origin location screen %d, %d, %d, \n", origin[0],origin[1],origin[2]);
	(*gameBall).pastScreenLoc[0] = (*gameBall).screenLoc[0];
	(*gameBall).pastScreenLoc[1] = (*gameBall).screenLoc[1];
	projectPixel((*gameBall).colour, (*gameBall).centre, origin, &((*gameBall).screenLoc[0]),&((*gameBall).screenLoc[1]));
	//safeguard to ensuree ball does not go out of bounds
	if((*gameBall).screenLoc[0]<0||(*gameBall).screenLoc[0]>=SCREENX||(*gameBall).screenLoc[1]<0||(*gameBall).screenLoc[1]>=SCREENY){
		(*gameBall).velocity[0] = 0;
		(*gameBall).velocity[1] = 0;
		(*gameBall).velocity[2] = 0;
	}
	
}
void eraseSimpleBall(struct ball gameBall){
	plot_pixel(gameBall.pastScreenLoc[0],gameBall.pastScreenLoc[1],0);
	plot_pixel(gameBall.pastScreenLoc[0]+1,gameBall.pastScreenLoc[1],0);
	plot_pixel(gameBall.pastScreenLoc[0],gameBall.pastScreenLoc[1]+1,0);
	plot_pixel(gameBall.pastScreenLoc[0]+1,gameBall.pastScreenLoc[1]+1,0);
}

void updateLocation(struct ball *gameBall){ //only bounded in z, y direction
	if((*gameBall).centre[1]<= GROUND_Y){
		(*gameBall).velocity[1] =  (*gameBall).velocity[1]*(-1);
	}
	//change position based on current velocity
	//printf("pre x: %d, y: %d,, z: %d \n", (*gameBall).centre[0],(*gameBall).centre[1],(*gameBall).centre[2]);
	(*gameBall).centre[0] += (*gameBall).velocity[0];
	(*gameBall).centre[1] += (*gameBall).velocity[1];
	(*gameBall).centre[2] += (*gameBall).velocity[2];
	//printf("post x: %d, y: %d,, z: %d \n", (*gameBall).centre[0],(*gameBall).centre[1],(*gameBall).centre[2]);
	//update vertical velocity
	(*gameBall).velocity[1] -=GRAVITY;
}




//below is ray tracing eexperimentation- incomplete - work on later
//algorithm for ball ray intersection - only works for camera in 0,0,0 pos
//also assumes ball always in front of camera
/*
float hitBall(struct ball ball, int origin[3], float viewDir[3]){
    float intersectionDist;
    if(ball.centre[2]<0){ //ball behind camera
        return -1;
    }
    int distance[3];
    distance[0] = ball.centre[0]-origin[0];
    distance[1] = ball.centre[1]-origin[1];
    distance[2] = ball.centre[2]-origin[2];
    
    float viewLength = dotProduct(viewDir,distance);
    float distToCentreSquared = dotProduct(distance, distance)-viewLength*viewLength;
    if(distToCentreSquared> ball.radius*ball.radius){
        return -1;
    }
    float heightToCentre = sqrtf(ball.radius*ball.radius - distToCentreSquared); //consider changing to fast sqrt algo
    return viewLength - heightToCentre;
}

void drawBall(struct ball ball, int origin[3], int short colour){
    int dz = origin[2]-ball.centre[2];
    if (ball.centre[2] ==origin[2]){
        return;
    }
	int topBoundY;
    int bottomBoundY;
    int leftBoundX;
    int rightBoundX;
	float distanceToPoint=0;
	int currentPoint[3]= ball.centre;
	float viewDir[3];
	float length = sqrtf(powf(currentPoint[0]-origin[0],2)+powf(currentPoint[1]-origin[1],2)+powf(currentPoint[2]-origin[0],2));
	viewDir[0] =  (currentPoint[0]-origin[0])/length;
	viewDir[1] =  (currentPoint[1]-origin[1])/length;
	viewDir[2] =  (currentPoint[2]-origin[2])/length;
	//= set to centre of ball
	while (distanceToPoint!=-1){
		distanceToPoint = hitBall(ball,origin, viewDir);
		projectPixel(colour, currentPoint[3], origin);
	}

    
    
    //int dx = point3D[0]-origin[0];
    //int dy = point3D[1]-origin[1];
    //int x = (dx/dz)*SCREENX/2+SCREENX/2;
    //int y = (dy/dz)*SCREENX/2+SCREENX/2;
    

}
*/

