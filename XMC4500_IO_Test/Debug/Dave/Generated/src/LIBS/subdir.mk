################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dave/Generated/src/LIBS/CCU8PWMLIB.c \
../Dave/Generated/src/LIBS/MOTORLIB.c 

OBJS += \
./Dave/Generated/src/LIBS/CCU8PWMLIB.o \
./Dave/Generated/src/LIBS/MOTORLIB.o 

C_DEPS += \
./Dave/Generated/src/LIBS/CCU8PWMLIB.d \
./Dave/Generated/src/LIBS/MOTORLIB.d 


# Each subdirectory must supply rules for building sources it contributes
Dave/Generated/src/LIBS/%.o: ../Dave/Generated/src/LIBS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"D:\DAVE-3.1.10\ARM-GCC/bin/arm-none-eabi-gcc" -DDAVE_CE -DUC_ID=4502003 -I"D:\DAVE-3.1.10\eclipse\/../CMSIS/Include" -I"D:\DAVE-3.1.10\eclipse\/../CMSIS/Infineon/Include" -I"D:\DAVE-3.1.10\ARM-GCC/arm-none-eabi/include" -I"D:\DAVE-3.1.10\eclipse\/../emWin/Start/GUI/inc" -I"D:\DAVE-3.1.10\eclipse\/../CMSIS/Infineon/XMC4500_series/Include" -I"E:\Project_Double_Wheel_Self_Balance_Car\Merge\XMC4500_IO_Test\Dave\Generated\inc\LIBS" -I"E:\Project_Double_Wheel_Self_Balance_Car\Merge\XMC4500_IO_Test\Dave\Generated\inc\MOTORLIBS" -I"E:\Project_Double_Wheel_Self_Balance_Car\Merge\XMC4500_IO_Test\Dave\Generated\inc\DAVESupport" -O0 -ffunction-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


