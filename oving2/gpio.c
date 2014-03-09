#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to set up GPIO mode and interrupts*/
void setupGPIO()
{
    /* Sets input and output pins for the joystick */

    /* Example of HW access from C code: turn on joystick LEDs D4-D8
    check efm32gg.h for other useful register definitions
    */
    *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO; /* enable GPIO clock*/
    *GPIO_PA_CTRL = 2;  /* set high drive strength */
    *GPIO_PA_MODEH = 0x55555555; /* set pins A8-15 as output */

    /*Set input pins*/
    *GPIO_PC_MODEL = 0x33333333; //Set pins 0-7 to input
    *GPIO_PC_DOUT = 0xff; //Set internal pull-up

    /*Interrupts*/
    *GPIO_EXTIPSELL = 0x22222222; //??

    *GPIO_EXTIRISE = 0xff; //Activate on rise
    *GPIO_EXTIFALL = 0xff; //Activate on fall

    *GPIO_IEN = 0xff; //Enable interrupt generation
}



