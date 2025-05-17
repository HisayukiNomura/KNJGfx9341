/// @brief このファイルは、MicropythonのC拡張モジュールとして使用する場合に有効にする。
/// ビルド時は、User C Moduleのcmakeファイルが存在する場所（ports/rp2/myModule等）に移動してビルドする。

#include <stdlib.h>

#include <stdio.h>
#include <cstdint>
#include <stdarg.h>
#include "../Adafruit_ILI9341/Adafruit_ILI9341.h"
#pragma GCC diagnostic ignored "-Wunused-variable"
#define DEBUG_PRINT
// デフォルトフォント。
// 予定？では、python側でフォントをsetKanjiFont指定できるようにする予定にした。そのためデフォルトフォントは一番サイズが小さいものにしておくことにした。
// 実際に実装しテストしてみると、様々な問題が発生した。
// 1. フォントデータをバイナリにすると、thonyでラズピコに保存できない。（保存できるのは *.pyのみ）
// 2. フォントデータをプログラムにすると、プログラムサイズが大きくなるためかthonyで保存してもハングする。（もしかしたら１～２日で完了するのかもしれないが）
// 3. プログラムを、thony からでなく、rshell で直接ラズピコに保存し、実行すると漢字データを読み込むにはメモリが足りない。
// 事実上、３が一番の問題で、pythonのユーザーCモジュール内や、SDKで作ったC++のプログラムでは漢字データをフラッシュメモリ（Picoで2Mbytes)に置けるので問題ないが、
// pythonプログラムで漢字データを保持するとなると、ヒープに保存するしかなく、256MBytesしかないメモリでは足りなすぎる。
//
// バイナリデータとしてフラッシュ上にrshellから保存し、ユーザーCモジュール側でそれを少しずつ取り出して使う、というのも考えたが、デプロイの手順が複雑になるし
// データの取り出しのオーバーヘッドなども考えると気が遠くなる。
//
// ということで、デフォルトフォントに頼らざるを得ない。

#include "../Kanji/Fonts/KanjiFontStructure.h"

// #include "../Kanji/Fonts/JF-Dot-Shinonome12_12x12_ALL.inc"
// #include "../Kanji/Fonts/JF-Dot-Shinonome14_14x14_LEVEL1.inc"
// #include "../Kanji/Fonts/JF-Dot-Shinonome16_16x16_ALL.inc"
#include "../Kanji/Fonts/JF-Dot-Shinonome16_16x16_LEVEL1.inc"
// #include "../Kanji/Fonts/ipaexg_24x24_ALL.inc"

#include "../Adafruit_GFX_Library/Fonts/FreeSerif12pt7b.h"
extern "C" {
#include "GFXModule.h"
#include <py/mphal.h>
#include <py/objstr.h>

	static int iDebugLevel = 1;

	using namespace ardPort;

	// static HW HWObj;
	// static ST7735 ST7735Obj;

	//  エラーメッセージ用のバッファ。mp_raise_XXXXでは、スタック変数（ヒープは未確認だが以前mallocしただけでハングする等不穏な動きをした）は使えない。
	//  そのため文字列リテラル以外を使う時にはこの静的変数を使う
	static char errTxt[128];

	// フォントを保存しておく構造体。
#ifdef TFT_ENABLE_FONTS
	static struct
	{
		const char* name;
		const GFXfont* font;
	} fontList[16];
#endif

	void raise_mustTupple()
	{
		mp_raise_TypeError(MP_ERROR_TEXT("Expected a tuple"));
	}
	void raise_mustInt()
	{
		mp_raise_TypeError(MP_ERROR_TEXT("Expected a int"));
	}
	void raise_mustArray()
	{
		mp_raise_TypeError(MP_ERROR_TEXT("Expected an array"));
	}
	void raise_mustBool()
	{
		mp_raise_TypeError(MP_ERROR_TEXT("Expected a bool"));
	}
	void raise_mustStr()
	{
		mp_raise_TypeError(MP_ERROR_TEXT("Expected a str"));
	}
	void raise_mustXYTupple()
	{
		mp_raise_ValueError(MP_ERROR_TEXT("Expected 2 elements in the tuple containing x,y"));
	}
	void raise_mustXYWHTupple()
	{
		mp_raise_ValueError(MP_ERROR_TEXT("Expected 4 elements in the tuple containing x,y,w,h"));
	}
	/// @brief 文字列を表示する関数。デバッグ用
	void msg_OnDebug(const char* format, ...)
	{
		if (iDebugLevel == 0) return;
		va_list args;
		va_start(args, format);
		unsigned int reqLen = vsnprintf(errTxt, sizeof(errTxt) - 6, format, args);
		va_end(args);
		if (reqLen > (sizeof(errTxt) - 6)) {
			strcat(errTxt, "...\r\n");
		}

		mp_hal_stdout_tx_str(errTxt);
	}

	/// @brief 与えられた変数のタイプを文字列で返す関数。デバッグ用
	mp_obj_t getTypeName(mp_obj_t obj)
	{
		const mp_obj_type_t* type = mp_obj_get_type(obj);
		const char* type_name = qstr_str(type->name);
		return mp_obj_new_str(type_name, strlen(type_name));
	}

#pragma region 初期化・設定関数
	Adafruit_ILI9341* pTFT;
	Adafruit_ILI9341 tft;
	/// @brief ハードウェアの初期化処理

	mp_obj_t initHW(mp_obj_t args)
	{
		// static Adafruit_ILI9341 tft;
		// pTFT = NULL;
		pTFT = &tft;

		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(args, &mp_type_tuple)) raise_mustTupple();
		// タプルの要素を取得
		mp_obj_tuple_get(args, &len, &items);
		// 要素の数を確認
		if (len != 7) {
			mp_raise_ValueError(MP_ERROR_TEXT("Expected 7 elements in the tuple"));
		}

		// #define TFT_SCK 18   // 液晶表示の SCK
		// #define TFT_MOSI 19 // 液晶表示の MOSI
		// #define TFT_DC 20   // 液晶画面の DC
		// #define TFT_RST 21  // 液晶画面の RST
		// #define TFT_CS 22   // 液晶画面の CS
		// タプル内の要素をintに変換
		int iSPIPortNo = mp_obj_get_int(items[0]);
		int iDC = mp_obj_get_int(items[1]);    // 20
		int iCS = mp_obj_get_int(items[2]);    // 22
		int iSCK = mp_obj_get_int(items[3]);   // 18
		int iMOSI = mp_obj_get_int(items[4]);  // 19
		int iReset = mp_obj_get_int(items[5]); // 21
		int iDebug = mp_obj_get_int(items[6]); // do not use

		// ハードウェア初期化
		spi::SPIClassRP2040* pSPI = iSPIPortNo == 0 ? &SPI : &SPI1;
		// spi::SPIClassRP2040* pSPI = NULL;

		/*
		このSPIとかが、静的に作られているの問題？　	m_new_obj(simpleclass_myclass_obj_t);　とかで動的確保できるか？ SPIに限らず、TFTもこの方法で作れれば良いのだが。
		あるいは、static mp_obj_t PicoMath_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
			PicoMath_obj_t *self = mp_obj_malloc(PicoMath_obj_t,type);

			return MP_OBJ_FROM_PTR(self);
		}
		か？
		"https: // harlepengren.com/a-journey-to-creating-a-micropython-user-modules/"
		*/

		pTFT->constructObject(pSPI, iDC, iCS, iReset);
		pSPI->setMOSI(iMOSI);
		pSPI->setSCK(iSCK);
		pTFT->begin(); // SPIの初期化を行う。引数はSPIの周波数。0はデフォルト値

		msg_OnDebug("initHW:PortNO=%d , DC=%d , CS=%d , SCK=%d , MOSI=%d , RESET=%d , DEBUG=%d\r\n", iSPIPortNo, iDC, iCS, iSCK, iMOSI, iReset, iDebug);

		//  結果を返す
		return mp_obj_new_int(1);
	}
	/// @brief デバッグモードの設定
	/// @param a_level デバッグモードのレベル。0は無効、1は有効
	/// @return 常に１
	mp_obj_t setDebugMode(mp_obj_t a_level)
	{
		if (!mp_obj_is_int(a_level)) raise_mustInt();

		iDebugLevel = mp_obj_get_int(a_level);
		msg_OnDebug("setDebugMode: Debug Level = %d\r\n", iDebugLevel);
		return mp_obj_new_int(1);
	}

	/// @brief デフォルトの漢字フォントを読み込む。デフォルトの漢字フォントはビルドされたMicroPythonに組み込まれているフォント。
	/// @return
	mp_obj_t loadDefaultKanjiFont()
	{
		msg_OnDebug("loadDefaultKanjiFont\r\n");
		// pTFT->setFont(JFDotShinonome14_14x14_LEVEL1, JFDotShinonome14_14x14_LEVEL1_bitmap);
		// pTFT->setFont(JFDotShinonome16_16x16_ALL, JFDotShinonome16_16x16_ALL_bitmap);
		// pTFT->setFont(JFDotShinonome12_12x12_ALL, JFDotShinonome12_12x12_ALL_bitmap);
		// pTFT->setFont(ipaexg_24x24_ALL, ipaexg_24x24_ALL_bitmap);
		pTFT->setFont(JFDotShinonome16_16x16_LEVEL1, JFDotShinonome16_16x16_LEVEL1_bitmap);

		return mp_obj_new_int(1);
	}
	/// @brief デフォルトのASCIIフォントを読み込む。
	/// @return
	mp_obj_t loadDefaultAsciiFont()
	{
		msg_OnDebug("loadDefaultAsciiFont\r\n");
		pTFT->setFont(&FreeSerif12pt7b);
		return mp_obj_new_int(1);
	}
	/// @brief 	画面を回転する
	/// @param a_Rotation  0-3の整数値。0は通常、1は時計回りに90度、2は180度、3は270度
	/// @return
	mp_obj_t setRotation(mp_obj_t a_Rotation)
	{
		if (!mp_obj_is_int(a_Rotation)) raise_mustInt();
		int iRotation = mp_obj_get_int(a_Rotation);
		if (iRotation < 0 || iRotation > 3) {
			mp_raise_ValueError(MP_ERROR_TEXT("Value must be 0-3"));
		}
		pTFT->setRotation(iRotation);
		msg_OnDebug("setRotation: %d\r\n", pTFT->getRotation());
		return mp_obj_new_int(1);
	}
	/// @brief 画面のスリープモード
	/// @param a_tf スリープモードに入るときはtrue、出るときはfalse
	/// @return
	mp_obj_t displaySleep(mp_obj_t a_tf)
	{
		if (!mp_obj_is_bool(a_tf)) raise_mustBool();
		bool tf = mp_obj_is_true(a_tf);
		pTFT->displaySleep(tf);
		msg_OnDebug("displaySleep: %d\r\n", tf);
		return mp_obj_new_int(1);
	}
#pragma endregion

#pragma region 特殊描画関数
	/// @brief 画面の反転
	/// @param a_tf 反転するときはtrue、しないときはfalse
	/// @return
	mp_obj_t invertDisplay(mp_obj_t a_tf)
	{
		if (!mp_obj_is_bool(a_tf)) raise_mustBool();
		bool tf = mp_obj_is_true(a_tf);
		pTFT->invertDisplay(tf);
		msg_OnDebug("invertDisplay: %d\r\n", tf);
		return mp_obj_new_int(1);
	}
	/// @brief 	スクロールの範囲を設定する
	/// @param a_top
	/// @param a_bottom
	/// @return
	mp_obj_t setScrollMargins(mp_obj_t a_top, mp_obj_t a_bottom)
	{
		if (!mp_obj_is_int(a_top)) raise_mustInt();
		if (!mp_obj_is_int(a_bottom)) raise_mustInt();
		int top = mp_obj_get_int(a_top);
		int bottom = mp_obj_get_int(a_bottom);
		pTFT->setScrollMargins(top, bottom);
		msg_OnDebug("setScrollMargins: %d , %d\r\n", top, bottom);
		return mp_obj_new_int(1);
	}
	/// @brief スクロール先の位置を指定する
	/// @param a_y
	/// @return
	mp_obj_t scrollTo(mp_obj_t a_y)
	{
		if (!mp_obj_is_int(a_y)) raise_mustInt();
		int y = mp_obj_get_int(a_y);
		pTFT->scrollTo(y);
		msg_OnDebug("scrollTo: %d\r\n", y);
		return mp_obj_new_int(1);
	}

#pragma endregion

#pragma region 基本描画関数
	/// @brief 画面を特定の色で塗りつぶす
	/// @param a_Color 塗りつぶす色
	/// @return 常に１が返される
	mp_obj_t fillScreen(mp_obj_t a_Color)
	{
		int color = mp_obj_get_int(a_Color);
		pTFT->fillScreen(color);
		msg_OnDebug("fillScreen:Color = %04X\r\n", color);
		return mp_obj_new_int(color);
	}
	/// @brief 画面の特定の場所に、点を書く
	/// @param a_xy	画面の座標。タプルで指定する。x,yの順番　
	/// @param a_color	色。16bitの整数値で指定する。RGB565形式
	/// @return
	mp_obj_t drawPixel(mp_obj_t a_xy, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xy, &len, &items);
		// 要素の数を確認
		if (len != 2) raise_mustXYTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int c = mp_obj_get_int(a_color);

		pTFT->drawPixel(x, y, c);
		msg_OnDebug("drawPixel:x=%d , y=%d , color=%04X\r\n", x, y, c);
		return mp_obj_new_int(1);
	}

	/// @brief 高速に（ほんと？）縦線を引く
	/// @param a_xy 開始の座標。タプルで指定する。x,yの順番
	/// @param a_h 	高さ。整数値
	/// @param a_color　色。16bitの整数値で指定する。RGB565形式
	/// @return
	mp_obj_t drawFastVLine(mp_obj_t a_xy, mp_obj_t a_h, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_h)) raise_mustInt();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xy, &len, &items);
		// 要素の数を確認
		if (len != 2) raise_mustXYTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int h = mp_obj_get_int(a_h);
		int c = mp_obj_get_int(a_color);

		pTFT->drawFastVLine(x, y, h, c);
		msg_OnDebug("drawFastVLine:x=%d , y=%d , h=%d , color=%04X\r\n", x, y, h, c);
		return mp_obj_new_int(1);
	}

	/// @brief 高速に（ほんと？）横線を引く
	/// @param a_xy 開始の座標。タプルで指定する。x,yの順番
	/// @param a_w  横幅。整数値
	/// @param a_color  色。16bitの整数値で指定する。RGB565形式
	/// @return
	mp_obj_t drawFastHLine(mp_obj_t a_xy, mp_obj_t a_w, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_w)) raise_mustInt();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xy, &len, &items);
		// 要素の数を確認
		if (len != 2) raise_mustXYTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(a_w);
		int c = mp_obj_get_int(a_color);

		pTFT->drawFastHLine(x, y, w, c);
		msg_OnDebug("drawFastHLine:x=%d , y=%d , w=%d , color=%04X\r\n", x, y, w, c);
		return mp_obj_new_int(1);
	}
	/// @brief 	画面の特定の場所に、塗りつぶした長方形を描く
	/// @param a_xywh 	画面の座標。タプルで指定する。x,y,w,hの順番
	/// @param a_color	色。16bitの整数値で指定する。RGB565形式
	/// @return
	mp_obj_t fillRect(mp_obj_t a_xywh, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xywh, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xywh, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(items[2]);
		int h = mp_obj_get_int(items[3]);
		int c = mp_obj_get_int(a_color);

		pTFT->fillRect(x, y, w, h, c);
		msg_OnDebug("fillRect:x=%d , y=%d , w=%d , h=%d , color=%04X\r\n", x, y, w, h, c);
		return mp_obj_new_int(1);
	}
	/// @brief 	画面の特定の場所に、長方形を描く
	/// @param a_xywh 	画面の座標。タプルで指定する。x,y,w,hの順番
	/// @param a_color 	色。16bitの整数値で指定する。RGB565形式
	/// @return
	mp_obj_t drawRect(mp_obj_t a_xywh, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xywh, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xywh, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(items[2]);
		int h = mp_obj_get_int(items[3]);
		int c = mp_obj_get_int(a_color);

		pTFT->drawRect(x, y, w, h, c);
		msg_OnDebug("drawRect:x=%d , y=%d , w=%d , h=%d , color=%04X\r\n", x, y, w, h, c);
		return mp_obj_new_int(1);
	}

	mp_obj_t drawLine(mp_obj_t a_xyxy, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xyxy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xyxy, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();
		int x1 = mp_obj_get_int(items[0]);
		int y1 = mp_obj_get_int(items[1]);
		int x2 = mp_obj_get_int(items[2]);
		int y2 = mp_obj_get_int(items[3]);
		int c = mp_obj_get_int(a_color);

		pTFT->drawLine(x1, y1, x2, y2, c);
		msg_OnDebug("drawLine:x1=%d , y1=%d , x2=%d , y2=%d , color=%04X\r\n", x1, y1, x2, y2, c);
		return mp_obj_new_int(1);
	}

	mp_obj_t drawCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xy, &len, &items);
		// 要素の数を確認
		if (len != 2) raise_mustXYTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int r = mp_obj_get_int(a_r);
		int c = mp_obj_get_int(a_color);

		pTFT->drawCircle(x, y, r, c);
		msg_OnDebug("drawCircle:x=%d , y=%d , r=%d , color=%04X\r\n", x, y, r, c);
		return mp_obj_new_int(1);
	}
	mp_obj_t fillCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xy, &len, &items);
		// 要素の数を確認
		if (len != 2) raise_mustXYTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int r = mp_obj_get_int(a_r);
		int c = mp_obj_get_int(a_color);

		pTFT->fillCircle(x, y, r, c);
		msg_OnDebug("fillCircle:x=%d , y=%d , r=%d , color=%04X\r\n", x, y, r, c);
		return mp_obj_new_int(1);
	}

	mp_obj_t drawRoundRect(mp_obj_t a_xywh, mp_obj_t a_radius, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xywh, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_radius)) raise_mustInt();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xywh, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();

		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(items[2]);
		int h = mp_obj_get_int(items[3]);
		int r = mp_obj_get_int(a_radius);
		int c = mp_obj_get_int(a_color);

		pTFT->drawRoundRect(x, y, w, h, r, c);
		msg_OnDebug("drawRoundRect:x=%d , y=%d , w=%d , h=%d , r=%d , color=%04X\r\n", x, y, w, h, r, c);
		return mp_obj_new_int(1);
	}

	mp_obj_t fillRoundRect(mp_obj_t a_xywh, mp_obj_t a_radius, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xywh, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_radius)) raise_mustInt();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xywh, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(items[2]);
		int h = mp_obj_get_int(items[3]);
		int r = mp_obj_get_int(a_radius);
		int c = mp_obj_get_int(a_color);

		pTFT->fillRoundRect(x, y, w, h, r, c);
		msg_OnDebug("fillRoundRect:x=%d , y=%d , w=%d , h=%d , r=%d , color=%04X\r\n", x, y, w, h, r, c);
		return mp_obj_new_int(1);
	}

	mp_obj_t drawTriangle(mp_obj_t a_xyxy, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xyxy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xyxy, &len, &items);
		// 要素の数を確認
		if (len != 6) mp_raise_ValueError(MP_ERROR_TEXT("Expected 6 elements in the tuple containing x1,y1,x2,y2,x3,y3"));
		int x1 = mp_obj_get_int(items[0]);
		int y1 = mp_obj_get_int(items[1]);
		int x2 = mp_obj_get_int(items[2]);
		int y2 = mp_obj_get_int(items[3]);
		int x3 = mp_obj_get_int(items[4]);
		int y3 = mp_obj_get_int(items[5]);
		int c = mp_obj_get_int(a_color);

		pTFT->drawTriangle(x1, y1, x2, y2, x3, y3, c);
		msg_OnDebug("drawTriangle:x1=%d , y1=%d , x2=%d , y2=%d , x3=%d , y3=%d , color=%04X\r\n", x1, y1, x2, y2, x3, y3, c);
		return mp_obj_new_int(1);
	}
	mp_obj_t fillTriangle(mp_obj_t a_xyxy, mp_obj_t a_color)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xyxy, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_int(a_color)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xyxy, &len, &items);
		// 要素の数を確認
		if (len != 6) mp_raise_ValueError(MP_ERROR_TEXT("Expected 6 elements in the tuple containing x1,y1,x2,y2,x3,y3"));
		int x1 = mp_obj_get_int(items[0]);
		int y1 = mp_obj_get_int(items[1]);
		int x2 = mp_obj_get_int(items[2]);
		int y2 = mp_obj_get_int(items[3]);
		int x3 = mp_obj_get_int(items[4]);
		int y3 = mp_obj_get_int(items[5]);
		int c = mp_obj_get_int(a_color);

		pTFT->fillTriangle(x1, y1, x2, y2, x3, y3, c);
		msg_OnDebug("fillTriangle:x1=%d , y1=%d , x2=%d , y2=%d , x3=%d , y3=%d , color=%04X\r\n", x1, y1, x2, y2, x3, y3, c);
		return mp_obj_new_int(1);
	}
#pragma endregion

#pragma region 文字描画関数

	/// @brief カーソル設定。余りに毎回間違えるので、タプルでも独立変数でも受け取れるようにする。

	/// @param a_xy
	/// @return

	mp_obj_t setCursor(size_t n_args, const mp_obj_t* args)
	{
		int x;
		int y;
		if (n_args == 1) { // 引数が１つのとき
			size_t len;
			mp_obj_t* items;
			if (!mp_obj_is_type(args[0], &mp_type_tuple)) raise_mustTupple();
			// タプルの要素を取得
			mp_obj_tuple_get(args[0], &len, &items);
			// 要素の数を確認
			if (len != 2) raise_mustXYTupple();
			x = mp_obj_get_int(items[0]);
			y = mp_obj_get_int(items[1]);
			msg_OnDebug("setCursor(tupple):x=%d , y=%d\r\n", x, y);
		} else if (n_args == 2) {
			if (!mp_obj_is_int(args[0])) raise_mustInt();
			if (!mp_obj_is_int(args[1])) raise_mustInt();
			x = mp_obj_get_int(args[0]);
			y = mp_obj_get_int(args[1]);
			msg_OnDebug("setCursor(x,y):x=%d , y=%d\r\n", x, y);
		} else {
			mp_raise_ValueError(MP_ERROR_TEXT("Expected 1 or 2 arguments"));
		}
		pTFT->setCursor(x, y);
		return mp_obj_new_int(1);
	}
	/// @brief 文字を描画する。 printfは実装しない。python側でf-stringを使ったほうが高機能なので。
	/// @param a_xy 画面の座標。タプルで指定する。x,yの順番
	/// @param a_str 描画する文字列。str型
	/// @return 常に1
	mp_obj_t print(mp_obj_t a_str)
	{
		// タプルを取得
		size_t len;
		if (!mp_obj_is_str(a_str)) raise_mustStr();
		const char* str = mp_obj_str_get_str(a_str);

		pTFT->print(str);
		msg_OnDebug("print:str=%s\r\n", str);
		return mp_obj_new_int(1);
	}
	mp_obj_t setTextWrap(mp_obj_t a_tf)
	{
		if (!mp_obj_is_bool(a_tf)) raise_mustBool();
		bool tf = mp_obj_is_true(a_tf);
		pTFT->setTextWrap(tf);
		msg_OnDebug("setTextWrap: %d\r\n", tf);
		return mp_obj_new_int(1);
	}
	mp_obj_t setTextForegroundColor(mp_obj_t a_color)
	{
		if (!mp_obj_is_int(a_color)) raise_mustInt();
		int color = mp_obj_get_int(a_color);
		pTFT->setTextColor(color);
		msg_OnDebug("setTextColor: %04X\r\n", color);
		return mp_obj_new_int(1);
	}
	mp_obj_t setTextBackgroundColor(mp_obj_t a_color)
	{
		if (!mp_obj_is_int(a_color)) raise_mustInt();
		int color = mp_obj_get_int(a_color);
		pTFT->setBackgroundColor(color);
		msg_OnDebug("setTextBGColor: %04X\r\n", color);
		return mp_obj_new_int(1);
	}
	mp_obj_t setTextColor(mp_obj_t a_fg, mp_obj_t a_bg)
	{
		if (!mp_obj_is_int(a_fg)) raise_mustInt();
		if (!mp_obj_is_int(a_bg)) raise_mustInt();
		int fg = mp_obj_get_int(a_fg);
		int bg = mp_obj_get_int(a_bg);
		pTFT->setTextColor(fg, bg);
		msg_OnDebug("setTextColor: %04X , %04X\r\n", fg, bg);
		return mp_obj_new_int(1);
	}
	mp_obj_t setTextSize(mp_obj_t a_size)
	{
		if (!mp_obj_is_int(a_size)) raise_mustInt();
		int size = mp_obj_get_int(a_size);
		pTFT->setTextSize(size);
		msg_OnDebug("setTextSize: %d\r\n", size);
		return mp_obj_new_int(1);
	}
	mp_obj_t setKanjiMode(mp_obj_t a_tf)
	{
		if (!mp_obj_is_bool(a_tf)) raise_mustBool();
		bool tf = mp_obj_is_true(a_tf);
		pTFT->KanjiMode(tf);
		msg_OnDebug("setKanjiMode: %d\r\n", tf);
		return mp_obj_new_int(1);
	}

	mp_obj_t setKanjiFont(mp_obj_t a_code, mp_obj_t a_bitmap)
	{
		return mp_obj_new_int(1);
	}
#pragma endregion

#pragma region ビットマップ描画関数
	mp_obj_t drawBitmap(mp_obj_t a_xywh, mp_obj_t a_bitmap)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xywh, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_type(a_bitmap, &mp_type_array)) raise_mustArray();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xywh, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();
		;
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(items[2]);
		int h = mp_obj_get_int(items[3]);

		mp_buffer_info_t bufInfo;
		mp_get_buffer_raise(a_bitmap, &bufInfo, MP_BUFFER_READ); // Pythonからバッファ取得

		size_t len2 = bufInfo.len / sizeof(uint8_t); // データ長をバイト単位で計算
		msg_OnDebug("drawBitmap: dataLength= %d\r\n", len2);
		const uint16_t* data = (uint16_t*)bufInfo.buf;

		pTFT->drawRGBBitmap(x, y, data, w, h);

		return mp_obj_new_int(1);
	}
	/// @brief	ビットマップを描画する。背景の透過色を指定できる。
	/// @param a_xywh
	/// @param a_bitmap
	/// @return
	mp_obj_t drawBitmapWithKeyColor(mp_obj_t a_xywh, mp_obj_t a_bitmap, mp_obj_t a_transparent)
	{
		// タプルを取得
		size_t len;
		mp_obj_t* items;
		if (!mp_obj_is_type(a_xywh, &mp_type_tuple)) raise_mustTupple();
		if (!mp_obj_is_type(a_bitmap, &mp_type_array)) raise_mustArray();
		if (!mp_obj_is_int(a_transparent)) raise_mustInt();

		// タプルの要素を取得
		mp_obj_tuple_get(a_xywh, &len, &items);
		// 要素の数を確認
		if (len != 4) raise_mustXYWHTupple();
		int x = mp_obj_get_int(items[0]);
		int y = mp_obj_get_int(items[1]);
		int w = mp_obj_get_int(items[2]);
		int h = mp_obj_get_int(items[3]);
		int transparent = mp_obj_get_int(a_transparent);

		mp_buffer_info_t bufInfo;
		mp_get_buffer_raise(a_bitmap, &bufInfo, MP_BUFFER_READ); // Pythonからバッファ取得

		size_t len2 = bufInfo.len / sizeof(uint8_t); // データ長をバイト単位で計算
		msg_OnDebug("drawBitmapWithKeyColor: dataLength= %d\r\n", len2);
		const uint16_t* data = (uint16_t*)bufInfo.buf;

		pTFT->drawRGBBitmap(x, y, data, w, h, transparent);

		return mp_obj_new_int(1);
	}

}	// namespace TFT
