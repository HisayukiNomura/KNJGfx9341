# Micropythonの拡張Cモジュールとしてビルドするときに使用する cmakeファイル。
# C/C++から使用するときにはこのcmakeファイルは使われない。

# インタフェースライブラリを作成する。
add_library(KNJGfx9341 INTERFACE)

set(SRC_FILES
                            ${CMAKE_CURRENT_LIST_DIR}/misc/defines.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/Adafruit_GFX_Library/Adafruit_GFX.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/Adafruit_GFX_Library/Adafruit_SPITFT.cpp

                            ${CMAKE_CURRENT_LIST_DIR}/Adafruit_ILI9341/Adafruit_ILI9341.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/spi/spi.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/Kanji/KanjiHelper.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/PIOProgram.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/cyw43_wrappers.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/delay.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/_freertos.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/CoreMutex.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/Print.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/RP2040Support.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/stdlib_noniso.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/WString.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/wiring_private.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/wiring_digital.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/core/wiring_analog.cpp

                            ${CMAKE_CURRENT_LIST_DIR}/XPT2046_Touchscreen/XPT2046_Touchscreen.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/misc/pins.cpp
)
if (PICO_BOARD_VALUE STREQUAL 3 OR PICO_BOARD_VALUE STREQUAL 4)
    message(STATUS "Pico W detected. cyw43_wrappers.cpp will be included.")
    list(APPEND SRC_FILES  ${PICO_SDK_PATH}/../lib/cyw43-driver/src/cyw43_wrappers.cpp)
else()
    message(STATUS "Pico W not detected. cyw43_wrappers.cpp will not be included.")
endif()
target_sources(KNJGfx9341 INTERFACE ${SOURCE_FILES})


# インクルードディレクトリをカレントディレクトリに設定。
target_include_directories(KNJGfx9341 INTERFACE

        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/misc
        ${CMAKE_CURRENT_LIST_DIR}/Adafruit_GFX_Library
        ${CMAKE_CURRENT_LIST_DIR}/Adafruit_GFX_Library/Fonts
        ${CMAKE_CURRENT_LIST_DIR}/Adafruit_BusIO
        ${CMAKE_CURRENT_LIST_DIR}/Adafruit_IL9341
        ${CMAKE_CURRENT_LIST_DIR}/core
        ${CMAKE_CURRENT_LIST_DIR}/spi
        ${CMAKE_CURRENT_LIST_DIR}/variants
        ${CMAKE_CURRENT_LIST_DIR}/Kanji
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
        ${PICO_SDK_PATH}/../lib/cyw43-driver/src/)


# ユーザーモジュールライブラリとしてリンクする
target_link_libraries(usermod INTERFACE KNJGfx9341)