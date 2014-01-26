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
    gpio_pa_base_addr:
        .long GPIO_PA_BASE

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

    /* GPIO Ports */
    //Set high drive strength
    ldr r1, gpio_pa_base_addr //Load CMU base adress
    mov r2, #0x2
    //store new value
    str r2, [r1, #GPIO_CTRL]

    //Set pins 8-15 to output
    mov r2, #0x55555555
    str r2, [r1, #GPIO_MODEH]

    //Set pin 9 to high
    mov r2, #0x200
    str r2, [r1, #GPIO_DOUT]
          b .  // do nothing

    /////////////////////////////////////////////////////////////////////////////
    //
    // GPIO handler
    // The CPU will jump here when there is a GPIO interrupt
    //
    /////////////////////////////////////////////////////////////////////////////

        .thumb_func
gpio_handler:

          b .  // do nothing

    /////////////////////////////////////////////////////////////////////////////

        .thumb_func
dummy_handler:
        b .  // do nothing
