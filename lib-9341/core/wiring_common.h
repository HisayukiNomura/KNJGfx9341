
// General GPIO/ADC layout info
#ifdef PICO_RP2350B
	#define __GPIOCNT 48
	#define __FIRSTANALOGGPIO 40
#else
	#define __GPIOCNT 30
	#define __FIRSTANALOGGPIO 26
#endif