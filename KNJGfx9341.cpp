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
#include "pico/stdlib.h"

#include "pico/stdlib.h"
#include "pico/malloc.h"

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
#include "KanjiHelper.h"

// #include "Kanji/Fonts/misaki_gothic_2nd_08x08_ALL.inc"
#include "Kanji/Fonts/JF-Dot-Shinonome12_12x12_ALL.inc"
// #include "Kanji/Fonts/JF-Dot-Shinonome14_14x14_ALL.inc"
// #include "Kanji/Fonts/JF-Dot-Shinonome16_16x16_ALL.inc"
// #include "Kanji/Fonts/ipaexg_24x24_ALL.inc"

#include "pictBackground.data"  // 背景データ

void demoBitmap(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(60, 50);
		tft.print("透過色なしの描画");
		sleep_ms(1000);
		tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height());  // 背景データを描画
		const uint16_t* array[] = {sakura1, sakura2, sakura3, sakura4, sakura5, sakura6};
		sleep_ms(1000);
		for (int i = 0; i < 500; i++) {
			int idx = rand() % 6;
			int x = rand() % TFT_WIDTH;
			int y = rand() % TFT_HEIGHT;
			int w = 24;
			int h = 24;
			tft.drawRGBBitmap(x, y, array[idx], w, h);  // 背景データを描画
		}
		sleep_ms(1000);
	}

	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(60, 50);
		tft.print("透過色ありの描画");
		sleep_ms(1000);
		tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height(), (uint16_t)0x0001);  // 背景データを描画
		sleep_ms(1000);
		const uint16_t* array[] = {sakura1, sakura2, sakura3, sakura4, sakura5, sakura6};
		for (int i = 0; i < 500; i++) {
			int idx = rand() % 6;
			int x = rand() % TFT_WIDTH;
			int y = rand() % TFT_HEIGHT;
			int w = 24;
			int h = 24;
			tft.drawRGBBitmap(x, y, array[idx], w, h, 0x0000);  // 背景データを描画
		}
		sleep_ms(1000);
	}
	// 　キャンバスを使った透過画像の描画
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

		tft.setCursor(60, 50);
		tft.print("スプライトを使用した描画");
		sleep_ms(1000);
		tft.drawRGBBitmap(0, 0, pictSakura, tft.width(), tft.height(), (uint16_t)0x0001);  // 背景データを描画
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
		sleep_ms(1000);
	}

	{
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
}

void demoText(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	static const char* sampleText =
		"恥の多い生涯を送って来ました。自分には、人間の生活というものが、見当つかないのです。"
		"自分は東北の田舎に生れましたので、汽車をはじめて見たのは、よほど大きくなってからでした。"
		"自分は停車場のブリッジを、上って、降りて、そうしてそれが線路をまたぎ越えるために造られたものだという事には全然気づかず、"
		"ただそれは停車場の構内を外国の遊戯場みたいに、複雑に楽しく、ハイカラにするためにのみ、設備せられてあるものだとばかり思っていました。"
		"しかも、かなり永い間そう思っていたのです。ブリッジの上ったり降りたりは、自分にはむしろ、ずいぶん垢抜けのした遊戯で、"
		"それは鉄道のサーヴィスの中でも、最も気のきいたサーヴィスの一つだと思っていたのですが、のちにそれはただ旅客が線路を"
		"またぎ越えるための頗る実利的な階段に過ぎないのを発見して、にわかに興が覚めました。"
		"また、自分は子供の頃、絵本で地下鉄道というものを見て、これもやはり、実利的な必要から案出せられたものではなく、"
		"地上の車に乗るよりは、地下の車に乗ったほうが風がわりで面白い遊びだから、とばかり思っていました。"
		"自分は子供の頃から病弱で、よく寝込みましたが、寝ながら、敷布、枕のカヴァ、掛蒲団のカヴァを、つくづく、"
		"つまらない装飾だと思い、それが案外に実用品だった事を、二十歳ちかくになってわかって、人間のつましさに暗然とし、悲しい思いをしました。"
		"また、自分は、空腹という事を知りませんでした。いや、それは、自分が衣食住に困らない家に育ったという意味ではなく、"
		"そんな馬鹿な意味ではなく、自分には「空腹」という感覚はどんなものだか、さっぱりわからなかったのです。"
		"へんな言いかたですが、おなかが空いていても、自分でそれに気がつかないのです。"
		"小学校、中学校、自分が学校から帰って来ると、周囲の人たちが、それ、おなかが空いたろう、自分たちにも覚えがある、"
		"学校から帰って来た時の空腹は全くひどいからな、甘納豆はどう？カステラも、パンもあるよ、などと言って騒ぎますので、"
		"自分は持ち前のおべっか精神を発揮して、おなかが空いた、と呟いて、甘納豆を十粒ばかり口にほうり込むのですが、空腹感とは、"
		"どんなものだか、ちっともわかっていやしなかったのです。自分だって、それは勿論、大いにものを食べますが、"
		"しかし、空腹感から、ものを食べた記憶は、ほとんどありません。めずらしいと思われたものを食べます。"
		"豪華と思われたものを食べます。また、よそへ行って出されたものも、無理をしてまで、たいてい食べます。"
		"そうして、子供の頃の自分にとって、最も苦痛な時刻は、実に、自分の家の食事の時間でした。"
		"自分の田舎の家では、十人くらいの家族全部、めいめいのお膳を二列に向い合せに並べて、末っ子の自分は、"
		"もちろん一ばん下の座でしたが、その食事の部屋は薄暗く、昼ごはんの時など、十幾人の家族が、ただ黙々としてめしを食っている有様には、"
		"自分はいつも肌寒い思いをしました。それに田舎の昔気質の家でしたので、おかずも、たいていきまっていて、めずらしいもの、"
		"豪華なもの、そんなものは望むべくもなかったので、いよいよ自分は食事の時刻を恐怖しました。"
		"自分はその薄暗い部屋の末席に、寒さにがたがた震える思いで口にごはんを少量ずつ運び、押し込み、人間は、"
		"どうして一日に三度々々ごはんを食べるのだろう、実にみな厳粛な顔をして食べている、これも一種の儀式のようなもので、"
		"家族が日に三度々々、時刻をきめて薄暗い一部屋に集り、お膳を順序正しく並べ、食べたくなくても無言でごはんを噛かみながら、"
		"うつむき、家中にうごめいている霊たちに祈るためのものかも知れない、とさえ考えた事があるくらいでした。"
		"めしを食べなければ死ぬ、という言葉は、自分の耳には、ただイヤなおどかしとしか聞えませんでした。その迷信は、"
		"（いまでも自分には、何だか迷信のように思われてならないのですが）しかし、いつも自分に不安と恐怖を与えました。"
		"人間は、めしを食べなければ死ぬから、そのために働いて、めしを食べなければならぬ、という言葉ほど自分にとって難解で晦渋かいじゅうで、そうして脅迫めいた響きを感じさせる言葉は、無かったのです。";

	size_t length = 0;
	const char* str = sampleText;
	int textHCount = tft.width() / KanjiHelper::getKanjiWidth();
	int textVCount = tft.height() / KanjiHelper::getKanjiHeight();
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
	const uint16_t colorTbl[] = {STDCOLOR.SUPERDARK_RED, STDCOLOR.SUPERDARK_GREEN, STDCOLOR.SUPERDARK_BLUE, STDCOLOR.SUPERDARK_YELLOW,
								 STDCOLOR.SUPERDARK_CYAN, STDCOLOR.SUPERDARK_MAGENTA, STDCOLOR.SUPERDARK_GRAY, STDCOLOR.SUPERDARK_PINK};
	int64_t elapsed_time_1;
	int64_t elapsed_time_2;
	int64_t elapsed_time_3;
	{
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色
		tft.setCursor(60, 50);
		tft.print("透過色なし - Window非使用");
		sleep_ms(3000);

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
		tft.setCursor(60, 50);
		tft.print("透過色なし - Window使用");
		sleep_ms(3000);

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
		tft.setCursor(60, 50);
		tft.print("透過あり - Window使用不能");
		sleep_ms(3000);
		tft.useWindowMode(true);
		tft.fillScreen(STDCOLOR.BLACK);  // 背景色

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
	tft.printf("描画時間（透過なしー低速）: %4.3f ms\n", (float)elapsed_time_1/1000);  // 経過時間を表示
	tft.setCursor(10, 70);
	tft.printf("描画時間（透過なしー高速）: %4.3f ms\n", (float)elapsed_time_2 / 1000);  // 経過時間を表示
	tft.setCursor(10, 90);
	tft.printf("描画時間（透過あり）　　　: %4.3f ms\n", (float)elapsed_time_3 / 1000);  // 経過時間を表示

	for (int i=0;i<2000;i++) {
		if (i % 100 == 0) {
			tft.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色
			tft.setCursor(50, 200);
			tft.printf("タッチで次のデモに進む");
		} else if (i % 100 == 50) {
			tft.fillRect(0,200,tft.width(),20,STDCOLOR.BLACK);  // テキスト色
		}
		if (ts.touched() == true ) {
			break;
		}
		sleep_ms(30);
	}
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
}

void demoAlphabetText(Adafruit_ILI9341 tft, XPT2046_Touchscreen ts) {
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.setFont(&FreeSans12pt7b);  // 英文フォントを設定		
	tft.setTextColor(STDCOLOR.RED, STDCOLOR.BLUE);  // テキスト色
	tft.print("Hello, World");
}



Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);  // ILI9341ディスプレイのインスタンスを作成
XPT2046_Touchscreen ts(TOUCH_CS);

int main() {  // タッチパネルのインスタンスを作成

	long i = clockCyclesPerMicrosecond();

	stdio_init_all();

	// ヒープの空きメモリを取得
	// struct mallinfo info = mallinfo();
	// printf("ヒープの空きメモリ: %d bytes\n", info.fordblks);
	// printf("ヒープの使用メモリ: %d bytes\n", info.uordblks);

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
	// GFXcanvas16 canvas(tft.width(), tft.height());
	// GFXcanvas8 canvas8(tft.width(), tft.height());
	GFXcanvas16 canvas16(tft.width(), tft.height());
	// GFXcanvas1 canvas1(tft.width(), tft.height());

	// canvas1.setTextSize(1);  // テキストサイズを設定

	// タッチパネル初期設定
	ts.begin();                              // タッチパネル初期化
	ts.setRotation(TFTROTATION.ROTATE_270);  // タッチパネルの回転を設定（液晶画面と合わせる）

	// 液晶初期設定
	tft.setRotation(TFTROTATION.ROTATE_270);  // TFTの回転を設定（0-3）

	// 使用可能な場合は、ウインドウモードを使用して高速に描画する
	tft.useWindowMode(true);

	int cnt = 0;
	long total = 0;
	uint64_t time_1 = time_us_64();

	// tft.setFont(misaki_gothic_2nd_08x08_ALL, misaki_gothic_2nd_08x08_ALL_bitmap);  // 漢字フォントの設定
	tft.setFont(JFDotShinonome12_12x12_ALL, JFDotShinonome12_12x12_ALL_bitmap);  // 漢字フォントの設定
	// tft.setFont(JFDotShinonome16_16x16_ALL, JFDotShinonome16_16x16_ALL_bitmap);  // 漢字フォントの設定
	// tft.setFont(JFDotShinonome14_14x14_ALL, JFDotShinonome14_14x14_ALL_bitmap);
	// tft.setFont(ipaexg_24x24_ALL,ipaexg_24x24_ALL_bitmap);  // 漢字フォントの設定
	// tft.setFont(misaki_gothic_2nd_08x08_ALL, misaki_gothic_2nd_08x08_ALL_bitmap);  // 漢字フォントの設定
	tft.fillScreen(STDCOLOR.BLACK);  // 背景色
	tft.printf("こんにちは！ｺﾝﾆﾁﾊＡＢＣΔΣБ 123456789");
	while (1) {
		demoAlphabetText(tft,ts); 
		demoText(tft, ts);    // 背景データを描画
		demoBitmap(tft, ts);  // 背景データを描画
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

			canvas16.fillScreen(0x0000);  // 背景色

			canvas16.setCursor(0, 0);
			canvas16.setTextColor(STDCOLOR.WHITE, STDCOLOR.BLACK);  // テキスト色（文字色、背景色）※背景色は省略可
			// canvas16.setFont(JFDotShinonome16_16x16_ALL, JFDotShinonome16_16x16_ALL_bitmap);  // 漢字フォントの設定
			canvas16.printf("実行時間:%d tick/ 温度：%f\r\n", clkcycle, analogReadTemp(3.3));
			canvas16.setFont(&FreeSans18pt7b);  // フォント指定
			canvas16.setCursor(0, 100);
			canvas16.printf("Cycle:%d tick/ Temp:%f\r\n", clkcycle, analogReadTemp(3.3));

			// tft.drawBWBitmap(0, 0, canvas1.getBuffer(), canvas1.width(), canvas1.height());
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
