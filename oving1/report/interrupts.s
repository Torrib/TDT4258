.syntax unified

.include "efm32gg.s"

    /////////////////////////////////////////////////////////////////////////////
    //
    // Exception vector table
    // This table contains addresses for all exception handlers
    //
    /////////////////////////////////////////////////////////////////////////////

.section .vectors

    .long   stack_top               /* Top of Stack                 */
    .long   _reset                  /* Reset Handler                */
    .long   dummy_handler           /* NMI Handler                  */
    .long   dummy_handler           /* Hard Fault Handler           */
    .long   dummy_handler           /* MPU Fault Handler            */
    .long   dummy_handler           /* Bus Fault Handler            */
    .long   dummy_handler           /* Usage Fault Handler          */
    .long   dummy_handler           /* Reserved                     */
    .long   dummy_handler           /* Reserved                     */
    .long   dummy_handler           /* Reserved                     */
    .long   dummy_handler           /* Reserved                     */
    .long   dummy_handler           /* SVCall Handler               */
    .long   dummy_handler           /* Debug Monitor Handler        */
    .long   dummy_handler           /* Reserved                     */
    .long   dummy_handler           /* PendSV Handler               */
    .long   dummy_handler           /* SysTick Handler              */

    /* External Interrupts */
    .long   dummy_handler
    .long   gpio_handler            /* GPIO even handler */
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   gpio_handler            /* GPIO odd handler */
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler
    .long   dummy_handler

.section .text

/////////////////////////////////////////////////////////////////////////////
//
// Reset handler
// The CPU will start executing here after a reset
//
/////////////////////////////////////////////////////////////////////////////
cmu_base_addr:
    .long CMU_BASE
gpio_base_addr:
    .long GPIO_BASE
gpio_pa_base_addr:
    .long GPIO_PA_BASE
gpio_pc_base_addr:
    .long GPIO_PC_BASE
nvic_iser0:
    .long ISER0

.globl  _reset
.type   _reset, %function
.thumb_func
_reset:
    /* GPIO CLOCK */
    ldr r1, cmu_base_addr //Load CMU base adress
    ldr r2, [r1, #CMU_HFPERCLKEN0] //Load current value of HFPERCLK_ENABLE

    mov r3, #1 //Set GPIO clock
    lsl r3, r3, #CMU_HFPERCLKEN0_GPIO

    orr r2, r2, r3

    //store new value
    str r2, [r1, #CMU_HFPERCLKEN0]

    /* GPIO Output */
    //Set high drive strength
    ldr r1, gpio_pa_base_addr //Load CMU base adress
    mov r2, #0x2
    //store new value
    str r2, [r1, #GPIO_CTRL]

    //Set pins 8-15 to output
    mov r2, #0x55555555
    str r2, [r1, #GPIO_MODEH]

    /* GPIO Input */
    //Set 0-7 to input
    ldr r2, gpio_pc_base_addr
    mov r3, #0x33333333
    str r3, [r2, #GPIO_MODEL]

    //Enable internal pull-up
    mov r3, #0xFF
    str r3, [r2, #GPIO_DOUT]

    /* Set up interrupts */
    ldr r0, gpio_base_addr
    mov r4, #0x22222222
    str r4, [r0, #GPIO_EXTIPSELL]

    mov r4, #0xFF
    str r4, [r0, #GPIO_EXTIRISE] //Activate on rise
    str r4, [r0, #GPIO_EXTIFALL] //Activate on fall 
    str r4, [r0, #GPIO_IEN] //Enable interrupts generation
    ldr r5, nvic_iser0
    ldr r4, =#0x802
    str r4, [r5] //Enable interrupt handling 

    bx lr //branch back to link register

/////////////////////////////////////////////////////////////////////////////
//
// GPIO handler
// The CPU will jump here when there is a GPIO interrupt
//
/////////////////////////////////////////////////////////////////////////////
.thumb_func
gpio_handler:
    mov r4, #0xFF
    str r4, [r0, #GPIO_IFC] //Clear interrupt

    ldr r4, [r2, #GPIO_DIN]
    lsl r4, r4, #8 //Left shift input 8bits
    mov r5, #0
    eor r4, r4, r5
    str r4, [r1, #GPIO_DOUT] //write back to lights

    bx lr //branch back to link register

/////////////////////////////////////////////////////////////////////////////
.thumb_func
dummy_handler:
    bx lr //branch back to link register
