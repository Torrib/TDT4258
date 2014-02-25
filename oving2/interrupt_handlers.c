#include <stdint.h>
#include <stdbool.h>

#include "interrupt_handlers.h"
#include "efm32gg.h"
#include "music.h"

uint16_t noise = 20000;
uint32_t counter = 0;

/*
C 523 Hz
B 494 Hz
Bb 466 Hz
A 440 Hz
Ab 415 Hz
G 392 Hz
Gb 370 Hz
F 349 Hz
E 330 Hz
Eb 311 Hz
D 294 Hz
Db 277 Hz
C 262 Hz
*/

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
    *TIMER1_IFC = 1;
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */
    *DAC0_CH0DATA = noise;
    *DAC0_CH1DATA = noise;
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
    //noise += 100;
    uint32_t input = *GPIO_PC_DIN;
    input = input << 8;
    input = input ^ 0;

    *GPIO_PA_DOUT = input;
}
