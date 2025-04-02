/*
    pinMode and digitalRead/Write for the Raspberry Pi Pico W RP2040
    Copyright (c) 2022 Earle F. Philhower, III <earlephilhower@yahoo.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// 元々、このファイルはcore/variants/picow とpico2wの中にあった、digital.h
// というファイルだった。
//　これらは、呼びだされ方が若干複雑で弱い参照などGCC依存の方法を使っている。
// しかも、Arduinoの構造に基づいてボード選択によりリンクされるファイルが決まっている。
// そこで、この部分は書き直すことにして、場所を/に移した。
// ただし、元のコードはコメントでそのまま残しておく。
// ・弱い参照は使わず、コンパイル時に渡されるマクロで条件コンパイル。（CMakleFiles.txtで定義される）
// ・元のコードは、ボードごとにほとんど同じ関数だったが、これらはまとめてしまう。


#include "defines.h"
#include "PortingCommon.h"
#include <cyw43_wrappers.h>

#ifdef STD_SDK
#include "core/wiring_digital.h"

/// @brief cy43の有無で、使用する関数を分ける
/// @param pin
/// @param mode
/// @details この処理は元のコードが凝っていて、弱い参照を使用している。
/// もともとは、このメソッドがあると（選択コンパイルでコンパイルされると）、wiringdigitalの中の pinMode定義は
/// 弱い参照なので無効になり、cy43_XXXX関数が使われるようになる。
/// 少し複雑なので、その難しい処理はやめることにする。
extern "C" void pinMode(pin_size_t pin, PinMode mode) {
	#if PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2  // PICO/PICO2
	__pinMode(pin, mode);
	#else
	cyw43_pinMode(pin, mode);
	#endif
}

extern "C" void digitalWrite(pin_size_t pin, PinStatus val) {
	#if PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2  // PICO/PICO2
    __digitalWrite(pin, val);
    #else
    cyw43_digitalWrite(pin, val);
    #endif
}

extern "C" PinStatus digitalRead(pin_size_t pin) {
	#if PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2  // PICO/PICO2
	return __digitalRead(pin);
	#else
	return cyw43_digitalRead(pin);
	#endif
}
/// @brief init_cyw43_wifi()の呼び出し
/// @details 元々、これはArduiunoのmainの中でweak参照された、何もない関数と、init_cyw43_wifiの切り替えになっていた。
extern "C" void initVariant() {
	#if PICO_BOARD_VALUE == 1 || PICO_BOARD_VALUE == 2  // PICO/PICO2
    #else
	init_cyw43_wifi();
    #endif
}

#else
	extern "C" void pinMode(pin_size_t pin, PinMode mode) {
		cyw43_pinMode(pin, mode);
}

extern "C" void digitalWrite(pin_size_t pin, PinStatus val) {
	cyw43_digitalWrite(pin, val);
}

extern "C" PinStatus digitalRead(pin_size_t pin) {
	return cyw43_digitalRead(pin);
}
#endif
