################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/app/Src/main.c 

OBJS += \
./Core/app/Src/main.o 

C_DEPS += \
./Core/app/Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Core/app/Src/%.o Core/app/Src/%.su Core/app/Src/%.cyclo: ../Core/app/Src/%.c Core/app/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/common/Inc -I../Core/app/Inc -I../Core/boot/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-app-2f-Src

clean-Core-2f-app-2f-Src:
	-$(RM) ./Core/app/Src/main.cyclo ./Core/app/Src/main.d ./Core/app/Src/main.o ./Core/app/Src/main.su

.PHONY: clean-Core-2f-app-2f-Src

