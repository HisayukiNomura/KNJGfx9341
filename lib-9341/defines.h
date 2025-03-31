#pragma once

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
#define PICO_CYW43_SUPPORTED 1
#define ARDUINO_ARCH_RP2040
#pragma GCC optimize("O0")