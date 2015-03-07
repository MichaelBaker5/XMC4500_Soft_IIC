################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Lib/subdir.mk
-include Dave/Generated/src/LIBS/subdir.mk
-include Dave/Generated/src/IO004/subdir.mk
-include Dave/Generated/src/DAVESupport/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
XMC4500_IO_Test.hex \

SECONDARY_LIST += \
XMC4500_IO_Test.lst \

SECONDARY_SIZE += \
XMC4500_IO_Test.siz \


# All Target
all: XMC4500_IO_Test.elf secondary-outputs

# Tool invocations
XMC4500_IO_Test.elf: $(OBJS) ../XMC4500_IO_Test.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM-GCC C Linker'
	"D:\DAVE-3.1.10\ARM-GCC/bin/arm-none-eabi-gcc" -T"../XMC4500_IO_Test.ld" -nostartfiles -L"D:\DAVE-3.1.10\eclipse\/../CMSIS/Infineon/Lib" -L"D:\DAVE-3.1.10\eclipse\/../Examples/Lib" -L"D:\DAVE-3.1.10\eclipse\/../emWin/Start/GUI" -Wl,-Map,"XMC4500_IO_Test.map" -mcpu=cortex-m4 -mthumb -g3 -gdwarf-2 -o "XMC4500_IO_Test.elf" "@makefile.rsp" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

XMC4500_IO_Test.hex: XMC4500_IO_Test.elf
	@echo 'Invoking: ARM-GCC Create Flash Image'
	"D:\DAVE-3.1.10\ARM-GCC/bin/arm-none-eabi-objcopy" -O ihex "XMC4500_IO_Test.elf"  "XMC4500_IO_Test.hex"
	@echo 'Finished building: $@'
	@echo ' '

XMC4500_IO_Test.lst: XMC4500_IO_Test.elf
	@echo 'Invoking: ARM-GCC Create Listing'
	"D:\DAVE-3.1.10\ARM-GCC/bin/arm-none-eabi-objdump" -h -S "XMC4500_IO_Test.elf" > "XMC4500_IO_Test.lst"
	@echo 'Finished building: $@'
	@echo ' '

XMC4500_IO_Test.siz: XMC4500_IO_Test.elf
	@echo 'Invoking: ARM-GCC Print Size'
	"D:\DAVE-3.1.10\ARM-GCC/bin/arm-none-eabi-size"  --format=berkeley XMC4500_IO_Test.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(SECONDARY_FLASH)$(S_DEPS)$(ARM_GCC_FOR_XMC_LINKER_OUTPUT_TYPE_OUTPUTS)$(SECONDARY_LIST) XMC4500_IO_Test.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
