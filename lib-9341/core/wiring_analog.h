#pragma once
extern "C" {
	// ADC RP2040-specific calls
	void analogReadResolution(int bits);
	float analogReadTemp(float vref = 3.3);  // Returns core temp in Centigrade
	// PWM RP2040-specific calls
	void analogWriteFreq(uint32_t freq);
	void analogWriteRange(uint32_t range);
	void analogWriteResolution(int res);
}