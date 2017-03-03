
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "uartstdio.h"

#include "configs.h"
#include "parser.h"

void systickevt(void);
void Timer0AIntHandler(void);
void Timer1AIntHandler(void);


uint8_t uart_char = 0, call_parser = 0, tick = 0;

int main(void)
{

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0X00);	// Toggle LED0 everytime a key is pressed
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

	//	UART Configuration
    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for Timer operation.
    //
    InitConsole();
    //
    // Display the example setup on the console.
    //
    UARTprintf("16-Bit Timer Interrupt ->");
    UARTprintf("\n   Timer = Timer0B");
    UARTprintf("\n   Mode = One Shot");
    UARTprintf("\n   Rate = 1ms");

    UARTprintf("16-Bit Timer Interrupt ->");
    UARTprintf("\n   Timer = Timer0A");
    UARTprintf("\n   Mode = Periodic");
    UARTprintf("\n   Rate = 0.25ms");

    //
    // The Timer0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    //
    // The Timer0 peripheral must be enabled for use.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);

    //
    // Set the Timer0B load value to 0.625ms.
    //
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 8000);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 500);

    TimerControlWaitOnTrigger(TIMER1_BASE, TIMER_A, true);
    //
    // Configure the Timer0B interrupt for timer timeout.
    //
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
    //
    // Enable the Timer0B interrupt on the processor (NVIC).
    //
    IntEnable(INT_TIMER0A);
    IntEnable(INT_TIMER1A);

    //
    // Enable Timer0B.
    //
    TimerEnable(TIMER0_BASE, TIMER_A );
    TimerEnable(TIMER1_BASE, TIMER_A );

/*
 *  SysTickPeriodSet(SysCtlClockGet()/8000);
	SysTickIntRegister(systickevt);
	SysTickIntEnable();
	SysTickEnable();

	transfer("\033[2J\033[H");									// Clear Screen
 *
 */
	while (1);


}

void isr_uart()
{
	uint32_t u0status;
	u0status = UARTIntStatus(UART0_BASE,true);
	UARTIntClear(UART0_BASE,u0status);
	if(UARTCharsAvail(UART0_BASE))
		{
			uart_char = UARTCharGet(UART0_BASE);
			call_parser = 1;
		}
}

void systickevt(){

	tick ^= 0xFF;
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, tick);

}


//*****************************************************************************
//
// The interrupt handler for the Timer0B interrupt.
//
//*****************************************************************************
void
Timer0AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	tick ^= 0xFF;
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, tick);

}

void
Timer1AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0x00);

//	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 500);

}

