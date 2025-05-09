/// @brief このファイルは、MicropythonのC拡張モジュールとして使用する場合に有効にする。

#include "GFXModule.h"

// Define a Python reference to the function we'll make available.
// See example.cpp for the definition.
static MP_DEFINE_CONST_FUN_OBJ_2(cppfunc_obj, cppfunc);

//#pragma region 初期化・設定関数
static MP_DEFINE_CONST_FUN_OBJ_1(initHW_obj, initHW);
static MP_DEFINE_CONST_FUN_OBJ_0(loadDefaultKanjiFont_obj, loadDefaultKanjiFont);
static MP_DEFINE_CONST_FUN_OBJ_0(loadDefaultAsciiFont_obj, loadDefaultAsciiFont);
static MP_DEFINE_CONST_FUN_OBJ_1(setDebugMode_obj, setDebugMode);
static MP_DEFINE_CONST_FUN_OBJ_1(getTypeName_obj, getTypeName);
static MP_DEFINE_CONST_FUN_OBJ_1(setRotation_obj, setRotation);
static MP_DEFINE_CONST_FUN_OBJ_1(displaySleep_obj, displaySleep);
//#pragma endregion
// 基本描画関数 static MP_DEFINE_CONST_FUN_OBJ_2(_obj, );


#pragma region 特殊描画関数
static MP_DEFINE_CONST_FUN_OBJ_1(invertDisplay_obj, invertDisplay);
static MP_DEFINE_CONST_FUN_OBJ_2(setScrollMargins_obj, setScrollMargins);
static MP_DEFINE_CONST_FUN_OBJ_1(scrollTo_obj, scrollTo);
#pragma endregion

#pragma region 基本描画関数
static MP_DEFINE_CONST_FUN_OBJ_1(fillScreen_obj, fillScreen);
static MP_DEFINE_CONST_FUN_OBJ_2(drawPixel_obj, drawPixel);
static MP_DEFINE_CONST_FUN_OBJ_3(drawFastHLine_obj, drawFastHLine);
static MP_DEFINE_CONST_FUN_OBJ_3(drawFastVLine_obj, drawFastVLine);
static MP_DEFINE_CONST_FUN_OBJ_2(fillRect_obj, fillRect);
static MP_DEFINE_CONST_FUN_OBJ_2(drawRect_obj, drawRect);
static MP_DEFINE_CONST_FUN_OBJ_2(drawLine_obj, drawLine);
static MP_DEFINE_CONST_FUN_OBJ_3(drawCircle_obj, drawCircle);
static MP_DEFINE_CONST_FUN_OBJ_3(fillCircle_obj, fillCircle);	
static MP_DEFINE_CONST_FUN_OBJ_3(drawRoundRect_obj, drawRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_3(fillRoundRect_obj, fillRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_2(drawTriangle_obj, drawTriangle);
static MP_DEFINE_CONST_FUN_OBJ_2(fillTriangle_obj, fillTriangle);
#pragma endregion

#pragma region テキスト描画関数
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(setCursor_obj, 1, 2, setCursor);
//static MP_DEFINE_CONST_FUN_OBJ_1(setCursor_obj, setCursor);
static MP_DEFINE_CONST_FUN_OBJ_1(print_obj, print);
static MP_DEFINE_CONST_FUN_OBJ_1(setTextWrap_obj, setTextWrap);
static MP_DEFINE_CONST_FUN_OBJ_1(setTextForegroundColor_obj, setTextForegroundColor);
static MP_DEFINE_CONST_FUN_OBJ_1(setTextBackgroundColor_obj, setTextBackgroundColor);
static MP_DEFINE_CONST_FUN_OBJ_2(setTextColor_obj, setTextColor);
static MP_DEFINE_CONST_FUN_OBJ_1(setTextSize_obj, setTextSize);
static MP_DEFINE_CONST_FUN_OBJ_1(setKanjiMode_obj, setKanjiMode);
static MP_DEFINE_CONST_FUN_OBJ_2(setKanjiFont_obj, setKanjiFont);
#pragma endregion

/*
static MP_DEFINE_CONST_FUN_OBJ_1(CreateGFX_obj, CreateGFX);
static MP_DEFINE_CONST_FUN_OBJ_0(doInit_obj, doInit);

static MP_DEFINE_CONST_FUN_OBJ_1(setTextWrap_obj, setTextWrap);
static MP_DEFINE_CONST_FUN_OBJ_1(setRotation_obj, setRotation);
static MP_DEFINE_CONST_FUN_OBJ_0(getWidth_obj, getWidth);
static MP_DEFINE_CONST_FUN_OBJ_0(getHeight_obj, getHeight);
// テキスト関連関数
static MP_DEFINE_CONST_FUN_OBJ_1(setAsciiFont_obj, setAsciiFont);
static MP_DEFINE_CONST_FUN_OBJ_0(getAvaiableAsciiFonts_obj, getAvaiableAsciiFonts);
static MP_DEFINE_CONST_FUN_OBJ_2(drawText_obj, drawText);
static MP_DEFINE_CONST_FUN_OBJ_2(drawTextKanji_obj, drawTextKanji);

// スクロール関連関数
static MP_DEFINE_CONST_FUN_OBJ_3(setScrollDefinition_obj, setScrollDefinition);
static MP_DEFINE_CONST_FUN_OBJ_1(verticalScroll_obj, verticalScroll);

// 基本描画関数 static MP_DEFINE_CONST_FUN_OBJ_2(_obj, );

static MP_DEFINE_CONST_FUN_OBJ_1(fillScreen_obj, fillScreen);
static MP_DEFINE_CONST_FUN_OBJ_2(fillRectangle_obj, fillRectangle);
static MP_DEFINE_CONST_FUN_OBJ_3(drawFastHLine_obj, drawFastHLine);
static MP_DEFINE_CONST_FUN_OBJ_3(drawFastVLine_obj, drawFastVLine);
static MP_DEFINE_CONST_FUN_OBJ_0(normalDisplay_obj, normalDisplay);
static MP_DEFINE_CONST_FUN_OBJ_2(drawPixel_obj, drawPixel);
static MP_DEFINE_CONST_FUN_OBJ_1(invertDisplay_obj, invertDisplay);

// 描画関数 static MP_DEFINE_CONST_FUN_OBJ_3(_obj, );
static MP_DEFINE_CONST_FUN_OBJ_2(drawRectWH_obj, drawRectWH);
static MP_DEFINE_CONST_FUN_OBJ_2(drawRect_obj, drawRect);
static MP_DEFINE_CONST_FUN_OBJ_2(fillRectWH_obj, fillRectWH);
static MP_DEFINE_CONST_FUN_OBJ_2(fillRect_obj, fillRect);
static MP_DEFINE_CONST_FUN_OBJ_3(drawCircle_obj, drawCircle);
static MP_DEFINE_CONST_FUN_OBJ_3(fillCircle_obj, fillCircle);
static MP_DEFINE_CONST_FUN_OBJ_3(drawRoundRectWH_obj, drawRoundRectWH);
static MP_DEFINE_CONST_FUN_OBJ_3(drawRountRect_obj, drawRountRect);
static MP_DEFINE_CONST_FUN_OBJ_3(fillRoundRectWH_obj, fillRoundRectWH);
static MP_DEFINE_CONST_FUN_OBJ_3(fillRoundRect_obj, fillRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_2(drawLine_obj, drawLine);

// ビットマップ関数 static MP_DEFINE_CONST_FUN_OBJ_3(_obj, );
static MP_DEFINE_CONST_FUN_OBJ_3(bmpDraw_obj, bmpDraw);
static MP_DEFINE_CONST_FUN_OBJ_3(registerBitmap_obj, registerBitmap);
static MP_DEFINE_CONST_FUN_OBJ_3(bmpRegDraw_obj, bmpRegDraw);
static MP_DEFINE_CONST_FUN_OBJ_1(bmpUseTransColor_obj, bmpUseTransColor);
static MP_DEFINE_CONST_FUN_OBJ_0(bmpUnuseTransColor_obj, bmpUnuseTransColor);
*/
// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t KNJGfx_globals_table[] = {
	// #pragma region 初期化・設定関数
	{MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_PTR(&initHW_obj)},
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
};

static MP_DEFINE_CONST_DICT(KNJGfx_module_globals, KNJGfx_globals_table);
const mp_obj_module_t mp_module_KNJGfx = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t*)&KNJGfx_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR_KNJGfx, mp_module_KNJGfx);

static const mp_rom_map_elem_t cppexample_module_globals_table[] = {
	{MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cppexample)},
	{MP_ROM_QSTR(MP_QSTR_cppfunc), MP_ROM_PTR(&cppfunc_obj)},
};

// モジュール定義

static MP_DEFINE_CONST_DICT(cppexample_module_globals, cppexample_module_globals_table);

// Define module object.
const mp_obj_module_t cppexample_user_cmodule = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t*)&cppexample_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_cppexample2, cppexample_user_cmodule);
