#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "music.h"
#include "math.h"

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

#define A 1.0594630943592953 
#define PI 3.141592653589793238462643383279502884197169399375105820974944592
#define A4_FREQ 440

uint16_t frequency = 0;
uint8_t n = 0;
uint8_t amplitude = 127;
uint16_t w = 0;
uint8_t time = 0;
uint8_t phase = 0;

uint8_t buffer[256];

uint8_t buffer_length = 0;
uint8_t buffer_placement = 0;
void musicSetFrequency(uint8_t note)
{
    /*
     * Clock runs at 14Mhz/sample_size
     * 14Mhz/frequency = sample_size
     */
    //uint16_t x = 14000000 / f;
    //x = x / sizeof(sine_wave);
    //*TIMER1_TOP = x;
    //location = 0;
    //
    //http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html

    /* fn = f0 * (a)^n
     * where
     * f0 = the frequency of one fixed note which must be defined. A common choice is setting the A above middle C (A4) at f0 = 440 Hz.
     * n = the number of half steps away from the fixed note you are. If you are at a higher note, n is positive. If you are on a lower note, n is negative.
     * fn = the frequency of the note n half steps away.
     */
    n += note;
    phase = w; //tan av allerede verdi?

    frequency = A4_FREQ * pow(A, n);

    w = 2 * PI * frequency;

	buffer_length = 44100 / w;

	for (int i = 0; i < buffer_length; i++)
	{
		buffer[i] = ceil(amplitude * sin(w * buffer_length * i));// + phase)
	}

    //w / # iterations? time count up to it.
    buffer_placement = 0;
    musicInterrupt();
}

void musicInterrupt()
{
    uint16_t sound = buffer[time];//sine_wave[location] + 500;
    buffer_placement += 1;

	if (buffer_placement >= buffer_length)
		buffer_placement = 0;

    //Set sine amplitude
    *DAC0_CH0DATA = sound;
    *DAC0_CH1DATA = sound;

    /*
    if(location > sizeof(sine_wave) - 1)
        location = 0;
    else
        location++;
    */
}

