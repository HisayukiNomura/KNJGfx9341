/// @brief このファイルは、MicropythonのC拡張モジュールとして使用する場合に有効にする。
/// ビルド時は、User C Moduleのcmakeファイルが存在する場所（ports/rp2/myModule等）に移動してビルドする。
// Include MicroPython API.
#include "py/runtime.h"

// Declare the function we'll make available in Python as cppexample.cppfunc().
mp_obj_t InitHW(mp_obj_t args);
/*
extern mp_obj_t CreateGFX(mp_obj_t a_pHW);
extern mp_obj_t doInit();
extern mp_obj_t setTextWrap(mp_obj_t a_tf);
extern mp_obj_t setRotation(mp_obj_t a_Rotation);
extern mp_obj_t getWidth();
extern mp_obj_t getHeight();
extern mp_obj_t normalDisplay();

extern mp_obj_t fillScreen(mp_obj_t a_color);
extern mp_obj_t fillRectangle(mp_obj_t a_xywh, mp_obj_t a_color);

// テキスト関連関数
extern mp_obj_t setAsciiFont(mp_obj_t a_fontname);
extern mp_obj_t getAvaiableAsciiFonts();
extern mp_obj_t drawText(mp_obj_t a_xyfbs, mp_obj_t a_str);
extern mp_obj_t drawTextKanji(mp_obj_t a_xyfbs, mp_obj_t a_str);

// スクロール関連関数
extern mp_obj_t setScrollDefinition(mp_obj_t a_top, mp_obj_t a_bottom, mp_obj_t a_direction);
extern mp_obj_t verticalScroll(mp_obj_t a_scrCnt);

// 基本描画関数
extern mp_obj_t drawFastHLine(mp_obj_t a_xy, mp_obj_t a_w, mp_obj_t a_c);
extern mp_obj_t drawFastVLine(mp_obj_t a_xy, mp_obj_t a_h, mp_obj_t a_c);
extern mp_obj_t drawPixel(mp_obj_t a_xy, mp_obj_t a_color);
extern mp_obj_t invertDisplay(mp_obj_t a_tf);

// 描画関数
extern mp_obj_t drawRectWH(mp_obj_t a_xywh, mp_obj_t a_color);
extern mp_obj_t drawRect(mp_obj_t a_xyxy, mp_obj_t a_color);
extern mp_obj_t fillRectWH(mp_obj_t a_xywh, mp_obj_t a_color);
extern mp_obj_t fillRect(mp_obj_t a_xyxy, mp_obj_t a_color);
extern mp_obj_t drawCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t fillCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t drawRoundRectWH(mp_obj_t a_xywh, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t drawRountRect(mp_obj_t a_xyxy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t fillRoundRectWH(mp_obj_t a_xywh, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t fillRoundRect(mp_obj_t a_xyxy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t drawLine(mp_obj_t a_xyxy, mp_obj_t a_color);

// ビットマップ関数
extern mp_obj_t bmpDraw(mp_obj_t a_xywh, mp_obj_t a_bmpData, mp_obj_t a_direction);
extern mp_obj_t registerBitmap(mp_obj_t a_idx, mp_obj_t a_wh, mp_obj_t a_bmpData);
extern mp_obj_t bmpRegDraw(mp_obj_t a_idx, mp_obj_t a_xy, mp_obj_t a_direction);
extern mp_obj_t bmpUseTransColor(mp_obj_t a_c);
extern mp_obj_t bmpUnuseTransColor();
*/
// その他
extern mp_obj_t cppfunc(mp_obj_t a_obj, mp_obj_t b_obj);
extern mp_obj_t getTypeName(mp_obj_t obj);