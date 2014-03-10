#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "music.h"
#include "math.h"
#include "timer.h"
#include "constants.h"

#define A_VALUE 1.0594630943592953
#define PI 3.1415926535897932
#define A4_FREQ 440
#define NOTE_LENGTH 50 /* The length of one note. */

void musicSetFrequency(int note);
int getSoundLen(double freq);

uint8_t amplitude = 127;

/* Sinus note buffer */
uint16_t buffer[1024];
int buffer_length = 0;
int buffer_placement = 0;

int song_placement = 0;
int song_length = 0;

int *song = 0;
int *song_note_length = 0;

/* Number of times one note is played */
int note_current_length = 0;

/**
 * Set the song to be played.
 * Params: Array of notes and note lengths
 */
void musicSetSong(int song_pointer)
{
	static int song_intro[11] = {1, 0, -2, 0, -2, 0, -4,0, -2, 0, -2};
	static int song_intro_note_length[11] = {6, 1, 3, 1, 1, 1, 1, 1, 3, 1, 9};

	static int song_victory[11] = {-1, 0, -1, 0, 2, 0, 5,0, 2, 0, -1};
	static int song_victory_note_length[11] = {6, 1, 3, 1, 1, 1, 1, 1, 3, 1, 9};

	static int song_hit[1] = {2};
	static int song_hit_note_length[1] = {4};

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
			song = song_intro;
			song_note_length = song_intro_note_length;
			song_length = sizeof(song_intro) / sizeof(int);
			break;
		case 2:
			song = song_hit;
			song_note_length = song_hit_note_length;
			song_length = sizeof(song_hit) / sizeof(int);
			break;

		default:
			song = 0;
			song_note_length = 0;
			song_length = 0;
	}

    musicSetFrequency(song[0]);
}

/**
 * Fill the buffer with one note that is n half steps away from A4.
 */
void musicSetFrequency(int note)
{
	/* Silence note*/
	if(note == 0)
	{
		buffer[0] = 0;
	}
	else
	{
			//http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html
			/* fn = f0 * (a)^n
			 * where
			 * f0 = the frequency of one fixed note which must be defined. A common choice is setting the A above middle C (A4) at f0 = 440 Hz.
			 * n = the number of half steps away from the fixed note you are. If you are at a higher note, n is positive. If you are on a lower note, n is negative.
			 * fn = the frequency of the note n half steps away.
			 */
            buffer_length = getSoundLen(A4_FREQ * pow(A_VALUE, note));

			for (int i = 0; i < buffer_length; i++)
			{
				buffer[i] = amplitude * (0 + sin(2 * PI * i / buffer_length));
			}
	}

	buffer_placement = 0;
}

/**
 * Get the sinus length of one note.
 */
int getSoundLen(double freq)
{
    return SAMPLE_RATE / freq;
}

/**
 * Run on each interrupt.
 * Sets the DAC data from the buffer. If the buffer is done, then it checks if it should play the note again.
 * Thereafter change the note. If all the notes have been played, then TIMER1 interrupts will be stopped.
 */
void musicInterrupt()
{
    uint16_t sound = buffer[buffer_placement];
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
            }
            /* Song is done */
            else
            {
				/* Reset data */ 
				song = 0;
				song_note_length = 0;
				song_length = 0;
				buffer_placement = 0;
                /* Stop the timer */
                stopTimer();
            }
        }
    }
}
