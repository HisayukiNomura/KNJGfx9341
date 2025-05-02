/*
 Print.cpp - Base class that provides print() and println()
 Copyright (c) 2008 David A. Mellis.  All right reserved.

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

 Modified 23 November 2006 by David A. Mellis
 Modified 03 August 2015 by Chuck Todd
 */
#include "misc/defines.h"
#ifdef STD_SDK
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <string>
	#include <math.h>
	#include "misc/PortingCommon.h"

	#include "core/Print.h"
using namespace ardPort::core;
#else
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <math.h>
	#include "Arduino.h"

	#include "Print.h"

#endif

// Public Methods //////////////////////////////////////////////////////////////

/**
 * @brief 現在のフォント設定を変更せずに、漢字フォントを使用するか英文フォントを使用するかを切り替える。
 * @param a_isEnable true:漢字フォントを使用する。false:通常フォントを使用する。
 * @details 漢字モードに切り替える前に、Adafruit_GFX::setFont()でフォントを設定しておく必要がある。
 */
void Print::KanjiMode(bool a_isEnable) {
	isKanji = a_isEnable;
}

/* default implementation: may be overridden */
/// @brief  文字列描画の仮想関数。必要に応じてオーバーロードする。
/// @param buffer 描画する文字列
/// @param size 描画する文字列のサイズ
/// @return 描画した文字数
/// @details 日本語対応のために拡張。
size_t Print::write(const uint8_t *buffer, size_t size) {
	// 漢字モードなら、文字列をUTF-8として処理する
	if (isKanji) {
		int charCount = 0;
		int i = 0;
		while (i < (int)size) {
			uint32_t utf8codes = 0;
			uint8_t step = 0;
			if ((buffer[i] & 0x80) == 0x00) {
				step = 1;  // 1バイト文字 (ASCII)
				utf8codes = buffer[i];
			} else if ((buffer[i] & 0xE0) == 0xC0) {
				step = 2;  // 2バイト文字
				utf8codes = (buffer[i] << 8) | buffer[i + 1];
			} else if ((buffer[i] & 0xF0) == 0xE0) {
				step = 3;  // 3バイト文字
				utf8codes = (buffer[i] << 16) | (buffer[i + 1] << 8) | buffer[i + 2];
			} else if ((buffer[i] & 0xF8) == 0xF0) {
				utf8codes = (buffer[i] << 24) | (buffer[i + 1] << 16) | (buffer[i + 2] << 8) | buffer[i + 3];
				step = 4;  // 4バイト文字
			} else {
				return 0;  // 不正なUTF-8バイト
			}

#ifdef TFT_FORCE_HANKANA  // 半角カナを1バイト文字として処理
			uint16_t top2bytes = (uint16_t)(utf8codes >> 8);
			if (top2bytes == 0xefbd) {
				utf8codes = utf8codes & 0x000000FF;
			} else if (top2bytes == 0xefbe) {
				utf8codes = (utf8codes & 0x000000FF) + 0x40;
			}
#endif
			charCount++;
			write(utf8codes);  // 文字列描画
			i += step;
		}
		return charCount;
	} else {  // 英字モードはそのまま動作させる
		size_t n = 0;
		while (size--) {
			if (write(*buffer++))
				n++;
			else
				break;
		}

		return n;
	}
}
#ifdef STD_SDK
// Raspberry pi PICOでは不要
#else
size_t Print::print(const __FlashStringHelper *ifsh) {
	PGM_P p = reinterpret_cast<PGM_P>(ifsh);
	size_t n = 0;
	while (1) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) break;
		if (write(c))
			n++;
		else
			break;
	}
	return n;
}
#endif

size_t Print::println(const Printable &x) {
	size_t n = print(x);
	n += println();
	return n;
}

// Private Methods /////////////////////////////////////////////////////////////

size_t Print::printNumber(unsigned long n, uint8_t base) {
	char buf[8 * sizeof(long) + 1];  // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];

	*str = '\0';

	// prevent crash if called with base == 1
	if (base < 2) base = 10;

	do {
		char c = n % base;
		n /= base;

		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while (n);

	return write(str);
}

size_t Print::printFloat(double number, uint8_t digits) {
	size_t n = 0;

	if (isnan(number)) return print("nan");
	if (isinf(number)) return print("inf");
	if (number > 4294967040.0) return print("ovf");   // constant determined empirically
	if (number < -4294967040.0) return print("ovf");  // constant determined empirically

	// Handle negative numbers
	if (number < 0.0) {
		n += print('-');
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i = 0; i < digits; ++i)
		rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	n += print(int_part);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) {
		n += print('.');
	}

	// Extract digits from the remainder one at a time
	while (digits-- > 0) {
		remainder *= 10.0;
		unsigned int toPrint = (unsigned int)(remainder);
		n += print(toPrint);
		remainder -= toPrint;
	}

	return n;
}
/**
 * @brief printfを追加実装する
 * @param format フォーマット文字列
 * @param ... 可変引数
 * @return 書き込まれた文字数
 * @details 多くの関数（浮動小数点を表示するとか）を自力で実装しているが、なぜかprintfが無い。
 * 現状、ArduinoもRaspberry PI SDKもsprintfは実装されているので、これを使って簡単に実装しておく。
 */

size_t Print::printf(const char *format, ...) {
	char buffer[1024];  // 出力用の一時バッファ
	va_list args;       // 可変引数を処理するリスト

	// 可変引数リストの初期化
	va_start(args, format);

	// snprintfでフォーマット文字列と可変引数を処理
	int written = vsnprintf(buffer, sizeof(buffer), format, args);

	// 可変引数リストの終了
	va_end(args);

	print(buffer);

	return (size_t)(written >= 0 ? written : 0);
}
