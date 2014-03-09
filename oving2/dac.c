#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "dac.h"

void setupDAC()
{
	// Enable the DAC clock
    *CMU_HFPERCLKEN0 |= 1 << 17;

    //Prescale=2^5=32
    *DAC0_CTRL = 0x50010; //frequency 14/32MHz = 437.5KHz

    //Enable left and right audio channels.
    *DAC0_CH0CTRL = 1;
    *DAC0_CH1CTRL = 1;
}
