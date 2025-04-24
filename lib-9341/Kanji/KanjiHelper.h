#pragma once
#pragma GCC optimize("O0")
#include <errno.h>
#include <iconv.h>
#include <stdlib.h>
#include <string.h>

#include "Fonts/KanjiFontStructure.h"


/// @brief 漢字フォントのデータを管理するためのクラス。staticなメソッドしか持たない
class KanjiHelper {
	private:
	// 漢字フォントデータのポインタ
	const static KanjiData* pKanjiData;
	const static uint8_t* pBmpData;
	
	static int DataSize;

	static uint8_t AsciiWidth;
	static uint8_t AsciiHeight;
	static uint8_t KanjiWidth;
	static uint8_t KanjiHeight;

   private:
	KanjiHelper() {};
	 ~KanjiHelper() {};

	private:
	 static int compareUnicode(const void* a, const void* b);

	public:
	 static void SetKanjiFont(const KanjiData *pKanjiData, const uint8_t* pBmpData);
	 static const KanjiData* FindKanji(uint32_t unicode);
	 static const KanjiData* FindAscii(uint8_t asciicode);
	 static const uint8_t getKanjiWidth() { return KanjiWidth; };
	 static const uint8_t getKanjiHeight() { return KanjiHeight; };
	 static const uint8_t getAsciiWidth() { return AsciiWidth; };
	 static const uint8_t getAsciiHeight() { return AsciiHeight; };
	 static const uint8_t* getBmpData(const KanjiData *pFont);

	 
};

