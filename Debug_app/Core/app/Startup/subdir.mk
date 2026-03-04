################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/app/Startup/startup_stm32f429zitx.s 

OBJS += \
./Core/app/Startup/startup_stm32f429zitx.o 

S_DEPS += \
./Core/app/Startup/startup_stm32f429zitx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/app/Startup/%.o: ../Core/app/Startup/%.s Core/app/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-app-2f-Startup

clean-Core-2f-app-2f-Startup:
	-$(RM) ./Core/app/Startup/startup_stm32f429zitx.d ./Core/app/Startup/startup_stm32f429zitx.o

.PHONY: clean-Core-2f-app-2f-Startup

