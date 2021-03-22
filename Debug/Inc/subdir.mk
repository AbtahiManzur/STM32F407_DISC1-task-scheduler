################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Inc/led.c 

OBJS += \
./Inc/led.o 

C_DEPS += \
./Inc/led.d 


# Each subdirectory must supply rules for building sources it contributes
Inc/led.o: ../Inc/led.c Inc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Inc/led.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

