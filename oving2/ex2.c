#include <stdint.h>
#include <stdbool.h>
#include "ex2.h"

#include "efm32gg.h"
#include "constants.h"
#include "gpio.h"
#include "timer.h"
#include "dac.h"
#include "interrupt_handlers.h"

int main(void)
{
	//Setup energy saving measurements
    setupEnergy();
    /* Enable interrupt handling */
    setupNVIC();
    /* Call the peripheral setup functions */
    setupGPIO();
	// Setup the DAC
    setupDAC();
	//Setup the timer with 14Mhz / 44100
    setupTimer(SAMPLE_PERIOD);

	//Enter sleep mode
    __asm("wfi");

    return 0;
}

void setupEnergy()
{
	//*EMU_MEMCTRL = 4; /* Disables SRAM blocks 1-3 */

    /* Set EM3 */
    //EM4CTRL, EMVREG, EM2BLOCK = 0, SLEEPDEEP=1
    *EMU_CTRL = 0;

    /* 2=sleep, 6=deep sleep.*/
    *SCR = 4;
}

void setupNVIC()
{
    // Enable GPIO interrupts
    *ISER0 |= 1 << 1; //even
    *ISER0 |= 1 << 11; //Odd

    // Enable Timer1 interrupts
    *ISER0 |= 1 << 12; //Enable timer interrupts
}

/* if other interrupt handlers are needed, use the following names:
   NMI_Handler
   HardFault_Handler
   MemManage_Handler
   BusFault_Handler
   UsageFault_Handler
   Reserved7_Handler
   Reserved8_Handler
   Reserved9_Handler
   Reserved10_Handler
   SVC_Handler
   DebugMon_Handler
   Reserved13_Handler
   PendSV_Handler
   SysTick_Handler
   DMA_IRQHandler
   GPIO_EVEN_IRQHandler
   TIMER0_IRQHandler
   USART0_RX_IRQHandler
   USART0_TX_IRQHandler
   USB_IRQHandler
   ACMP0_IRQHandler
   ADC0_IRQHandler
   DAC0_IRQHandler
   I2C0_IRQHandler
   I2C1_IRQHandler
   GPIO_ODD_IRQHandler
   TIMER1_IRQHandler
   TIMER2_IRQHandler
   TIMER3_IRQHandler
   USART1_RX_IRQHandler
   USART1_TX_IRQHandler
   LESENSE_IRQHandler
   USART2_RX_IRQHandler
   USART2_TX_IRQHandler
   UART0_RX_IRQHandler
   UART0_TX_IRQHandler
   UART1_RX_IRQHandler
   UART1_TX_IRQHandler
   LEUART0_IRQHandler
   LEUART1_IRQHandler
   LETIMER0_IRQHandler
   PCNT0_IRQHandler
   PCNT1_IRQHandler
   PCNT2_IRQHandler
   RTC_IRQHandler
   BURTC_IRQHandler
   CMU_IRQHandler
   VCMP_IRQHandler
   LCD_IRQHandler
   MSC_IRQHandler
   AES_IRQHandler
   EBI_IRQHandler
   EMU_IRQHandler
*/
