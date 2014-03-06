#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "music.h"

uint16_t test = 0;

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

//DAC0_IRQHandler

void gpio_handler()
{
    test += 1;
    //musicSetFrequency(test);
    //noise += 100;
    uint32_t input = *GPIO_PC_DIN;
    input = input << 8;
    input = input ^ 0;

    *GPIO_PA_DOUT = input;
}
