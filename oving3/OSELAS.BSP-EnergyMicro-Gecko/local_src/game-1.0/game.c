#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <linux/fb.h>
#include <inttypes.h>
#include <stdbool.h>

#include "cross.h"
#include "circle.h"

#define BACKGROUND 0x0000
#define FOREGROUND 0x7777
#define HIGHLIGHT 0x3333

int framebuffer;
int gamepad;

// The memory mapped screen framebuffer location
uint16_t *screen;

//TICTACTOE

// Prototypes for tictac toe
void initialize_board();
int hasWon();
int check_move(int x, int y);
void move(int x, int y);
int frame_available();
int init_tictactoe();
void drawGame();
void drawLocation();
void select_frame();

static void interrupt_handler(int, siginfo_t*, void*);
void tictactoe_event(uint8_t event);

int posX = 0;
int posY = 0;
int active_player = 1;
int running = 1;

int board[3][3]; 

//Consts for drawing
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

struct fb_copyarea rect;

long int framebuffer_size;

int main(int argc, char *argv[])
{
    printf("Starting game\n");

    //Open the framebuffer file
    framebuffer = open("/dev/fb0", O_RDWR);

    //Exit if the framebuffer file does not exist or can't be opened.
    if(framebuffer < 0)
    {
        printf("Error opening frame buffer\n");
        return 1;
    }


    //Gets static information about the framebuffer (Screensize etc)
    if(ioctl(framebuffer, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading fixed framebuffer info\n");
        return 1;
    }

    //Gets dynamic information about the framebuffer (?)
    // if(ioctl(framebuffer, FBIOGET_VSCREENINFO, &vinfo))
    // {
    //     printf("Error reading variable framebuffer info\n");
    //     return 1;
    // }

    gamepad = open("/dev/gamepad", O_RDWR);
    if(gamepad < 0)
    {
        printf("Error: opening gamepad file\n");
        return 1;
    }

    framebuffer_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    //Initiate the screen
    screen = (uint16_t *) mmap(NULL, framebuffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer, 0);

    if(*screen == -1)
    {
        printf("Error: Mapping memory failed with code %"PRIu16"\n", screen);
        return 1;
    }

    // Setup signal handling
    struct sigaction sign;
    sign.sa_sigaction = interrupt_handler;
    sign.sa_flags = SA_SIGINFO;
    sigaction(50, &sign, NULL);

    char pid_array[5];
    sprintf(pid_array, "%d", getpid());
    write(gamepad, pid_array, strlen(pid_array) +1);

    memset(screen, BACKGROUND, framebuffer_size);
	//Setup the draw area
    rect.dx = 0;
    rect.dy = 0;
    rect.width = 320;
    rect.height = 240;
    ioctl(framebuffer, 0x4680, &rect);

    // The game begins!
    init_tictactoe();

    return 0;
}

void drawGame()
{
    //Draw all the squares
    for(int y = 0; y < 3; y++)
        for(int x = 0; x < 3; x++)
			drawLocation(y, x);


    //Command driver to update display
    //ioctl(framebuffer, 0x4680, &rect);
}

void drawLocation(int y, int x)
{
	bool marker = false;

	//Offset for our working area
	int xOffset = 10*x + 100*x;
	int yOffset = 10*y + 73*y;

	//Draw the current marker field
	if(posX == x && posY == y)
	{
		marker = true;

		for(int y = 0; y < image_circle.height; y++)
			for(int x = 0; x < image_circle.width; x++)
			{
				screen[((yOffset + y) * 320) + xOffset + x] = HIGHLIGHT;
			}
	}
	else // Remove the marked location showing
		for(int y = 0; y < image_circle.height; y++)
			for(int x = 0; x < image_circle.width; x++)
				screen[((yOffset + y) * 320) + xOffset + x] = FOREGROUND;
    	//memset(screen + yOffset * vinfo.xres + xOffset, 0x0000, 100 * 73 * vinfo.bits_per_pixel / 8);

	/* Draw game items of there is any */
   if(board[x][y] ==  1)
    {
		for(int yy = 0; yy < image_cross.height; yy++)
			for(int xx = 0; xx < image_cross.width; xx++)
			{
				long int pos = (image_cross.width * yy + xx) * image_cross.bytes_per_pixel;
				uint16_t color = image_cross.pixel_data[pos];

				//Do not draw if nothing
				if(color == 0)
					continue;

				//printf("%d %d %d\n", xx, yy, color);
				screen[((yOffset + yy) * 320) + xOffset + xx] = color;
			}

    }
    else if (board[x][y] == 2){
		//Offset for line
		int xOffset = 10*x + 100*x;
		int yOffset = 10*y + 73*y;

		for(int yy = 0; yy < image_circle.height; yy++)
			for(int xx = 0; xx < image_circle.width; xx++)
			{
				long int pos = (image_circle.width * yy + xx) * image_circle.bytes_per_pixel;
				uint16_t color = image_circle.pixel_data[pos];

				//Do not draw if nothing
				if(color == 0)
					continue;

				screen[((yOffset + yy) * 320) + xOffset + xx] = color;
			}
	}
	else if(!marker)
		for(int y = 0; y < image_circle.height; y++)
			for(int x = 0; x < image_circle.width; x++)
				screen[((yOffset + y) * 320) + xOffset + x] = FOREGROUND;

	rect.dx = xOffset;
    rect.dy = yOffset;
    rect.width = 100;
    rect.height = 73;
    ioctl(framebuffer, 0x4680, &rect);
}

void interrupt_handler(int n, siginfo_t *info, void *unused) {
    uint8_t buttons = (uint8_t) (info->si_int);

    //Propagate the event here. [0 4] Up, down, left, right, click
    tictactoe_event(buttons);
}


/*
 * The main init code for tictactoe.
 *
 */
int init_tictactoe()
{
    printf("Game initializing\n");

    initialize_board();

    while (running == 1) {}

    return 0;
}

int check_move(int x, int y)
{
    if(x > 2 || x < 0 || y > 2 || y < 0)
        return 0;

    return 1;
}

void move(int x, int y)
{
	int tempY = posY;
	int tempX = posX;
    posX = x;
    posY = y;
	//Draw the new area	
	drawLocation(posY, posX);

	// Null out the last area
	drawLocation(tempY, tempX);
}

int frame_available()
{
    if(board[posX][posY] == 0)
        return 1;
    
    return 0;
}

void select_frame()
{
    board[posX][posY] = active_player;

    drawLocation(posY, posX);
    if(hasWon() == 1)
    {
        printf("Player %d won!\n", active_player);
        running = 0;
        return;
    }

    if(active_player == 1)
        active_player = 2;
    else
        active_player = 1;
}

int hasWon()
{
    //Check rows
    for (int x = 0; x < 3; x++) 
    {
        if ((board[x][0] == active_player) && (board[x][0] == board[x][1]) && (board[x][1] == board[x][2]))             
            return 1;
        
    }
    //Check columns
    for (int y = 0; y < 3; y++) 
    {
        if ((board[0][y] == active_player) && (board[0][y] == board[1][y]) && (board[1][y] == board[2][y])) 
            return 1;
        
    }

    if((board[0][0] == active_player) && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2]))
        return 1;
    

    if((board[0][2] == active_player) && (board[0][2] == board[1][1]) && (board[1][1] == board[2][0]))
        return 1;
    
    
    return 0;
}

void initialize_board(){
    for (int x = 0; x < 3; x++) 
    {
        for (int y = 0; y < 3; y++) 
        {
            board[x][y] = 0;
        }
    }
	
	drawGame();
}

void tictactoe_event(uint8_t event)
{
    int newx = posX;
    int newy = posY;

    if(event == 1)
    {
        newx--;
    }
    else if(event == 2)
    {
        newy--;
    }
    else if(event == 4)
    {
        newx++;
    }
    else if(event == 8)
    {
        newy++;
    }
    else if(event == 128)
    {
        if(frame_available() == 1)
            select_frame();
        else
            printf("Square taken\n");
    }

    if(newy != posY || newx != posX)
    {
        if(check_move(newx, newy) == 1)
    		move(newx, newy);
        else
            printf("Illegal move\n");
    }

}
