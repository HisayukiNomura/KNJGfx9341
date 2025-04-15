#pragma once
struct KanjiData {
	uint32_t Unicode;
	uint16_t SJIS;
	uint16_t JIS;
	uint8_t width;
	uint8_t height;
	uint32_t offsetBMP;
};