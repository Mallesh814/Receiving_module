
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"

#include "configs.h"
#include "parser.h"
#include "tlv5636.h"

void Timer0AIntHandler(void);
void Timer1AIntHandler(void);


uint8_t uart_char = 0, call_parser = 0, tick = 0, t0 = 0, t1 = 0, mux = 0;

int main(void)
{

	char num[10]="\0";
	uint16_t dac_val = 0;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0X00);	// Toggle LED0 everytime a key is pressed
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0XFF);	// Toggle LED0 everytime a key is pressed

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

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

    dec_ascii(SysCtlClockGet() , num);
    transfer(num);
    transfer("\n\r");


    //
    // The SSI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

    //
    // For this example SSI1 is used with PortD[3:0].  The actual port and pins
    // used may be different on your part, consult the data sheet for more
    // information.  GPIO port D needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //
    // Configure the pin muxing for SSI1 functions on port D0, D1, D2 and D3.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PD0_SSI1CLK);
    GPIOPinConfigure(GPIO_PD1_SSI1FSS);
    GPIOPinConfigure(GPIO_PD2_SSI1RX);
    GPIOPinConfigure(GPIO_PD3_SSI1TX);

    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PD3 - SSI0Tx
    //      PD2 - SSI0Rx
    //      PD1 - SSI0Fss
    //      PD0 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 |
                   GPIO_PIN_0);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI1,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2,SSI_MODE_MASTER, 1000000, 16);

    //
    // Enable the SSI1 module.
    //
    SSIEnable(SSI1_BASE);



    //
    // The Timer0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //
    // The Timer0 peripheral must be enabled for use.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_ONE_SHOT);

    //
    // Set the Timer0B load value to 0.625ms.
    //
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 4000);
    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet() / 14285);

    //
    // Configure the Timer0B interrupt for timer timeout.
    //
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    TimerIntRegister(TIMER0_BASE, TIMER_B, Timer1AIntHandler);
    //
    // Enable the Timer0B interrupt on the processor (NVIC).
    //
    IntEnable(INT_TIMER0A);
    IntEnable(INT_TIMER0B);

    //
    // Enable Timer0B.
    //
    TimerEnable(TIMER0_BASE, TIMER_A );
    TimerEnable(TIMER0_BASE, TIMER_B );

/*
	transfer("\033[2J\033[H");									// Clear Screen
 *
 */
    tlv5636_init();
    tlv5636_set_output(dac_val);

	while (1){
		if (call_parser) {
			call_parser = 0;
			dac_val += 10;
		    tlv5636_set_output(dac_val);
		}
	}


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

	t0 ^= 0xFF;
    t1 = 0xFF;

    if(mux < 3) mux++;
    else mux = 0;

    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2, t0);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, t1);
    TimerEnable(TIMER0_BASE, TIMER_B );

}

void
Timer1AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    t1 = 0x00;

	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, t1);

//	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 500);

}

