/*
	Core debug macros header for the Raspberry Pi Pico RP2040

	Copyright (c) 2021 Earle F. Philhower, III <earlephilhower@yahoo.com>

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
/**
 * @file debug_internal.h
 * @brief デバッグ用のマクロ。Raspberry PI標準SDKで使うことに限定して不要なものは消した
 */

#pragma once
#include "../misc/defines.h"

#ifndef ARDUINO
namespace ardPort {
	// clang-format off
	#ifdef ENABLE_DEBUG_PRINTF
		#define DEBUGV(fmt, ...) {printf("%s:%d - ", __FILE__, __LINE__);printf(fmt, ##__VA_ARGS__);}

		#ifdef ENABLE_DEBUG_SPI
			#define DEBUGSPI(fmt, ...) {printf("%s:%d - ", __FILE__, __LINE__);printf(fmt, ##__VA_ARGS__);}
		#else
			#define DEBUGSPI(fmt, ...) {};
		#endif

		#ifdef ENABLE_DEBUG_CORE
			#define DEBUGCORE(fmt, ...) {printf("%s:%d - ", __FILE__, __LINE__);printf(fmt, ##__VA_ARGS__);}
		#else
			#define DEBUGCORE(fmt, ...) {};
		#endif

		#ifdef ENABLE_DEBUG_WIRE
			#define DEBUGWIRE(fmt, ...) {printf("%s:%d - ", __FILE__, __LINE__);printf(fmt, ##__VA_ARGS__);}
		#else
			#define DEBUGWIRE(fmt, ...) {};
		#endif
	#else
		#ifndef DEBUGV
			#define DEBUGV(fmt, ...) {};
		#endif
		#ifndef DEBUGSPI
			#define DEBUGSPI(fmt, ...) {};
		#endif
		#ifndef DEBUGCORE
			#define DEBUGCORE(fmt, ...) {};
		#endif
		#ifndef DEBUGWIRE
			#define DEBUGWIRE(fmt, ...) {};
		#endif
	#endif
	}
	#ifdef __cplusplus
		extern void
				hexdump(const void* mem, uint32_t len, uint8_t cols = 16);
	#endif

#else // ARDUINO
#if !defined(DEBUG_RP2040_PORT)
#define DEBUGV(...) do { } while(0)
#define DEBUGCORE(...) do { } while(0)
#define DEBUGWIRE(...) do { } while(0)
#define DEBUGSPI(...) do { } while(0)
#else
#define DEBUGV(fmt, ...) do { DEBUG_RP2040_PORT.printf(fmt, ## __VA_ARGS__); DEBUG_RP2040_PORT.flush(); } while (0)

#if defined(DEBUG_RP2040_CORE)
#define DEBUGCORE(fmt, ...) do { DEBUG_RP2040_PORT.printf(fmt, ## __VA_ARGS__); DEBUG_RP2040_PORT.flush(); } while (0)
#else
#define DEBUGCORE(...) do { } while(0)
#endif

#if defined(DEBUG_RP2040_WIRE)
#define DEBUGWIRE(fmt, ...) do { DEBUG_RP2040_PORT.printf(fmt, ## __VA_ARGS__); DEBUG_RP2040_PORT.flush(); } while (0)
#else
#define DEBUGWIRE(...) do { } while(0)
#endif

#if defined(DEBUG_RP2040_SPI)
#define DEBUGSPI(fmt, ...) do { DEBUG_RP2040_PORT.printf(fmt, ## __VA_ARGS__); DEBUG_RP2040_PORT.flush(); } while (0)
#else
#define DEBUGSPI(...) do { } while(0)
#endif
#endif

#endif // ARDUINO

