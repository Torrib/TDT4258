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
void initialize_board();
int hasWon();
int check_move(int x, int y);
void move(int x, int y);
int frame_avalable();
int init_tictactoe();
void drawGame(char board[][3]);
void select_frame();

static void interrupt_handler(int, siginfo_t*, void*);
void tictactoe_event(uint8_t event);

int posx = 1;
int posy = 1;
int active_player = 1;

int board[3][3]; 

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

    // Setup signal handling
    struct sigaction sign;
    sign.sa_sigaction = interrupt_handler;
    sign.sa_flags = SA_SIGINFO;
    sigaction(50, &sign, NULL);

    sprintf(pid_buf, "%d", getpid());
    write(gamepad, pid_buf, strlen(pid_buf) +1);

    // The game begins!
    init_tictactoe();

    while(1){}

    return 0;
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

    int play = 1;
    initialize_board();

    //drawGame(board);

    while (play == 1) 
    {
        //TODO indikate active square

    }

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
    posx = x;
    posy = y;
    printf("%d-%d\n", x, y);
    //TODO change active box;
}

int frame_available()
{
    if(board[posx][posy] == 0)
        return 1;
    
    return 0;
}

void select_frame()
{
    board[posx][posy] = active_player;

    if(hasWon() == 1)
    {
        printf("Player %d won!\n", active_player);
        play = 0;
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
}

void tictactoe_event(uint8_t event)
{
    int newx = posx;
    int newy = posy;

    if(event == 1)
    {
        newx--;
        printf("Left\n");
    }
    else if(event == 2)
    {
        newy--;
        printf("Up\n");
    }
    else if(event == 4)
    {
        newx--;
        printf("Right\n");
    }
    else if(event == 8)
    {
        newy++;
        printf("Down\n");
    }
    else if(event == 128)
    {
        if(frame_avalable() == 1)
            select_frame();
        printf("Action\n");
    }

    if(newy != posy || newx != posx)
    {
        if(check_move(newx, newy) == 1)
            move(newx, newy)
    }

}
