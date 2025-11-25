################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
targetConfigs/Debug/%.o: ../targetConfigs/Debug/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj" -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj/Debug" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source/third_party/CMSIS/Core/Include" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source" -gdwarf-3 -MMD -MP -MF"targetConfigs/Debug/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


