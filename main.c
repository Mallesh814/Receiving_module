
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
#include "AD7124_Configs.h"


void isr_debugConsole(void);
void Timer0AIntHandler(void);


uint8_t uart_char = 0, call_parser = 0;
uint8_t timer_int = 0;
uint32_t debugConsole, dacHandle, decimal;


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

	while(SSIBusy(SSI3_BASE));

	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0XFF);	// PULL UP Slave Slect PIN

	SSIDataGet(SSI3_BASE, &ui32Receive);
	SSIDataGet(SSI3_BASE, &ui32Receive);
	ui32ReadIDFlash = ui32Receive;

	return ui32ReadIDFlash;
}
uint32_t toggle = 0;

int main(void)
{
	ad7124_st_reg ad7124_regs[] =
	{
		{0x00, 0x00,   1, 2}, /* AD7124_Status */
		{0x01, 0x0000, 2, 1}, /* AD7124_ADC_Control */
		{0x02, 0x0000, 3, 2}, /* AD7124_Data */
		{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
		{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
		{0x05, 0x02,   1, 2}, /* AD7124_ID */
		{0x06, 0x0000, 3, 2}, /* AD7124_Error */
		{0x07, 0x0044, 3, 1}, /* AD7124_Error_En */
		{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
		{0x09, 0x8001, 2, 1}, /* AD7124_Channel_0 */
		{0x0A, 0x0001, 2, 1}, /* AD7124_Channel_1 */
		{0x0B, 0x0001, 2, 1}, /* AD7124_Channel_2 */
		{0x0C, 0x0001, 2, 1}, /* AD7124_Channel_3 */
		{0x0D, 0x0001, 2, 1}, /* AD7124_Channel_4 */
		{0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
		{0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
		{0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
		{0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
		{0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
		{0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
		{0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
		{0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
		{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
		{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
		{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
		{0x19, 0x0860, 2, 1}, /* AD7124_Config_0 */
		{0x1A, 0x0860, 2, 1}, /* AD7124_Config_1 */
		{0x1B, 0x0860, 2, 1}, /* AD7124_Config_2 */
		{0x1C, 0x0860, 2, 1}, /* AD7124_Config_3 */
		{0x1D, 0x0860, 2, 1}, /* AD7124_Config_4 */
		{0x1E, 0x0860, 2, 1}, /* AD7124_Config_5 */
		{0x1F, 0x0860, 2, 1}, /* AD7124_Config_6 */
		{0x20, 0x0860, 2, 1}, /* AD7124_Config_7 */
		{0x21, 0x060180, 3, 1}, /* AD7124_Filter_0 */
		{0x22, 0x060180, 3, 1}, /* AD7124_Filter_1 */
		{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
		{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
		{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
		{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
		{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
		{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
		{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
		{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
		{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
		{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
		{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
		{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
		{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
		{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
		{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
		{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
		{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
		{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
		{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
		{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
		{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
		{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
	};
	char num[10]="\0";
	uint8_t buffer[] = {0,0,0,0,0,0,0,0};
	uint32_t status;
	uint32_t averageDC = 6710886;
	uint32_t dac_val=0;
	ad7124_device myad7124;
//	ad7124_st_reg myad7124_regs[0x38];
//	ad7124_st_reg *pmyad7124_regs = ad7124_regs;

	myad7124.slave_select_id = SSI3_BASE;
	myad7124.check_ready = 1;
	myad7124.regs = ad7124_regs;
	myad7124.useCRC = 0;
	myad7124.spi_rdy_poll_cnt = 1000;

//	myad7124_regs[AD7124_ID].addr = AD7124_ID;
//	myad7124_regs[AD7124_ID].rw = 2;
//	myad7124_regs[AD7124_ID].size = 1;
//	myad7124_regs[AD7124_ID].value = 0;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, 0XFF);	// Toggle LED0 everytime a key is pressed

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

	//	UART Configuration
    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for Timer operation.
    //
    debugConsole = InitConsole(UART0_BASE,460800);
	UARTFIFOEnable(debugConsole);
	UARTFIFOLevelSet(debugConsole,UART_FIFO_TX7_8,UART_FIFO_RX1_8);
	UARTIntRegister(debugConsole,isr_debugConsole);
	UARTIntEnable(debugConsole,UART_INT_RX | UART_INT_RT);
	status = UARTIntStatus(debugConsole,true);
	UARTIntClear(debugConsole,status);
    //
    // Display the example setup on the console.
    //
	InitSPI(SSI0_BASE, SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 1000000, 16, 1);

    dacHandle = InitI2C(I2C1_BASE,1);



	InitSPI(SSI3_BASE, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 1000000, 8, 0);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1, 0XFF);
    AD7124_Rset();


	transfer("\033[2J\033[H", debugConsole);									// Clear Screen
    dec_ascii(num, SysCtlClockGet());
    transfer(num, debugConsole);
    transfer("\n\r", debugConsole);


	transfer("SPI Initialized", debugConsole);

    AD7124_ChannelConfig(&myad7124, &ad7124_regs);
    AD7124_ChannelSetups(&myad7124, &ad7124_regs);
    AD7124_ADCControlConfig(&myad7124, &ad7124_regs);

    status = AD7124_ReadRegister(&myad7124, &ad7124_regs[AD7124_Status]);
	if(status){
	    transfer("AD7124 Status register: ", debugConsole);
	    int_hex_ascii(num, ad7124_regs[AD7124_Status].value);
	    transfer(num, debugConsole);
	    transfer("\n\r", debugConsole);
	    }
	else{
		transfer("Error Reading data !! \n\r",debugConsole);
	}

	status = AD7124_ReadRegister(&myad7124, &ad7124_regs[AD7124_Channel_0]);
	if(status){
	    transfer("AD7124 Channel 0 register: ", debugConsole);
	    int_hex_ascii(num, ad7124_regs[AD7124_Channel_0].value);
	    transfer(num, debugConsole);
	    transfer("\n\r", debugConsole);
	    }
	else{
		transfer("Error Reading data !! \n\r",debugConsole);
	}

	status = AD7124_ReadRegister(&myad7124, &ad7124_regs[AD7124_Config_0]);
	if(status){
	    transfer("AD7124 Config 0 register: ", debugConsole);
	    int_hex_ascii(num, ad7124_regs[AD7124_Config_0].value);
	    transfer(num, debugConsole);
	    transfer("\n\r", debugConsole);
	    }
	else{
		transfer("Error Reading data !! \n\r",debugConsole);
	}

	status = AD7124_ReadRegister(&myad7124, &ad7124_regs[AD7124_Filter_0]);
	if(status){
	    transfer("AD7124 Filter 0 register: ", debugConsole);
	    int_hex_ascii(num, ad7124_regs[AD7124_Filter_0].value);
	    transfer(num, debugConsole);
	    transfer("\n\r", debugConsole);
	    }
	else{
		transfer("Error Reading data !! \n\r",debugConsole);
	}

	status = AD7124_ReadRegister(&myad7124, &ad7124_regs[AD7124_ADC_Control]);
	if(status){
	    transfer("AD7124 Control register: ", debugConsole);
	    int_hex_ascii(num, ad7124_regs[AD7124_ADC_Control].value);
	    transfer(num, debugConsole);
	    transfer("\n\r", debugConsole);
	    }
	else{
		transfer("Error Reading data !! \n\r",debugConsole);
	}

	
	tlv5636_init();
    tlv5636_set_output(0x7FF);

    TimerConfig(1000);

    while (1){

		if(call_parser){
			call_parser = 0;
			switch(uart_char){
			case 'i':dac_val += 10;
			break;
			case 'd':dac_val -= 10;
			break;
			case 'r':dac_val = 0;
			break;
			case 'h':dac_val = 0x7FF;
			break;
			case 'm':dac_val = 0xFFF;
			break;
			default:dac_val += 10;
			break;
			}
			uart_char = 0;
		    tlv5636_set_output(dac_val);

		    transfer("W:", debugConsole);
		    dec_ascii(num, dac_val);
		    transfer(num, debugConsole);
		    transfer("\n\r", debugConsole);

		    /*		    decimal = dac_val << 4;
		    buffer[0] = 0x4F;
		    buffer[1] = 0xD0;
		    buffer[2] = decimal >> 8;
		    buffer[3] = decimal & 0x00FF;
		    I2C_Write(dacHandle, buffer, 4);
		    transfer("W:", debugConsole);
		    dec_ascii(num, dac_val);
		    transfer(num, debugConsole);
		    transfer("\n\r", debugConsole);

		    buffer[0] = 0x4F;
		    buffer[1] = 0xD0;
		    buffer[2] = 0x00;
		    buffer[3] = 0x00;

		    I2C_Read(dacHandle, buffer, 4);
		    decimal = (buffer[2] << 4) + (buffer[3] >> 4);
		    transfer("R:", debugConsole);
		    dec_ascii(num, decimal);
		    transfer(num, debugConsole);
		    transfer("\n\r", debugConsole);
*/
		}

		if(timer_int){
			timer_int = 0;

			AD7124_WaitForConvReady(&myad7124, 10000);
			status = AD7124_ReadRegister(&myad7124, &ad7124_regs[AD7124_Data]);
            averageDC = (uint32_t)((int32_t)averageDC + (((int32_t)ad7124_regs[AD7124_Data].value - (int32_t)averageDC) >> 9));
            dac_val = ((averageDC)>> 14) * 3 ;

            decimal = dac_val << 4;
		    buffer[0] = 0x4F;
		    buffer[1] = 0xD0;
		    buffer[2] = decimal >> 8;
		    buffer[3] = decimal & 0x00FF;
		    I2C_Write(dacHandle, buffer, 4);

		    transfer("W:", debugConsole);
		    dec_ascii(num, dac_val);
		    transfer(num, debugConsole);
		    transfer("\n\r", debugConsole);

		    transfer("D: ", debugConsole);
		    dec_ascii(num, ad7124_regs[AD7124_Data].value);
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
//			call_parser = 1;
		}
}
void Timer0AIntHandler(void)
{
    //
    // Clear the timer interrupt flag.
    //
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//	call_parser = 1;

    timer_int=1;
//    toggle ^= 0xFF;
//	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1, toggle);	// Toggle LED0 everytime a key is pressed
}


