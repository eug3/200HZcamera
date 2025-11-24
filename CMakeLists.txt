cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(200hzcamera C ASM)

set(CMAKE_C_STANDARD 11)

# MCU specific definitions
add_compile_options(
    -mcpu=cortex-m3
    -mthumb
    -Og
    -Wall
    -fdata-sections
    -ffunction-sections
    -g
    -gdwarf-2
)

add_definitions(
    -DUSE_HAL_DRIVER
    -DSTM32F103xB
)

# Include directories
include_directories(
    Core/Inc
    Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    Drivers/STM32F1xx_HAL_Driver/Inc
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
    Drivers/CMSIS/Include
)

# Source files
set(SOURCES
    Core/Src/main.c
    Core/Src/stm32f1xx_it.c
    Core/Src/stm32f1xx_hal_msp.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    Core/Src/system_stm32f1xx.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
    startup_stm32f103xb.s
)

# Linker script
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103XX_FLASH.ld)

# Executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Linker options
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -specs=nano.specs
    -T${LINKER_SCRIPT}
    -lc -lm
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)

# Generate HEX and BIN
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary -S ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf
    COMMENT "Building ${PROJECT_NAME}.hex and ${PROJECT_NAME}.bin"
)
