#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "dac.h"

void setupDAC()
{
  /*
    TODO enable and set up the Digital-Analog Converter
    1. Enable the DAC clock by setting bit 17 in CMU_HFPERCLKEN0
    2. Prescale DAC clock by writing 0x50010 to DAC0_CTRL
    3. Enable left and right audio channels by writing 1 to DAC0_CH0CTRL and DAC0_CH1CTRL
    4. Write a continuous stream of samples to the DAC data registers, DAC0_CH0DATA and DAC0_CH1DATA, for example from a timer interrupt
  */

    *CMU_HFPERCLKEN0 |= 1 << 17;

    //Prescale=2^5=32
    *DAC0_CTRL = 0x50010; //frequency 14/32MHz = 437.5KHz
	*DAC0_CTRL |= 1 << 1; //Enables SINEMOD

    //Enable left and right audio channels.
    *DAC0_CH0CTRL = 1;
    *DAC0_CH1CTRL = 1;
}
