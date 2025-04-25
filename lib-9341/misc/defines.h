#pragma once
#ifdef __cplusplus
	#include <cstddef>
	#include <cstdint>
#else
	#include <stddef.h>
	#include <stdint.h>
#endif

// C++の場合
#include <stdlib.h>
#define STD_SDK
#define PROGMEM
#define PGM_P const char *
#ifndef pgm_read_byte
	#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
	#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
	#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif
//  #define PICO_CYW43_SUPPORTED
#define ARDUINO_ARCH_RP2040

// #define ENABLE_DEBUG_PRINTF
// #define ENABLE_DEBUG_CORE
// #define ENABLE_DEBUG_SPI
// #define ENABLE_DEBUG_WIRE

/// @brief 特定のビットを立てるための関数
/// @tparam N
/// @param a
/// @param i
/// @return
/// @example
/// ```cpp
/// constexpr int bit_positions[] = {0, 3, 5};
/// constexpr uint64_t bitmask = __bitset(bit_positions);
/// bitmaskは、0, 3, 5ビットが立った64bitのuintになる。

template <size_t N>
constexpr uint64_t __bitset(const int (&a)[N], size_t i = 0U) {
	return i < N ? (1LL << a[i]) | __bitset(a, i + 1) : 0;
}


/// 漢字関連のシンボル
#define 	TFT_FORCE_HANKANA				// Unicodeの半角カナ文字を強制的に１バイトカナとして扱う

#pragma GCC optimize("O0")