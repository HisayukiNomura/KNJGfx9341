#pragma once
#include "../misc/defines.h"
/***
 * @file pins_arduino.h
 * @brief ボード種別ごとのピン定義。とはいっても、PICO/PICOWで内容に違いはない。一応、元のコードに準拠した。
 * ただし、MicroPythonのコードでは、ビルドしているボードの種類を#ifで判断できないので、特別にしてある。
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
#if defined(MICROPY_BUILD_TYPE)
	#ifdef PIN_LED
		#undef PIN_LED

		// Serial
		#undef PIN_SERIAL1_TX
		#undef PIN_SERIAL1_RX

		#undef PIN_SERIAL2_TX
		#undef PIN_SERIAL2_RX

		// SPI
		#undef PIN_SPI0_MISO
		#undef PIN_SPI0_MOSI
		#undef PIN_SPI0_SCK
		#undef PIN_SPI0_SS

		#undef PIN_SPI1_MISO
		#undef PIN_SPI1_MOSI
		#undef PIN_SPI1_SCK
		#undef PIN_SPI1_SS

		// Wire
		#undef PIN_WIRE0_SDA
		#undef PIN_WIRE0_SCL

		#undef PIN_WIRE1_SDA
		#undef PIN_WIRE1_SCL

		#undef SERIAL_HOWMANY
		#undef SPI_HOWMANY
		#undef WIRE_HOWMANY
	#endif
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

#elif PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2 || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_2) // PICO/PICO2
	// LEDs
	#ifdef PIN_LED
		#undef PIN_LED

		// Serial
		#undef PIN_SERIAL1_TX
		#undef PIN_SERIAL1_RX

		#undef PIN_SERIAL2_TX
		#undef PIN_SERIAL2_RX

		// SPI
		#undef PIN_SPI0_MISO
		#undef PIN_SPI0_MOSI
		#undef PIN_SPI0_SCK
		#undef PIN_SPI0_SS

		#undef PIN_SPI1_MISO
		#undef PIN_SPI1_MOSI
		#undef PIN_SPI1_SCK
		#undef PIN_SPI1_SS

		// Wire
		#undef PIN_WIRE0_SDA
		#undef PIN_WIRE0_SCL

		#undef PIN_WIRE1_SDA
		#undef PIN_WIRE1_SCL

		#undef SERIAL_HOWMANY
		#undef SPI_HOWMANY
		#undef WIRE_HOWMANY
	#endif
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
	#pragma once
	#include "../misc/defines.h"
	/***
	 * @file pins_arduino.h
	 * @brief ボード種別ごとのピン定義。とはいっても、PICO/PICOWで内容に違いはない。一応、元のコードに準拠した。
	 * ただし、MicroPythonのコードでは、ビルドしているボードの種類を#ifで判断できないので、特別にしてある。
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
	#if defined(MICROPY_BUILD_TYPE)
		#ifdef PIN_LED
			#undef PIN_LED

			// Serial
			#undef PIN_SERIAL1_TX
			#undef PIN_SERIAL1_RX

			#undef PIN_SERIAL2_TX
			#undef PIN_SERIAL2_RX

			// SPI
			#undef PIN_SPI0_MISO
			#undef PIN_SPI0_MOSI
			#undef PIN_SPI0_SCK
			#undef PIN_SPI0_SS

			#undef PIN_SPI1_MISO
			#undef PIN_SPI1_MOSI
			#undef PIN_SPI1_SCK
			#undef PIN_SPI1_SS

			// Wire
			#undef PIN_WIRE0_SDA
			#undef PIN_WIRE0_SCL

			#undef PIN_WIRE1_SDA
			#undef PIN_WIRE1_SCL

			#undef SERIAL_HOWMANY
			#undef SPI_HOWMANY
			#undef WIRE_HOWMANY
		#endif
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

	#elif PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2 || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_2) // PICO/PICO2
		// LEDs
		#ifdef PIN_LED
			#undef PIN_LED

			// Serial
			#undef PIN_SERIAL1_TX
			#undef PIN_SERIAL1_RX

			#undef PIN_SERIAL2_TX
			#undef PIN_SERIAL2_RX

			// SPI
			#undef PIN_SPI0_MISO
			#undef PIN_SPI0_MOSI
			#undef PIN_SPI0_SCK
			#undef PIN_SPI0_SS

			#undef PIN_SPI1_MISO
			#undef PIN_SPI1_MOSI
			#undef PIN_SPI1_SCK
			#undef PIN_SPI1_SS

			// Wire
			#undef PIN_WIRE0_SDA
			#undef PIN_WIRE0_SCL

			#undef PIN_WIRE1_SDA
			#undef PIN_WIRE1_SCL

			#undef SERIAL_HOWMANY
			#undef SPI_HOWMANY
			#undef WIRE_HOWMANY
		#endif
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
	#elif PICO_BOARD_VALUE == 3 || PICO_BOARD_VALUE == 4 || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO_2W) // PICO W/2W
		#ifdef PIN_LED
			#undef PIN_LED

			// Serial
			#undef PIN_SERIAL1_TX
			#undef PIN_SERIAL1_RX

			#undef PIN_SERIAL2_TX
			#undef PIN_SERIAL2_RX

			// SPI
			#undef PIN_SPI0_MISO
			#undef PIN_SPI0_MOSI
			#undef PIN_SPI0_SCK
			#undef PIN_SPI0_SS

			#undef PIN_SPI1_MISO
			#undef PIN_SPI1_MOSI
			#undef PIN_SPI1_SCK
			#undef PIN_SPI1_SS

			// Wire
			#undef PIN_WIRE0_SDA
			#undef PIN_WIRE0_SCL

			#undef PIN_WIRE1_SDA
			#undef PIN_WIRE1_SCL

			#undef SERIAL_HOWMANY
			#undef SPI_HOWMANY
			#undef WIRE_HOWMANY
		#endif
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
		#if defined(ARDUINO)
			#define ERRMSG "It appears to have been built from the Arduino IDE, but the specified target board is not supported."
			#error ERRMSG
		#else
			#error "It appears to have been build from standard SDK , but PICO_BOARD is not defined. Please define it in CMakeLists.txt(recomended) or in defines.h"
		#endif
	#endif

	#define PIN_WIRE1_SDA (26u)
	#define PIN_WIRE1_SCL (27u)

	#define SERIAL_HOWMANY (3u)
	#define SPI_HOWMANY (2u)
	#define WIRE_HOWMANY (2u)
#elif PICO_BOARD_VALUE == 3 || PICO_BOARD_VALUE == 4 || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_RASPBERRY_PI_PICO_2W) // PICO W/2W
	#ifdef PIN_LED
		#undef PIN_LED

		// Serial
		#undef PIN_SERIAL1_TX
		#undef PIN_SERIAL1_RX

		#undef PIN_SERIAL2_TX
		#undef PIN_SERIAL2_RX

		// SPI
		#undef PIN_SPI0_MISO
		#undef PIN_SPI0_MOSI
		#undef PIN_SPI0_SCK
		#undef PIN_SPI0_SS

		#undef PIN_SPI1_MISO
		#undef PIN_SPI1_MOSI
		#undef PIN_SPI1_SCK
		#undef PIN_SPI1_SS

		// Wire
		#undef PIN_WIRE0_SDA
		#undef PIN_WIRE0_SCL

		#undef PIN_WIRE1_SDA
		#undef PIN_WIRE1_SCL

		#undef SERIAL_HOWMANY
		#undef SPI_HOWMANY
		#undef WIRE_HOWMANY
	#endif
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
	#if defined(ARDUINO)
		#define ERRMSG "It appears to have been built from the Arduino IDE, but the specified target board is not supported."
		#error ERRMSG
	#else
		#error "It appears to have been build from standard SDK , but PICO_BOARD is not defined. Please define it in CMakeLists.txt(recomended) or in defines.h"
	#endif
#endif
