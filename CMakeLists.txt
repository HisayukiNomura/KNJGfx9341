# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags for Release build" FORCE)
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags for Release build" FORCE)

# set(CMAKE_BUILD_TYPE Release)


# set(CMAKE_BUILD_TYPE Debug)
# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# check board type
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type") 

if (NOT DEFINED PICO_BOARD)
    message(FATAL_ERROR "PICO_BOARD is not defined. Make sure to configure CMakeLists according to the target board.")
else()
    message (STATUS "PICO_BOARD = ${PICO_BOARD}")
endif()
# message(FATAL_ERROR "debug stop")

# プログラム内で処理を分けられるように、マクロ値としてコンパイラに渡す
if(PICO_BOARD STREQUAL "pico")
    set(PICO_BOARD_VALUE 1)
elseif(PICO_BOARD STREQUAL "pico_w")
    set(PICO_BOARD_VALUE 3)
elseif(PICO_BOARD STREQUAL "pico2")
    set(PICO_BOARD_VALUE 2)
elseif(PICO_BOARD STREQUAL "pico2_w")
    set(PICO_BOARD_VALUE 4)
else()
    set(PICO_BOARD_VALUE 0) # 未知のボード
endif()
add_compile_definitions(PICO_BOARD_VALUE=${PICO_BOARD_VALUE})


# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(KNJGfx9341 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()



# Add executable. Default name is the project name, version 0.1

set(SRC_FILES
                            KNJGfx9341.cpp 
                            lib-9341/misc/defines.cpp

                            lib-9341/Adafruit_GFX_Library/Adafruit_GFX.cpp
                            lib-9341/Adafruit_GFX_Library/Adafruit_SPITFT.cpp

                            lib-9341/Adafruit_ILI9341/Adafruit_ILI9341.cpp
                            lib-9341/spi/spi.cpp
                            lib-9341/Kanji/KanjiHelper.cpp
                            lib-9341/core/PIOProgram.cpp
                            lib-9341/core/delay.cpp
                            lib-9341/core/_freertos.cpp
                            lib-9341/core/CoreMutex.cpp
                            lib-9341/core/Print.cpp
                            lib-9341/core/RP2040Support.cpp
                            lib-9341/core/stdlib_noniso.cpp
                            lib-9341/core/WString.cpp
                            lib-9341/core/wiring_private.cpp
                            lib-9341/core/wiring_digital.cpp
                            lib-9341/core/wiring_analog.cpp

                            lib-9341/XPT2046_Touchscreen/XPT2046_Touchscreen.cpp

                            lib-9341/misc/pins.cpp
)
if (PICO_BOARD_VALUE STREQUAL 3 OR PICO_BOARD_VALUE STREQUAL 4)
    message(STATUS "Pico W detected. cyw43_wrappers.cpp will be included.")
    list(APPEND SRC_FILES lib-9341/core/cyw43_wrappers.cpp)
else()
    message(STATUS "Pico W not detected. cyw43_wrappers.cpp will not be included.")
endif()
add_executable(KNJGfx9341 ${SRC_FILES})




pico_set_program_name(KNJGfx9341 "KNJGfx9341")
pico_set_program_version(KNJGfx9341 "0.1")
target_link_options(KNJGfx9341 PRIVATE -Wl,-Map=output.map)

# Generate PIO header
#pico_generate_pio_header(KNJGfx9341 ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(KNJGfx9341 1)
pico_enable_stdio_usb(KNJGfx9341 0)

# Modify the below lines to enable/disable output over UART/USB
#pico_enable_stdio_uart(KNJGfx9341 0)
#pico_enable_stdio_usb(KNJGfx9341 0)

# Add the standard library to the build
target_link_libraries(KNJGfx9341
        pico_stdlib
        pico_rand
        pico_unique_id
        hardware_exception
        )

# Add the standard include files to the build
target_include_directories(KNJGfx9341 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/misc
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_GFX_Library/Fonts
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_BusIO
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Adafruit_IL9341
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/core
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/spi
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/variants
        ${CMAKE_CURRENT_LIST_DIR}/lib-9341/Kanji
        ${CMAKE_CURRENT_LIST_DIR}/XPT2046_Touchscreen    
        ${PICO_SDK_PATH}/src/rp2_common/hardware_exception/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_driver/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_driver/include/pico
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_cyw43_arch/include/pico
        ${PICO_SDK_PATH}/src/rp2_common/pico_async_context/include
        ${PICO_SDK_PATH}/src/rp2_common/pico_async_context/include/pico
        ${PICO_SDK_PATH}/src/rp2_common/hardware_adc/include/
        ${PICO_SDK_PATH}/src/rp2_common/hardware_pwm/include/
        ${PICO_SDK_PATH}/lib/lwip/src/include
        ${PICO_SDK_PATH}/lib/lwip/src/include/iwip
        ${PICO_SDK_PATH}/lib/cyw43-driver/src/
)

# Add any user requested libraries
set (LINK_LIB hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_interp
        hardware_timer
        hardware_watchdog
        hardware_clocks
        hardware_exception
        hardware_adc
)

if (PICO_BOARD_VALUE STREQUAL 3 OR PICO_BOARD_VALUE STREQUAL 4)
    message(STATUS "Pico W detected. pico_cyw43_arch_none is temporary added to target_link_libraries. Please adjust it according to your required functionality.")
    list(APPEND LINK_LIB pico_cyw43_arch_none)
else()
    message(STATUS "Pico W not detected. target_link_libraries will not change.")
endif()
target_link_libraries(KNJGfx9341 ${LINK_LIB})
pico_add_extra_outputs(KNJGfx9341)
