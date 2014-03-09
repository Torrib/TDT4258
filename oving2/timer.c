#include <stdint.h>
#include <stdbool.h>

#include "timer.h"
#include "efm32gg.h"

/* function to setup the timer */
void setupTimer(uint16_t period)
{
  /*
    Enable and set up the timer

    1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0
    2. Write the period to register TIMER1_TOP
    3. Enable timer interrupt generation by writing 1 to TIMER1_IEN
    4. Start the timer by writing 1 to TIMER1_CMD

    This will cause a timer interrupt to be generated every (period) cycles. Remember to configure the NVIC as well, otherwise the interrupt handler will not be invoked.
  */

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

