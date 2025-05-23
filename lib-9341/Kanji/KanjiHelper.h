#pragma once
#pragma GCC optimize("O0")
#include <errno.h>
#include <iconv.h>
#include <stdlib.h>
#include <string.h>

#include "Fonts/KanjiFontStructure.h"

/*!
  @brief
	漢字フォントデータの管理・検索・取得を行う静的ユーティリティクラス。

  @details
	KanjiHelperクラスは、漢字やASCII文字のフォントデータを管理し、描画時に必要な情報を提供するための
	静的ユーティリティクラスです。インスタンス化はできず、すべてのメソッド・メンバはstaticとして
	実装されています。

	主な機能は、外部からセットされた漢字フォントデータ（KanjiData配列とビットマップデータ）への
	アクセス、UnicodeやASCIIコードによるフォントデータの検索、文字幅・高さ情報の取得、
	指定フォントデータに対応するビットマップデータの取得などです。

	このクラスは主に、グラフィックライブラリ（例：Adafruit_GFXを拡張した日本語描画対応クラスや
	テキスト描画処理）から呼び出され、文字列描画時に各文字のビットマップデータやサイズ情報を
	効率よく取得するために利用されます。たとえば、日本語テキスト描画をサポートするディスプレイクラスや
	カスタムフォント描画エンジンなどが本クラスのAPIを利用します。

	フォントデータの検索はバイナリサーチで高速に行われ、描画時のパフォーマンス向上に寄与します。
	また、フォントデータのセットアップや取得インターフェースを統一することで、
	アプリケーションや描画エンジン側の実装を簡素化できます。

	【メンバー変数の解説】
	  - pKanjiData: 漢字フォントデータ（KanjiData構造体配列）へのポインタ
	  - pBmpData:   漢字ビットマップデータへのポインタ
	  - DataSize:   フォントデータのエントリ数
	  - AsciiWidth, AsciiHeight:  ASCII文字の幅・高さ（ピクセル単位）
	  - KanjiWidth, KanjiHeight:  漢字文字の幅・高さ（ピクセル単位）

	これらの情報をもとに、FindKanji/FindAsciiでフォント情報を検索し、getBmpDataでビットマップを取得できます。
*/
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

