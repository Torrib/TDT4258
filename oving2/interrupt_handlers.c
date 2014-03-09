#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "music.h"
#include "timer.h"

#define BUTTON_CLICK(var, pos) ((var) & (1<<(pos)))


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	//Resets timer1 interrupt
    *TIMER1_IFC = *TIMER1_IF;

    //Change amplitude
    musicInterrupt();
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
    gpio_handler();
	
	//Reset GPIO interupt
    *GPIO_IFC = *GPIO_IF; 
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
    gpio_handler();

	//Reset GPIO interupt
    *GPIO_IFC = *GPIO_IF; 
}

void gpio_handler()
{
	uint32_t input = *GPIO_PC_DIN;
   	
    /* Hightlight the pressed led */
	input = input << 8;
    input = input ^ 0;
    *GPIO_PA_DOUT = input;

    //Haavard bruker GPIO_IF (finne i dokumentasjon)
    // GPIO_IFC skal settes til denne verdien også.
    if(BUTTON_CLICK(*GPIO_IF, 7))
    {
        /* Start timer and set correct song */
        musicSetSong(0);
        startTimer();
    }
    
	if(BUTTON_CLICK(*GPIO_IF, 6))
    {
        /* Start timer and set correct song */
        musicSetSong(1);
        startTimer();
    }

	
	if(BUTTON_CLICK(*GPIO_IF, 5))
    {
        /* Start timer and set correct song */
        musicSetSong(2);
        startTimer();
    }
}
