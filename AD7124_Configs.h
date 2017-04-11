/*
 * AD7124_Configs.h
 *
 *  Created on: Mar 23, 2017
 *      Author: edsys
 */

#ifndef AD7124_CONFIGS_H_
#define AD7124_CONFIGS_H_
#include "AD7124.h"

void AD7124_ChannelConfig(ad7124_device *device,  ad7124_st_reg (*regs)[]);
void AD7124_ChannelSetups(ad7124_device *device, ad7124_st_reg (*regs)[]);
void AD7124_ADCControlConfig(ad7124_device *device, ad7124_st_reg (*regs)[]);



#endif /* AD7124_CONFIGS_H_ */
