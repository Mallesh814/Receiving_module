################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AD7124.c \
../AD7124_Configs.c \
../communication.c \
../configs.c \
../junk_modules.c \
../main.c \
../parser.c \
../startup_gcc.c \
../tlv5636.c \
../uartstdio.c 

OBJS += \
./AD7124.o \
./AD7124_Configs.o \
./communication.o \
./configs.o \
./junk_modules.o \
./main.o \
./parser.o \
./startup_gcc.o \
./tlv5636.o \
./uartstdio.o 

C_DEPS += \
./AD7124.d \
./AD7124_Configs.d \
./communication.d \
./configs.d \
./junk_modules.d \
./main.d \
./parser.d \
./startup_gcc.d \
./tlv5636.d \
./uartstdio.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -DPART_TM4C123GH6PM -DTARGET_IS_BLIZZARD_RB1 -DARM_MATH_CM4 -I"C:\ti\TivaWare_C_Series-2.1.2.111" -O0 -g3 -Wall -c -fmessage-length=0 -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


