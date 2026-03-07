################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/boot/Src/boot.c \
../Core/boot/Src/eeplog.c 

OBJS += \
./Core/boot/Src/boot.o \
./Core/boot/Src/eeplog.o 

C_DEPS += \
./Core/boot/Src/boot.d \
./Core/boot/Src/eeplog.d 


# Each subdirectory must supply rules for building sources it contributes
Core/boot/Src/%.o Core/boot/Src/%.su Core/boot/Src/%.cyclo: ../Core/boot/Src/%.c Core/boot/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/common/Inc -I../Core/app/Inc -I../Core/boot/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-boot-2f-Src

clean-Core-2f-boot-2f-Src:
	-$(RM) ./Core/boot/Src/boot.cyclo ./Core/boot/Src/boot.d ./Core/boot/Src/boot.o ./Core/boot/Src/boot.su ./Core/boot/Src/eeplog.cyclo ./Core/boot/Src/eeplog.d ./Core/boot/Src/eeplog.o ./Core/boot/Src/eeplog.su

.PHONY: clean-Core-2f-boot-2f-Src

