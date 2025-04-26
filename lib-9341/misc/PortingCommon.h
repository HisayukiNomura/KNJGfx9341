
/**
 * @file GlobalCommon.h
 * @brief 元々、このファイルはcore/arduino.hだった。標準SDKに対応させるため、各種の変更を行ったが、すでにArduino用では
 *        なくなったため、名前を変更した。完全な元のファイルは、Adafruit_GFX_LibraryのGITHubリポジトリを参照。
 * 		  このヘッダファイルには、なるべく追加せず、それぞれのcppは、それぞれの h　を持つようにするのが好み。
 *
 *        元になったコード（STD_SDKシンボルの有無で元の状態としてビルドされる）は、末尾にコメントとして残してある。
 */
#pragma once
#include "../misc/defines.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../core/stdlib_noniso.h"  // stdlibで非標準のitoaなどの関数。stdlib.hにないものも含んでいる
#include <hardware/clocks.h>              // 標準SDK：　Required for the port*Register macros

#include <hardware/gpio.h>  // 標準SDK：　Required for the port*Register macros
#include "WString.h"        // 組み込みむけに改変されたStringクラス。C++ stdのstringに書き換えたいのだが

// Arduino IDEでは、F_CPUの定義がおそらく Tools > CPU SPEED で設定されてF_CPUに定義されているようだ。(デフォルトはなぜか200Mhzでオーバークロック)
// 似た方法でもよかった（CMakeFile.txt）が、一応正攻法にする
#undef clockCyclesPerMicrosecond
#undef clockCyclesToMicroseconds
#undef microsecondsToClockCycles

#define clockCyclesPerMicrosecond() (clock_get_hz(clk_sys) / 1000000L)    // システムクロック周波数を取得
#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())  // クロックサイクルからマイクロ秒への変換
#define microsecondsToClockCycles(a) ((a) * clockCyclesPerMicrosecond())  // クロックサイクルからマイクロ秒への変換

// FreeRTOS potential calls
extern bool __isFreeRTOS;

// PSTR/etc.
#ifndef FPSTR
	#define FPSTR (const char *)
#endif

#ifndef PGM_VOID_P
	#define PGM_VOID_P const void *
#endif

// emptyString is an ESP-ism, a constant string with ""

extern const String emptyString;

// Warn users trying to use Pico SDK's STDIO implementation

#include <pico/stdio.h>  // Ensure it won't be re-included elsewhere

#if false
	#pragma once
	#include "../misc/defines.h"
	#ifdef STD_SDK
		#include <stdint.h>
		#include <stdlib.h>
		#include <string.h>
		#include <math.h>
		#include "lib-9341/core/stdlib_noniso.h"  // stdlibで非標準のitoaなどの関数。stdlib.hにないものも含んでいる
		#include <hardware/gpio.h>                // 標準SDK：　Required for the port*Register macros
		#include "WString.h"                      // 組み込みむけに改変されたStringクラス。C++ stdのstringに書き換えたいのだが
// const String emptyString = "";
	#else
		#include <stdint.h>
		#include <stdlib.h>
		#include <string.h>
		#include "stdlib_noniso.h"  // Wacky deprecated AVR compatibility functions
		#include "RP2040Version.h"
		#include "api/ArduinoAPI.h"
		#include "api/itoa.h"  // ARM toolchain doesn't provide itoa etc, provide them
		#include <pins_arduino.h>
		#include <hardware/gpio.h>  // Required for the port*Register macros
		#include "debug_internal.h"
	#endif
// Try and make the best of the old Arduino abs() macro.  When in C++, use
// the sane std::abs() call, but for C code use their macro since stdlib abs()
// is int but their macro "works" for everything (with potential side effects)
	#ifdef STD_SDK
// Raspberry PI PICOでは、absもroundもstdlibで定義されているように見えるので、削除しても構わない
	#else
		#ifdef abs
			#undef abs
		#endif  // abs
		#if defined(__cplusplus) && !defined(__riscv)
using std::abs;
	// in my stdlib.h, no round function is defined. So use old Ardiuno round function here.
			#ifdef STD_SDK
				#define round(x) ({ __typeof__(x) _x = (x); _x >= 0 ? (long)(_x + 0.5) : (long)(_x - 0.5); })
			#else
using std::round;
			#endif
		#else
			#define abs(x) ({ __typeof__(x) _x = (x); _x >= 0 ? _x : -_x; })
			#define round(x) ({ __typeof__(x) _x = (x); _x >= 0 ? (long)(_x + 0.5) : (long)(_x - 0.5); })
		#endif
	#endif

	#ifdef STD_SDK
// Arduino IDEでは、F_CPUの定義がおそらく Tools > CPU SPEED で設定されてF_CPUに定義されているようだ。(デフォルトはなぜか200Mhzでオーバークロック)
// 似た方法でもよかった（CMakeFile.txt）が、一応正攻法にする
		#define clockCyclesPerMicrosecond() (clock_get_hz(clk_sys) / 1000000L)    // システムクロック周波数を取得
		#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())  // クロックサイクルからマイクロ秒への変換
		#define microsecondsToClockCycles(a) ((a) * clockCyclesPerMicrosecond())  // クロックサイクルからマイクロ秒への変換
	#else
// For compatibility to many platforms and libraries
		#define clockCyclesPerMicrosecond() (F_CPU / 1000000L)
		#define clockCyclesToMicroseconds(a) ((a) / clockCyclesPerMicrosecond())
		#define microsecondsToClockCycles(a) ((a) * clockCyclesPerMicrosecond())
	#endif

	#ifdef STD_SDK
// interrupts と noInterruptsは wiring_private.cppで定義されている。ヘッダをきちんと用意して、そっちをIncludeしてもらおう。
	#else
// Disable/re-enable all interrupts.  Safely handles nested disables
void interrupts();
void noInterrupts();
	#endif

	#ifdef STD_SDK
// Adafruit Metro RP2350などでは、PSRAMという特殊なメモリが積まれているが、
// このプログラムではターゲットにしない。テストできないのと、TFTの処理では使用されてないので。
// 本体は...\rp2040\hardware\rp2040\4.5.1\cores\rp2040\malloc-lock.cpp
// にあるので、使うときはこれを含める。
	#else
// Only implemented on some RP2350 boards, not the OG Pico 2
		#ifdef RP2350_PSRAM_CS
void *pmalloc(size_t size);
void *pcalloc(size_t count, size_t size);
		#else
[[deprecated("This chip does not have PSRAM, pmalloc will always fail")]] void *pmalloc(size_t size);
[[deprecated("This chip does not have PSRAM, pcalloc will always fail")]] void *pcalloc(size_t count, size_t size);
		#endif
	#endif

	#ifdef STD_SDK
// AVRなどで、USE＿FAST_PINIOシンボルを定義したときに、直接ポートを操作するときに使われるらしい。
// #if defined(__AVR__)
// 		for (uint8_t i = 0; i < NUM_DIGITAL_PINS; i++) {
//		 if ((PORTreg_t)portOutputRegister(digitalPinToPort(i)) == tft8.writePort)
// のようなコードがみられる。
// AVR以外の場合は、digitalWireが呼びだされる。呼び出しのオーバーヘッドを減らすためだろう。
// PICOでも、こういうカタチにコードを書き直すこともできそうだが、今はditialWriteを使う。
// 確かに、ditigalWriteの本体（wiring_digital.cpp:__digitalWrite を見ると、ポート出力の前に
// directionの設定など一度だけやっておけば済むようなコードがたくさんある。

	#else
		#ifdef __cplusplus
extern "C" {
		#endif  // __cplusplus
	// AVR compatibility macros...naughty and accesses the HW directly
		#define digitalPinToPort(pin) (0)
		#define digitalPinToBitMask(pin) (1UL << (pin))
		#define digitalPinToTimer(pin) (0)
		#define digitalPinToInterrupt(pin) (pin)
		#define NOT_AN_INTERRUPT (-1)
		#define portOutputRegister(port) ((volatile uint32_t *)&(sio_hw->gpio_out))
		#define portInputRegister(port) ((volatile uint32_t *)&(sio_hw->gpio_in))
		#define portModeRegister(port) ((volatile uint32_t *)&(sio_hw->gpio_oe))
		#define digitalWriteFast(pin, val) (val ? sio_hw->gpio_set = (1 << pin) : sio_hw->gpio_clr = (1 << pin))
		#define digitalReadFast(pin) ((1 << pin) & sio_hw->gpio_in)
		#define sei() interrupts()
		#define cli() noInterrupts()

		#ifdef __cplusplus
}
		#endif  // __cplusplus
	#endif

	#ifdef STD_SDK
// Raspberry PI PICO用の、アナログポート操作関数。本体はwiring_analog.cppなので、ヘッダをきちんと用意して、そっちをIncludeしてもらおう。
	#else

		#ifdef __cplusplus
extern "C" {
		#endif  // __cplusplus
	// ADC RP2040-specific calls
	void analogReadResolution(int bits);
		#ifdef __cplusplus
	float analogReadTemp(float vref = 3.3);  // Returns core temp in Centigrade
		#endif

	// PWM RP2040-specific calls
	void analogWriteFreq(uint32_t freq);
	void analogWriteRange(uint32_t range);
	void analogWriteResolution(int res);
		#ifdef __cplusplus
}  // extern "C"
		#endif
	#endif

// FreeRTOS potential calls
extern bool __isFreeRTOS;

	#ifdef STD_SDK
	#else
	// Ancient AVR defines
		#define HAVE_HWSERIAL0
		#define HAVE_HWSERIAL1
		#define HAVE_HWSERIAL2
	#endif

// PSTR/etc.
	#ifndef FPSTR
		#define FPSTR (const char *)
	#endif

	#ifndef PGM_VOID_P
		#define PGM_VOID_P const void *
	#endif

// emptyString is an ESP-ism, a constant string with ""

extern const String emptyString;

	#ifdef STD_SDK
// ここに昔、__bitsetというテンプレートがあったが、defines.hに移動した。
// 順番や、相互に依存しない単純なヘッダファイルにある方がふさわしいため
	#else
extern const String emptyString;

		#ifdef USE_TINYUSB
	// Needed for declaring Serial
			#include "Adafruit_USBD_CDC.h"
		#else
			#include "SerialUSB.h"
		#endif

		#include "SerialUART.h"
		#include "SerialSemi.h"
		#include "RP2040Support.h"
		#include "SerialPIO.h"
		#include "Bootsel.h"
// Template which will evaluate at *compile time* to a single 32b number
// with the specified bits set.
template <size_t N>
constexpr uint64_t __bitset(const int (&a)[N], size_t i = 0U) {
	return i < N ? (1LL << a[i]) | __bitset(a, i + 1) : 0;
}
	#endif

// Warn users trying to use Pico SDK's STDIO implementation

	#include <pico/stdio.h>  // Ensure it won't be re-included elsewhere
	#ifdef STD_SDK           // serial portはArduinoのときはシステムで占有されているが、標準SDKならOKなのでは？

	#else
		#undef stdio_uart_init
		#define stdio_uart_init(...) static_assert(0, "stdio_uart_init is not supported or needed. Either use Serial.printf() or set the debug port in the IDE to Serial/1/2 and use printf().  See https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1540354673 and https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1546783109")
		#undef stdio_init_all
		#define stdio_init_all(...) static_assert(0, "stdio_init_all is not supported or needed. Either use Serial.printf() or set the debug port in the IDE to Serial/1/2 and use printf().  See https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1540354673 and https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1546783109")
		#undef stdio_usb_init
		#define stdio_usb_init(...) static_assert(0, "stdio_usb_init is not supported or needed. Either use Serial.printf() or set the debug port in the IDE to Serial/1/2 and use printf().  See https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1540354673 and https://github.com/earlephilhower/arduino-pico/issues/1433#issuecomment-1546783109")
	
	// PSRAM decorator
		#define PSRAM __attribute__((section("\".psram\"")))

	// General GPIO/ADC layout info
		#ifdef PICO_RP2350B
			#define __GPIOCNT 48
			#define __FIRSTANALOGGPIO 40
		#else
			#define __GPIOCNT 30
			#define __FIRSTANALOGGPIO 26
		#endif

	#endif

	#ifdef STD_SDK
	#else
		#ifdef __cplusplus
using namespace arduino;
		#endif
	#endif
#endif