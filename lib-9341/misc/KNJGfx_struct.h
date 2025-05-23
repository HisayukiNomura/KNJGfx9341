#pragma once
#include <stdlib.h>
#include <cstdint>
namespace ardPort {

	/**
	 * @brief 標準カラー定数をまとめたクラス
	 * @details
	 * 16ビットRGB565形式で、明るい色・標準色・暗い色・超暗い色などのカラー定数を提供します。
	 * グラフィックス描画時の色指定に利用できます。
	 * 例: tft.fillScreen(STDCOLOR.BLACK); のようにSTDCOLOR経由で色定数を指定します。
	 */
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
		const uint16_t GRAY = 0x8410;    // 灰色
		const uint16_t ORANGE = 0xFC00;  // オレンジ
		const uint16_t PINK = 0xF81F;    // ピンク

		// 暗い色
		const uint16_t DARK_BLUE = 0x0010;     // ダークブルー
		const uint16_t DARK_GREEN = 0x03E0;    // ダークグリーン
		const uint16_t DARK_RED = 0x7800;      // ダークレッド
		const uint16_t DARK_CYAN = 0x025F;     // 暗いシアン
		const uint16_t DARK_MAGENTA = 0x580F;  // 暗いマゼンタ
		const uint16_t DARK_YELLOW = 0xBFE0;   // 暗い黄色
		const uint16_t DARK_GRAY = 0x4210;     // 暗い灰色
		const uint16_t DARK_ORANGE = 0x7C00;   // 暗いオレンジ
		const uint16_t DARK_PINK = 0x780F;     // 暗いピンク

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

	/**
	 * @brief TFTディスプレイの回転定数クラス
	 * @details
	 * TFTディスプレイの回転方向を指定するための定数をまとめています。
	 * tft.setRotation(TFTROTATION.ROTATE_90); のように使用します。
	 */
	class {
	   public:
		const uint8_t NORMAL = 0;      // 接続端子を下
		const uint8_t ROTATE_90 = 1;   // 時計回り９０度
		const uint8_t ROTATE_180 = 2;  // １８０度
		const uint8_t ROTATE_270 = 3;  // ２７０度
	} TFTROTATION;

	class XYWH {
	   public:
		int16_t x;
		int16_t y;
		int16_t w;
		int16_t h;

	   public:
		XYWH(int16_t x, int16_t y, int16_t w, int16_t h) :
			x(x),
			y(y),
			w(w),
			h(h) {}
		XYWH() :
			x(0),
			y(0),
			w(0),
			h(0) {}

		void setxyxy(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
			// X方向の正規化
			if (x2 < x1) {
				int16_t tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			// Y方向の正規化
			if (y2 < y1) {
				int16_t tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			// 正規化後の座標と幅／高さの計算
			x = x1;
			y = y1;
			w = x2 - x1 + 1;
			h = y2 - y1 + 1;
		}
		void setxywh(int16_t x, int16_t y, int16_t w, int16_t h) {
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}
		void setRandom(int16_t xMax, int16_t yMax) {
			setxyxy(rand() % xMax, rand() % yMax, rand() % xMax, rand() % yMax);
		}
	};
	/**
	 * @brief 2次元座標（x, y）を表す構造体
	 * @details
	 * 点や位置を管理します。Adafruit_GFX系のdrawPixelやdrawLineなどのメソッドでxy型引数として利用できます。
	 * 例: tft.drawPixel(xy, color);
	 */
	class XY {
	   public:
		int16_t x;
		int16_t y;

	   public:
		XY(int16_t a_x, int16_t a_y) :
			x(a_x),
			y(a_y) {}
		XY() {
			x = 0;
			y = 0;
		}
		void setxy(int16_t a_x, int16_t a_y) {
			x = a_x;
			y = a_y;
		}
		void setRandom(int16_t xMax, int16_t yMax) {
			x = rand() % xMax;
			y = rand() % yMax;
		}
	};
}  // namespace ardPort
