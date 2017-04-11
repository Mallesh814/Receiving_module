/*
 * AD7124_Configs.c
 *
 *  Created on: Mar 23, 2017
 *      Author: edsys
 */


#include "AD7124_Configs.h"

void AD7124_ChannelConfig(ad7124_device *device, ad7124_st_reg (*regs)[]){
	uint32_t status;
	(*regs)[AD7124_Channel_0].value = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) | AD7124_CH_MAP_REG_AINP(0) | AD7124_CH_MAP_REG_AINM(7);
	status = AD7124_WriteRegister(device, (*regs)[AD7124_Channel_0]);
//	(*regs)[AD7124_Channel_1].value = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(1) | AD7124_CH_MAP_REG_AINP(5) | AD7124_CH_MAP_REG_AINM(6);
//	status = AD7124_WriteRegister(device, (*regs)[AD7124_Channel_1]);
}


void AD7124_ChannelSetups(ad7124_device *device, ad7124_st_reg (*regs)[]){
	uint32_t status;
	(*regs)[AD7124_Config_0].value = AD7124_CFG_REG_REF_SEL(2) | AD7124_CFG_REG_PGA(0);
	status = AD7124_WriteRegister(device, (*regs)[AD7124_Config_0]);

//	(*regs)[AD7124_Config_1].value = AD7124_CFG_REG_REF_SEL(2) | AD7124_CFG_REG_PGA(0);
//	status = AD7124_WriteRegister(device, (*regs)[AD7124_Config_1]);

	(*regs)[AD7124_Filter_0].value = AD7124_FILT_REG_FILTER(0) | AD7124_FILT_REG_REJ60 | AD7124_FILT_REG_FS(4);
	status = AD7124_WriteRegister(device, (*regs)[AD7124_Filter_0]);

//	(*regs)[AD7124_Filter_1].value = AD7124_FILT_REG_FILTER(0) | AD7124_FILT_REG_REJ60 | AD7124_FILT_REG_FS(1);
//	status = AD7124_WriteRegister(device, (*regs)[AD7124_Filter_1]);

	/*
	(*regs)[AD7124_Offset_0].value = 0x800000;
	status = AD7124_NoCheckWriteRegister(device, (*regs)[AD7124_Offset_0]);

	(*regs)[AD7124_Offset_1].value = 0x800000;
	status = AD7124_NoCheckWriteRegister(device, (*regs)[AD7124_Offset_1]);

	(*regs)[AD7124_Gain_0].value = ;
	status = AD7124_NoCheckWriteRegister(device, (*regs)[AD7124_Gain_0]);

	(*regs)[AD7124_Gain_1].value = ;
	status = AD7124_NoCheckWriteRegister(device, (*regs)[AD7124_Gain_1]);

	*/
}

void AD7124_ADCControlConfig(ad7124_device *device, ad7124_st_reg (*regs)[]){
	uint32_t status;

	(*regs)[AD7124_ADC_Control].value = AD7124_ADC_CTRL_REG_REF_EN | AD7124_ADC_CTRL_REG_POWER_MODE(3) |  AD7124_ADC_CTRL_REG_MODE(0) | AD7124_ADC_CTRL_REG_CLK_SEL(1);
	status = AD7124_WriteRegister(device, (*regs)[AD7124_ADC_Control]);
}
