/*!
 * @file Adafruit_ILI9341.cpp
 *
 * #mainpage Adafruit ILI9341 TFT Displays
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's ILI9341 driver for the
 * Arduino platform.
 *
 * This library works with the Adafruit 2.8" Touch Shield V2 (SPI)
 *    http://www.adafruit.com/products/1651
 *
 * Adafruit 2.4" TFT LCD with Touchscreen Breakout w/MicroSD Socket - ILI9341
 *    https://www.adafruit.com/product/2478
 *
 * 2.8" TFT LCD with Touchscreen Breakout Board w/MicroSD Socket - ILI9341
 *    https://www.adafruit.com/product/1770
 *
 * 2.2" 18-bit color TFT LCD display with microSD card breakout - ILI9340
 *    https://www.adafruit.com/product/1770
 *
 * TFT FeatherWing - 2.4" 320x240 Touchscreen For All Feathers
 *    https://www.adafruit.com/product/3315
 *
 * These displays use SPI to communicate, 4 or 5 pins are required
 * to interface (RST is optional).
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries.
 * Modified by Hisayuki Nomura to support Japanese multibyte (aka. Kanji)
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */
#include "defines.h"

#ifdef STD_SDK
	#include "Adafruit_ILI9341.h"
	#include "misc/pins.h"
	#ifndef pgm_read_byte
		#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
	#endif
	#ifndef pgm_read_word
		#define pgm_read_word(addr) (*(const unsigned short *)(addr))
	#endif
	#ifndef pgm_read_dword
		#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
	#endif
using namespace ardPort;
#else
	#include "Adafruit_ILI9341.h"
	#ifndef ARDUINO_STM32_FEATHER
		#include "pins_arduino.h"
		#ifndef RASPI
			#include "wiring_private.h"
		#endif
	#endif
#endif

#include <limits.h>

#if defined(ARDUINO_ARCH_ARC32) || defined(ARDUINO_MAXIM)
	#define SPI_DEFAULT_FREQ 16000000
// Teensy 3.0, 3.1/3.2, 3.5, 3.6
#elif defined(__MK20DX128__) || defined(__MK20DX256__) || \
	defined(__MK64FX512__) || defined(__MK66FX1M0__)
	#define SPI_DEFAULT_FREQ 40000000
#elif defined(__AVR__) || defined(TEENSYDUINO)
	#define SPI_DEFAULT_FREQ 8000000
#elif defined(ESP8266) || defined(ESP32)
	#define SPI_DEFAULT_FREQ 40000000
#elif defined(RASPI)
	#define SPI_DEFAULT_FREQ 80000000
#elif defined(ARDUINO_ARCH_STM32F1)
	#define SPI_DEFAULT_FREQ 36000000
#else
	#define SPI_DEFAULT_FREQ 24000000  ///< Default SPI data clock frequency
#endif

#define MADCTL_MY 0x80   ///< Bottom to top
#define MADCTL_MX 0x40   ///< Right to left
#define MADCTL_MV 0x20   ///< Reverse Mode
#define MADCTL_ML 0x10   ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00  ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08  ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04   ///< LCD refresh right to left

/**************************************************************************/
/*!
	@brief  ソフトウェアSPIを使ってAdafruit ILI9341ドライバを初期化する
	@param    cs    チップセレクトピン番号
	@param    dc    データ/コマンドピン番号
	@param    mosi  SPI MOSIピン番号
	@param    sclk  SPIクロックピン番号
	@param    rst   リセットピン番号（省略可、未使用なら-1）
	@param    miso  SPI MISOピン番号（省略可、未使用なら-1）
*/
/**************************************************************************/
Adafruit_ILI9341::Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst, int8_t miso) :
	Adafruit_SPITFT(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, cs, dc, mosi, sclk, rst, miso) {
	bUseWindow = false;
}

/**************************************************************************/
/*!
	@brief  ハードウェアSPI（デフォルトSPI）を使ってAdafruit ILI9341ドライバを初期化する
	@param  cs   チップセレクトピン番号（GNDに接続している場合は-1でOK）
	@param  dc   データ/コマンドピン番号（必須）
	@param  rst  リセットピン番号（省略可、未使用なら-1）
*/
/**************************************************************************/
Adafruit_ILI9341::Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t rst) :
	Adafruit_SPITFT(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, cs, dc, rst) {
	bUseWindow = false;
}

#if !defined(ESP8266)
/**************************************************************************/
/*!
	@brief  特定のSPIクラスを使ってAdafruit ILI9341ドライバを初期化する
	@param  spiClass  SPIクラスへのポインタ（例: &SPIや&SPI1）
	@param  dc        データ/コマンドピン番号（必須）
	@param  cs        チップセレクトピン番号（省略可、未使用なら-1かつCSはGNDに接続）
	@param  rst       リセットピン番号（省略可、未使用なら-1）
*/
/**************************************************************************/
Adafruit_ILI9341::Adafruit_ILI9341(SPIClass *spiClass, int8_t dc, int8_t cs, int8_t rst) :
	Adafruit_SPITFT(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, spiClass, cs, dc, rst) {
	bUseWindow = false;
}
#endif  // end !ESP8266

/**************************************************************************/
/*!
	@brief  パラレルインターフェースを使ってAdafruit ILI9341ドライバを初期化する
	@param  busWidth  tft16の場合は16ビットインターフェース、それ以外は8ビット
	@param  d0        データピン0（PORTレジスタのLSBでバイトまたはワードアライン必須）
	@param  wr        ライトストローブピン番号（必須）
	@param  dc        データ/コマンドピン番号（必須）
	@param  cs        チップセレクトピン番号（省略可、未使用なら-1かつCSはGNDに接続）
	@param  rst       リセットピン番号（省略可、未使用なら-1）
	@param  rd        リードストローブピン番号（省略可、未使用なら-1）
*/
/**************************************************************************/
Adafruit_ILI9341::Adafruit_ILI9341(tftBusWidth busWidth, int8_t d0, int8_t wr,
								   int8_t dc, int8_t cs, int8_t rst, int8_t rd) :
	Adafruit_SPITFT(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, busWidth, d0, wr, dc,
					cs, rst, rd) {}

// clang-format off
static const uint8_t PROGMEM initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};
// clang-format on

/**************************************************************************/
/*!
	@brief   ILI9341チップを初期化する
	         SPI経由でILI9341に接続し、初期化コマンド列を送信する
	@param    freq  希望するSPIクロック周波数
*/
/**************************************************************************/
void Adafruit_ILI9341::begin(uint32_t freq) {
	if (!freq)
		freq = SPI_DEFAULT_FREQ;
	initSPI(freq);
	if (_rst < 0) {                    // If no hardware reset pin...
		sendCommand(ILI9341_SWRESET);  // Engage software reset
		delay(150);
	}

	uint8_t cmd, x, numArgs;
	const uint8_t *addr = initcmd;
	while ((cmd = pgm_read_byte(addr++)) > 0) {
		x = pgm_read_byte(addr++);
		numArgs = x & 0x7F;
		sendCommand(cmd, addr, numArgs);
		addr += numArgs;
		if (x & 0x80)
			delay(150);
	}

	_width = ILI9341_TFTWIDTH;
	_height = ILI9341_TFTHEIGHT;
}

/**************************************************************************/
/*!
	@brief   (0,0)を原点とし、TFTディスプレイの向きを設定する
	@param   m  回転のインデックス（0～3）
*/
/**************************************************************************/
void Adafruit_ILI9341::setRotation(uint8_t m) {
	rotation = m % 4;  // can't be higher than 3
	switch (rotation) {
		case 0:
			m = (MADCTL_MX | MADCTL_BGR);
			_width = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 1:
			m = (MADCTL_MV | MADCTL_BGR);
			_width = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
		case 2:
			m = (MADCTL_MY | MADCTL_BGR);
			_width = ILI9341_TFTWIDTH;
			_height = ILI9341_TFTHEIGHT;
			break;
		case 3:
			m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width = ILI9341_TFTHEIGHT;
			_height = ILI9341_TFTWIDTH;
			break;
	}

	sendCommand(ILI9341_MADCTL, &m, 1);
}

/**************************************************************************/
/*!
	@brief   ディスプレイの色反転を有効/無効にする
	@param   invert Trueで反転、Falseで通常色
*/
/**************************************************************************/
void Adafruit_ILI9341::invertDisplay(bool invert) {
	sendCommand(invert ? ILI9341_INVON : ILI9341_INVOFF);
}

/**************************************************************************/
/*!
	@brief   ディスプレイメモリをスクロールする
	@param   y スクロールするピクセル数
*/
/**************************************************************************/
void Adafruit_ILI9341::scrollTo(uint16_t y) {
	uint8_t data[2];
	data[0] = y >> 8;
	data[1] = y & 0xff;
	sendCommand(ILI9341_VSCRSADD, (uint8_t *)data, 2);
}

/**************************************************************************/
/*!
	@brief   上部・下部スクロールマージンの高さを設定する
	@param   top 上部スクロールマージンの高さ
	@param   bottom 下部スクロールマージンの高さ
 */
/**************************************************************************/
void Adafruit_ILI9341::setScrollMargins(uint16_t top, uint16_t bottom) {
	// TFA+VSA+BFA must equal 320
	if (top + bottom <= ILI9341_TFTHEIGHT) {
		uint16_t middle = ILI9341_TFTHEIGHT - (top + bottom);
		uint8_t data[6];
		data[0] = top >> 8;
		data[1] = top & 0xff;
		data[2] = middle >> 8;
		data[3] = middle & 0xff;
		data[4] = bottom >> 8;
		data[5] = bottom & 0xff;
		sendCommand(ILI9341_VSCRDEF, (uint8_t *)data, 6);
	}
}

/**************************************************************************/
/*!
	@brief   アドレスウィンドウ（次にRAMへ書き込む矩形領域）を設定する
	         ILI9341は各行が埋まるごとに自動的にデータをラップする
	@param   x1  TFTメモリの'x'開始座標
	@param   y1  TFTメモリの'y'開始座標
	@param   w   矩形領域の幅
	@param   h   矩形領域の高さ
*/
/**************************************************************************/
void Adafruit_ILI9341::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w,
									 uint16_t h) {
	static uint16_t old_x1 = 0xffff, old_x2 = 0xffff;
	static uint16_t old_y1 = 0xffff, old_y2 = 0xffff;

	uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);
	if (x1 != old_x1 || x2 != old_x2) {
		writeCommand(ILI9341_CASET);  // Column address set
		SPI_WRITE16(x1);
		SPI_WRITE16(x2);
		old_x1 = x1;
		old_x2 = x2;
	}
	if (y1 != old_y1 || y2 != old_y2) {
		writeCommand(ILI9341_PASET);  // Row address set
		SPI_WRITE16(y1);
		SPI_WRITE16(y2);
		old_y1 = y1;
		old_y2 = y2;
	}
	writeCommand(ILI9341_RAMWR);  // Write to RAM
}

/**************************************************************************/
/*!
	@brief  ILI9341の設定レジスタから8ビットデータを読み取る（RAMからではない）。
			この機能は公式にはあまり文書化・サポートされていないが、一応動作する。
	@param    commandByte  読み出すコマンドレジスタ
	@param    index  コマンド内のバイトインデックス
	@return   ILI9341レジスタから読み取った8ビットのデータ
 */
/**************************************************************************/
uint8_t Adafruit_ILI9341::readcommand8(uint8_t commandByte, uint8_t index) {
	// Indexレジスタを設定
	uint8_t data = 0x10 + index;
	sendCommand(0xD9, &data, 1);  // Set Index Register
	// 指定コマンドの値を読み出す
	return Adafruit_SPITFT::readcommand8(commandByte);
}

/**
 * @brief   サイズが１倍で、背景色が無く、ILI9341を使っているときは、文字の描画にウインドウを使用して高速に処理する
 * @param   x   描画開始位置の左上座標
 * @param   y   描画開始位置の左上座標
 * @param   w   描画するフォントビットマップの横ドット数
 * @param   h   描画するフォントビットマップの縦ドット数
 * @param   bmpData  描画するフォントビットマップデータ
 * @param   color 文字の前景色（565カラー）
 * @param   bg 文字の背景色（565カラー）　前景色と同じ色が指定されたら、透過表示とみなす。
 * @param   size_x 文字の横方向の拡大率
 * @param   size_y 文字の縦方向の拡大率
 * @return  None
 * @details このメソッドは、Adafruit_GFXの仮想関数をオーバーロードする。ILI9341の場合、ウインドウを指定してそこに画像の色を連続的に
 * 送ることで、毎回の座標指定をせずに描画できる。Adafruit_GFXの文字表示では、XY座標を指定し、毎回点を打っているのでそれを避けるために
 * ここでオーバーライドしている。
 * 実測で PICO_Wを使って１万文字表示で、Adafruitオリジナルで32秒、このルーチンで15秒と改善していると思われる。
 */
void Adafruit_ILI9341::drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmpData, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y) {
	if (bUseWindow == true &&  size_x == 1 && size_y == 1 && color != bg) {
		if ((x >= _width) || (y >= _height) || ((x + w * size_x - 1) < 0) || ((y + h * size_y - 1) < 0)) return;

		uint8_t w_bytes = (w + 8 - 1) / 8;   // 横方向のバイト数
		uint8_t h_bytes = h;                 // 縦方向のバイト数
		uint16_t bmpIdx = 0;                 // ビットマップ情報には、bmp + yy*w_bytes + xx でアクセスできるが、順番に並んでいるので最初から順に読むほうが速いのでは？
		bool isByteMultiple = (w % 8 == 0);  // 横幅が8の倍数かのフラグ。横１２ドットなどの場合は、８の倍数にならないので調整が必要になる

		startWrite();
		setAddrWindow(x, y, w, h);
		for (int8_t yy = 0; yy < h_bytes; yy++) {
			for (int8_t xx = 0; xx < w_bytes; xx++) {
				uint8_t bitCnt;
				if (isByteMultiple) {
					bitCnt = 8;
				} else {
					bitCnt = (xx != (w_bytes - 1)) ? 8 : (w % 8);
				}
				uint8_t bits = bmpData[bmpIdx];
				for (int8_t bb = 0; bb < bitCnt; bb++) {
					if (bits & 0x80) {
						pushColor(color);
					} else {
						pushColor(bg);
					}
					bits <<= 1;
				}
				bmpIdx++;
			}
		}
	} else {
		/// 拡大フォントや背景色が透過の場合にはウインドウでの描画は使用できないので基底クラスのdrawCharを呼び出す
		Adafruit_GFX::drawChar(x, y, w, h, bmpData, color, bg, size_x, size_y);
	}
}