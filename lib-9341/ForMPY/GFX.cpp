/// @brief このファイルは、MicropythonのC拡張モジュールとして使用する場合に有効にする。
/// ビルド時は、User C Moduleのcmakeファイルが存在する場所（ports/rp2/myModule等）に移動してビルドする。
#include <stdlib.h>

#include <stdio.h>
#include "../Adafruit_ILI9341/Adafruit_ILI9341.h"

#pragma GCC diagnostic ignored "-Wunused-variable"

extern "C" {
#include <py/objstr.h>
#include "GFXModule.h"

	using namespace ardPort;
	// Here we implement the] function using C++ code, but since it's
	// declaration has to be compatible with C everything goes in extern "C" scope.
	mp_obj_t cppfunc(mp_obj_t a_obj, mp_obj_t b_obj) {
		// The following no-ops are just here to verify the static assertions used in
		// the public API all compile with C++.
		MP_STATIC_ASSERT_STR_ARRAY_COMPATIBLE;
		if (mp_obj_is_type(a_obj, &mp_type_BaseException)) {
		}

		// Prove we have (at least) C++11 features.
		const auto a = mp_obj_get_int(a_obj);
		const auto b = mp_obj_get_int(b_obj);
		const auto sum = [&]() {
			return mp_obj_new_int(a + b);
		}();
		// Prove we're being scanned for QSTRs.
		mp_obj_t tup[] = {sum, MP_ROM_QSTR(MP_QSTR_hellocpp)};
		return mp_obj_new_tuple(2, tup);
	}

	// static HW HWObj;
	// static ST7735 ST7735Obj;
	//  エラーメッセージ用のバッファ。mp_raise_XXXXでは、スタック変数（ヒープは未確認だが以前mallocしただけでハングする等不穏な動きをした）は使えない。
	//  そのため文字列リテラル以外を使う時にはこの静的変数を使う
	// static char errTxt[128];

	// フォントを保存しておく構造体。
#ifdef TFT_ENABLE_FONTS
	static struct
	{
		const char *name;
		const GFXfont *font;
	} fontList[16];
#endif

	/// @brief 与えられた変数のタイプを文字列で返す関数。デバッグ用
	mp_obj_t getTypeName(mp_obj_t obj) {
		const mp_obj_type_t *type = mp_obj_get_type(obj);
		const char *type_name = qstr_str(type->name);
		return mp_obj_new_str(type_name, strlen(type_name));
	}

	/// @brief ハードウェアの初期化処理
	Adafruit_ILI9341 tft;
	mp_obj_t InitHW(mp_obj_t args) {
		// タプルを取得
		size_t len;
		mp_obj_t *items;
		if (!mp_obj_is_type(args, &mp_type_tuple)) {
			mp_raise_TypeError(MP_ERROR_TEXT("Expected a tuple"));
		}
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
		int iDC = mp_obj_get_int(items[1]);     // 20
		int iCS = mp_obj_get_int(items[2]);     // 22
		int iSCK = mp_obj_get_int(items[3]);    // 18
		int iMOSI = mp_obj_get_int(items[4]);   // 19
		int iReset = mp_obj_get_int(items[5]);  // 21
		int iDebug = mp_obj_get_int(items[6]);  // do not use
		// ハードウェア初期化
		spi::SPIClassRP2040 *pSPI = iSPIPortNo == 0 ? &SPI : &SPI1;

		tft.constructObject(pSPI, iDC, iCS, iReset);
		pSPI->setMOSI(iMOSI);
		pSPI->setSCK(iSCK);
		tft.begin();  // SPIの初期化を行う。引数はSPIの周波数。0はデフォルト値

		//  結果を返す
		return mp_obj_new_int(1);
	}
	/*
		/// @brief グラフィックスオブジェクトを作成する。ソフトウェア関連の初期化を行う
		mp_obj_t CreateGFX(mp_obj_t a_pHW)
		{
			// フォント関連の初期化
	#ifdef TFT_ENABLE_FONTS
			for (int i = 0; i < 16; i++)
			{
				fontList[i].name = NULL;
				fontList[i].font = NULL;
			}
			fontList[0].font = &FreeMono9pt7b;
			fontList[0].name = "FreeMono9pt7b";
			fontList[1].font = &FreeMono18pt7b;
			fontList[1].name = "FreeMono18pt7b";
			fontList[2].font = &FreeMonoOblique12pt_sub;
			fontList[2].name = "FreeMonoOblique12pt_sub";
			fontList[3].font = &FreeMonoOblique12pt7b;
			fontList[3].name = "FreeMonoOblique12pt7b";

	#endif

			if (HWObj.portSPI == NULL)
			{
				mp_raise_ValueError("Must call InitHW before CreateGFX");
				return mp_obj_new_int(0);
			}
			ST7735Obj.SetSPIHW(&HWObj);
			if (ST7735Obj.pSpiHW == NULL)
			{
				mp_raise_ValueError("SPI HW Object is null");
				return mp_obj_new_int(0);
			}

			return mp_obj_new_int(1);
		}

		/// @brief ハードウェアに初期化命令を流す
		mp_obj_t doInit()
		{
			ST7735Obj.doInit();
			return mp_obj_new_int(1);
		}

		/// @brief  テキストの折り返し有無を設定する
		mp_obj_t setTextWrap(mp_obj_t a_tf)
		{
			if (!mp_obj_is_bool(a_tf))
				mp_raise_TypeError("Expected a bool");
			bool tf = mp_obj_is_true(a_tf);
			ST7735Obj.setTextWrap(tf);
			return mp_obj_new_int(1);
		}

		/// @brief 画面の回転角度を設定する
		mp_obj_t setRotation(mp_obj_t a_Rotation)
		{
			if (!mp_obj_is_int(a_Rotation))
				mp_raise_TypeError("Expected a int");
			int iRotation = mp_obj_get_int(a_Rotation);
			if (iRotation < 0 || iRotation > 3)
			{
				mp_raise_ValueError("Value must be 0-3");
			}
			ST7735_ROTATION rot = (ST7735_ROTATION)iRotation;
			ST7735Obj.SetRotation(rot);
			return mp_obj_new_int(1);
		}

		/// @brief 画面の横幅を求める
		mp_obj_t getWidth() { return mp_obj_new_int(ST7735Obj.getWidth()); }
		/// @brief 画面の高さを求める
		mp_obj_t getHeight() { return mp_obj_new_int(ST7735Obj.getHeight()); }

		/// @brief 画面を特定の色で塗りつぶす
		mp_obj_t fillScreen(mp_obj_t a_Color)
		{
			int color = mp_obj_get_int(a_Color);
			ST7735Obj.fillScreen(color);
			return mp_obj_new_int(color);
		}

		/// @brief 画面の特定の場所を、特定の色で塗りつぶす。
		mp_obj_t fillRectangle(mp_obj_t a_xywh, mp_obj_t a_color)
		{
			// タプルを取得
			size_t len;
			mp_obj_t *items;
			if (!mp_obj_is_type(a_xywh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");

			// タプルの要素を取得
			mp_obj_tuple_get(a_xywh, &len, &items);
			// 要素の数を確認
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x,y,w,h");
			// タプル内の要素をintに変換
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int w = mp_obj_get_int(items[2]);
			int h = mp_obj_get_int(items[3]);
			int c = mp_obj_get_int(a_color);

			ST7735Obj.fillRectangle(x, y, w, h, c);
			return mp_obj_new_int(1);
		}

		/// @brief 画面に文字を表示する
		mp_obj_t drawText(mp_obj_t a_xyfbs, mp_obj_t a_str)
		{
	#if !defined(TFT_ENABLE_TEXT)
			mp_raise_NotImplementedError("Since TFT_ENABLE_TEXT is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			// タプルを取得
			size_t len;
			mp_obj_t *items;
			if (!mp_obj_is_type(a_xyfbs, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_str(a_str))
				mp_raise_TypeError("Expected a string for 2nd argument");

			// xy座標のタプルの要素を取得
			int x, y, foreColor, backColor, isBig;
			{
				mp_obj_tuple_get(a_xyfbs, &len, &items);
				if (len != 5)
					mp_raise_ValueError("Expected 5 elements in the tuple containing x,y,forecolor,backcolor,isBigsize");
				x = mp_obj_get_int(items[0]);
				y = mp_obj_get_int(items[1]);
				foreColor = mp_obj_get_int(items[2]);
				backColor = mp_obj_get_int(items[3]);
				if (!mp_obj_is_bool(items[4]))
					mp_raise_TypeError("Expected a bool for isBigsize");
				isBig = mp_obj_is_true(items[4]) ? 2 : 1;
			}
			// タプル内の要素をintに変換
			const char *str = mp_obj_str_get_str(a_str);
			ST7735Obj.drawText(x, y, str, foreColor, backColor, isBig);
			return mp_obj_new_int(str[0]);
	#endif
		}

		/// @brief 画面に漢字カナ（含む半角）文字を表示する
		mp_obj_t drawTextKanji(mp_obj_t a_xyfbs, mp_obj_t a_str)
		{
	#if !defined(TFT_ENABLE_TEXT)
			mp_raise_NotImplementedError("since TFT_ENABLE_TEXt is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#elif !defined(TFT_ENABLE_KANJI)
			mp_raise_NotImplementedError("since TFT_ENABLE_KANJI is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			// タプルを取得
			size_t len;
			mp_obj_t *items;
			if (!mp_obj_is_type(a_xyfbs, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_str(a_str))
				mp_raise_TypeError("Expected a string for 2nd argument");

			// xy座標のタプルの要素を取得
			int x, y, foreColor, backColor, isBig;
			{
				mp_obj_tuple_get(a_xyfbs, &len, &items);
				if (len != 5)
					mp_raise_ValueError("Expected 5 elements in the tuple containing x,y,forecolor,backcolor,isBigsize");
				x = mp_obj_get_int(items[0]);
				y = mp_obj_get_int(items[1]);
				foreColor = mp_obj_get_int(items[2]);
				backColor = mp_obj_get_int(items[3]);
				if (!mp_obj_is_bool(items[4]))
					mp_raise_TypeError("Expected a bool for isBigsize");
				isBig = mp_obj_is_true(items[4]) ? 2 : 1;
			}
			// タプル内の要素をintに変換
			const char *str = mp_obj_str_get_str(a_str);
			ST7735Obj.drawTextKanji(x, y, str, foreColor, backColor, isBig);
			return mp_obj_new_int(strlen(str));
	#endif
		}
		// void ST7735::setScrollDefinition(uint8_t top_fix_height, uint8_t bottom_fix_height, bool _scroll_direction)

		/// @brief 画面のスクロール範囲を指定する
		mp_obj_t setScrollDefinition(mp_obj_t a_top, mp_obj_t a_bottom, mp_obj_t a_direction)
		{
	#if !defined(TFT_ENABLE_SCROLL)
			mp_raise_NotImplementedError("since TFT_ENABLE_SCROLL is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_int(a_top))
				mp_raise_TypeError("Expected a int for 1st argument");
			if (!mp_obj_is_int(a_bottom))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_bool(a_direction))
				mp_raise_TypeError("Expected a bool for 3rd argument");
			int top = mp_obj_get_int(a_top);
			int bottom = mp_obj_get_int(a_bottom);
			bool dir = mp_obj_is_true(a_direction);
			ST7735Obj.setScrollDefinition(top, bottom, dir);
			return mp_obj_new_int(1);
	#endif
		}

		/// @brief 画面をスクロールする
		mp_obj_t verticalScroll(mp_obj_t a_scrCnt)
		{
	#if !defined(TFT_ENABLE_SCROLL)
			mp_raise_NotImplementedError("since TFT_ENABLE_SCROLL is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_int(a_scrCnt))
				mp_raise_TypeError("Expected a int for 1st argument");
			int scrCnt = mp_obj_get_int(a_scrCnt);
			ST7735Obj.verticalScroll(scrCnt);
			return mp_obj_new_int(scrCnt);
	#endif
		}

		/// @brief フォント機能が有効な時、drawTextで使用されるフォントを名前で指定する
		mp_obj_t setAsciiFont(mp_obj_t a_fontname)
		{
	#if !defined(TFT_ENABLE_TEXT)
			mp_raise_NotImplementedError("since TFT_ENABLE_TEXT is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#elif !defined(TFT_ENABLE_FONTS)
			mp_raise_NotImplementedError("since TFT_ENABLE_FONTS is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_str(a_fontname))
				mp_raise_TypeError("Expected a string for 1st argument");
			const char *str = mp_obj_str_get_str(a_fontname);
			for (int i = 0; fontList[i].name != NULL; i++)
			{
				if (strcmp(str, fontList[i].name) == 0)
				{
					ST7735Obj.setFont(fontList[i].font);
					return mp_obj_new_int(1);
				}
			}
			mp_raise_ValueError("Font name {str} not found. Check the specified font name or define the font in the fontList structure in GFX.cpp");
	#endif
		}

		/// @brief フォント機能が有効な時、drawTextで使用できるフォント名を返す
		/// @return
		mp_obj_t getAvaiableAsciiFonts()
		{
	#if !defined(TFT_ENABLE_TEXT)
			mp_raise_NotImplementedError("since TFT_ENABLE_TEXT is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#elif !defined(TFT_ENABLE_FONTS)
			mp_raise_NotImplementedError("since TFT_ENABLE_FONTS is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			mp_obj_t list = mp_obj_new_list(0, NULL);
			for (int i = 0; fontList[i].name != NULL; i++)
			{
				mp_obj_list_append(list, mp_obj_new_str(fontList[i].name, strlen(fontList[i].name)));
			}
			return list;
	#endif
		}

		/// @brief 基本的な描画処理関数…　横線
		mp_obj_t drawFastHLine(mp_obj_t a_xy, mp_obj_t a_w, mp_obj_t a_c)
		{
			if (!mp_obj_is_type(a_xy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_w))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_c))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xy, &len, &items);
			if (len != 2)
				mp_raise_ValueError("Expected 2 elements in the tuple containing x,y");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int w = mp_obj_get_int(a_w);
			int c = mp_obj_get_int(a_c);
			ST7735Obj.drawFastHLine(x, y, w, c);
			return mp_obj_new_int(1);
		}

		/// @brief 基本的な描画処理関数 縦線
		mp_obj_t drawFastVLine(mp_obj_t a_xy, mp_obj_t a_h, mp_obj_t a_c)
		{
			if (!mp_obj_is_type(a_xy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_h))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_c))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xy, &len, &items);
			if (len != 2)
				mp_raise_ValueError("Expected 2 elements in the tuple containing x,y");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int h = mp_obj_get_int(a_h);
			int c = mp_obj_get_int(a_c);
			ST7735Obj.drawFastVLine(x, y, h, c);
			return mp_obj_new_int(1);
		}

		/// @brief 描画関数 矩形、座標と大きさを指定
		mp_obj_t drawRectWH(mp_obj_t a_xywh, mp_obj_t a_color)
		{
			if (!mp_obj_is_type(a_xywh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xywh, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x,y,w,h");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int w = mp_obj_get_int(items[2]);
			int h = mp_obj_get_int(items[3]);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawRectWH(x, y, w, h, c);
			return mp_obj_new_int(1);
		}

		/// @brief 描画関数 矩形、座標ペアを指定
		mp_obj_t drawRect(mp_obj_t a_xyxy, mp_obj_t a_color)
		{
			if (!mp_obj_is_type(a_xyxy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xyxy, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x0,y0,x1,y1");
			int x0 = mp_obj_get_int(items[0]);
			int y0 = mp_obj_get_int(items[1]);
			int x1 = mp_obj_get_int(items[2]);
			int y1 = mp_obj_get_int(items[3]);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawRect(x0, y0, x1, y1, c);
			return mp_obj_new_int(1);
		}

		/// @brief 塗りつぶし矩形　座標と大きさを指定
		mp_obj_t fillRectWH(mp_obj_t a_xywh, mp_obj_t a_color)
		{
			if (!mp_obj_is_type(a_xywh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xywh, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x,y,w,h");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int w = mp_obj_get_int(items[2]);
			int h = mp_obj_get_int(items[3]);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.fillRectWH(x, y, w, h, c);
			return mp_obj_new_int(1);
		}

		/// @brief 塗りつぶし矩形　座標ペアを指定
		mp_obj_t fillRect(mp_obj_t a_xyxy, mp_obj_t a_color)
		{
			if (!mp_obj_is_type(a_xyxy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xyxy, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x0,y0,x1,y1");
			int x0 = mp_obj_get_int(items[0]);
			int y0 = mp_obj_get_int(items[1]);
			int x1 = mp_obj_get_int(items[2]);
			int y1 = mp_obj_get_int(items[3]);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.fillRect(x0, y0, x1, y1, c);
			return mp_obj_new_int(1);
		}
		/// @brief 点の描画
		mp_obj_t drawPixel(mp_obj_t a_xy, mp_obj_t a_color)
		{
			if (!mp_obj_is_type(a_xy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xy, &len, &items);
			if (len != 2)
				mp_raise_ValueError("Expected 2 elements in the tuple containing x,y");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawPixel(x, y, c);
			return mp_obj_new_int(1);
		}
		/// @brief 通常画面に戻す
		mp_obj_t normalDisplay()
		{
			ST7735Obj.NormalDisplay();
			return mp_obj_new_int(1);
		}
		/// @brief 画面を反転させる
		mp_obj_t invertDisplay(mp_obj_t a_tf)
		{
			if (!mp_obj_is_bool(a_tf))
				mp_raise_TypeError("Expected a bool");
			bool tf = mp_obj_is_true(a_tf);
			ST7735Obj.invertDisplay(tf);
			return mp_obj_new_int(1);
		}

		/// @brief  円の描画
		mp_obj_t drawCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else

			if (!mp_obj_is_type(a_xy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_r))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xy, &len, &items);
			if (len != 2)
				mp_raise_ValueError("Expected 2 elements in the tuple containing x,y");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int r = mp_obj_get_int(a_r);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawCircle(x, y, r, c);
			return mp_obj_new_int(1);
	#endif
		}
		/// @brief  円の塗りつぶし描画
		mp_obj_t fillCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_type(a_xy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_r))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xy, &len, &items);
			if (len != 2)
				mp_raise_ValueError("Expected 2 elements in the tuple containing x,y");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int r = mp_obj_get_int(a_r);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.fillCircle(x, y, r, c);
			return mp_obj_new_int(1);
	#endif
		}
		/// @brief 角丸矩形の描画・座標と大きさ
		mp_obj_t drawRoundRectWH(mp_obj_t a_xywh, mp_obj_t a_r, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_type(a_xywh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_r))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xywh, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x,y,w,h");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int w = mp_obj_get_int(items[2]);
			int h = mp_obj_get_int(items[3]);
			int r = mp_obj_get_int(a_r);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawRoundRectWH(x, y, w, h, r, c);
			return mp_obj_new_int(1);
	#endif
		}
		/// @brief 角丸矩形の描画・座標ペア
		mp_obj_t drawRountRect(mp_obj_t a_xyxy, mp_obj_t a_r, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_type(a_xyxy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_r))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xyxy, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x0,y0,x1,y1");
			int x0 = mp_obj_get_int(items[0]);
			int y0 = mp_obj_get_int(items[1]);
			int x1 = mp_obj_get_int(items[2]);
			int y1 = mp_obj_get_int(items[3]);
			int r = mp_obj_get_int(a_r);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawRoundRect(x0, y0, x1, y1, r, c);
			return mp_obj_new_int(1);
	#endif
		}

		mp_obj_t fillRoundRectWH(mp_obj_t a_xywh, mp_obj_t a_r, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else

			if (!mp_obj_is_type(a_xywh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_r))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xywh, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x,y,w,h");
			int x = mp_obj_get_int(items[0]);
			int y = mp_obj_get_int(items[1]);
			int w = mp_obj_get_int(items[2]);
			int h = mp_obj_get_int(items[3]);
			int r = mp_obj_get_int(a_r);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.fillRoundRectWH(x, y, w, h, r, c);
			return mp_obj_new_int(1);
	#endif
		}

		mp_obj_t fillRoundRect(mp_obj_t a_xyxy, mp_obj_t a_r, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_type(a_xyxy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_r))
				mp_raise_TypeError("Expected a int for 2nd argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 3rd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xyxy, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x0,y0,x1,y1");
			int x0 = mp_obj_get_int(items[0]);
			int y0 = mp_obj_get_int(items[1]);
			int x1 = mp_obj_get_int(items[2]);
			int y1 = mp_obj_get_int(items[3]);
			int r = mp_obj_get_int(a_r);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.fillRoundRect(x0, y0, x1, y1, r, c);
			return mp_obj_new_int(1);
	#endif
		}

		mp_obj_t drawLine(mp_obj_t a_xyxy, mp_obj_t a_color)
		{
	#if !defined(TFT_ENABLE_SHAPES)
			mp_raise_NotImplementedError("since TFT_ENABLE_SHAPES is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
			return mp_obj_new_int(0);
	#else
			if (!mp_obj_is_type(a_xyxy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_int(a_color))
				mp_raise_TypeError("Expected a int for 2nd argument");
			size_t len;
			mp_obj_t *items;
			mp_obj_tuple_get(a_xyxy, &len, &items);
			if (len != 4)
				mp_raise_ValueError("Expected 4 elements in the tuple containing x0,y0,x1,y1");
			int x0 = mp_obj_get_int(items[0]);
			int y0 = mp_obj_get_int(items[1]);
			int x1 = mp_obj_get_int(items[2]);
			int y1 = mp_obj_get_int(items[3]);
			int c = mp_obj_get_int(a_color);
			ST7735Obj.drawLine(x0, y0, x1, y1, c);
			return mp_obj_new_int(1);
	#endif
		}

		// void ST7735::bmpDraw(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t* , uint8_t direction)

		mp_obj_t bmpDraw(mp_obj_t a_xywh, mp_obj_t a_bmpData, mp_obj_t a_direction)
		{
	#if !defined(TFT_ENABLE_BITMAP)
			mp_raise_NotImplementedError("since TFT_ENABLE_BITMAP is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
	#else
			if (!mp_obj_is_type(a_xywh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 1st argument");
			if (!mp_obj_is_type(a_bmpData, &mp_type_array))
				mp_raise_TypeError("Expected a array for 2nd argument");
			if (!mp_obj_is_int(a_direction))
				mp_raise_TypeError("Expected a int for 3rd argument");
			int x, y, w, h, d;
			{
				size_t len;
				mp_obj_t *items;
				mp_obj_tuple_get(a_xywh, &len, &items);
				if (len != 4)
					mp_raise_ValueError("Expected 4 elements in the tuple containing x,y,w,h");
				x = mp_obj_get_int(items[0]);
				y = mp_obj_get_int(items[1]);
				w = mp_obj_get_int(items[2]);
				h = mp_obj_get_int(items[3]);
				d = mp_obj_get_int(a_direction);
			}
			uint16_t *data;
			{
				mp_buffer_info_t bufInfo;
				mp_get_buffer_raise(a_bmpData, &bufInfo, MP_BUFFER_READ);
				size_t len = bufInfo.len / sizeof(uint16_t);

				unsigned int dataCnt = w * h;
				if (len != dataCnt)
				{
					snprintf(errTxt, sizeof(errTxt), "invalid data size. Expected %d words, actual %d words", dataCnt, len);
					mp_raise_ValueError(errTxt);
				}
				data = (uint16_t *)bufInfo.buf;
			}

			ST7735Obj.bmpDraw(x, y, w, h, data, d);
			return mp_obj_new_int(1);
	#endif
		}
		struct
		{
			uint8_t w;
			uint8_t h;
			uint16_t *data;
		} bmpData[16];

		mp_obj_t registerBitmap(mp_obj_t a_idx, mp_obj_t a_wh, mp_obj_t a_bmpData)
		{
	#if !defined(TFT_ENABLE_BITMAP)
			mp_raise_NotImplementedError("since TFT_ENABLE_BITMAP is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
	#else

			if (!mp_obj_is_int(a_idx))
				mp_raise_TypeError("Expected a int for 1st argument");
			if (!mp_obj_is_type(a_wh, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 2nd argument");
			if (!mp_obj_is_type(a_bmpData, &mp_type_array))
				mp_raise_TypeError("Expected a array for 3rd argument");

			int idx = mp_obj_get_int(a_idx);
			int w, h;
			{
				size_t len;
				mp_obj_t *items;
				mp_obj_tuple_get(a_wh, &len, &items);
				if (len != 2)
					mp_raise_ValueError("Expected 2 elements in the tuple containing w,h");
				w = mp_obj_get_int(items[0]);
				h = mp_obj_get_int(items[1]);
			}
			bmpData[idx].w = w;
			bmpData[idx].h = h;

			uint16_t *data;
			{
				mp_buffer_info_t bufInfo;
				mp_get_buffer_raise(a_bmpData, &bufInfo, MP_BUFFER_READ);
				size_t len = bufInfo.len / sizeof(uint16_t);

				unsigned int dataCnt = w * h;
				if (len != dataCnt)
				{
					snprintf(errTxt, sizeof(errTxt), "invalid data size. Expected %d words, actual %d words", dataCnt, len);
					mp_raise_ValueError(errTxt);
				}
				data = (uint16_t *)bufInfo.buf;
			}
			bmpData[idx].data = (uint16_t *)m_malloc(w * h * sizeof(uint16_t));
			memcpy(bmpData[idx].data, data, w * h * sizeof(uint16_t));
			return bmpData[idx].data;
	#endif
		}
		mp_obj_t bmpRegDraw(mp_obj_t a_idx, mp_obj_t a_xy, mp_obj_t a_direction)
		{
			if (!mp_obj_is_int(a_idx))
				mp_raise_TypeError("Expected a int for 1st argument");
			if (!mp_obj_is_type(a_xy, &mp_type_tuple))
				mp_raise_TypeError("Expected a tuple for 2nd argument");
			if (!mp_obj_is_int(a_direction))
				mp_raise_TypeError("Expected a int for 3rd argument");
			int x, y;
			{
				size_t len;
				mp_obj_t *items;
				mp_obj_tuple_get(a_xy, &len, &items);
				if (len != 2)
					mp_raise_ValueError("Expected 2 elements in the tuple containing x,y");
				x = mp_obj_get_int(items[0]);
				y = mp_obj_get_int(items[1]);
			}
			int idx = mp_obj_get_int(a_idx);
			int d = mp_obj_get_int(a_direction);

			ST7735Obj.bmpDraw(x, y, bmpData[idx].w, bmpData[idx].h, bmpData[idx].data, d);

			return mp_obj_new_int(1);
		}
		mp_obj_t bmpUseTransColor(mp_obj_t a_c)
		{
	#if !defined(TFT_ENABLE_BITMAP)
			mp_raise_NotImplementedError("since TFT_ENABLE_BITMAP is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
	#else
			if (!mp_obj_is_int(a_c))
				mp_raise_TypeError("Expected a int for 1st argument");
			int c = mp_obj_get_int(a_c);
			ST7735Obj.bmpUseTransColor(c);
			return mp_obj_new_int(1);
	#endif
		}
		mp_obj_t bmpUnuseTransColor()
		{
	#if !defined(TFT_ENABLE_BITMAP)
			mp_raise_NotImplementedError("since TFT_ENABLE_BITMAP is disabled during the build, this function cannot be used. Check ST7735_TFT.h ");
	#else
			ST7735Obj.bmpUnuseTransColor();
			return mp_obj_new_int(1);
		}
	#endif

	*/
}  //
