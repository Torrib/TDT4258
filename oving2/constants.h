/* The period between sound samples, in clock cycles
 * Clock runs at 14Mhz/sample_size
 * 14000000/44100 = 317.460317
 */
#define   CLOCK_FREQ 14000000
#define   SAMPLE_RATE 44100
#define   SAMPLE_PERIOD CLOCK_FREQ/SAMPLE_RATE
