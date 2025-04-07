#pragma once
#pragma GCC optimize("O0")
#include <errno.h>
#include <iconv.h>
#include <stdlib.h>
#include <string.h>

/// @brief 使用する漢字データの大きさを指定する。<br/>
/// - 16...全角 16x16 半角8x16<br/>
/// - 12...全角 12x12 半角6x12<br/>
/// - 8....全角  8x 8 半角4x8 (かなり視認性は低い）<br/>
#define TFT_KANJI_DOT 12

/// @brief 使用する漢字データの種類を指定する。<br/>
/// - 0...JISに定義されているすべてのコード（約6800文字、289KBytes/16dot) <br/>
/// - 1...JIS第一水準+かな+カナ+記号1,2+ギリシャ+キリル+罫線（約3400文字、83KBytes/16dot) <br/>
/// - 2...常用漢字+かな+カナ+記号1,2（約2500文字、79KBytes/16dot) <br/>
/// - 3...教育漢字+かな+カナ+記号1,2（約840文字、 25KBytes/16dot) <br/>
#define TFT_KANJI_LEVEL 0

#if TFT_KANJI_DOT == 16
	#if TFT_KANJI_LEVEL == 0
		#include "Fonts/Font_Kanji16All.inc"
	#elif TFT_KANJI_LEVEL == 1
		#include "Fonts/Font_Kanji16Level1.inc"
	#elif TFT_KANJI_LEVEL == 2
		#include "Fonts/Font_Kanji16Jyoyo.inc"
	#elif TFT_KANJI_LEVEL == 3
		#include "Fonts/Font_Kanji16Kyoiku.inc"
	#endif
#elif TFT_KANJI_DOT == 12
	#if TFT_KANJI_LEVEL == 0
		#include "Fonts/Font_Kanji12All.inc"
	#elif TFT_KANJI_LEVEL == 1
		#include "Fonts/Font_Kanji12Level1.inc"
	#elif TFT_KANJI_LEVEL == 2
		#include "Fonts/Font_Kanji12Jyoyo.inc"
	#elif TFT_KANJI_LEVEL == 3
		#include "Fonts/Font_Kanji12Kyoiku.inc"
	#endif
#elif TFT_KANJI_DOT == 8
	#if TFT_KANJI_LEVEL == 0
		#include "Fonts/Font_Kanji8All.inc"
	#elif TFT_KANJI_LEVEL == 1
		#include "Fonts/Font_Kanji8Level1.inc"
	#elif TFT_KANJI_LEVEL == 2
		#include "Fonts/Font_Kanji8Jyoyo.inc"
	#elif TFT_KANJI_LEVEL == 3
		#include "Fonts/Font_Kanji8Kyoiku.inc"
	#endif
#endif


/// @brief 漢字フォントのデータを管理するためのクラス。staticなメソッドしか持たない
class KanjiHelper {
	private:
	KanjiHelper() {};
	 ~KanjiHelper() {};

	private:
	 static int compareUnicode(const void* a, const void* b);
	 static const KanjiFont* binarySearchUnicode(const KanjiFont array[], size_t size, uint32_t unicode);
	 static const AsciiFont* SearchAscii(const AsciiFont array[],uint8_t asciicode);

	public:
	 static const KanjiFont* FindKanji(uint32_t codeUTF);
	 static const AsciiFont* FindAscii(uint8_t codeAscii);
};