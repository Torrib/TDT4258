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
void drawGame();
void map(uint16_t **region, int fd);

//Consts for drawing

struct fb_copyarea rect;


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

    //Initiate the screen
   	map(&screen, framebuffer);
    printf("%d", *screen);
	
    if(screen == -1)
	{
		printf("Error: Mapping memory failed with code %d \n", screen);
        return 1;
	}
	

	// Setup signal handling
    struct sigaction sign;
    sign.sa_sigaction = interrupt_handler;
    sign.sa_flags = SA_SIGINFO;
    sigaction(42, &sign, NULL);

    sprintf(pid_buf, "%d", getpid());
    write(gamepad, pid_buf, strlen(pid_buf) +1);

    // The game begins!
    init_tictactoe();

	return 0;
}

void map(uint16_t **region, int fd)
{
    *region = (uint16_t *) mmap(NULL, 320 * 240 * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void draw(int x, int y, uint16_t color)
{
    // Find memory location for x and y pos

	//printf("Performing draw.\nvinfo.xoffset=%d\nvinfo.yoffset=%d\nvinfo.bits_per_pixel=%d\nfinfo.line_length=%d\n",
	//	vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel, finfo.line_length);

	//Draw at the location
    //*(screen + location + 1) = 20;
    //*(screen + location + 2) = 200;
    //*(screen + location) = 0;
}

void drawGame()
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

	drawGame();

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

		drawGame();		

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

		drawGame();
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
