#include <stdint.h>
#include <stdbool.h>

#include "timer.h"
#include "efm32gg.h"

/* function to setup the timer */
void setupTimer(uint16_t period)
{
	//Enable clock timer
    *CMU_HFPERCLKEN0 |= 1 << 6;
	
	//Sets the number of clock cycles to skip between interrupts
    *TIMER1_TOP = period;

	//Enable timer interrupt generation
    *TIMER1_IEN = 1;
}

void startTimer()
{
	//Starts timer1
    *TIMER1_CMD = 1;
}

void stopTimer()
{
	// Stops timer1
    *TIMER1_CMD = 2;
    //__asm("wfi");
}

