/// @brief このファイルは、MicropythonのC拡張モジュールとして使用する場合に有効にする。
/// ビルド時は、User C Moduleのcmakeファイルが存在する場所（ports/rp2/myModule等）に移動してビルドする。
// Include MicroPython API.
#include "py/runtime.h"

// Declare the function we'll make available in Python as cppexample.cppfunc().


#pragma region 初期化・設定関数
extern mp_obj_t initHW(mp_obj_t args);
extern mp_obj_t getTypeName(mp_obj_t obj);
extern mp_obj_t setDebugMode(mp_obj_t a_mode);
extern mp_obj_t loadDefaultKanjiFont();
extern mp_obj_t loadDefaultAsciiFont();
extern mp_obj_t setRotation(mp_obj_t a_Rotation);
extern mp_obj_t displaySleep(mp_obj_t a_tf);
#pragma endregion

#pragma region 特殊描画関数
extern mp_obj_t invertDisplay(mp_obj_t a_tf);
extern mp_obj_t setScrollMargins(mp_obj_t a_top, mp_obj_t a_bottom);
extern mp_obj_t scrollTo(mp_obj_t a_y);
#pragma endregion

#pragma region 基本描画関数
extern mp_obj_t fillScreen(mp_obj_t a_color);
extern mp_obj_t drawPixel(mp_obj_t a_xy, mp_obj_t a_color);
extern mp_obj_t drawFastHLine(mp_obj_t a_xy, mp_obj_t a_w, mp_obj_t a_c);
extern mp_obj_t drawFastVLine(mp_obj_t a_xy, mp_obj_t a_h, mp_obj_t a_c);
extern mp_obj_t fillRect(mp_obj_t a_xyxy, mp_obj_t a_color);
extern mp_obj_t drawRect(mp_obj_t a_xyxy, mp_obj_t a_color);
extern mp_obj_t drawLine(mp_obj_t a_xyxy, mp_obj_t a_color);
extern mp_obj_t drawCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t fillCircle(mp_obj_t a_xy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t drawRoundRect(mp_obj_t a_xyxy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t fillRoundRect(mp_obj_t a_xyxy, mp_obj_t a_r, mp_obj_t a_color);
extern mp_obj_t drawTriangle(mp_obj_t a_xyxy, mp_obj_t a_color);
extern mp_obj_t fillTriangle(mp_obj_t a_xyxy, mp_obj_t a_color);
#pragma endregion

#pragma region テキスト描画関数
//extern mp_obj_t setCursor(mp_obj_t a_xy);
extern mp_obj_t setCursor(size_t n_args, const mp_obj_t* args);
extern mp_obj_t print(mp_obj_t a_str);
extern mp_obj_t setTextWrap(mp_obj_t a_tf);
extern mp_obj_t setTextForegroundColor(mp_obj_t a_color);
extern mp_obj_t setTextBackgroundColor(mp_obj_t a_color);
extern mp_obj_t setTextColor(mp_obj_t a_color, mp_obj_t a_bgcolor);
extern mp_obj_t setTextSize(mp_obj_t a_size);
extern mp_obj_t setKanjiMode(mp_obj_t a_tf);
extern mp_obj_t setKanjiFont(mp_obj_t a_code, mp_obj_t a_bitmap);

// その他
extern mp_obj_t cppfunc(mp_obj_t a_obj, mp_obj_t b_obj);
