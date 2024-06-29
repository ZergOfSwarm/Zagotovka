################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/cJSON.c \
../Core/Src/db.c \
../Core/Src/ds18b20.c \
../Core/Src/freertos.c \
../Core/Src/lwdtc.c \
../Core/Src/main.c \
../Core/Src/mongoose.c \
../Core/Src/multi_button.c \
../Core/Src/net.c \
../Core/Src/onewire.c \
../Core/Src/packed_fs.c \
../Core/Src/setings.c \
../Core/Src/stm32f7xx_hal_msp.c \
../Core/Src/stm32f7xx_hal_timebase_tim.c \
../Core/Src/stm32f7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f7xx.c 

OBJS += \
./Core/Src/cJSON.o \
./Core/Src/db.o \
./Core/Src/ds18b20.o \
./Core/Src/freertos.o \
./Core/Src/lwdtc.o \
./Core/Src/main.o \
./Core/Src/mongoose.o \
./Core/Src/multi_button.o \
./Core/Src/net.o \
./Core/Src/onewire.o \
./Core/Src/packed_fs.o \
./Core/Src/setings.o \
./Core/Src/stm32f7xx_hal_msp.o \
./Core/Src/stm32f7xx_hal_timebase_tim.o \
./Core/Src/stm32f7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f7xx.o 

C_DEPS += \
./Core/Src/cJSON.d \
./Core/Src/db.d \
./Core/Src/ds18b20.d \
./Core/Src/freertos.d \
./Core/Src/lwdtc.d \
./Core/Src/main.d \
./Core/Src/mongoose.d \
./Core/Src/multi_button.d \
./Core/Src/net.d \
./Core/Src/onewire.d \
./Core/Src/packed_fs.d \
./Core/Src/setings.d \
./Core/Src/stm32f7xx_hal_msp.d \
./Core/Src/stm32f7xx_hal_timebase_tim.d \
./Core/Src/stm32f7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/db.cyclo ./Core/Src/db.d ./Core/Src/db.o ./Core/Src/db.su ./Core/Src/ds18b20.cyclo ./Core/Src/ds18b20.d ./Core/Src/ds18b20.o ./Core/Src/ds18b20.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/lwdtc.cyclo ./Core/Src/lwdtc.d ./Core/Src/lwdtc.o ./Core/Src/lwdtc.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mongoose.cyclo ./Core/Src/mongoose.d ./Core/Src/mongoose.o ./Core/Src/mongoose.su ./Core/Src/multi_button.cyclo ./Core/Src/multi_button.d ./Core/Src/multi_button.o ./Core/Src/multi_button.su ./Core/Src/net.cyclo ./Core/Src/net.d ./Core/Src/net.o ./Core/Src/net.su ./Core/Src/onewire.cyclo ./Core/Src/onewire.d ./Core/Src/onewire.o ./Core/Src/onewire.su ./Core/Src/packed_fs.cyclo ./Core/Src/packed_fs.d ./Core/Src/packed_fs.o ./Core/Src/packed_fs.su ./Core/Src/setings.cyclo ./Core/Src/setings.d ./Core/Src/setings.o ./Core/Src/setings.su ./Core/Src/stm32f7xx_hal_msp.cyclo ./Core/Src/stm32f7xx_hal_msp.d ./Core/Src/stm32f7xx_hal_msp.o ./Core/Src/stm32f7xx_hal_msp.su ./Core/Src/stm32f7xx_hal_timebase_tim.cyclo ./Core/Src/stm32f7xx_hal_timebase_tim.d ./Core/Src/stm32f7xx_hal_timebase_tim.o ./Core/Src/stm32f7xx_hal_timebase_tim.su ./Core/Src/stm32f7xx_it.cyclo ./Core/Src/stm32f7xx_it.d ./Core/Src/stm32f7xx_it.o ./Core/Src/stm32f7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f7xx.cyclo ./Core/Src/system_stm32f7xx.d ./Core/Src/system_stm32f7xx.o ./Core/Src/system_stm32f7xx.su

.PHONY: clean-Core-2f-Src

