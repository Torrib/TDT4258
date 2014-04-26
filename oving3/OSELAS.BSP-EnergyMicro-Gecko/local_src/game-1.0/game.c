#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdint.h>

int framebuffer;
int gamepad;

static void game_stuff(void);

int main(int argc, char *argv[])
{
	printf("Starting game");

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
	sigaction(5, &sign, NULL);


	while(1)
	{

	}

	//game_stuff();
}

void interrupt_handler(int n, siginfo_t *info, void *unused) {
	printf("Interrupt!\n")
	//buttons = (uint8_t) ~(info->si_int);
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
