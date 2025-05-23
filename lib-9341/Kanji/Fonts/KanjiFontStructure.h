#pragma once
/*!
  @brief
	漢字フォントデータ1文字分の情報を保持する構造体。

  @details
	KanjiData構造体は、1文字分の漢字またはASCII文字のフォント情報を格納します。
	Unicode値、SJISコード、JISコード、文字の幅・高さ（ピクセル単位）、および
	ビットマップデータ領域内でのオフセット（offsetBMP）を持ちます。

	この構造体は、フォントデータ配列の各要素として利用され、KanjiHelperなどの
	フォント管理クラスから検索・参照されます。描画時には、対応するビットマップデータを
	offsetBMPを使って取得し、文字の描画に利用します。
*/
struct __attribute__((packed)) KanjiData {
	uint32_t Unicode;
	uint16_t SJIS;
	uint16_t JIS;
	uint8_t width;
	uint8_t height;
	uint32_t offsetBMP;
};