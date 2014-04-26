#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>

int framebuffer;
int gamepad;

int main(int argc, char *argv[])
{
	printf("TBA");
}

// 	//Open the framebuffer file
// 	framebuffer = open("/dev/fb0", O_RDWR);

// 	if(framebuffer < 0)
// 	{
// 		printf("Error opening frame buffer\n");
// 		return 1;
// 	}

// 	//Map the framebuffer to the memory
// 	if(map_file(&framebuffer, fd_framebuffer, FB_SIZE) != 0)
// 	{
// 		printf("Error: Mapping buffer to memory");
// 		return 1;
// 	}

// 	gamepad = open("/dev/gamepad", O_RDWR);
// 	if(gamepad < 0) 
// 	{
// 		printf("Error: opening gamepad file\n");
// 		return 1;
// 	}

// 	game_stuff();


// }

// void game_stuff()
// {
// 	memset(framebuffer, 0x0000, FB_SIZE);

// 	struct fb_copyarea rect;

// 	rect.x = 100;
// 	rect.y = 100;
// 	rect.width = 30;
// 	rect.height = 30;

// 	ioctl(framebuffer, 0x4680, &rect);
// }
