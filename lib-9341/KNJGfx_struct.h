#pragma once
namespace ardPort {

	class {
	   public:
		// 明るい色
		const uint16_t LIGHT_BLACK = 0x0841;    // 明るい黒
		const uint16_t LIGHT_BLUE = 0x03EF;     // ライトブルー
		const uint16_t LIGHT_RED = 0xF81F;      // 明るい赤
		const uint16_t LIGHT_GREEN = 0x07E8;    // ライトグリーン
		const uint16_t LIGHT_CYAN = 0x87FF;     // 明るいシアン
		const uint16_t LIGHT_MAGENTA = 0xFC1F;  // 明るいマゼンタ
		const uint16_t LIGHT_YELLOW = 0xFFE7;   // 明るい黄色
		const uint16_t LIGHT_WHITE = 0xFFFF;    // 明るい白
		const uint16_t LIGHT_GRAY = 0xC618;     // 明るい灰色
		const uint16_t LIGHT_ORANGE = 0xFD20;   // 明るいオレンジ
		const uint16_t LIGHT_PINK = 0xFE6F;     // 明るいピンク

		
		const uint16_t BLACK = 0x0000;
		const uint16_t BLUE = 0x001F;
		const uint16_t RED = 0xF800;
		const uint16_t GREEN = 0x07E0;
		const uint16_t CYAN = 0x07FF;
		const uint16_t MAGENTA = 0xF81F;
		const uint16_t YELLOW = 0xFFE0;
		const uint16_t WHITE = 0xFFFF;
		const uint16_t GRAY = 0x8410;         // 灰色
		const uint16_t ORANGE = 0xFC00;       // オレンジ
		const uint16_t PINK = 0xF81F;         // ピンク

		// 暗い色
		const uint16_t DARK_BLUE = 0x0010;         // ダークブルー
		const uint16_t DARK_GREEN = 0x03E0;        // ダークグリーン
		const uint16_t DARK_RED = 0x7800;          // ダークレッド
		const uint16_t DARK_CYAN = 0x025F;         // 暗いシアン
		const uint16_t DARK_MAGENTA = 0x580F;      // 暗いマゼンタ
		const uint16_t DARK_YELLOW = 0xBFE0;       // 暗い黄色
		const uint16_t DARK_GRAY = 0x4210;         // 暗い灰色
		const uint16_t DARK_ORANGE = 0x7C00;       // 暗いオレンジ
		const uint16_t DARK_PINK = 0x780F;         // 暗いピンク

		const uint16_t SUPERDARK_BLUE = 0x0008;     // 超暗い青
		const uint16_t SUPERDARK_GREEN = 0x01E0;    // 超暗い緑
		const uint16_t SUPERDARK_RED = 0x3800;      // 超暗い赤
		const uint16_t SUPERDARK_CYAN = 0x015F;     // 超暗いシアン
		const uint16_t SUPERDARK_MAGENTA = 0x280F;  // 超暗いマゼンタ
		const uint16_t SUPERDARK_YELLOW = 0x5FE0;   // 超暗い黄色
		const uint16_t SUPERDARK_GRAY = 0x2108;     // 超暗い灰色
		const uint16_t SUPERDARK_ORANGE = 0x3C00;   // 超暗いオレンジ
		const uint16_t SUPERDARK_PINK = 0x380F;     // 超暗いピンク

	} STDCOLOR;

	class {
	   public:
		const uint8_t NORMAL = 0;      // 接続端子を下
		const uint8_t ROTATE_90 = 1;   // 時計回り９０度
		const uint8_t ROTATE_180 = 2;  // １８０度
		const uint8_t ROTATE_270 = 3;  // ２７０度
	} TFTROTATION;
}  // namespace ardport