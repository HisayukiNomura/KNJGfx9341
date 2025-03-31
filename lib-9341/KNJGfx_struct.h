#pragma once

class {
   public:
	const uint16_t BLACK = 0x0000;
	const uint16_t BLUE = 0x001F;
	const uint16_t RED = 0xF800;
	const uint16_t GREEN = 0x07E0;
	const uint16_t CYAN = 0x07FF;
	const uint16_t MAGENTA = 0xF81F;
	const uint16_t YELLOW = 0xFFE0;
	const uint16_t WHITE = 0xFFFF;
	const uint16_t GRAY = 0x8410;         // 灰色
	const uint16_t LIGHT_BLUE = 0x03EF;   // ライトブルー
	const uint16_t DARK_GREEN = 0x03E0;   // ダークグリーン
	const uint16_t DARK_BLUE = 0x0010;    // ダークブルー
	const uint16_t LIGHT_GREEN = 0x07E8;  // ライトグリーン
	const uint16_t DARK_RED = 0x7800;     // ダークレッド
	const uint16_t ORANGE = 0xFC00;       // オレンジ
	const uint16_t PINK = 0xF81F;         // ピンク
} STDCOLOR;

class {
   public:
	const uint8_t NORMAL = 0;      // 接続端子を下
	const uint8_t ROTATE_90 = 1;   // 時計回り９０度
	const uint8_t ROTATE_180 = 2;  // １８０度
	const uint8_t ROTATE_270 = 3;  // ２７０度
} TFTROTATION;