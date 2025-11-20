################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj" -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj/Debug" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source/third_party/CMSIS/Core/Include" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-2019655745: ../kyles_kirb-3.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/Users/elibell/ti/sysconfig_1.25.0/sysconfig_cli.sh" --script "/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj/kyles_kirb-3.syscfg" -o "." -s "/Users/elibell/ti/mspm0_sdk_2_08_00_03/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-2019655745 ../kyles_kirb-3.syscfg
device.opt: build-2019655745
device.cmd.genlibs: build-2019655745
ti_msp_dl_config.c: build-2019655745
ti_msp_dl_config.h: build-2019655745
Event.dot: build-2019655745

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj" -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj/Debug" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source/third_party/CMSIS/Core/Include" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: /Users/elibell/ti/mspm0_sdk_2_08_00_03/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj" -I"/Users/elibell/Documents/kirb-3/kyles_final_proj/kyles_final_proj/Debug" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source/third_party/CMSIS/Core/Include" -I"/Users/elibell/ti/mspm0_sdk_2_08_00_03/source" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


