/// @brief このファイルは、MicropythonのC拡張モジュールとして使用する場合に有効にする。

#include "GFXModule.h"
#include <obj.h>

// Define a Python reference to the function we'll make available.
// See example.cpp for the definition.


//#pragma region 初期化・設定関数
static MP_DEFINE_CONST_FUN_OBJ_1(initHW_obj, initHW);
static MP_DEFINE_CONST_FUN_OBJ_1(loadDefaultKanjiFont_obj, loadDefaultKanjiFont);
static MP_DEFINE_CONST_FUN_OBJ_1(loadDefaultAsciiFont_obj, loadDefaultAsciiFont);
static MP_DEFINE_CONST_FUN_OBJ_1(setDebugMode_obj, setDebugMode);
static MP_DEFINE_CONST_FUN_OBJ_1(getTypeName_obj, getTypeName);
static MP_DEFINE_CONST_FUN_OBJ_1(setRotation_obj, setRotation);
static MP_DEFINE_CONST_FUN_OBJ_1(displaySleep_obj, displaySleep);
// #pragma endregion
//  基本描画関数 static MP_DEFINE_CONST_FUN_OBJ_2(_obj, );

#pragma region 特殊描画関数
static MP_DEFINE_CONST_FUN_OBJ_1(invertDisplay_obj, invertDisplay);
static MP_DEFINE_CONST_FUN_OBJ_2(setScrollMargins_obj, setScrollMargins);
static MP_DEFINE_CONST_FUN_OBJ_1(scrollTo_obj, scrollTo);
#pragma endregion

#pragma region 基本描画関数
static MP_DEFINE_CONST_FUN_OBJ_2(fillScreen_obj, fillScreen);
static MP_DEFINE_CONST_FUN_OBJ_3(drawPixel_obj, drawPixel);
static MP_DEFINE_CONST_FUN_OBJ_3(drawFastHLine_obj, drawFastHLine);
static MP_DEFINE_CONST_FUN_OBJ_3(drawFastVLine_obj, drawFastVLine);
static MP_DEFINE_CONST_FUN_OBJ_3(fillRect_obj, fillRect);
static MP_DEFINE_CONST_FUN_OBJ_3(drawRect_obj, drawRect);
static MP_DEFINE_CONST_FUN_OBJ_3(drawLine_obj, drawLine);
static MP_DEFINE_CONST_FUN_OBJ_3(drawCircle_obj, drawCircle);
static MP_DEFINE_CONST_FUN_OBJ_3(fillCircle_obj, fillCircle);
static MP_DEFINE_CONST_FUN_OBJ_3(drawRoundRect_obj, drawRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_3(fillRoundRect_obj, fillRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_3(drawTriangle_obj, drawTriangle);
static MP_DEFINE_CONST_FUN_OBJ_3(fillTriangle_obj, fillTriangle);
#pragma endregion

#pragma region テキスト描画関数
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(setCursor_obj, 2, 3, setCursor);
static MP_DEFINE_CONST_FUN_OBJ_2(print_obj, print);
static MP_DEFINE_CONST_FUN_OBJ_2(setTextWrap_obj, setTextWrap);
static MP_DEFINE_CONST_FUN_OBJ_2(setTextForegroundColor_obj, setTextForegroundColor);
static MP_DEFINE_CONST_FUN_OBJ_2(setTextBackgroundColor_obj, setTextBackgroundColor);
static MP_DEFINE_CONST_FUN_OBJ_3(setTextColor_obj, setTextColor);
static MP_DEFINE_CONST_FUN_OBJ_2(setTextSize_obj, setTextSize);
static MP_DEFINE_CONST_FUN_OBJ_2(setKanjiMode_obj, setKanjiMode);
static MP_DEFINE_CONST_FUN_OBJ_3(setKanjiFont_obj, setKanjiFont);
#pragma endregion

#pragma region ビットマップ関数
static MP_DEFINE_CONST_FUN_OBJ_3(drawBitmap_obj, drawBitmap);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawBitmapWithKeyColor_obj, 4,4,drawBitmapWithKeyColor);
// static MP_DEFINE_CONST_FUN_OBJ_4(drawBitmapWithKeyColor_obj, drawBitmapWithKeyColor);
static MP_DEFINE_CONST_FUN_OBJ_3(drawBitmapFromCanvas_obj, drawBitmapFromCanvas);
#pragma endregion

#pragma region キャンバス関数
static MP_DEFINE_CONST_FUN_OBJ_1(createCanvas_obj, createCanvas);
static MP_DEFINE_CONST_FUN_OBJ_1(deleteCanvas_obj, deleteCanvas);
static MP_DEFINE_CONST_FUN_OBJ_2(setCanvasKeyColor_obj, setCanvasKeyColor);
static MP_DEFINE_CONST_FUN_OBJ_1(resetCanvasKeyColor_obj, resetCanvasKeyColor);
#pragma endregion

#pragma region タッチ関連
static MP_DEFINE_CONST_FUN_OBJ_2(initTouchHW_obj, initTouchHW);
static MP_DEFINE_CONST_FUN_OBJ_1(setTouchCalibrationValue_obj, setTouchCalibrationValue);
static MP_DEFINE_CONST_FUN_OBJ_0(beginTouch_obj, beginTouch);
static MP_DEFINE_CONST_FUN_OBJ_0(isTouch_obj, isTouch);
static MP_DEFINE_CONST_FUN_OBJ_1(setTouchRotation_obj, setTouchRotation);
static MP_DEFINE_CONST_FUN_OBJ_0(getTouchRawXYZ_obj, getTouchRawXYZ);
static MP_DEFINE_CONST_FUN_OBJ_0(getTouchXY_obj, getTouchXY);
#pragma endregion

static const mp_rom_map_elem_t KNJGfx_globals_table[] = {
	// #pragma region 初期化・設定関数
	{MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_PTR(&initHW_obj)},
	//	{MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_KNJGfx)},
	// 初期化関数
	{MP_ROM_QSTR(MP_QSTR_initHW), MP_ROM_PTR(&initHW_obj)},

	{MP_ROM_QSTR(MP_QSTR_loadDefaultKanjiFont), MP_ROM_PTR(&loadDefaultKanjiFont_obj)},
	{MP_ROM_QSTR(MP_QSTR_loadDefaultAsciiFont), MP_ROM_PTR(&loadDefaultAsciiFont_obj)},
	{MP_ROM_QSTR(MP_QSTR_setDebugMode), MP_ROM_PTR(&setDebugMode_obj)},
	{MP_ROM_QSTR(MP_QSTR_getTypeName), MP_ROM_PTR(&getTypeName_obj)},
	{MP_ROM_QSTR(MP_QSTR_setRotation), MP_ROM_PTR(&setRotation_obj)},
	{MP_ROM_QSTR(MP_QSTR_displaySleep), MP_ROM_PTR(&displaySleep_obj)},
	// #pragma endregion

	// #pragma region 特殊描画関数
	{MP_ROM_QSTR(MP_QSTR_invertDisplay), MP_ROM_PTR(&invertDisplay_obj)},
	{MP_ROM_QSTR(MP_QSTR_setScrollMargins), MP_ROM_PTR(&setScrollMargins_obj)},
	{MP_ROM_QSTR(MP_QSTR_scrollTo), MP_ROM_PTR(&scrollTo_obj)},
	// #pragma endregion

	// #pragma region 基本描画関数
	// 基本描画関数　		{MP_ROM_QSTR(MP_QSTR_), MP_ROM_PTR(&_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillScreen), MP_ROM_PTR(&fillScreen_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawPixel), MP_ROM_PTR(&drawPixel_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawFastHLine), MP_ROM_PTR(&drawFastHLine_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawFastVLine), MP_ROM_PTR(&drawFastVLine_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillRect), MP_ROM_PTR(&fillRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawRect), MP_ROM_PTR(&drawRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawLine), MP_ROM_PTR(&drawLine_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawCircle), MP_ROM_PTR(&drawCircle_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillCircle), MP_ROM_PTR(&fillCircle_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawRoundRect), MP_ROM_PTR(&drawRoundRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillRoundRect), MP_ROM_PTR(&fillRoundRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawTriangle), MP_ROM_PTR(&drawTriangle_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillTriangle), MP_ROM_PTR(&fillTriangle_obj)},
	// #pragma endregion

	// #pragma region テキスト描画関数
	{MP_ROM_QSTR(MP_QSTR_setCursor), MP_ROM_PTR(&setCursor_obj)},
	{MP_ROM_QSTR(MP_QSTR_print), MP_ROM_PTR(&print_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTextWrap), MP_ROM_PTR(&setTextWrap_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTextForegroundColor), MP_ROM_PTR(&setTextForegroundColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTextBackgroundColor), MP_ROM_PTR(&setTextBackgroundColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTextColor), MP_ROM_PTR(&setTextColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTextSize), MP_ROM_PTR(&setTextSize_obj)},
	{MP_ROM_QSTR(MP_QSTR_setKanjiMode), MP_ROM_PTR(&setKanjiMode_obj)},
	{MP_ROM_QSTR(MP_QSTR_setKanjiFont), MP_ROM_PTR(&setKanjiFont_obj)},
	// #pragma endregion
	// #pragma region テキスト描画関数
	{MP_ROM_QSTR(MP_QSTR_drawBitmap), MP_ROM_PTR(&drawBitmap_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawBitmapWithKeyColor), MP_ROM_PTR(&drawBitmapWithKeyColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawBitmapFromCanvas), MP_ROM_PTR(&drawBitmapFromCanvas_obj)},

	{MP_ROM_QSTR(MP_QSTR_createCanvas), MP_ROM_PTR(&createCanvas_obj)},
	{MP_ROM_QSTR(MP_QSTR_deleteCanvas), MP_ROM_PTR(&deleteCanvas_obj)},
	{MP_ROM_QSTR(MP_QSTR_setCanvasKeyColor), MP_ROM_PTR(&setCanvasKeyColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_resetCanvasKeyColor), MP_ROM_PTR(&resetCanvasKeyColor_obj)},
	// #pragma endregion
	{MP_ROM_QSTR(MP_QSTR_initTouchHW), MP_ROM_PTR(&initTouchHW_obj)},
	{MP_ROM_QSTR(MP_QSTR_beginTouch), MP_ROM_PTR(&beginTouch_obj)},
	{MP_ROM_QSTR(MP_QSTR_isTouch), MP_ROM_PTR(&isTouch_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTouchRotation), MP_ROM_PTR(&setTouchRotation_obj)},
	{MP_ROM_QSTR(MP_QSTR_setTouchCalibrationValue), MP_ROM_PTR(&setTouchCalibrationValue_obj)},
	{MP_ROM_QSTR(MP_QSTR_getTouchRawXYZ), MP_ROM_PTR(&getTouchRawXYZ_obj)},
	{MP_ROM_QSTR(MP_QSTR_getTouchXY), MP_ROM_PTR(&getTouchXY_obj)},
	// #pragma ednregion
};

static MP_DEFINE_CONST_DICT(KNJGfx_module_globals, KNJGfx_globals_table);

const mp_obj_module_t mp_module_KNJGfx = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t*)&KNJGfx_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_KNJGfx, mp_module_KNJGfx);



