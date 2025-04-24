﻿#include <stdio.h>
// https://logikara.blog/pico-ili9341/
#include <array>
#include <malloc.h>

#include "defines.h"
#include "Adafruit_GFX.h"
#include "defines.h"
#include "hardware/spi.h"
#include "pico/malloc.h"

#include <string>
#include "lib-9341/core/WString.h"
#include "lib-9341/Adafruit_GFX_Library/Adafruit_GFX.h"

// 使用するフォントだけをインクルードする
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSans12pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSansOblique9pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSans9pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSerif9pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSerifItalic9pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeMono9pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeMonoOblique9pt7b.h"

#include "lib-9341/Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "lib-9341/XPT2046_Touchscreen/XPT2046_Touchscreen.h"  // タッチパネル制御用ライブラリ
#include "lib-9341/KNJGfx_struct.h"
#include "pico/stdlib.h"
#include "wiring_analog.h"
#include "WString.h"
#include "time.h"
#include "pico/stdlib.h"

#include "pico/stdlib.h"
#include "pico/malloc.h"

#include "hardware/spi.h"

/*
// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for
information on GPIO assignments #define SPI_PORT spi0 #define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
*/

#define TFT_WIDTH 320   // 画面幅
#define TFT_HEIGHT 240  // 画面高さ

#define TFT_SCK 18   // 液晶表示の SCK
#define TFT_MOSI 19  // 液晶表示の MOSI
#define TFT_DC 20    // 液晶画面の DC
#define TFT_RST 21   // 液晶画面の RST
#define TFT_CS 22    // 液晶画面の CS

#define TOUCH_MISO 16  // タッチパネルの MISO
#define TOUCH_CS 17    // タッチパネルの CS

using namespace ardPort;
using namespace ardPort::spi;

#include "Adafruit_SPITFT.h"

#include "KanjiHelper.h"

// #include "Kanji/Fonts/misaki_gothic_2nd_08x08_ALL.inc"
#include "Kanji/Fonts/JF-Dot-Shinonome12_12x12_ALL.inc"
// #include "Kanji/Fonts/JF-Dot-Shinonome14_14x14_ALL.inc"
// #include "Kanji/Fonts/JF-Dot-Shinonome16_16x16_ALL.inc"
// #include "Kanji/Fonts/ipaexg_24x24_ALL.inc"

#include "pictBackground.data"  // 背景データ

int getTextByteLen(Adafruit_ILI9341 tft, const char* Text, int width = 0, int height = 0) {
	if (width == 0) width = tft.width();
	if (height == 0) height = tft.height();

	size_t length = 0;
	const char* str = sampleText;
	int textHCount = width / KanjiHelper::getKanjiWidth();
	int textVCount = height / KanjiHelper::getKanjiHeight();
	int textCount = textHCount * textVCount;
	int byteLen = 0;
	while (*str) {
		if ((*str & 0xC0) != 0x80) {  // UTF-8の先頭バイトを検出
			length++;
			if (length >= textCount) {
				byteLen = str - sampleText;
				break;
			}
		}
		str++;
	}
	return byteLen;
}

bool waitForTouchOrTimeout(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts, int timeout_ms, const char* message = nullptr) {
	absolute_time_t start_time = get_absolute_time();  // 開始時間を取得

	int count = 0;
	int touchCount = 0;
	while (true) {
		if (message != nullptr) {
			//struct mallinfo info = mallinfo();
			//tft.setCursor(0, 0);
			//tft.printf("Heap : %d used/ %d free", info.fordblks, info.uordblks);

			if (count % 200 == 0) {
				tft.setCursor(10, 220);
				tft.printf(message);
			} else if (count % 100 == 0) {
				tft.fillRect(0, 220, tft.width(), 20, STDCOLOR.BLACK);
			}
			count += 1;
		}

		if (ts.touched()) {
			touchCount++;
			TS_Point tPoint = ts.getPoint();                           // タッチ座標を取得
			int16_t x = (tPoint.x - 400) * TFT_WIDTH / (4095 - 550);   // タッチx座標をTFT画面の座標に換算
			int16_t y = (tPoint.y - 230) * TFT_HEIGHT / (4095 - 420);  // タッチy座標をTFT画面の座標に換算

			// 円の連続で線を描画
			tft.drawCircle(x, y, touchCount * 2, rand() % 0xFFFF);  // タッチ座標に塗り潰し円を描画
			if (touchCount == 10) {									// 10回タッチしたら、押されたと判断するのだが・・・
				int waitRelease = 0;
				while (ts.touched()) {
					if (waitRelease > 1000) break;
					sleep_ms(10);  // CPU負荷を下げるために少し待機
					waitRelease++;
				}
				break; 
			}
		} else {
			touchCount = 0;
		}
		if (absolute_time_diff_us(start_time, get_absolute_time()) > timeout_ms * 1000) {
			return true;  // タイムアウトしたらtrueを返す
		}
		sleep_ms(10);  // CPU負荷を下げるために少し待機
	}
	return false;  // タイムアウトせずにループを抜けたらfalseを返す
}

void demoBitmap(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // 文字色、背景色
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(10, 20);
		tft.print("３種類のビットマップ表示方法");

		tft.setCursor(10, 60);
		tft.print("DMA、透過なしソフトウェア、透過付きソフトウェアの３種類の方法で、画面全体に画像を表示して速度を確認します。\n\n透過表示を行わない場合位置指定の回数を減らせることができるので無駄がありません。\n\n透過色を指定すると描画するすべてのピクセルで位置指定が必要になります");

		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		int64_t elapsed_time_1;
		int64_t elapsed_time_2;
		int64_t elapsed_time_3;

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(60, 50);
		tft.print("DMAで全画面画面更新");
		sleep_ms(1000);
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		tft.drawDMABitmap(pictSakura);
		absolute_time_t end_time = get_absolute_time();                // 終了時間を取得
		elapsed_time_1 = absolute_time_diff_us(start_time, end_time);  // 経過時間を計算

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(60, 50);
		tft.print("ソフトウェアで全画面画面更新");
		sleep_ms(1000);
		start_time = get_absolute_time();                                // 開始時間を取得
		tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height());  // 背景データを描画
		end_time = get_absolute_time();                                  // 終了時間を取得
		elapsed_time_2 = absolute_time_diff_us(start_time, end_time);    // 経過時間を計算

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(60, 50);
		tft.print("透過つきで全画面更新");
		sleep_ms(1000);
		start_time = get_absolute_time();                                        // 開始時間を取得
		tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height(), 0x0001);  // 背景データを描画
		end_time = get_absolute_time();                                          // 終了時間を取得
		elapsed_time_3 = absolute_time_diff_us(start_time, end_time);            // 経過時間を計算
		sleep_ms(1000);

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(10, 50);
		tft.printf("描画時間（DMA）: %4.3f ms\n", (float)elapsed_time_1 / 1000);  // 経過時間を表示
		tft.setCursor(10, 70);
		tft.printf("描画時間（SPI-高速）: %4.3f ms\n", (float)elapsed_time_2 / 1000);  // 経過時間を表示
		tft.setCursor(10, 90);
		tft.printf("描画時間（SPI-低速）: %4.3f ms\n", (float)elapsed_time_3 / 1000);  // 経過時間を表示

		tft.setCursor(10, 180);
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	}
	{
		int64_t elapsed_time_1;
		int64_t elapsed_time_2;
		int64_t elapsed_time_3;
		{
			tft.fillScreen(STDCOLOR.BLACK);  // 背景色

			tft.setCursor(10, 20);
			tft.print("Canvas16(スプライト）の使用");
			tft.setCursor(10, 60);
			tft.print("24x24の画像を500回、GFXcanvas16クラス（RAMメモリ上の描画領域、スプライト）を使用して描画します。\n\n透過色無し、ありでの速度の違いと動作の確認ができます。\n\n３つ目の例は、描画前に描画エリアの背景画像をスプライトとして保持し、描画後に背景画像を掻き戻します。いわゆる「バッキングストア」です。");
			waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
			{
				tft.fillScreen(STDCOLOR.BLACK);  // 背景色

				tft.setCursor(60, 50);
				tft.print("透過色なしの描画");
				sleep_ms(1000);
				tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height());  // 背景データを描画
				const uint16_t* array[] = {sakura1, sakura2, sakura3, sakura4, sakura5, sakura6};
				GFXcanvas16* sakura[6];  // 花びら描画用のスプライト
				for (int i = 0; i < 6; i++) {
					sakura[i] = new GFXcanvas16(24, 24);  // 16bitカラースプライト（オフスクリーンバッファ）
					sakura[i]->drawRGBBitmap(0, 0, array[i % 6], 24, 24);
				}
				sleep_ms(1000);
				absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
				for (int i = 0; i < 500; i++) {
					int idx = rand() % 6;
					int x = rand() % TFT_WIDTH;
					int y = rand() % TFT_HEIGHT;
					int w = 24;
					int h = 24;
					tft.drawRGBBitmap(x, y, sakura[i % 6]);
				}
				elapsed_time_1 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
				sleep_ms(1000);
				for (int i = 0; i < 6; i++) {
					delete sakura[i];
				}
			}

			{
				tft.fillScreen(STDCOLOR.BLACK);  // 背景色

				tft.setCursor(60, 50);
				tft.print("透過色ありの描画");
				sleep_ms(1000);
				tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height());  // 背景データを描画
				const uint16_t* array[] = {sakura1, sakura2, sakura3, sakura4, sakura5, sakura6};
				GFXcanvas16* sakura[6];  // 花びら描画用のスプライト
				for (int i = 0; i < 6; i++) {
					int idx = i % 6;
					sakura[i] = new GFXcanvas16(24, 24);  // 16bitカラースプライト（オフスクリーンバッファ）
					sakura[i]->drawRGBBitmap(0, 0, array[idx], 24, 24);
					sakura[i]->useTransparentColor(0x0000);  // 透過色を指定
				}
				sleep_ms(1000);
				absolute_time_t start_time = get_absolute_time();  // 開始時間を取得

				for (int i = 0; i < 500; i++) {
					int idx = rand() % 6;
					int x = rand() % TFT_WIDTH;
					int y = rand() % TFT_HEIGHT;
					int w = 24;
					int h = 24;
					tft.drawRGBBitmap(x, y, sakura[idx]);
				}
				elapsed_time_2 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
				sleep_ms(1000);
				for (int i = 0; i < 6; i++) {
					delete sakura[i];
				}
			}
			// 　キャンバスを使った透過画像の描画
			{
				tft.fillScreen(STDCOLOR.BLACK);  // 背景色

				tft.setCursor(60, 50);
				tft.print("バッキングストアを使用した描画");
				sleep_ms(1000);
				tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height());  // 背景データを描画
				sleep_ms(1000);

				GFXcanvas16 canvas(24, 24);    // 花びら描画用のスプライト
				GFXcanvas16* pCanvasBack[20];  // 花びらの背景を保存しておく為のスプライト
				for (int i = 0; i < 20; i++) {
					pCanvasBack[i] = new GFXcanvas16(24, 24);  // 16bitカラースプライト（オフスクリーンバッファ）
				}
				const uint16_t* array[] = {sakura1, sakura2, sakura3, sakura4, sakura5, sakura6};

				int16_t sakuraX[20];
				int16_t sakuraY[20];

				for (int i = 0; i < 20; i++) {
					sakuraX[i] = sakuraY[i] = -1;
				}
				absolute_time_t start_time = get_absolute_time();  // 開始時間を取得

				for (int j = 0; j < 500; j++) {
					for (int i = 0; i < 20; i++) {
						if (sakuraX[i] == -1 && sakuraY[i] == -1) {
							if (rand() % 10 != 0) continue;
							sakuraX[i] = rand() % TFT_WIDTH;
							sakuraY[i] = 0;
							pCanvasBack[i]->copyRGBBitmap(sakuraX[i], sakuraY[i], 24, 24, pictSakura, tft.width(), tft.height());  // キャンバスに背景画像をコピー
						}
						int dx = (rand() % 6) - 3;
						int dy = (rand() % 3) + 2;
						tft.drawRGBBitmap(sakuraX[i], sakuraY[i], pCanvasBack[i]);
						if (sakuraY[i] + dy > TFT_HEIGHT) {
							sakuraX[i] = sakuraY[i] = -1;
							continue;
						}
						sakuraX[i] += dx;
						sakuraY[i] += dy;
						pCanvasBack[i]->copyRGBBitmap(sakuraX[i], sakuraY[i], 24, 24, pictSakura, tft.width(), tft.height());  // キャンバスに花びらの移動先部分の背景画像を、背景の全景からコピー
						canvas = *pCanvasBack[i];                                                                              // キャンバスに背景画像をコピー
						canvas.drawRGBBitmap(0, 0, array[j % 6], 24, 24, 0x0000);                                              // キャンバスに桜の画像を描画
						tft.drawRGBBitmap(sakuraX[i], sakuraY[i], &canvas);                                                    // キャンバスを液晶に描画
					}
				}
				elapsed_time_3 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
				for (int i = 0; i < 20; i++) {
					delete pCanvasBack[i];
				}
				sleep_ms(1000);
			}
			tft.fillScreen(STDCOLOR.BLACK);  // 背景色
			tft.setCursor(10, 50);
			tft.printf("描画時間（透過なし）: %4.3f ms\n", (float)elapsed_time_1 / 1000);  // 経過時間を表示
			tft.setCursor(10, 70);
			tft.printf("描画時間（透過あり）: %4.3f ms\n", (float)elapsed_time_2 / 1000);  // 経過時間を表示
			tft.setCursor(10, 90);
			tft.printf("描画時間（背景保存）: %4.3f ms\n", (float)elapsed_time_3 / 1000);  // 経過時間を表示

			tft.setCursor(10, 180);
			waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
		}
	}

	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(10, 20);
		tft.print("発色の確認");
		tft.setCursor(10, 60);
		tft.print("画面には、320x200の、色合いの異なる４枚の画像が順番に表示されます。タッチで画面が変わります。");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		const char* pictMsg[] = {"ビットマップ表示のデモンストレーション",
								 "タッチで画面が変わります　　　　　　　",
								 "drawRGBBitmapを使っています　　　　　",
								 "透過色無しの高速描画です　　　　　　　"};
		const uint16_t* array[] = {pictFuji1, pictFuji2, pictFuji3, pictFuji4};
		int idx = 0;
		int msIdx = 0;
		tft.setCursor(0, 208);
		tft.setTextColor(STDCOLOR.WHITE);        // テキスト色
		tft.setBackgroundColor(STDCOLOR.BLACK);  // 背景色
		tft.print(pictMsg[idx % 4]);

		tft.drawRGBBitmap(0, 0, array[idx % 4], tft.width(), 200);  // 背景データを描画
		while (idx < 20) {
			if (ts.touched() == true || msIdx > 3000) {
				idx++;
				tft.drawRGBBitmap(0, 0, array[idx % 4], tft.width(), 200);  // 背景データを描画
				msIdx = 0;
				tft.setCursor(0, 208);
				tft.print(pictMsg[idx % 4]);

				while (ts.touched() == false) {
					sleep_ms(10);
					if (ts.touched() == false) break;
				}
			}
			sleep_ms(1);
			msIdx++;
		}
	}
	sleep_ms(1000);
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
}

void demoText(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色

	int byteLen = getTextByteLen(tft, sampleText);

	const uint16_t colorTbl[] = {STDCOLOR.SUPERDARK_RED, STDCOLOR.SUPERDARK_GREEN, STDCOLOR.SUPERDARK_BLUE, STDCOLOR.SUPERDARK_YELLOW,
								 STDCOLOR.SUPERDARK_CYAN, STDCOLOR.SUPERDARK_MAGENTA, STDCOLOR.SUPERDARK_GRAY, STDCOLOR.SUPERDARK_PINK};
	int64_t elapsed_time_1;
	int64_t elapsed_time_2;
	int64_t elapsed_time_3;
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(10, 20);
		tft.print("透過なし描画（オリジナル）");
		tft.setCursor(10, 60);
		tft.print("画面に表示可能な日本語の文字を表示します。\n\n透過無しのため、文字の背景は黒として描画され、背景の画像は消えます。\n\n文字はそのドットのすべてが個別に描画されるため低速です。Adafruitのオリジナルの表示方法を踏襲したモードです");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.useWindowMode(false);

		for (int i = 0; i < 8; i++) {
			tft.fillRect(0, i * (tft.height() / 8), tft.width(), 30, colorTbl[i]);  // 背景色
		}
		tft.setCursor(0, 0);  // 描画開始位置を指定

		tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		tft.write(sampleText, byteLen);
		absolute_time_t end_time = get_absolute_time();                // 終了時間を取得
		elapsed_time_1 = absolute_time_diff_us(start_time, end_time);  // 経過時間を計算
		sleep_ms(1000);
	}
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(10, 20);
		tft.print("透過なし描画（ウインドウ指定）");
		tft.setCursor(10, 60);
		tft.print("画面に表示可能な日本語の文字を表示します。\n\n透過無しのため、文字の背景は黒として描画され、背景の画像は消えます。\n\n文字はその大きさの矩形をウインドウとして定義し、そこに連続して画素の色を流し込みます。\n一般的に、オリジナルよりも高速です");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.useWindowMode(true);

		for (int i = 0; i < 8; i++) {
			tft.fillRect(0, i * (tft.height() / 8), tft.width(), 30, colorTbl[i]);  // 背景色
		}
		tft.setCursor(0, 0);  // 描画開始位置を指定

		tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		tft.write(sampleText, byteLen);
		absolute_time_t end_time = get_absolute_time();                // 終了時間を取得
		elapsed_time_2 = absolute_time_diff_us(start_time, end_time);  // 経過時間を計算
		sleep_ms(1000);
	}
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(10, 20);
		tft.print("透過あり描画");
		tft.setCursor(10, 60);
		tft.print(
			"画面に表示可能な日本語の文字を表示します。\n\n透過ありのため、文字の背景には影響されません。\n\nこの方法ではウインドウは使用できず画素一つ一つを個別に表示します。\n一般的に文字は描画が必要な部分よりも描画の必要がない部分のほうが少ないため、透過表示を行うほうが高速です。\n\n"
			"理論上、事前に文字の表示対象となる部分をfillRectで塗りつぶすなどの方法を使用すればより高速化できますが、改行などがあるため、文字が表示される部分を事前に正確に予測するのも難しいため実装していません。");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		for (int i = 0; i < 8; i++) {
			tft.fillRect(0, i * (tft.height() / 8), tft.width(), 30, colorTbl[i]);  // 背景色
		}
		tft.setCursor(0, 0);                               // 描画開始位置を指定
		tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.WHITE);  // テキスト色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		tft.write(sampleText, byteLen);
		absolute_time_t end_time = get_absolute_time();                // 終了時間を取得
		elapsed_time_3 = absolute_time_diff_us(start_time, end_time);  // 経過時間を計算
		sleep_ms(1000);
	}
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setCursor(10, 50);
	tft.printf("描画時間（透過なしー低速）: %4.3f ms\n", (float)elapsed_time_1 / 1000);  // 経過時間を表示
	tft.setCursor(10, 70);
	tft.printf("描画時間（透過なしー高速）: %4.3f ms\n", (float)elapsed_time_2 / 1000);  // 経過時間を表示
	tft.setCursor(10, 90);
	tft.printf("描画時間（透過あり）　　　: %4.3f ms\n", (float)elapsed_time_3 / 1000);  // 経過時間を表示

	tft.setCursor(10, 180);
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
}

void demoAlphabetText(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色

	tft.setCursor(10, 20);

	tft.print("英文の表示");
	tft.setCursor(10, 60);
	tft.print("この部分は、オリジナルのAdafruitのコードそのままで実装されています。英文字は、フットプリントが小さいので複数のフォントをプログラムに同時に組み込むことも現実的に可能です。\n\nこのデモでは、複数のフォントを切り替えて画面に表示しています。\n\nフォントが指定されない場合、システムに組み込まれている5x7ドットの文字が表示されます。。\n\n");
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	tft.setKanjiMode(false);  // 日本語モードをOFFにする

	const char* text = "Jackdaws love my big sphinx of quartz";
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setFont(&FreeSerif9pt7b);    // 英文フォントを設定
	tft.setTextColor(STDCOLOR.RED);  // フォント指定モードについては、背景色は指定できない。（仕様）
	tft.setCursor(0, 20);
	tft.print(text);
	tft.setFont(&FreeSerifItalic9pt7b);  // 英文フォントを設定
	tft.setTextColor(STDCOLOR.RED);      // フォント指定モードについては、背景色は指定できない。（仕様）
	tft.setCursor(0, 40);
	tft.print(text);
	tft.setFont(&FreeSans9pt7b);       // 英文フォントを設定
	tft.setTextColor(STDCOLOR.GREEN);  // フォント指定モードについては、背景色は指定できない。（仕様）
	tft.setCursor(0, 60);
	tft.print(text);
	tft.setFont(&FreeSansOblique9pt7b);  // 英文フォントを設定
	tft.setTextColor(STDCOLOR.GREEN);    // フォント指定モードについては、背景色は指定できない。（仕様）
	tft.setCursor(0, 80);
	tft.print(text);
	tft.setFont(&FreeMono9pt7b);      // 英文フォントを設定
	tft.setTextColor(STDCOLOR.BLUE);  // フォント指定モードについては、背景色は指定できない。（仕様）
	tft.setCursor(0, 100);
	tft.print(text);
	tft.setFont(&FreeMonoOblique9pt7b);  // 英文フォントを設定
	tft.setTextColor(STDCOLOR.BLUE);     // フォント指定モードについては、背景色は指定できない。（仕様）
	tft.setCursor(0, 140);
	tft.print(text);

	tft.setFont(NULL);
	tft.setCursor(0, 160);
	tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLUE);  // 組み込みフォントの場合は、背景色を指定できる。
	tft.print(text);

	tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // 組み込みフォントの場合は、背景色を指定できる。
	tft.setCursor(10, 220);
	tft.setTextSize(2);
	tft.write("Tap screen to proceed");
	waitForTouchOrTimeout(tft, ts, 10000);
	tft.setTextSize(2);
	tft.setKanjiMode(true);
}

void demoShape(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色

	tft.setCursor(10, 20);
	tft.print("図形などの描画");
	tft.setCursor(10, 60);
	tft.print("直線や矩形、塗りつぶしなどを実行します。基本的に、Adafruitのコードそのままです。\n\n時間を測定しますが、この時間には描画時間の他に、乱数による描画位置や色の決定時間も含まれます。");
	tft.print("順番に、点、直線、水平線、垂直線、矩形、角丸矩形、直線や矩形、塗りつぶしなどを実行します。基本的に、Adafruitのコードそのままです。\n\n時間を測定しますが、この時間には描画時間の他に、乱数による描画位置や色の決定時間も含まれます。");

	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

	int64_t elapsed_time_1;
	int64_t elapsed_time_2;
	int64_t elapsed_time_3;
	int64_t elapsed_time_4;
	int64_t elapsed_time_5;
	int64_t elapsed_time_6;
	int64_t elapsed_time_7;
	int64_t elapsed_time_8;
	int64_t elapsed_time_9;
	int64_t elapsed_time_10;
	int64_t elapsed_time_11;
	int64_t elapsed_time_12;
	int64_t elapsed_time_13;

	{                                                      // 点
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 10000; i++) {
			tft.drawPixel(rand() % tft.width(), rand() % tft.height(), rand() & 0xFFFF);
		}
		elapsed_time_1 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 直線
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XY xy0;
			XY xy1;
			xy0.setRandom(tft.width(), tft.height());
			xy1.setRandom(tft.width(), tft.height());
			tft.drawLine(xy0, xy1, rand() & 0xFFFF);
		}
		elapsed_time_2 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}

	{                                                      // 水平線
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			tft.drawFastHLine(0, rand() % tft.height(), tft.width(), rand() & 0xFFFF);
		}
		elapsed_time_3 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 垂直線
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			tft.drawFastVLine(rand() % tft.width(), 0, tft.height(), rand() & 0xFFFF);
		}
		elapsed_time_4 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 矩形
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XYWH xywh;
			xywh.setRandom(tft.width(), tft.height());
			tft.drawRect(xywh, rand() & 0xFFFF);
		}
		elapsed_time_5 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 角丸矩形
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XYWH xywh;
			while (1) {
				xywh.setRandom(tft.width(), tft.height());
				if (xywh.w <= 10 || xywh.h < 10) continue;
				break;
			}
			tft.drawRoundRect(xywh, 5, rand() & 0xFFFF);
		}
		elapsed_time_6 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 三角形
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XY xy0;
			XY xy1;
			XY xy2;
			xy0.setRandom(tft.width(), tft.height());
			xy1.setRandom(tft.width(), tft.height());
			xy2.setRandom(tft.width(), tft.height());
			tft.drawTriangle(xy0, xy1, xy2, rand() & 0xFFFF);
		}
		elapsed_time_7 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}

	{                                                      // 円
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XY xy;
			uint8_t r = 0;
			while (1) {
				r = rand() % 20;
				xy.setRandom(tft.width(), tft.height());
				if (xy.x + r >= tft.width() || xy.y + r >= tft.height()) continue;
				if (xy.x - r < 0 || xy.y - r < 0) continue;
				break;
			}
			tft.drawCircle(xy, r, rand() & 0xFFFF);
		}
		elapsed_time_8 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 塗りつぶし矩形
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XYWH xywh;
			xywh.setRandom(tft.width(), tft.height());
			tft.fillRect(xywh, rand() & 0xFFFF);
		}
		elapsed_time_9 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 塗りつぶし角丸矩形
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XYWH xywh;
			while (1) {
				xywh.setRandom(tft.width(), tft.height());
				if (xywh.w <= 10 || xywh.h < 10) continue;
				break;
			}
			tft.fillRoundRect(xywh, 5, rand() & 0xFFFF);
		}
		elapsed_time_10 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	{                                                      // 塗りつぶし三角形
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XY xy0;
			XY xy1;
			XY xy2;
			xy0.setRandom(tft.width(), tft.height());
			xy1.setRandom(tft.width(), tft.height());
			xy2.setRandom(tft.width(), tft.height());
			tft.fillTriangle(xy0, xy1, xy2, rand() & 0xFFFF);
		}
		elapsed_time_11 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}

	{                                                      // 塗りつぶし円
		tft.fillScreen(STDCOLOR.BLACK);                    // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 1000; i++) {
			XY xy;
			uint8_t r = 0;
			while (1) {
				r = rand() % 20;
				xy.setRandom(tft.width(), tft.height());
				if (xy.x + r >= tft.width() || xy.y + r >= tft.height()) continue;
				if (xy.x - r < 0 || xy.y - r < 0) continue;
				break;
			}
			tft.fillCircle(xy, r, rand() & 0xFFFF);
		}
		elapsed_time_12 = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
	}
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setCursor(0, 10);
	tft.print("一回当たりの描画時間（描画位置や色の決定時間を含む）\n\n");
	int cnt = 0;
	for (int y = 34; y < 240; y += 12) {
		tft.fillRect(0, y, tft.width(), 12, ((cnt % 2) == 0) ? STDCOLOR.SUPERDARK_GRAY : STDCOLOR.BLACK);  // 背景色
		cnt++;
	}
	tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.WHITE);                                     // テキスト色
	tft.printf("　点　　　　　　　: %8.4f μS　　　　\n", (float)elapsed_time_1 / 10000);  // 経過時間を表示
	tft.printf("　直線　　　　　　: %8.4f μS　　　　\n", (float)elapsed_time_2 / 1000);   // 経過時間を表示
	tft.printf("　水平線　　　　　: %8.4f μS　　　　\n", (float)elapsed_time_3 / 1000);   // 経過時間を表示
	tft.printf("　垂直線　　　　　: %8.3f μS　　　　\n", (float)elapsed_time_4 / 1000);   // 経過時間を表示
	tft.printf("　矩形　　　　　　: %8.3f μS　　　　\n", (float)elapsed_time_5 / 1000);   // 経過時間を表示
	tft.printf("　角丸矩形　　　　: %8.3f μS　　　　\n", (float)elapsed_time_6 / 1000);   // 経過時間を表示
	tft.printf("　三角形　　　　　: %8.3f μS　　　　\n", (float)elapsed_time_7 / 1000);   // 経過時間を表示
	tft.printf("　円　　　　　　　: %8.3f μS　　　　\n", (float)elapsed_time_8 / 1000);   // 経過時間を表示
	tft.printf("　矩形（塗）　　　: %8.3f μS　　　　\n", (float)elapsed_time_9 / 1000);   // 経過時間を表示
	tft.printf("　角丸矩形（塗）　: %8.3f μS　　　　\n", (float)elapsed_time_10 / 1000);  // 経過時間を表示
	tft.printf("　三角形（塗）　　: %8.3f μS　　　　\n", (float)elapsed_time_11 / 1000);  // 経過時間を表示
	tft.printf("　円（塗）　　　　: %8.3f μS　　　　\n", (float)elapsed_time_12 / 1000);  // 経過時間を表示

	tft.setCursor(10, 180);
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
}

void demoCanvas(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setCursor(10, 20);
	tft.print("キャンバスによるフリッカーレス表示");

	tft.setCursor(10, 60);
	// キャンバス用のバッファ。ヒープは使用せず、このスタック上に確保する。
	// やはり、組み込み系のデバイスでヒープを使うのはどうも気持ちが悪いので。
	uint16_t screenbuffer[320 * 240];

	tft.print("GFXcanvas16にテキストを描画し、これを一括で画面に転送することで、文字を表示するときのちらつき（フリッカー）を押さえることができます。");
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	{
		GFXcanvas16* pCanvas = new GFXcanvas16(tft.width(), tft.height() / 2,false);                              // 画面半分分のキャンバスの作成
		pCanvas->setBuffer(screenbuffer);                                                                      // バッファの設定
		pCanvas->fillScreen(STDCOLOR.DARK_BLUE);                                                             // 背景色
		pCanvas->setTextColor(STDCOLOR.WHITE, STDCOLOR.DARK_BLUE);                                           // テキスト色
		pCanvas->setFont(JFDotShinonome12_12x12_ALL, JFDotShinonome12_12x12_ALL_bitmap);                     // 漢字フォントの設定
		pCanvas->setCursor(30, 0);
		pCanvas->print("キャンバス経由で描画");

		absolute_time_t start_time = get_absolute_time();     // 開始時間を取得
		tft.fillScreen(STDCOLOR.DARK_RED);                    // 背景色
		tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.DARK_RED);  // テキスト色
		tft.setCursor(30, 0);
		tft.print("直接描画");
		int idx = 0;
		while (true) {
			absolute_time_t end_time = get_absolute_time();                       // 終了時間を取得
			uint32_t elapsed_time = absolute_time_diff_us(start_time, end_time);  // 経過時間を計算
			if (elapsed_time > 20000000) break;

			tft.setCursor(0, 40);  // 描画開始位置を指定
			tft.write(sampleText + idx, 396);

			pCanvas->setCursor(0, 40);  // 描画開始位置を指定
			pCanvas->write(sampleText + idx, 396);
			tft.drawRGBBitmap(0, 120, pCanvas);

			idx++;
			if (idx >= 1000) idx = 0;
		}
		delete pCanvas;
	}
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setCursor(10, 20);
	tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色
	tft.print("パフォーマンス測定");
	tft.setCursor(10, 60);
	tft.print("画面の大きさでキャンバスを作成し、それを１００回表示して速度を測定します。");
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

	uint32_t elapsed_time;
	{
		GFXcanvas16 canvas16(tft.width(), tft.height(),false);
		canvas16.setBuffer(screenbuffer);
		canvas16.fillScreen(STDCOLOR.DARK_BLUE);           // 背景色
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		for (int i = 0; i < 100; i++) {
			tft.drawRGBBitmap(0, 0, &canvas16);
		}
		elapsed_time = absolute_time_diff_us(start_time, get_absolute_time());
	}
	tft.fillScreen(STDCOLOR.BLACK);
	tft.setCursor(10, 50);
	tft.printf("全画面更新にかかる時間: %4.3f ms\n", ((float)elapsed_time / 1000) / 100);  // 経過時間を表示

	tft.setCursor(10, 180);
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
}

void demoScrollEtc(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setCursor(10, 20);
	tft.print("その他の機能");

	tft.setCursor(10, 60);
	tft.print("スクロールなどの機能を実行します。");
	waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	// 画面のローテーション
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(10, 20);
		tft.print("画面のローテーション");

		tft.setCursor(10, 60);
		tft.print("画面を回転させます。");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	
		const char* text[] = { "TFTROTATION.NORMAL",  "TFTROTATION.ROTATE_90", "TFTROTATION.ROTATE_180", "TFTROTATION.ROTATE_270" };
		tft.setFont(&FreeSerif9pt7b);  // 英文フォントを設定
		tft.setKanjiMode(false);
		for (int i = 0; i < 4; i++) {
			tft.setRotation(i);
			tft.fillScreen(STDCOLOR.BLACK);  // 背景色
			tft.setCursor(10, 20);
			tft.printf(text[i]);
			tft.setCursor(30, 50);
			tft.printf(".width() = %4d\n", tft.width());
			tft.setCursor(30, 70);
			tft.printf(".height() = %4d\n", tft.height());

			sleep_ms(3000);
		}
		tft.setKanjiMode(true);
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	}

	// 画面の反転
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(10, 20);
		tft.print("画面を反転させます。");

		tft.setCursor(10, 60);
		tft.print("画面のちらつきが苦手な人は、何もせずに放置してください。");

		bool isTimeout = waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		if (isTimeout == false) {
			static uint16_t colorBar[] = {STDCOLOR.GRAY, STDCOLOR.WHITE, STDCOLOR.YELLOW, STDCOLOR.CYAN, STDCOLOR.GREEN, STDCOLOR.MAGENTA, STDCOLOR.RED, STDCOLOR.BLUE};
			for (int i = 0; i < 8; i++) {
				tft.fillRect(i * 40, 0, 40, 160, colorBar[i]);
				sleep_ms(100);
			}
			for (int i = 0; i < 8; i++) {
				tft.fillRect(i * 40, 160, 40, 40, colorBar[8 - i]);
				sleep_ms(100);
			}
			tft.setCursor(0, 210);
			tft.print("あんぱん、めろんぱん、クリームパン、チョコレートパン");
			sleep_ms(1000);
			absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
			int invertCnt = 300;
			int dir = -5;
			while (1) {
				uint32_t elapsed_time = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
				if (elapsed_time > 20000000) break;
				sleep_ms(invertCnt);
				tft.invertDisplay(true);
				sleep_ms(invertCnt);
				tft.invertDisplay(false);
				invertCnt += 1;
				if (invertCnt < 10 || invertCnt > 1000) {
					dir = -dir;
				}
			}
		}
	}
 
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(10, 20);
		tft.print("縦画面でのスクロール");

		tft.setCursor(10, 60);
		tft.print("縦画面でスクロールさせます。画面の上と下には固定のスクロールしない領域があります。");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		tft.setRotation(TFTROTATION.NORMAL);
		int byteLen = getTextByteLen(tft, sampleText, 0, tft.height() - 40);

		tft.fillScreen(STDCOLOR.BLACK);                                    // 背景色
		tft.drawRGBBitmap(0, 0, topBar, tft.width(), 20);                  // 上部固定領域
		tft.drawRGBBitmap(0, tft.height() - 20, btmBar, tft.width(), 20);  // 下部固定領域
		tft.setScrollMargins(20, 20);

		tft.setCursor(0, 20);
		tft.write(sampleText, byteLen);  // テキストを描画

		tft.setCursor(80, tft.height() - 14);
		tft.print("スクロール位置: ");  // スクロール位置を表示
		tft.setTextColor(STDCOLOR.WHITE, 0x6C3F);
		for (int i = 20; i < tft.height() - 40; i++) {
			tft.scrollTo(i);
			sleep_ms(10);
			tft.setCursor(200, tft.height() - 14);
			tft.printf("%3d", i);  // スクロール位置を表示
		}
		for (int i = 20; i < tft.height() - 40; i++) {
			tft.scrollTo(tft.height() - 40 - i);
			sleep_ms(10);
			tft.setCursor(200, tft.height() - 14);
			tft.printf("%3d", i);  // スクロール位置を表示
		}

		tft.scrollTo(0);
		tft.setScrollMargins(0, 0);
		tft.setRotation(TFTROTATION.ROTATE_270);
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	}

	// 横画面でスクロール
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(10, 20);
		tft.print("横画面でのスクロール");

		tft.setCursor(10, 60);
		tft.print("横画面でスクロールさせます。画面の右側をタッチすると右に、左側をタッチすると左にスクロールします。中央をタッチすると停止します。");
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");

		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(0, 208);
		tft.setTextColor(STDCOLOR.WHITE);                        // テキスト色
		tft.setBackgroundColor(STDCOLOR.BLACK);                  // 背景色
		tft.drawRGBBitmap(0, 0, pictScroll1, tft.width(), 240);  // 背景データを描画

		//		tft.setScrollMargins(16,16);
		tft.scrollTo(0);
		int waitms = 30;
		int pos = 0;
		int8_t dir = -1;
		absolute_time_t start_time = get_absolute_time();  // 開始時間を取得
		while (1) {
			uint32_t elapsed_time = absolute_time_diff_us(start_time, get_absolute_time());  // 経過時間を計算
			if (elapsed_time > 20000000) break;

			if (ts.touched()) {
				TS_Point p = ts.getPoint();
				int16_t x = (p.x - 400) * TFT_WIDTH / (4095 - 550);   // タッチx座標をTFT画面の座標に換算
				int16_t y = (p.y - 230) * TFT_HEIGHT / (4095 - 420);  // タッチy座標をTFT画面の座標に換算
				if (x > 200) {
					if (dir == -1) {                                     // 同じ方向に動いていたら、待ち時間を減らして加速
						waitms = (waitms != 3) ? (waitms - 1) : waitms;  // 10ms 以上は早くしない
					} else if (dir == 1) {                               // 逆方向に動いてたら、待ち時間を増やして減速
						waitms = waitms != 30 ? waitms + 1 : waitms;     // 30ms 以下は遅くしない
						if (waitms == 30) dir = 0;
					} else {
						dir = -1;  // 逆方向に動いてたら、待ち時間を増やして減速
						waitms = 30;
					}
				} else if (x < 120) {
					if (dir == 1) {                                   // 同じ方向に動いていたら、待ち時間を減らして加速
						waitms = waitms != 3 ? waitms - 1 : waitms;   // 10ms 以上は早くしない
					} else if (dir == -1) {                           // 逆方向に動いてたら、待ち時間を増やして減速
						waitms = waitms != 30 ? waitms + 1 : waitms;  // 30ms 以下は遅くしない
						if (waitms == 30) dir = 0;
					} else {
						dir = 1;  // 逆方向に動いてたら、待ち時間を増やして減速
						waitms = 30;
					}
				} else {  // 即時停止
					dir = 0;
					waitms = 30;
				}
			}

			pos += dir;
			if (pos >= tft.width()) {
				pos = 0;
			} else if (pos <= 0) {
				pos = tft.width();
			}
			sleep_ms(waitms);
			tft.scrollTo(pos);
		}
		tft.setCursor(10, 180);
		tft.scrollTo(0);
		waitForTouchOrTimeout(tft, ts, 10000, "<< タッチで次へ >>");
	}

}

int main() {  // タッチパネルのインスタンスを作成
	Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);  // ILI9341ディスプレイのインスタンスを作成
	XPT2046_Touchscreen ts(TOUCH_CS);
	long i = clockCyclesPerMicrosecond();

	stdio_init_all();

	SPI.setTX(TFT_MOSI);  // SPI0のTX(MOSI)
	SPI.setSCK(TFT_SCK);  // SPI0のSCK

	// TFT初期設定
	tft.begin();  // TFTを初期化

	// タッチパネル初期設定
	ts.begin();                              // タッチパネル初期化
	ts.setRotation(TFTROTATION.ROTATE_270);  // タッチパネルの回転を設定（液晶画面と合わせる）
	// 液晶初期設定
	tft.setRotation(TFTROTATION.ROTATE_270);  // TFTの回転を設定（0-3）
	// 使用可能な場合は、ウインドウモードを使用して高速に描画する
	tft.useWindowMode(true);

	// tft.setFont(misaki_gothic_2nd_08x08_ALL, misaki_gothic_2nd_08x08_ALL_bitmap);  // 漢字フォントの設定
	
	tft.setFont(JFDotShinonome12_12x12_ALL, JFDotShinonome12_12x12_ALL_bitmap);  // 漢字フォントの設定
	// tft.setFont(JFDotShinonome16_16x16_ALL, JFDotShinonome16_16x16_ALL_bitmap);  // 漢字フォントの設定
	// tft.setFont(JFDotShinonome14_14x14_ALL, JFDotShinonome14_14x14_ALL_bitmap);
	// tft.setFont(ipaexg_24x24_ALL,ipaexg_24x24_ALL_bitmap);  // 漢字フォントの設定
	// tft.setFont(misaki_gothic_2nd_08x08_ALL, misaki_gothic_2nd_08x08_ALL_bitmap);  // 漢字フォントの設定
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	uint32_t elapsed_time = 0;
	while (1) {
		demoShape(tft, ts);      // 図形描画
		demoBitmap(tft, ts);     // 背景データを描画
		demoAlphabetText(tft, ts);
		demoText(tft, ts);    // 背景データを描画
		demoCanvas(tft, ts);  // キャンバスによるフリッカーレス表示
		demoScrollEtc(tft, ts);  // スクロールなどの機能を実行
	}
}
