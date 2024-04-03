#include <stdlib.h> 
#include <stdbool.h>
#include <stdio.h>
#include <graphics.h>

 int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];
short int tempFrame[240*320];

const int SCREENX = 320;
const int SCREENY = 240;

const int PLAYER_LOC_Z = -250;
const int GROUND_Y = -350;
const int OPPONENT_LOC_Z = -1100;
short int origin[3] = {0,90,0};
const short int heightBall=30;
struct ball gameBall ;
struct plane table;
struct shadow ballShadow;
volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
float GRAVITY = 2;


int position[3];//gloabal position variable


void startGraphics();
short int colour_packing(short int R, short int G,short  int B);
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_line(short int x0, short int y0, short int x1, short int y1,short int line_colour);
void clear_screen();
// void colour_unpack();
// float hitBall(struct ball ball, int origin[3], float viewDir[3]);
// void drawBall(struct ball ball, int origin[3], int short colour);
void projectPixel(short int colour, float point3D[3], short int origin[3],
short int *x, short int *y);
void eraseSimpleBall();
void simpleDrawBall(short int origin[3]);
void updateLocation();
void setUpGame( short int startPosition, short int hitTime);
void updateFrame();
void bounceBall(short int hitTime, short int startPosition, short int nextPosition);
void initilizePlane();
void drawPlane();
void saveFrame();
void drawImgBackground(const short image[]);
void scaleGravity(int hitTime);

void eraseShadow(){ //change the 50 to larger number if shadow gets large
    //for(short int x = ballShadow.pastCentre[0]-ballShadow.pastRadiusScreen; x<ballShadow.pastCentre[0]+ballShadow.pastRadiusScreen; x++){
    for(short int x = ballShadow.pastCentre[0]-50; x<ballShadow.pastCentre[0]+50&&x<SCREENX&&x>0; x++){
        //for(short int y = ballShadow.pastCentre[1]-ballShadow.pastRadiusScreen; y<ballShadow.pastCentre[1]+ballShadow.pastRadiusScreen; y++){
        for(short int y = ballShadow.pastCentre[1]-50; y<=ballShadow.pastCentre[1]+50&&y<SCREENY&&y>0; y++){
            //plot_pixel(x,y,tempFrame[x+y*SCREENX]);
            plot_pixel(x,y, tempFrame[x+y*SCREENX]);
        }
    }
}

void drawShadow(){
	//skylight/ plane of light going straight down
    float lightDistance = 400;
    int radiusProjection = (float)gameBall.radius/(lightDistance-gameBall.centre[1])*(lightDistance-GROUND_Y);
    
    
    int x = radiusProjection;
    int y = 0;
    int xChange = 1 - (radiusProjection << 1);
    int yChange = 0;
    int radiusError = 0;
    float shadowPoint[3] = {gameBall.centre[0],GROUND_Y,gameBall.centre[2]};
    short int drawPoint[2];
    projectPixel(0,shadowPoint,origin, &(ballShadow.pastCentre[0]),&(ballShadow.pastCentre[1]));
    //ballShadow.pastRadiusScreen = -radiusProjection/(gameBall.centre[2])*SCREENX;

    while (x >= y)
    {
        for (int i = gameBall.centre[0] - x; i <= gameBall.centre[0] + x; i++)
        {
            shadowPoint[0] = i;
            shadowPoint[2] = gameBall.centre[2]+y;
            if(shadowPoint[2]>OPPONENT_LOC_Z&&shadowPoint[2]<PLAYER_LOC_Z){
                projectPixel(0,shadowPoint, origin, &(drawPoint[0]), &(drawPoint[1]));
                plot_pixel(drawPoint[0], drawPoint[1], 0);
            }

            shadowPoint[2] = gameBall.centre[2]-y;
            if(shadowPoint[2]>OPPONENT_LOC_Z&&shadowPoint[2]<PLAYER_LOC_Z){
                projectPixel(0,shadowPoint, origin, &(drawPoint[0]), &(drawPoint[1]));
                plot_pixel(drawPoint[0], drawPoint[1], 0);
            }
        }
        for (int i = gameBall.centre[0] - y; i <= gameBall.centre[0] + y; i++)
        {
            shadowPoint[0] = i;
            shadowPoint[2] = gameBall.centre[2] +x;
            if(shadowPoint[2]>OPPONENT_LOC_Z&&shadowPoint[2]<PLAYER_LOC_Z){
                projectPixel(0,shadowPoint, origin, &(drawPoint[0]), &(drawPoint[1]));
                plot_pixel(drawPoint[0], drawPoint[1], 0);
            }
            shadowPoint[2] = gameBall.centre[2] -x;
            if(shadowPoint[2]>OPPONENT_LOC_Z&&shadowPoint[2]<PLAYER_LOC_Z){
                projectPixel(0,shadowPoint, origin, &(drawPoint[0]), &(drawPoint[1]));
                plot_pixel(drawPoint[0], drawPoint[1], 0);
            }
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
    
}

void saveFrame(){
	short int *one_pixel_address;
	for(int x = 0; x<SCREENX; x++){
		for(int y=0; y<SCREENY; y++){
			one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
			tempFrame[x+y*SCREENX] = *one_pixel_address;
		}
	}
}
void drawImgBackground(const short image[]){ 
	//draws static image on both buffers, will stay there, no neeed to redraw
	for(int x = 0; x<SCREENX; x++){
		for(int y=0; y<SCREENY; y++){
			plot_pixel(x,y,image[x+y*SCREENX]);
		}
	}
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	for(int x = 0; x<SCREENX; x++){
		for(int y=0; y<SCREENY; y++){
			plot_pixel(x,y,image[x+y*SCREENX]);
		}
	}
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	
}

void initilizePlane(){
    table.colour = colour_packing(5,7,0);
    float tempCorner[3] ={(float)PLAYER_LOC_Z, (float)GROUND_Y,(float)OPPONENT_LOC_Z};//start witth top left corner
    projectPixel(table.colour, tempCorner, origin, &(table.point1[0]),&(table.point1[1]));
    //plot_pixel(table.point1[0], table.point1[1],table.colour);
    //top right corner
    tempCorner[0] = -(float)PLAYER_LOC_Z;
    tempCorner[1] = (float)GROUND_Y;
    tempCorner[2] = (float)OPPONENT_LOC_Z;
    projectPixel(table.colour, tempCorner, origin, &(table.point2[0]), &(table.point2[1]));
   // plot_pixel(table.point2[0], table.point2[1],table.colour);
    //bottom left corner
    tempCorner[0] = (float)PLAYER_LOC_Z;//to get ball off of edge add 10
    tempCorner[1] = (float)GROUND_Y;
    tempCorner[2] = (float)PLAYER_LOC_Z;
    projectPixel(table.colour, tempCorner,origin, &(table.point3[0]), &(table.point3[1]));
    //plot_pixel(table.point3[0], table.point3[1],table.colour);
    //bottom right corner
    tempCorner[0] = -(float)PLAYER_LOC_Z;
    tempCorner[1] = (float)GROUND_Y;
    tempCorner[2] = (float)PLAYER_LOC_Z;
    projectPixel(table.colour, tempCorner, origin, &(table.point4[0]), &(table.point4[1]));
    //plot_pixel(table.point4[0], table.point4[1],table.colour);
}

void drawPlane(){
	for(int j =0; j<=1; j++){
		short int x0Left = table.point1[0];
		short int y0Left = table.point1[1];
		short int x1Left = table.point3[0];
		short int y1Left = table.point3[1];

		short int x0Right = table.point2[0];
		short int y0Right = table.point2[1];
		short int x1Right = table.point4[0];
		short int y1Right = table.point4[1];
		short int temp;

		int deltayLeft = y1Right-y0Right;
		int deltayRight = y1Right-y0Right;
		int deltaxRight = abs(x1Right-x0Right);
		int deltaxLeft = abs(x1Left-x0Left);
		int errorRight = -(deltayRight/2);
		int errorLeft = -(deltayLeft/2);
		int xRight= x0Right;
		int xLeft = x0Left;
		int x_stepLeft=-1;
		int x_stepRight=1;
		

		for(int yLeft =y0Left; yLeft<y1Left&&yLeft<SCREENY; yLeft++){
				plot_pixel(xLeft,yLeft,table.colour);
				for(int across = xLeft+1; across<xRight; across++){
					plot_pixel(across,yLeft,table.colour);
				}

			errorRight = errorRight +deltaxRight;
			errorLeft = errorLeft + deltaxLeft;
			if(errorRight>0){
				xRight=xRight+x_stepRight;
				errorRight = errorRight -deltayRight;
			}
			if (errorLeft>0){
				xLeft=xLeft+x_stepLeft;
				errorLeft = errorLeft -deltayLeft;
			}
		}
		//draw midline
		
		short int tempPoint1[2] = {(table.point1[0]+table.point2[0])/2, table.point1[1]};
		short int tempPoint2[2] = {(table.point3[0]+table.point4[0])/2, table.point3[1]};
		draw_line(tempPoint1[0], tempPoint1[1], tempPoint2[0], tempPoint2[1],colour_packing(25,30,25));
		
		//draw net
		short int netLeft[2] = {(table.point1[0]-table.point3[0])*2/3+table.point3[0], (table.point3[1]-table.point1[1])/3+table.point1[1]};
		short int netRight[2] = {(table.point4[0]-table.point2[0])/3+table.point2[0], (table.point4[1]-table.point2[1])/3+table.point2[1]};
		for (int height=0; height <10; height+=2){
			draw_line(netLeft[0], netLeft[1]-height, netRight[0], netRight[1]-height,colour_packing(25,30,25));
		}
		for (int width=netRight[0]-netLeft[0]; width >0; width-=(netRight[0]-netLeft[0])/30){
			draw_line(netRight[0]-width, netRight[1]-10, netRight[0]-width, netRight[1],colour_packing(25,30,25));
		}
		draw_line(netLeft[0], netLeft[1]-10, netRight[0], netRight[1]-10,colour_packing(25,20,30));
		
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	}

}
void scaleGravity(int hitTime){
	//adapt gravity based on ball speed - values chosen based on what looks good
    /*
    <15 g = 12
    15 - 20 g =8
    20-25 g = 5
    25 - 30 - g = 3
    30 - 60 g = 1.5
    */
    if(hitTime <15){
    	GRAVITY = 12;
    }
    else if(hitTime <20){
    	GRAVITY = 8;
    }
    else if(hitTime <25){
    	GRAVITY = 5;
    }
    else if(hitTime <30){
    	GRAVITY = 3;
    }
    else if(hitTime <60){
    	GRAVITY = 1.5;
    }
    else{
    	GRAVITY = 0.8;
    }
}

void startGraphics()
{
    gameBall.colour = colour_packing(31, 0,0);
	gameBall.radius = 10;//no effect atm
    // declare other variables(not shown)
	
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

	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
	
}

void bounceBall(short int hitTime, short int startPosition, short int nextPosition){ 
	scaleGravity(hitTime);
	gameBall.velocity[1] = -((gameBall.centre[1]-GROUND_Y)-0.5*GRAVITY*(hitTime*(2.5/4.0))*hitTime*(2.5/4.0))/(hitTime*(2.5/4.0));
	if (startPosition == 0){
		gameBall.centre[0] = PLAYER_LOC_Z/2;
		gameBall.centre[2] = OPPONENT_LOC_Z+1;
		if(nextPosition == 2){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 3){
			gameBall.velocity[0] = -PLAYER_LOC_Z/(float)hitTime;
		}
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime;
	}

	else if(startPosition == 2){
		gameBall.centre[0] = PLAYER_LOC_Z/2;
		gameBall.centre[2] = PLAYER_LOC_Z-1;
		if(nextPosition == 0){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 3){
			gameBall.velocity[0] = -PLAYER_LOC_Z/(float)hitTime;
		}
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime;
	}
	else if (startPosition == 1){
		gameBall.centre[0] = -PLAYER_LOC_Z/2;
		gameBall.centre[2] = OPPONENT_LOC_Z+1;
		if(nextPosition == 3){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 2){
			gameBall.velocity[0] = PLAYER_LOC_Z/(float)hitTime;
		}
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime;
	}
	else if(startPosition == 3){
		gameBall.centre[0] = -PLAYER_LOC_Z/2;
		gameBall.centre[2] = PLAYER_LOC_Z-1;
		if(nextPosition == 1){
			gameBall.velocity[0] = 0;
		}
		else if(nextPosition == 0){
			gameBall.velocity[0] = PLAYER_LOC_Z/(float)hitTime;
		}
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime;
	}
	
}

void updateFrame(){
	eraseSimpleBall();
	eraseShadow();
    drawShadow();
	simpleDrawBall(origin); 
	updateLocation();
	wait_for_vsync(); // swap front and back buffers on VGA vertical sync
	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer

}

//used for serves -always diagonal
void setUpGame(short int startPosition, short int hitTime){
	eraseSimpleBall();
	scaleGravity(hitTime);
	if (startPosition == 0){
		gameBall.centre[0] = PLAYER_LOC_Z/2;
		gameBall.velocity[0] = -PLAYER_LOC_Z/(float)hitTime;
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime;
		gameBall.centre[2] = OPPONENT_LOC_Z+1;

	}
	else if(startPosition == 2){
		gameBall.centre[0] = PLAYER_LOC_Z/2;
		gameBall.velocity[0] = -PLAYER_LOC_Z/(float)hitTime;
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime;
		gameBall.centre[2] = PLAYER_LOC_Z-1;
	}
	else if (startPosition == 1){
		gameBall.centre[0] = -PLAYER_LOC_Z/2;
		gameBall.velocity[0] = PLAYER_LOC_Z/(float)hitTime;
		gameBall.velocity[2] = (float)(PLAYER_LOC_Z-OPPONENT_LOC_Z)/(float)hitTime;
		gameBall.centre[2] = OPPONENT_LOC_Z+1;
	}
	else if(startPosition == 3){
		gameBall.centre[0] = -PLAYER_LOC_Z/2;
		gameBall.velocity[0] = PLAYER_LOC_Z/(float)hitTime;
		gameBall.velocity[2] = (float)(OPPONENT_LOC_Z-PLAYER_LOC_Z)/(float)hitTime;
		gameBall.centre[2] = PLAYER_LOC_Z-1;
	}
	
	gameBall.centre[1] = GROUND_Y+heightBall; //100 = height from table 
	gameBall.pastScreenLoc[0] = 0;
	gameBall.pastScreenLoc[1] = 0;
	gameBall.pastProjectedRadius = 0;
	gameBall.currentProjectedRadius = 0;
	gameBall.velocity[1] = -((heightBall)-0.5*GRAVITY*(hitTime*(2.5/4.0))*hitTime*(2.5/4.0))/(hitTime*(2.5/4.0));
	simpleDrawBall(origin); 
	updateLocation();
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

void draw_line(short int x0, short int y0, short int x1, short int y1,short int line_colour){
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
	colour = (B<<1) +(G<<6)+(R<<11);
	return colour;
}


float dotProduct(int A[3], int B[3]){
    return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
    
}
//following function only works with camera shifting vertically
void projectPixel(short int colour, float point3D[3], short int origin[3],
short int *x, short int *y){
    float dz = point3D[2]-origin[2];
    if (dz ==0){
        return;
    }
    float dx = (float)point3D[0]-(float)origin[0];
    float dy = point3D[1]-origin[1];
    *x = ((dx/-dz)*(float)SCREENX/2)+SCREENX/2;
    *y = ((dy/dz)*(float)SCREENY/2)+SCREENY/2-origin[1];
	//printf(" 3d x %f, y %f, z, %f", point3D[0],point3D[1],point3D[2]);
	printf("x location screen %d, ", *x);
	printf("\n");
	printf("y location screen %d", *y);
	printf("\n");
	if(*y>=SCREENY-1||*y<0||*x<0||*x>=SCREENX-1){//ensures pixel not drawn outside if screen
		return;
	}
    plot_pixel(*x,*y,colour);
	plot_pixel(*x+1,*y,colour);
	plot_pixel(*x+1,*y+1,colour);
	plot_pixel(*x,*y+1,colour);
}
void simpleDrawBall( short int origin[3]){
	gameBall.pastScreenLoc[0] = gameBall.screenLoc[0];
	gameBall.pastScreenLoc[1] = gameBall.screenLoc[1];
	gameBall.pastProjectedRadius = gameBall.currentProjectedRadius;
	//safeguard to ensuree ball does not go out of bounds
	//if(gameBall.screenLoc[0]<0||gameBall.screenLoc[0]>=SCREENX||gameBall.screenLoc[1]<0||gameBall.screenLoc[1]>=SCREENY){
		//gameBall.velocity[0] = 0;
		//gameBall.velocity[1] = 0;
		//gameBall.velocity[2] = 0;
	//}
	//projectradius to find screen size
	gameBall.currentProjectedRadius = -gameBall.radius/(gameBall.centre[2]-origin[2])*SCREENX;
    if(gameBall.screenLoc[1]>=SCREENY-1||gameBall.screenLoc[1]<0||gameBall.screenLoc[0]<0||gameBall.screenLoc[0]>=SCREENX-1){//ensures pixel not drawn outside if screen
		return;
	}
	projectPixel(gameBall.colour, gameBall.centre, origin, &(gameBall.screenLoc[0]),&(gameBall.screenLoc[1]));
	//draw ball based on radius current and centre screenLoc

    int x = gameBall.currentProjectedRadius;
    int y = 0;
    int xChange = 1 - (gameBall.currentProjectedRadius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = gameBall.screenLoc[0] - x; i <= gameBall.screenLoc[0] + x; i++)
        {
            plot_pixel(i, gameBall.screenLoc[1] + y, gameBall.colour);
            plot_pixel(i, gameBall.screenLoc[1] - y,gameBall.colour);
        }
        for (int i = gameBall.screenLoc[0] - y; i <= gameBall.screenLoc[0] + y; i++)
        {
            plot_pixel(i, gameBall.screenLoc[1] + x,gameBall.colour);
            plot_pixel(i, gameBall.screenLoc[1] - x,gameBall.colour);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
    // plot_pixel(gameBall.screenLoc[0],gameBall.screenLoc[1],gameBall.colour);
	// plot_pixel(gameBall.screenLoc[0]+1,gameBall.screenLoc[1],gameBall.colour);
	// plot_pixel(gameBall.screenLoc[0]+1,gameBall.screenLoc[1]+1,gameBall.colour);
	// plot_pixel(gameBall.screenLoc[0],gameBall.screenLoc[1]+1,gameBall.colour);	
}
void eraseSimpleBall(){

    short int x = gameBall.pastProjectedRadius;
    short int y = 0;
    short int xChange = 1 - (gameBall.pastProjectedRadius << 1);
    short int yChange = 0;
    short int radiusError = 0;
    while (x >= y)
    {
        for (short int i = gameBall.pastScreenLoc[0] - x; i <= gameBall.pastScreenLoc[0] + x; i++)
        {
            plot_pixel(i, gameBall.pastScreenLoc[1] + y, tempFrame[i +(gameBall.pastScreenLoc[1] + y)*SCREENX]);
            plot_pixel(i, gameBall.pastScreenLoc[1] - y,tempFrame[i +(gameBall.pastScreenLoc[1] - y)*SCREENX]);
        }
        for (short int i = gameBall.pastScreenLoc[0] - y; i <= gameBall.pastScreenLoc[0] + y; i++)
        {
            plot_pixel(i, gameBall.pastScreenLoc[1] + x,tempFrame[i +(gameBall.pastScreenLoc[1] + x)*SCREENX]);
            plot_pixel(i, gameBall.pastScreenLoc[1] - x,tempFrame[i +(gameBall.pastScreenLoc[1] - x)*SCREENX]);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

void updateLocation(){ //only bounded in z, y direction
	if(gameBall.centre[1]<= GROUND_Y){
		gameBall.velocity[1] =  gameBall.velocity[1]*(-1);
	}
	//change position based on current velocity
	gameBall.centre[0] += gameBall.velocity[0];
	gameBall.centre[1] += gameBall.velocity[1];
	gameBall.centre[2] += gameBall.velocity[2];
	gameBall.velocity[1] -=GRAVITY; //change vertical acceleration
}

