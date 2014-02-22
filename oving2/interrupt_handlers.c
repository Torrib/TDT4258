#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"

int noise = 0;
int frequency = 0;

int counter = 0;
int i = 200;

int song[] = { 146, 196, 130, 146, 146 };


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	//Clearing pending interrupt
    *TIMER1_IFC = 1;

	*DAC0_CH0DATA = noise;
    *DAC0_CH1DATA = noise;

	*TIMER1_TOP = i;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
    gpio_handler();
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
    gpio_handler();
}

void gpio_handler()
{
	//i += 25;
	noise += 25;

	//Reset GPIO interupts
    *GPIO_IFC = 0xff;

	//Enables coresponding led on button press
    uint32_t input = *GPIO_PC_DIN;
    input = input << 8;
    input = input ^ 0;

    *GPIO_PA_DOUT = input;
}

