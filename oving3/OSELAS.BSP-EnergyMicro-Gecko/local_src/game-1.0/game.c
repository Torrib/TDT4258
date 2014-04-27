#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "cross.h"
#include "circle.h"
#include "efm32gg.h"

#define BACKGROUND 0x0000
#define FOREGROUND 0x7777
#define HIGHLIGHT 0x3333

int framebuffer;
int gamepad;

// The memory mapped screen framebuffer location
uint16_t *screen;

//TICTACTOE

// Prototypes for tictac toe
int init_tictactoe();
int hasWon();
int check_move(int x, int y);
void move(int x, int y);
void drawLocation();
void select_frame();
static void interrupt_handler(int, siginfo_t*, void*);

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
    printf("Setup starting\n");

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
    if(ioctl(framebuffer, FBIOGET_FSCREENINFO, &finfo))
    {
         printf("Error reading variable framebuffer info\n");
         return 1;
    }

    //Open the gamepad driver and exit if an error occurs
    gamepad = open("/dev/gamepad", O_RDWR);
    if(gamepad < 0)
    {
        printf("Error: opening gamepad file\n");
        return 1;
    }

    //Calculates the framebuffer size based on the screens resolution and pixelsize
    framebuffer_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    //Memory map the screen and framebuffer
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

    //Clear the entire screen
    memset(screen, BACKGROUND, framebuffer_size);
    rect.dx = 0;
    rect.dy = 0;
    rect.width = 320;
    rect.height = 240;
    ioctl(framebuffer, 0x4680, &rect);

    //Starts the game
    init_tictactoe();

      __asm("wfi");

    return 0;
}


int init_tictactoe()
{
    printf("Game Starting!\n");

    //Initiate the board 
    for (int x = 0; x < 3; x++) 
    {
        for (int y = 0; y < 3; y++) 
        {
            board[x][y] = 0;
            drawLocation(y, x);
        }
    }

    //Run while the game has not finished
    // while (running == 1) 
    // {
    //     sleep(1000);
    // }

    return 0;
}

void drawLocation(int y, int x)
{
	bool active = false;

	//Offset for our working area
	int xOffset = 10*x + 100*x;
	int yOffset = 10*y + 73*y;

	//Mark the currently active square
	if(posX == x && posY == y)
	{
		active = true;

		for(int y = 0; y < image_circle.height; y++)
			for(int x = 0; x < image_circle.width; x++)
			{
				screen[((yOffset + y) * 320) + xOffset + x] = HIGHLIGHT;
			}
	}
	else // Redraw the previous active square
		for(int y = 0; y < image_circle.height; y++)
			for(int x = 0; x < image_circle.width; x++)
				screen[((yOffset + y) * 320) + xOffset + x] = FOREGROUND;


    //Draw symbols
    if(board[x][y] ==  1)
    {
		for(int yy = 0; yy < image_cross.height; yy++)
			for(int xx = 0; xx < image_cross.width; xx++)
			{
                //Gets the color and position of the symbol
				long int pos = (image_cross.width * yy + xx) * image_cross.bytes_per_pixel;
				uint16_t color = image_cross.pixel_data[pos];

				//Skip if the color is suposed to be blank
				if(color == 0)
					continue;

				screen[((yOffset + yy) * 320) + xOffset + xx] = color;
			}

    }
    else if (board[x][y] == 2)
    {
		for(int yy = 0; yy < image_circle.height; yy++)
        {
			for(int xx = 0; xx < image_circle.width; xx++)
			{
                //Gets the color and position of the symbol
				long int pos = (image_circle.width * yy + xx) * image_circle.bytes_per_pixel;
				uint16_t color = image_circle.pixel_data[pos];

				//Skip if the color is suposed to be blank
				if(color == 0)
					continue;

				screen[((yOffset + yy) * 320) + xOffset + xx] = color;
			}
        }
	}
    //Redraw the squares background color
	else if(!active)
		for(int y = 0; y < image_circle.height; y++)
			for(int x = 0; x < image_circle.width; x++)
				screen[((yOffset + y) * 320) + xOffset + x] = FOREGROUND;

    //Redraws the exact squre that was changed
	rect.dx = xOffset;
    rect.dy = yOffset;
    rect.width = 100;
    rect.height = 73;
    ioctl(framebuffer, 0x4680, &rect);
}

void interrupt_handler(int n, siginfo_t *info, void *unused) 
{
    uint8_t buttons = (uint8_t) (info->si_int);

    int newx = posX;
    int newy = posY;

    //Checks the button pressed values
    if(buttons == 1)
    {
        newx--;
    }
    else if(buttons == 2)
    {
        newy--;
    }
    else if(buttons == 4)
    {
        newx++;
    }
    else if(buttons == 8)
    {
        newy++;
    }
    else if(buttons == 128)
    {
        //Checks if the square is taken, if it is free the player will pick it
        if(board[posX][posY] == 0)
            select_frame();
        else
            printf("Square taken\n");
    }

    if(newy != posY || newx != posX)
    {
        //Checks if the move is legal, if it is, the active square will be changed
        if(check_move(newx, newy) == 1)
            move(newx, newy);
        else
            printf("Illegal move\n");
    }
}

//Checks id a move is legal
int check_move(int x, int y)
{
    if(x > 2 || x < 0 || y > 2 || y < 0)
        return 0;

    return 1;
}

//Moves the active square
void move(int x, int y)
{
	int tempY = posY;
	int tempX = posX;
    posX = x;
    posY = y;

	//Draw the newly selected square
	drawLocation(posY, posX);

	//Redraw the previously selected square
	drawLocation(tempY, tempX);
}

//Picks the selected frame
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

    //Checks diagonals
    if((board[0][0] == active_player) && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2]))
        return 1;
    

    if((board[0][2] == active_player) && (board[0][2] == board[1][1]) && (board[1][1] == board[2][0]))
        return 1;
    
    
    return 0;
}
