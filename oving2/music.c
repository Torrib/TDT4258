#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "music.h"
#include "math.h"
#include "timer.h"

#define C 523
#define B 494
#define Bb 466
#define A 440
#define Ab 415
#define G 392
#define Gb 370
#define F 349
#define E 330
#define Eb 311
#define D 294
#define Db 277
//#define C 262

/*
    TODO
        PROBLEMS FOR THE MOMENT
        1. Need three songs
        2. Song referencing from interrupt
        FIXED 3. Wierd sound at end. How to set no sound?
        FIXED 4. How to start/ stop timer correctly
        5. How to avoid gpio_handler to run code when started?
            Run when specific buttons are pressed instead
                macro added. Still reacts on interrupts. Still using dirtyfix
        6. Handle note length correctly
*/

#define A_VALUE 1.0594630943592953
#define PI 3.1415926535897932
#define A4_FREQ 440
#define NOTE_LENGTH 50 /* The length of one note. */

void musicSetFrequency(int note);
int getSoundLen(double freq);

uint8_t amplitude = 127;

/* Buffer */
uint16_t buffer[1024];
int buffer_length = 0;
int buffer_placement = 0;

/* The song*/
//int *song;
int song_placement = 0;
int song_length = 0;

int *song = 0;
int *song_note_length = 0;

/* Number of times one note is played */
int note_current_length = 0;

void musicSetSong(int song_pointer)
{
	static int song_victory[9] = {-7, -7, -2, 3, -7, -7};
	static int song_victory_note_length[9] = {9, 3, 1, 1, 3, 15};

	/* New song, start at the beginning */
    song_placement = 0;

	switch(song_pointer)
	{
		case 0:
			song = song_victory;
			song_note_length = song_victory_note_length;
			song_length = sizeof(song_victory) / sizeof(int);
			break;
		case 1:
			break;
		case 2:
			break;
		
		default:
			song = 0;
			song_note_length = 0;
			song_length = 0;
	}

    musicSetFrequency(song[0]);
}

void musicSetFrequency(int note)
{
    /*
     * Clock runs at 14Mhz/sample_size
     * 14000000/44100 = 317.460317
     */

    //http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html
    /* fn = f0 * (a)^n
     * where
     * f0 = the frequency of one fixed note which must be defined. A common choice is setting the A above middle C (A4) at f0 = 440 Hz.
     * n = the number of half steps away from the fixed note you are. If you are at a higher note, n is positive. If you are on a lower note, n is negative.
     * fn = the frequency of the note n half steps away.
     */
    double frequency = A4_FREQ * pow(A_VALUE, note);

    buffer_length = getSoundLen(frequency);

    for (int i = 0; i < buffer_length; i++)
    {
        buffer[i] = amplitude * (0 + sin(2 * PI * i / buffer_length));
    }

	buffer_placement = 0;
}

int getSoundLen(double freq)
{
    return 44100 / freq;
}

void musicInterrupt()
{
    uint16_t sound = buffer[buffer_placement] + 2000;
    buffer_placement += 1;

    //Set sine amplitude
    *DAC0_CH0DATA = sound;
    *DAC0_CH1DATA = sound;

    /* If buffer of notes is complete */
    if (buffer_placement > buffer_length)
    {
        /* Check if we should play the note again */
        note_current_length++;
        if(note_current_length < song_note_length[song_placement] * NOTE_LENGTH)
            buffer_placement = 0;
	/* Change note */
        else
        {
                /* Go to next note */
                song_placement++;

                note_current_length = 0;

                if(song_placement < song_length)
                {
                    musicSetFrequency(song[song_placement]);
                    //TODO Fetch new note_length
                }
                /* Song is done */
                else
                {
                    /* Stop the timer */
                    stopTimer();
                }
        }
    }
}

