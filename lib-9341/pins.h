#pragma once
#include "defines.h"
/***
 * @file pins_arduino.h
 * @brief ボード種別ごとのピン定義
 * @details ボード種別のマクロにより、ピン番号を定義する。このマクロは、通常、CMakeLists.txt内で次のように定義される。
 * set(PICO_BOARD pico_w CACHE STRING "Board type")
 * if(PICO_BOARD STREQUAL "pico")
 *  set(PICO_BOARD_VALUE 1)
 * elseif(PICO_BOARD STREQUAL "pico_w")
 *	set(PICO_BOARD_VALUE 2)
 * elseif(PICO_BOARD STREQUAL "pico2")
 *	set(PICO_BOARD_VALUE 2)
 * elseif(PICO_BOARD STREQUAL "pico2_w")
 *	set(PICO_BOARD_VALUE 2)
 * else()
 *	set(PICO_BOARD_VALUE 0) # 未知のボード
 * endif()
 * add_compile_definitions(PICO_BOARD_VALUE=${PICO_BOARD_VALUE})
 * 
 * とにかく、このヘッダファイルをincludeするすべてのソースファイルで、PICO_BOARD_VALUEが定義されているようにすればよい
 * ので、define.hなどで定義しても良いが、ボードを変えるたびに修正が必要になる。
 */

#if PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2  // PICO/PICO2
	// LEDs
	#define PIN_LED (25u)

	// Serial
	#define PIN_SERIAL1_TX (0u)
	#define PIN_SERIAL1_RX (1u)

	#define PIN_SERIAL2_TX (8u)
	#define PIN_SERIAL2_RX (9u)

	// SPI
	#define PIN_SPI0_MISO (16u)
	#define PIN_SPI0_MOSI (19u)
	#define PIN_SPI0_SCK (18u)
	#define PIN_SPI0_SS (17u)

	#define PIN_SPI1_MISO (12u)
	#define PIN_SPI1_MOSI (15u)
	#define PIN_SPI1_SCK (14u)
	#define PIN_SPI1_SS (13u)

	// Wire
	#define PIN_WIRE0_SDA (4u)
	#define PIN_WIRE0_SCL (5u)

	#define PIN_WIRE1_SDA (26u)
	#define PIN_WIRE1_SCL (27u)

	#define SERIAL_HOWMANY (3u)
	#define SPI_HOWMANY (2u)
	#define WIRE_HOWMANY (2u)
#elif PICO_BOARD_VALUE == 3 || PICO_BOARD_VALUE == 4  // PICO W/2W
	// LEDs
	#define PIN_LED (25u)

	// Serial
	#define PIN_SERIAL1_TX (0u)
	#define PIN_SERIAL1_RX (1u)

	#define PIN_SERIAL2_TX (8u)
	#define PIN_SERIAL2_RX (9u)

	// SPI
	#define PIN_SPI0_MISO (16u)
	#define PIN_SPI0_MOSI (19u)
	#define PIN_SPI0_SCK (18u)
	#define PIN_SPI0_SS (17u)

	#define PIN_SPI1_MISO (12u)
	#define PIN_SPI1_MOSI (15u)
	#define PIN_SPI1_SCK (14u)
	#define PIN_SPI1_SS (13u)

	// Wire
	#define PIN_WIRE0_SDA (4u)
	#define PIN_WIRE0_SCL (5u)

	#define PIN_WIRE1_SDA (26u)
	#define PIN_WIRE1_SCL (27u)

	#define SERIAL_HOWMANY (3u)
	#define SPI_HOWMANY (2u)
	#define WIRE_HOWMANY (2u)
#else
#error "PICO_BOARD is not defined. Please define it in CMakeLists.txt(recomended) or in defines.h"
#endif


