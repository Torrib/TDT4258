#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "music.h"

const uint16_t Sine12bit[32] = {
    2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
    3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
    599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

uint16_t frequency = 0;

void music_setFrequency(uint16_t f)
{
    /*
     * Clock runs at 14Mhz/sample_size
     * 14Mhz/frequency = sample_size
     */
    *TIMER1_TOP = 14000000 / f;

}

void music_interrupt()
{

    //Set sine data
    *DAC0_CH0DATA = 0;
    *DAC0_CH1DATA = 0;
}
