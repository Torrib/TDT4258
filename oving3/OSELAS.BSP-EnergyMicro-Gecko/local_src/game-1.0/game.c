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

#include "cross.h"

int framebuffer;
int gamepad;

// The memory mapped screen framebuffer location
uint16_t *screen;

//TICTACTOE

// Prototypes for tictac toe
void initializeBoard(char[][3]);
void printBoard(char[][3]);
int hasWon(char[][3], int hasTurn);
int moveAllowed(char[][3], int *column,int *row);

//Protos for logic
static void interrupt_handler(int, siginfo_t*, void*);

//Protos for tic
int init_tictactoe();
void tictactoe_event(uint8_t event);

//protos for drawing
/** Low level draw method */
void draw(int x, int y, uint16_t color);
/** Hook for drawing the current game state */
void drawGame(char board[][3]);

//Consts for drawing
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

struct fb_copyarea rect;

long int framebuffer_size;

int main(int argc, char *argv[])
{
    printf("Starting game\n");

    char pid_buf[10];

    //Open the framebuffer file
    framebuffer = open("/dev/fb0", O_RDWR);

    //Exit if the framebuffer file does not exist or cant be opened.
    if(framebuffer < 0)
    {
        printf("Error opening frame buffer\n");
        return 1;
    }


	/* get fixed info*/
	if(ioctl(framebuffer, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error reading fixed framebuffer info\n");
		return 1;
	}

	if(ioctl(framebuffer, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error reading variable framebuffer info\n");
		return 1;
	}

    gamepad = open("/dev/gamepad", O_RDWR);
    if(gamepad < 0)
    {
        printf("Error: opening gamepad file\n");
        return 1;
    }

	//Setup the draw area
	rect.dx = 0;
	rect.dy = 0;
	rect.width = 320;
	rect.height = 240;

	framebuffer_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    //Initiate the screen
    screen = (uint16_t *) mmap(NULL, framebuffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer, 0);

    if(*screen == -1)
	{
		printf("Error: Mapping memory failed with code %"PRIu16"\n", screen);
        return 1;
	}

	printf("Framebuffer size: %ld\nFramebuffer address: %"PRIu16"\n", framebuffer_size, screen);
	printf("vinfo.xoffset=%d\nvinfo.yoffset=%d\nvinfo.bits_per_pixel=%d\nfinfo.line_length=%d\n",
		vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel, finfo.line_length);

	// Setup signal handling
    struct sigaction sign;
    sign.sa_sigaction = interrupt_handler;
    sign.sa_flags = SA_SIGINFO;
    sigaction(50, &sign, NULL);

    sprintf(pid_buf, "%d", getpid());
    write(gamepad, pid_buf, strlen(pid_buf) +1);

    while(1)
    {
        
    }

    // The game begins!
    init_tictactoe();

	return 0;
}

void draw(int x, int y, uint16_t color)
{
    // Find memory location for x and y pos
    long int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel/8)
        + (y + vinfo.yoffset);// * finfo.line_length;

	//Draw at the location
    *(screen + location) = 0xffff;
    //*(screen + location + 1) = 20;
    //*(screen + location + 2) = 200;
    //*(screen + location) = 0;
}

void drawGame(char board[][3])
{
	//Empty the memory
	memset(screen, 0x0000, framebuffer_size);

	/* Draw the board using the draw method*/
    for(int i = 0; i < 100; i++)
        for(int y = 0; y <250; y++)
		{
			//draw(i, y, 100);
			screen[i * 320 + y] = 0xffff;
		}

    for(int y = 0; y < 3; y++)
        for(int x = 0; x < 3; x++)
        {
            if(board[y][x] ==  'X')
            {
                //Offset for line
                int xOffset = 10*x + 100*x;
                int yOffset = 10*y + 74*y;

                for(int yy = 0; yy < cross_image.height; yy++)
                    for(int xx = 0; xx < cross_image.width; xx++)
                    {
                        uint16_t color = cross_image.pixel_data[cross_image.width * yy + xx * cross_image.bytes_per_pixel];

                        screen[xOffset * 320 + yOffset] = color;
                    }

            }
            else if (board[y][x] == 'Y'){}
        }

	//Command driver to update display
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
    printf("Game initializing");

    //hasTurn is 1 when it is player 1, and 2 when it is player two
    int hasTurn = 1;
    int play = 1;
    int column, row;
    char board[3][3];

    initializeBoard(board);

	drawGame(board);

    while (play == 1) {
        while (hasTurn == 1) {
            printBoard(board);
            printf("Player 1 - Choose column 0 - 2 :\n");
            scanf("%d", &column);
            printf("Player 1 - Choose row 0 - 2 :\n");
            scanf("%d", &row);

            //Check if the column and row is allowed
            //Should be in a method

            if(moveAllowed(board, &column, &row) == 1)
                board[column][row] = 'X';
            else
                printf("Column or row not valid - try again.\n");

            //Check if player 1 won
            if (hasWon(board,hasTurn) == 1){
                printf("%s", "Player 1 won!");
                play = 0;
            }
            else
                hasTurn = 2;
        }

		drawGame(board);

        //Do the same for player two - is there a way to do this without the loops?

        while (hasTurn == 2) {
            printBoard(board);
            printf("Player 2 - Choose column 0 - 2 :\n");
            scanf("%d", &column);
            printf("Player 2 - Choose row 0 - 2 :\n");
            scanf("%d", &row);

            //Check if the column and row is allowed
            if(moveAllowed(board, &column, &row) == 1)
                board[column][row] = '0';
            else
                printf("Column or row not valid - try again.\n");

            //Check if player 2 has won
            if (hasWon(board,hasTurn) == 1){
                printf("%s", "Player 1 won!");
                play = 0;
            }
            else
                hasTurn = 1;

        }

		drawGame(board);
    }

    return 0;
}

int moveAllowed(char board[][3], int *column,int *row){
    if((*column > 2
        || *column < 0
        || *row > 2
        || *row < 0)) {

        return 0;
    }

    if(board[*column][*row] == 'X'
       || board[*column][*row] == '0'){
        return 0;
    }

    return 1;
}

int hasWon(char board[][3], int hasTurn){
    //Check rows
    for (int x = 0; x < 3; x++) {
        if ((board[x][0] == 'X') && (board[x][0] == board[x][1]) && (board[x][1] == board[x][2])) {
            printf("%s", "Player 1 won the row \n");
            return 1;
        }
    }
    //Check columns
    for (int y = 0; y < 3; y++) {
        if ((board[0][y] == 'X') && (board[0][y] == board[1][y]) && (board[1][y] == board[2][y])) {
            printf("%s", "Player 1 won the column! \n");
            return 1;
        }
    }

    if((board[0][0] == 'X') && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2])){
        printf("%s", "Player 1 won the diagonal! \n");
        return 1;
    }

    if((board[0][2] == 'X') && (board[0][2] == board[1][1]) && (board[1][1] == board[2][0])){
        printf("%s", "Player 1 won! \n");
        return 1;
    }
    else
        return 0;
}

void initializeBoard(char board[][3]){
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            board[x][y] = '.';
        }
    }

    //Write bits to the screen


    //Update the screen
}

void printBoard(char board[][3]){
    //Use this to add things in the buffer?
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            printf("%c", board[i][x]);
        }
        printf("\n");
    }
}

void tictactoe_event(uint8_t event)
{
    if(event == 1)
    {
        //Left
        printf("Left\n");
    }
    else if(event == 2)
    {
        //Up
        printf("Up\n");
    }
    else if(event == 4)
    {
        //Right
        printf("Right\n");
    }
    else if(event == 8)
    {
        //Down
        printf("Down\n");
    }
    else if(event == 128)
    {
        //Action
        printf("Action\n");
    }

}
