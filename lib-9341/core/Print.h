/*
  Print.h - Base class that provides print() and println()
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
*/

#pragma once

#include "../misc/defines.h" 

#include <type_traits>
#include <string>
#include <cstring>
#include <inttypes.h>
#include <stdio.h>  // for size_t

#include "core/WString.h"
#include "core/Printable.h"


#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN  // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
	#undef BIN
#endif
#define BIN 2

#ifdef STD_SDK
namespace ardPort::core {

#endif
	/*!
	  @brief
		文字列や数値などを出力するための基底クラス。

	  @details
		Printクラスは、print()やprintln()などのテキスト出力機能を提供する抽象基底クラスです。
		Arduino環境の多くのクラス（シリアル通信、ディスプレイ、SDカードなど）はこのPrintクラスを継承しており、
		共通の出力インターフェースを実現しています。

		Printクラスは、文字列や数値、浮動小数点数、Printableインターフェースを実装したオブジェクトなどを
		print()/println()で出力できるようにし、さらにprintfやUTF-8（漢字）対応の拡張も含まれています。
		write()メソッドは純粋仮想関数として定義されており、派生クラスで実際の出力先（ディスプレイ、シリアルなど）に
		応じて実装されます。

		Adafruit_GFXクラスはPrintクラスを継承しているため、Adafruit_GFXを継承したディスプレイクラス
		（例：Adafruit_ILI9341やAdafruit_SPITFTなど）でもprint()/println()によるテキスト出力が可能です。
		これは、Printクラスのインターフェースを通じて、ディスプレイ上に直接テキストや数値を描画できるためです。

		この設計により、ユーザーはSerialやディスプレイなど出力先を意識せず、同じAPIでテキスト出力を行うことができます。
	*/
	class Print {
	   protected:
		bool isKanji;  // 漢字フォントを使用するかのフラグ

	   private:
		int write_error;
		size_t printNumber(unsigned long, uint8_t);
		size_t printFloat(double, uint8_t);

	   protected:
		void setWriteError(int err = 1) { write_error = err; }

	   public:
		Print() :
			write_error(0) {
			isKanji = false;
		}
		void KanjiMode(bool a_isKanji);  // 漢字フォントを使用するかを指定する

		int getWriteError() { return write_error; }
		void clearWriteError() { setWriteError(0); }

		virtual size_t write(uint8_t) = 0;
		virtual size_t write(uint32_t) = 0;

		/// @brief 文字列を表示する
		/// @param str 表示する文字列。ゼロ終了。
		/// @return 文字数。
		size_t write(const char *str) {
			if (str == NULL) return 0;
			return write((const uint8_t *)str, strlen(str));
		}
		virtual size_t write(const uint8_t *buffer, size_t size);
		size_t write(const char *buffer, size_t size) {
			return write((const uint8_t *)buffer, size);
		}

		// default to zero, meaning "a single write may block"
		// should be overridden by subclasses with buffering
		virtual int availableForWrite() { return 0; }

		bool isKanjiMode() { return isKanji; }
		void setKanjiMode(bool tf) {isKanji = tf;}

#ifdef STD_SDK
#else
	size_t print(const __FlashStringHelper *);
#endif
		/*
		size_t print(const String &);
		size_t print(const char[]);
		size_t print(char);
		size_t print(unsigned char, int = DEC);
		size_t print(int, int = DEC);
		size_t print(unsigned int, int = DEC);
		size_t print(long, int = DEC);
		size_t print(unsigned long, int = DEC);
		size_t print(double, int = 2);
		*/
		size_t print(const Printable &);
		// std::is_same<T, std::string>::value
		template <typename T>
		constexpr static int DefaultOption() {
			if constexpr (std::is_floating_point<T>::value) {
				return 2;  // 浮動小数点型のデフォルト桁数
			} else if constexpr (std::is_same<T, long>::value) {
				return 10;  // 整数型のデフォルト進数
			} else if constexpr (std::is_integral<T>::value) {
				return 10;  // 整数型のデフォルト進数
			} else {
				return 0;  // その他の型は0
			}
		}

		// template <typename T>	size_t print(T value, int option = DefaultOption<T>());
		template <typename T>
		size_t print(T value, int option = DefaultOption<T>()) {
			if constexpr (std::is_same<T, std::string>::value) {
				return write(value.c_str(), value.size());
			} else if constexpr (std::is_same<T, String>::value) {
				return write(value.c_str(), value.length());
			} else if constexpr (std::is_same<T, const char *>::value) {
				return write(value);
			} else if constexpr (std::is_same<T, char *>::value) {
				return write(value);
			} else if constexpr (std::is_same<T, char>::value) {
				int t = isKanji ? write(static_cast<uint32_t>(value)) : write(static_cast<uint8_t>(value));
				return t;
			} else if constexpr (std::is_same<T, long>::value) {
				if (option == 0) {
					return write((uint8_t)value);
				} else if (option == 10) {
					if (value < 0) {
						int t = isKanji ? write(static_cast<uint32_t>('-')) : write(static_cast<uint8_t>('-'));
						value = -value;
						return printNumber(value, 10) + t;
					}
					return printNumber(value, 10);
				} else {
					return printNumber(value, option);
				}
			} else if constexpr (std::is_integral<T>::value) {
				if (option == 0) {
					return write((uint8_t)value);
				} else if (option == 10) {
					if (value < 0) {
						int t = isKanji ? write(static_cast<uint32_t>('-')) : write(static_cast<uint8_t>('-'));
						value = -value;
						return printNumber(value, 10) + t;
					}
					return printNumber(value, 10);
				} else {
					return printNumber(value, option);
				}
			} else if constexpr (std::is_floating_point<T>::value) {
				return printFloat(static_cast<double>(value), option);
			} else
#if defined(STD_SDK)
#else
			else if constexpr (std::is_same<T, __FlashStringHelper *>::value) {
				return write((const uint8_t *)value, strlen_P((const char *)value));
			}
#endif
			{
				write("TYPERRR!");
				// static_assert(std::is_same<T, void>::value, "Unsupported type for Print::print.");
				return 0;
			}
		}

#ifdef STD_SDK
#else
	size_t
	println(const __FlashStringHelper *);
#endif

		size_t println(const Printable &);
		size_t println(void) { return write("\r\n"); }

		template <typename T>
		size_t println(T value, int option = DefaultOption<T>()) {
			if constexpr (std::is_same<T, std::string>::value) {
				int n = write(value.c_str(), value.size());
				n += println();
				return n;
			} else if constexpr (std::is_same<T, String>::value) {
				int n = write(value.c_str(), value.length());
				n += println();
				return n;
			} else if constexpr (std::is_same<T, const char *>::value) {
				int n = write(value);
				n += println();
				return n;
			} else if constexpr (std::is_same<T, char *>::value) {
				int n = write(value);
				n += println();
				return n;
			} else if constexpr (std::is_integral<T>::value) {
				size_t n = print(value, option);
				n += println();
				return n;
			} else if constexpr (std::is_floating_point<T>::value) {
				size_t n = print(value, option);
				n += println();
				return n;
			}
#if defined(STD_SDK)
#else
		else if constexpr (std::is_same<T, __FlashStringHelper *>::value) {
			size_t n = print(value);
			n += println();
			return n;
		}
#endif
			else {
				write("TYPERRR!");
				// static_assert(std::is_same<T, void>::value, "Unsupported type for Print::print.");
				return 0;
			}
		}

		size_t printf(const char *format, ...);
		virtual void flush() { /* Empty implementation for backward compatibility */ }
	};

#ifdef STD_SDK
}
#endif
