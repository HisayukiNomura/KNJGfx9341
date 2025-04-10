#include <stdio.h>
// https://logikara.blog/pico-ili9341/
#include <array>

#include "defines.h"
#include "Adafruit_GFX.h"
#include "defines.h"
#include "hardware/spi.h"
#include <string>
#include "lib-9341/core/WString.h"
#include "lib-9341/Adafruit_GFX_Library/Adafruit_GFX.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSans12pt7b.h"
#include "lib-9341/Adafruit_GFX_Library/Fonts/FreeSans18pt7b.h"
#include "lib-9341/Adafruit_ILI9341/Adafruit_ILI9341.h"
#include "lib-9341/XPT2046_Touchscreen/XPT2046_Touchscreen.h"  // タッチパネル制御用ライブラリ
#include "lib-9341/KNJGfx_struct.h"
#include "pico/stdlib.h"
#include "wiring_analog.h"
#include "WString.h"
#include "time.h"

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

std::array<int, 4> RandXYXY() {
	int x = rand() % TFT_WIDTH;
	int y = rand() % TFT_HEIGHT;
	int x2 = rand() % TFT_WIDTH;
	int y2 = rand() % TFT_HEIGHT;
	return {x, y, x2, y2};
}
#include "Adafruit_SPITFT.h"
// GFXcanvas16 canvas(TFT_WIDTH, TFT_HEIGHT);  // 16bitカラースプライト（オフスクリーンバッファ）

#define RANDXYWH rand() % TFT_WIDTH), (rand() % TFT_HEIGHT), (rand() % 100), (rand() % 100
	Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);  // ILI9341ディスプレイのインスタンスを作成
	XPT2046_Touchscreen ts(TOUCH_CS);
int main() {  // タッチパネルのインスタンスを作成

	long i = clockCyclesPerMicrosecond();

	stdio_init_all();

	/*
	String str = "Hello";
	String str2 = "World";
	String str3 = str + str2;

	char buf[100];
	itoa(123, buf, 10);
	String str4 = buf;
	*/

	SPI.setTX(TFT_MOSI);  // SPI0のTX(MOSI)
	SPI.setSCK(TFT_SCK);  // SPI0のSCK

	// TFT初期設定
	tft.begin();  // TFTを初期化
	// tft.setRotation(TFTROTATION.ROTATE_270);  // TFTの回転を設定（0-3）
	// GFXcanvas16 canvas(tft.width(), tft.height());
	GFXcanvas8 canvas8(tft.width(), tft.height());
	GFXcanvas16 canvas16(tft.width(), tft.height());
	GFXcanvas1 canvas1(tft.width(), tft.height());

	canvas1.setTextSize(1);  // テキストサイズを設定

	// タッチパネル初期設定
	ts.begin();                              // タッチパネル初期化
	ts.setRotation(TFTROTATION.ROTATE_270);  // タッチパネルの回転を設定（液晶画面と合わせる）

	int cnt = 0;
	long total = 0;
	uint64_t time_1 = time_us_64();

	while (1) {
		long clkcycle = clockCyclesPerMicrosecond();

		uint64_t time_2 = time_us_64();
		long time_d = (long)(time_2 - time_1);
		total = total + time_d;
		time_1 = time_2;
		cnt++;
		if (ts.touched() == true) {                                    // タッチされていれば
			TS_Point tPoint = ts.getPoint();                           // タッチ座標を取得
			int16_t x = (tPoint.x - 400) * TFT_WIDTH / (4095 - 550);   // タッチx座標をTFT画面の座標に換算
			int16_t y = (tPoint.y - 230) * TFT_HEIGHT / (4095 - 420);  // タッチy座標をTFT画面の座標に換算

			// 円の連続で線を描画
			tft.fillCircle(x, y, 2, 0x0A08);  // タッチ座標に塗り潰し円を描画
		} else {
			for (int i = 0; i < 100; i++) {
				std::array<int, 4> xyxy = RandXYXY();
				tft.drawLine(xyxy[0], xyxy[1], xyxy[2], xyxy[3], RANDCOLOR);  // ランダムな色で線を描画
			}

			canvas16.fillScreen(0x0000);        // 背景色

			canvas16.setFont(&FreeSans18pt7b);  // フォント指定
			canvas16.setKanjiFont(true);
			canvas16.setCursor(0, 0);
			canvas16.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色（文字色、背景色）※背景色は省略可
			canvas16.printf("実行時間:%d tick/ 温度：%f\r\n", clkcycle, analogReadTemp(3.3));
			canvas16.setKanjiFont(false);
			canvas16.setCursor(0, 100);
			canvas16.printf("Cycle:%d tick/ Temp:%f\r\n", clkcycle, analogReadTemp(3.3));
			//tft.drawBWBitmap(0, 0, canvas1.getBuffer(), canvas1.width(), canvas1.height());
			tft.drawRGBBitmap(0, 0, &canvas16);


			// tft.drawRGBBitmap(0, 0, canvas16.getBuffer(), canvas.width(), canvas.height());
			/*
			tft.fillScreen(STDCOLOR.BLACK);  // 背景色
			tft.setFont(&FreeSans12pt7b);    // フォント指定
			tft.setKanjiFont(true);
			tft.setCursor(10, 10);
			tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.DARK_BLUE);  // テキスト色（文字色、背景色）※背景色は省略可

			std::string str = "こんにちは、世界!";  // 表示内容
			String wstr = "ハロー！World!";  // WStringに変換
			tft.useWindowMode(true);
			tft.setTextSize(1, 1);
			tft.println(str);
			tft.println(wstr);
			tft.printf("実行時間:%d tick/ 温度：%f\r\n", clockCyclesPerMicrosecond(), analogReadTemp(3.3));
			*/
			/*

			tft.useWindowMode(false);
			tft.setTextSize(1, 1);
			tft.print("標準：ABC,漢字、ひらがな、ﾊﾝｶｸｶﾅ、〇×△、┏┳┓\r\n");
			tft.useWindowMode(false);
			tft.setTextSize(2, 2);
			tft.print("拡大：ABC,漢、ひら、ﾊﾝｶｸ、〇×、┏┓\r\n");
			tft.useWindowMode(true);
			tft.setTextSize(1, 1);
			tft.print("高速：ABC,漢字、ひらがな、ﾊﾝｶｸｶﾅ、〇×△、┏┳┓\r\n");
			float temp = analogReadTemp(3.3);     // 温度センサーの値を取得
			tft.print("温度：");
			tft.println(temp);
			long clkcycle = clockCyclesPerMicrosecond();
			tft.print("サイクル：");
			tft.print(clkcycle);
			tft.print("  HEX:");
			tft.println(clkcycle,16);
			tft.setKanjiFont(false);
			tft.print("Temp:");  // ASCII文字列を表示
			tft.println(temp);
			tft.print("Cycle:");
			tft.print(" HEX:");
			tft.println(clkcycle, 16);
			*/
			/*
			{
				tft.setTextSize(1, 1);
				clock_t start_time = clock();
				for (int j = 0; j < 100; j++) {
					tft.setCursor(0, 0);
					for (int i = 0; i < 100; i++) {
						tft.write((uint32_t)0xe38182);  // UTF-8で「あ」を表示
					}
				}
				clock_t end_time = clock();
				double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
				printf("標準モード経過時間: %.3f 秒\n", elapsed_time);
			}
			{
				tft.setTextSize(1, 1);
				tft.useWindowMode(true);
				clock_t start_time = clock();
				for (int j = 0; j < 100; j++) {
					tft.setCursor(0, 0);
					for (int i = 0; i < 100; i++) {
						tft.write((uint32_t)0xe38182);  // UTF-8で「あ」を表示
					}
				}
				clock_t end_time = clock();
				double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
				printf("高速モード経過時間: %.3f 秒\n", elapsed_time);
			}
			*/
			/*
				tft.setTextSize(2, 2);
				tft.write((uint32_t)0xe38184);  // UTF-8で「あ」を表示

	   canvas.fillScreen(0x0000);  // 背景色
	   // 文字表示
	   canvas.setCursor(48, 125);            // 表示座標指定
	   canvas.setTextColor(STDCOLOR.WHITE);  // テキスト色（文字色、背景色）※背景色は省略可
	   canvas.setFont(&FreeSans18pt7b);      // フォント指定

	   float temp = analogReadTemp(3.3);     // 温度センサーの値を取得
	   char buf[100];
	   canvas.println(ltoa(clkcycle, buf, 10));  // 表示内容
	   sprintf(buf, "こんにちは:%f", temp);
	   canvas.println(temp);  // 表示内容


	   // 実行時間
	   canvas.setFont(&FreeSans12pt7b);  // フォント指定
	   canvas.setCursor(0, 22);          // 表示座標指定
	   canvas.print(time_d);             // 経過時間をms単位で表示
	   tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);
	   DEBUGV("Tick:%d\r\n", time_d);
	   */
		}
	}
}
