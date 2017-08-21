################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/App.c \
../Sources/FTM.c \
../Sources/GPIO.c \
../Sources/PORT.c \
../Sources/PWM.c \
../Sources/Pin.c \
../Sources/SysTick.c \
../Sources/UART.c \
../Sources/joy.c 

OBJS += \
./Sources/App.o \
./Sources/FTM.o \
./Sources/GPIO.o \
./Sources/PORT.o \
./Sources/PWM.o \
./Sources/Pin.o \
./Sources/SysTick.o \
./Sources/UART.o \
./Sources/joy.o 

C_DEPS += \
./Sources/App.d \
./Sources/FTM.d \
./Sources/GPIO.d \
./Sources/PORT.d \
./Sources/PWM.d \
./Sources/Pin.d \
./Sources/SysTick.d \
./Sources/UART.d \
./Sources/joy.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"CPU_MK64FN1M0VMD12" -I"../Sources" -I"../Project_Settings/Startup_Code" -I"../SDK/platform/CMSIS/Include" -I"../SDK/platform/devices" -I"../SDK/platform/devices/MK64F12/include" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


