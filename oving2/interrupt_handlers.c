#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"


uint32_t loop = 0;
uint32_t superLoop = 0;


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
	*TIMER1_IFC = 1;
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */  
	superLoop++;

	if (superLoop == 10000)
	{
		*GPIO_PA_DOUT = 1 << loop;
		loop++;
		if (loop == 16)
			loop = 0;	
		superLoop = 0;
	}
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	*GPIO_IFC = 0xff;
    /* TODO handle button pressed event, remember to clear pending interrupt */
	//*GPIO_PA_DOUT = 0x0000; /* turn on LEDs D4-D8 (LEDs are active low) */
	gpio_handler();
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	*GPIO_IFC = 0xff;
    /* TODO handle button pressed event, remember to clear pending interrupt */
	//*GPIO_PA_DOUT = 0x0000; /* turn on LEDs D4-D8 (LEDs are active low) */
	gpio_handler();
}


void gpio_handler()
{
	uint32_t input = *GPIO_PC_DIN;
	input = input << 8;
	input = input ^ 0;

	*GPIO_PA_DOUT = input;
}
