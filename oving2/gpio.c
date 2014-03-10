#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to set up GPIO mode and interrupts*/
void setupGPIO()
{
    /* Sets input and output pins for the joystick */

	//Enable GPIO clock
    *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;
	//Set high drive strength
    *GPIO_PA_CTRL = 2;
	//Set pins A8-A15 to outputs
    *GPIO_PA_MODEH = 0x55555555;

    /*Set pins 0-7 to input*/
    *GPIO_PC_MODEL = 0x33333333;
    //Setup internal pull-up
	*GPIO_PC_DOUT = 0xff;

    //Enable interrupts
    *GPIO_EXTIPSELL = 0x22222222;

    *GPIO_EXTIRISE = 0xff; //Activate on rise
    *GPIO_EXTIFALL = 0xff; //Activate on fall

    *GPIO_IEN = 0xff;
}



