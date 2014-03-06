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
    *TIMER1_IFC = 1;
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */
    //*DAC0_CH0DATA = noise;
    //*DAC0_CH1DATA = noise;

    //Change amplitude
    musicInterrupt();
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
    //*GPIO_IFC = 0xff;
    /* TODO handle button pressed event, remember to clear pending interrupt */
    //*GPIO_PA_DOUT = 0x0000; /* turn on LEDs D4-D8 (LEDs are active low) */
    gpio_handler();
    *GPIO_IFC = *GPIO_IF; //0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
    /* TODO handle button pressed event, remember to clear pending interrupt */
    //*GPIO_PA_DOUT = 0x0000; /* turn on LEDs D4-D8 (LEDs are active low) */
    gpio_handler();
    *GPIO_IFC = *GPIO_IF; //0xff;
}

int i = 0;

//DAC0_IRQHandler

//int song[9] = {0, 1, 2, 3, 4, 7, -5, 18, -20};
int *song_pointer; //= song;

void gpio_handler()
{
	if(i < 3){
		i++;
		return;
	}
	
    uint32_t input = *GPIO_PC_DIN;
	//Haavard bruker GPIO_IF (finne i dokumentasjon)
	// GPIO_IFC skal settes til denne verdien også.
	if(BUTTON_CLICK(*GPIO_IF, 7))
	{
		/* Start timer and set correct song */
		musicSetSong(song_pointer);
	
		startTimer();
	}

	/* Hightlight the pressed led */
    input = input << 8;
    input = input ^ 0;
    *GPIO_PA_DOUT = input;
}
