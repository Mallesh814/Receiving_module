
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "inc/hw_memmap.h"
#include "driverlib/timer.h"

#include "configs.h"
#include "parser.h"
#include "tlv5636.h"

void isr_debugConsole(void);
void Timer0AIntHandler(void);


uint8_t uart_char = 0, call_parser = 0;
uint32_t debugConsole;


void TimerConfig(uint32_t freq){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //
    // The Timer0 peripheral must be enabled for use.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    //
    // Set the Timer0B load value to 0.625ms.
    //
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / freq);

    //
    // Configure the Timer0B interrupt for timer timeout.
    //
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0AIntHandler);
    //
    // Enable the Timer0B interrupt on the processor (NVIC).
    //
    IntEnable(INT_TIMER0A);
    //
    // Enable Timer0B.
    //
    TimerEnable(TIMER0_BASE, TIMER_A );

}

void AD7124_Rset(){
		uint32_t ui32Receive;
	   uint8_t data = 0xFF;

		while(SSIDataGetNonBlocking(SSI3_BASE, &ui32Receive)); // Clear FIFO Before Initiation a read Operation

		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0X00);	// PULL DOWN Slave Slect PIN

		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);
		SSIDataPut(SSI3_BASE, data);

		while(SSIBusy(SSI3_BASE));

		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0XFF);	// PULL UP Slave Slect PIN

		while(SSIDataGetNonBlocking(SSI3_BASE, &ui32Receive)); // Clear FIFO Before Initiation a read Operation
}

uint32_t AD7124_ReadID()
{
   uint32_t ui32Receive, ui32ReadIDFlash;
   uint8_t data = (0<<7) | (1<<6) | (0x05 & 0x3F);

	while(SSIDataGetNonBlocking(SSI3_BASE, &ui32Receive)); // Clear FIFO Before Initiation a read Operation

	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0X00);	// PULL DOWN Slave Slect PIN

	SSIDataPut(SSI3_BASE, data);
	SSIDataPut(SSI3_BASE, 0x00);
	SSIDataPut(SSI3_BASE, 0x00);

	while(SSIBusy(SSI3_BASE));

	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0XFF);	// PULL UP Slave Slect PIN

	SSIDataGet(SSI3_BASE, &ui32Receive);
	SSIDataGet(SSI3_BASE, &ui32Receive);
	SSIDataGet(SSI3_BASE, &ui32Receive);
	ui32ReadIDFlash = ui32Receive;

	return ui32ReadIDFlash;
}


int main(void)
{

	char num[10]="\0";
	uint32_t status;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
/*	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0X00);	// Toggle LED0 everytime a key is pressed
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0XFF);	// Toggle LED0 everytime a key is pressed
*/

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

	//	UART Configuration
    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for Timer operation.
    //
    debugConsole = InitConsole(UART0_BASE,115200);
//	UARTFIFOEnable(debugConsole);
//	UARTFIFOLevelSet(debugConsole,UART_FIFO_TX7_8,UART_FIFO_RX1_8);
	UARTIntRegister(debugConsole,isr_debugConsole);
	UARTIntEnable(debugConsole,UART_INT_RX);
	status = UARTIntStatus(debugConsole,true);
	UARTIntClear(debugConsole,status);
    //
    // Display the example setup on the console.
    //

	transfer("\033[2J\033[H", debugConsole);									// Clear Screen
    dec_ascii(SysCtlClockGet() , num);
    transfer(num, debugConsole);
    transfer("\n\r", debugConsole);
	InitSPI(SSI3_BASE, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 10000, 8, 0);
    transfer("SPI Initialized", debugConsole);
    AD7124_Rset();
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0XFF);	// Toggle LED0 everytime a key is pressed

    TimerConfig(10);


	while (1){
		if (call_parser) {
			call_parser = 0;
			status = AD7124_ReadID();
		    transfer("AD7124 ID register", debugConsole);
		    dec_ascii(status , num);
		    transfer(num, debugConsole);
		    transfer("\n\r", debugConsole);
		}
	}


}

void isr_debugConsole(void)
{
	uint32_t u0status;
	u0status = UARTIntStatus(debugConsole,true);
	UARTIntClear(debugConsole,u0status);
	if(UARTCharsAvail(debugConsole))
		{
			uart_char = UARTCharGet(debugConsole);
			call_parser = 1;
		}
}
void Timer0AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    call_parser=1;
}


