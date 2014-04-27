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

#include "tictactoe.h"

int framebuffer;
int gamepad;

static void game_stuff(void);
static void interrupt_handler(int, siginfo_t*, void*);

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

	//Initiate the screen
	uint16_t *screen;
	screen = (uint16_t *) mmap(NULL, 320*240*2, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer, 0);

	if(screen == NULL)
		return 1;

	struct sigaction sign;
	sign.sa_sigaction = interrupt_handler;
	sign.sa_flags = SA_SIGINFO;
	sigaction(42, &sign, NULL);

	sprintf(pid_buf, "%d", getpid());
	write(gamepad, pid_buf, strlen(pid_buf)

	while(1)
	{

	}

	//game_stuff();

    // The game begins!
    init_tictactoe();
}

void interrupt_handler(int n, siginfo_t *info, void *unused) {
	printf("Interrupt!\n");
	//buttons = (uint8_t) ~(info->si_int);

    //Propagate the event here. [0 4] Up, down, left, right, click
    //tictactoe_event(0);
}

void game_stuff(void)
{
	//memset(framebuffer, 0x0000, FB_SIZE);

	// struct fb_copyarea rect;

	// rect.x = 100;
	// rect.y = 100;
	// rect.width = 30;
	// rect.height = 30;

	// ioctl(framebuffer, 0x4680, &rect);
}
