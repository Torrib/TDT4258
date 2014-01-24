/*  Place the following from address 0 in memory */
.long  0x1000

/*  Initial stack pointer */
.long reset

/*  Address of reset handler */
/*  if required, other handler addresses follows here */
.thumb func

reset:
/*  program code here */

/* Step 1: CMU GPU*/
CMU_BASE = 0x400c8000
CMU_HFPERCLKEN0 = 0 x044
CMU_HFPERCLKEN0 GPIO = 13
// base address of CMU
// offset from base
// bit representing GPIO

// load CMU base address
ldr r1, cmu_base_addr

// load current value of HFPERCLK_ENABLE
ldr r2, [r1, #CMU_HFPERCLKEN0]

// set bit for GPIO clock
mov r3, #1
lsl r3, r3 ,#CMU_HFPERCLKEN0_GPIO
orr r2, r2 ,r3

// store new value
str r2, [r1, #CMU_HFPERCLKEN0]

cmu_base_addr:
    .long CMU_BASE




/* Set igh drive strenght by writing 0x2 to GPIO_PA_CTRL*/


/* Set pins 8-15 to output by writing 0x55555555 to GPIO_PA_MODEH register */


/* Pins 8-15 can now be set high or low by writing to bits 8-15 of GPIO PA DOUT */
