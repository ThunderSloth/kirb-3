################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
targetConfigs/Debug/%.o: ../targetConfigs/Debug/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/kyles/OneDrive/Desktop/kirb-3/kyles_final_proj/kyles_final_proj" -I"C:/Users/kyles/OneDrive/Desktop/kirb-3/kyles_final_proj/kyles_final_proj/Debug" -I"C:/ti/mspm0_sdk_2_08_00_03/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_08_00_03/source" -gdwarf-3 -MMD -MP -MF"targetConfigs/Debug/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


