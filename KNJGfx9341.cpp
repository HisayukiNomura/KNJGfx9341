#include <stdio.h>
// https://logikara.blog/pico-ili9341/
#include <array>

#include "Adafruit_GFX.h"
#include "defines.h"
#include "hardware/spi.h"
#include "lib-9341/Adafruit_GFX_Library/Adafruit_GFX.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSans12pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSans18pt7b.h"
#include "lib-9341/Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "lib-9341/KNJGfx_struct.h"
#include "pico/stdlib.h"
#include "pins_arduino.h"
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

// ILI9341ディスプレイのインスタンスを作成
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

// スプライト（メモリ描画領域から一括表示）をcanvasとして準備
// 画面表示をtftではなくcanvasで指定して一括描画することでチラツキなく表示できる
GFXcanvas16 canvas(
	TFT_WIDTH, TFT_HEIGHT);  // 16bitカラースプライト（オフスクリーンバッファ）

#define RANDCOLOR (rand() % 0xFFFF)

std::array<int, 4> RandXYWH() {
	int x = rand() % TFT_WIDTH;
	int y = rand() % TFT_HEIGHT;
	int x2 = rand() % TFT_WIDTH;
	int y2 = rand() % TFT_HEIGHT;
	if (x > x2) std::swap(x, x2);
	if (y > y2) std::swap(y, y2);
	int w = abs(x2 - x);
	int h = abs(y2 - y);
	if (w == 0) w = 1;
	if (h == 0) h = 1;

	return {x, y, w, h};
}

#define RANDXYWH rand() % TFT_WIDTH), (rand() % TFT_HEIGHT), (rand() % 100), (rand() % 100
int main() {
	SPI.setTX(TFT_MOSI);  // SPI0のTX(MOSI)
	SPI.setSCK(TFT_SCK);  // SPI0のSCK

	// TFT初期設定
	tft.begin();            // TFTを初期化
	tft.setRotation(3);     // TFTの回転を設定（0-3）
	canvas.setTextSize(1);  // テキストサイズを設定
	int cnt = 0;
	long total = 0;
	uint64_t time_1 = time_us_64();

	while (1) {
		uint64_t time_2 = time_us_64();
		long time_d = (long)(time_2 - time_1);
		total = total + time_d;
		time_1 = time_2;
		cnt++;

		canvas.fillScreen(0x0000);  // 背景色
		// 文字表示
		canvas.setCursor(48, 125);  // 表示座標指定
		canvas.setTextColor(
			STDCOLOR.WHITE);              // テキスト色（文字色、背景色）※背景色は省略可
		canvas.setFont(&FreeSans18pt7b);  // フォント指定
		canvas.println("Test");           // 表示内容

		// 実行時間
		canvas.setFont(&FreeSans12pt7b);  // フォント指定
		canvas.setCursor(0, 22);          // 表示座標指定
		canvas.print(time_d);             // 経過時間をms単位で表示
		tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);
		for (int i = 0; i < 1000; i++) {
			std::array<int, 4> resxywh;
			resxywh = RandXYWH();
			tft.drawRect(resxywh[0], resxywh[1], resxywh[2], resxywh[3], RANDCOLOR);
			resxywh = RandXYWH();
			tft.fillRect(resxywh[0], resxywh[1], resxywh[2], resxywh[3], RANDCOLOR);
		}
	}
}
