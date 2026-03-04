################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/common/Src/clock.c \
../Core/common/Src/gpio.c \
../Core/common/Src/lcd2.c \
../Core/common/Src/stm32f4xx_it.c \
../Core/common/Src/syscalls.c \
../Core/common/Src/sysmem.c \
../Core/common/Src/system_stm32f4xx.c \
../Core/common/Src/timbase.c \
../Core/common/Src/uart6.c 

OBJS += \
./Core/common/Src/clock.o \
./Core/common/Src/gpio.o \
./Core/common/Src/lcd2.o \
./Core/common/Src/stm32f4xx_it.o \
./Core/common/Src/syscalls.o \
./Core/common/Src/sysmem.o \
./Core/common/Src/system_stm32f4xx.o \
./Core/common/Src/timbase.o \
./Core/common/Src/uart6.o 

C_DEPS += \
./Core/common/Src/clock.d \
./Core/common/Src/gpio.d \
./Core/common/Src/lcd2.d \
./Core/common/Src/stm32f4xx_it.d \
./Core/common/Src/syscalls.d \
./Core/common/Src/sysmem.d \
./Core/common/Src/system_stm32f4xx.d \
./Core/common/Src/timbase.d \
./Core/common/Src/uart6.d 


# Each subdirectory must supply rules for building sources it contributes
Core/common/Src/%.o Core/common/Src/%.su Core/common/Src/%.cyclo: ../Core/common/Src/%.c Core/common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Core/common/Inc -I../Core/app/Inc -I../Core/boot/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-common-2f-Src

clean-Core-2f-common-2f-Src:
	-$(RM) ./Core/common/Src/clock.cyclo ./Core/common/Src/clock.d ./Core/common/Src/clock.o ./Core/common/Src/clock.su ./Core/common/Src/gpio.cyclo ./Core/common/Src/gpio.d ./Core/common/Src/gpio.o ./Core/common/Src/gpio.su ./Core/common/Src/lcd2.cyclo ./Core/common/Src/lcd2.d ./Core/common/Src/lcd2.o ./Core/common/Src/lcd2.su ./Core/common/Src/stm32f4xx_it.cyclo ./Core/common/Src/stm32f4xx_it.d ./Core/common/Src/stm32f4xx_it.o ./Core/common/Src/stm32f4xx_it.su ./Core/common/Src/syscalls.cyclo ./Core/common/Src/syscalls.d ./Core/common/Src/syscalls.o ./Core/common/Src/syscalls.su ./Core/common/Src/sysmem.cyclo ./Core/common/Src/sysmem.d ./Core/common/Src/sysmem.o ./Core/common/Src/sysmem.su ./Core/common/Src/system_stm32f4xx.cyclo ./Core/common/Src/system_stm32f4xx.d ./Core/common/Src/system_stm32f4xx.o ./Core/common/Src/system_stm32f4xx.su ./Core/common/Src/timbase.cyclo ./Core/common/Src/timbase.d ./Core/common/Src/timbase.o ./Core/common/Src/timbase.su ./Core/common/Src/uart6.cyclo ./Core/common/Src/uart6.d ./Core/common/Src/uart6.o ./Core/common/Src/uart6.su

.PHONY: clean-Core-2f-common-2f-Src

