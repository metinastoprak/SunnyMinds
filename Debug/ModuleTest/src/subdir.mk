################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ModuleTest/src/app_buttonled_test.c \
../ModuleTest/src/app_common.c \
../ModuleTest/src/app_moduletest.c 

OBJS += \
./ModuleTest/src/app_buttonled_test.o \
./ModuleTest/src/app_common.o \
./ModuleTest/src/app_moduletest.o 

C_DEPS += \
./ModuleTest/src/app_buttonled_test.d \
./ModuleTest/src/app_common.d \
./ModuleTest/src/app_moduletest.d 


# Each subdirectory must supply rules for building sources it contributes
ModuleTest/src/%.o ModuleTest/src/%.su ModuleTest/src/%.cyclo: ../ModuleTest/src/%.c ModuleTest/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H563xx -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I../AZURE_RTOS/App -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -I../ModuleTest/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ModuleTest-2f-src

clean-ModuleTest-2f-src:
	-$(RM) ./ModuleTest/src/app_buttonled_test.cyclo ./ModuleTest/src/app_buttonled_test.d ./ModuleTest/src/app_buttonled_test.o ./ModuleTest/src/app_buttonled_test.su ./ModuleTest/src/app_common.cyclo ./ModuleTest/src/app_common.d ./ModuleTest/src/app_common.o ./ModuleTest/src/app_common.su ./ModuleTest/src/app_moduletest.cyclo ./ModuleTest/src/app_moduletest.d ./ModuleTest/src/app_moduletest.o ./ModuleTest/src/app_moduletest.su

.PHONY: clean-ModuleTest-2f-src

