/*!
 * @file Adafruit_ILI9341.h
 *
 * This is the documentation for Adafruit's ILI9341 driver for the
 * Arduino platform.
 *
 * This library works with the Adafruit 2.8" Touch Shield V2 (SPI)
 *    http://www.adafruit.com/products/1651
 * Adafruit 2.4" TFT LCD with Touchscreen Breakout w/MicroSD Socket - ILI9341
 *    https://www.adafruit.com/product/2478
 * 2.8" TFT LCD with Touchscreen Breakout Board w/MicroSD Socket - ILI9341
 *    https://www.adafruit.com/product/1770
 * 2.2" 18-bit color TFT LCD display with microSD card breakout - ILI9340
 *    https://www.adafruit.com/product/1770
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
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef _ADAFRUIT_ILI9341H_
#define _ADAFRUIT_ILI9341H_
#include "../misc/defines.h"

#ifdef STD_SDK
#include "../Adafruit_GFX_Library/Adafruit_GFX.h"
#include "../misc/PortingCommon.h"
#include "../core/Print.h"
#include "../Adafruit_GFX_Library/Adafruit_SPITFT.h"
#include "../spi/SPI.h"
#else
#include "Adafruit_GFX.h"
#include "Arduino.h"
#include "Print.h"
#include <Adafruit_SPITFT.h>
#include <SPI.h>
#endif

#ifdef STD_SDK
namespace ardPort
{
#endif

#define ILI9341_TFTWIDTH 240  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 320 ///< ILI9341 max TFT height

#define ILI9341_NOP 0x00	 ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04	 ///< Read display identification information
#define ILI9341_RDDST 0x09	 ///< Read Display Status

#define ILI9341_SLPIN 0x10	///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12	///< Partial Mode ON
#define ILI9341_NORON 0x13	///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A		///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B	///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C	///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D	///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20	  ///< Display Inversion OFF
#define ILI9341_INVON 0x21	  ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29	  ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30	  ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36	  ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A	  ///< COLMOD: Pixel Format Set

#define ILI9341_IFCTRL 0xF6 /// Interface Control

#define ILI9341_FRMCTR1 \
	0xB1					 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 \
	0xB3					 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4	 ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction
// #define ILI9341_PWCTR6     0xFC

// Color definitions
#define ILI9341_BLACK 0x0000	   ///<   0,   0,   0
#define ILI9341_NAVY 0x000F		   ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF	   ///<   0, 125, 123
#define ILI9341_MAROON 0x7800	   ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F	   ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0	   ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF	   ///< 123, 125, 123
#define ILI9341_BLUE 0x001F		   ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0	   ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF		   ///<   0, 255, 255
#define ILI9341_RED 0xF800		   ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F	   ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0	   ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF	   ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20	   ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18		   ///< 255, 130, 198

	enum ILI9341_GAMMA
	{
		GAMMA_CURVE_1 = 0x01,
		GAMMA_CURVE_2 = 0x02,
		GAMMA_CURVE_3 = 0x04,
		GAMMA_CURVE_4 = 0x08,
	};

	enum ILI9341_IFCTRL_DM
	{
		DM_INTERNALCLOCK = 0x00,
		DM_RGBINTERFACE = 0x01,
		DM_VSYNCINTERFACE = 0x02,
		DM_SETTINGDISABLE = 0x04,
	};
	enum ILI9341_IFCTRL_RM
	{
		RM_SYSIF = 0x00,
		RM_RGBIF = 0x01,
	};
	enum ILI9341_IFCTRL_ENDIAN
	{
		ENDIAN_BIG = 0x00,
		ENDIAN_LITTLE = 0x01,
	};
	enum ILI9341_IFCTRL_WEMODE
	{
		WEMODE_IGNORE = 0x00,
		WEMODE_FOLLOWING = 0x01,
	};
	enum ILI9341_IFCTRL_RIM
	{
		RIM_RGB18OR16BIT = 0,
		RIM_RBG6BIT = 1,
	};
	/*!
	  @brief
		ILI9341コントローラ搭載TFTディスプレイ用のドライバクラス。

	  @details
		Adafruit_ILI9341クラスは、Adafruit_SPITFTクラスを継承し、ILI9341コントローラを搭載した
		各種カラーTFTディスプレイ（SPI/パラレル接続対応）を制御するための機能を提供します。
		このクラスは、ディスプレイの初期化、描画、スクロール、回転、色反転、アドレスウィンドウ設定など
		ILI9341特有の制御コマンドをラップし、簡単に扱えるようにしています。

		Adafruit_ILI9341はAdafruit_SPITFTを継承し、さらにAdafruit_SPITFTはAdafruit_GFXを継承しているため、
		Adafruit_GFXで提供される全てのグラフィックスAPI（drawPixel, drawLine, drawRect, drawBitmap, setCursor, print等）が
		Adafruit_ILI9341のインスタンスでもそのまま利用できます。
		これは、Adafruit_GFXの仮想関数を本クラスや親クラスで実装しているため、すべての描画命令が
		ILI9341ディスプレイ固有の処理に適切に転送される仕組みになっているためです。

		また、SPIやパラレルの低レベル制御、DMA転送、ピクセル単位の高速描画、バッファ管理など
		ディスプレイ制御に必要な共通機能も利用できます。
		これにより、アプリケーション側はAdafruit_GFXのAPIを使うだけで、ILI9341ディスプレイを
		簡単かつ高速に制御できます。
	*/
	class Adafruit_ILI9341 : public Adafruit_SPITFT
	{
	private:
		bool bUseWindow;

	public:
		Adafruit_ILI9341(); // デフォルトコンストラクタ
		void constructObject(int8_t cs, int8_t dc, int8_t mosi, int8_t sck, int8_t rst = -1, int8_t miso = -1);
		void constructObject(SPIClass *spiClass, int8_t dc, int8_t cs = -1, int8_t rst = -1);

		Adafruit_ILI9341(int8_t _CS, int8_t _DC, int8_t _MOSI, int8_t _SCLK,
						 int8_t _RST = -1, int8_t _MISO = -1);
		Adafruit_ILI9341(int8_t _CS, int8_t _DC, int8_t _RST = -1);
#if !defined(ESP8266)
		Adafruit_ILI9341(SPIClass *spiClass, int8_t dc, int8_t cs = -1, int8_t rst = -1);
#endif // end !ESP8266
		Adafruit_ILI9341(tftBusWidth busWidth, int8_t d0, int8_t wr, int8_t dc,
						 int8_t cs = -1, int8_t rst = -1, int8_t rd = -1);

		void begin(uint32_t freq = 0);
		void setRotation(uint8_t r);
		void invertDisplay(bool i);
		void scrollTo(uint16_t y);
		void setScrollMargins(uint16_t top, uint16_t bottom);
		/**
		 * @brief 漢字の描画にWindow機能を使うかのフラグ
		 */
		void useWindowMode(bool tf)
		{
			bUseWindow = tf;
		}

		// virtual void drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t* bmpData, uint16_t color, uint16_t bg, uint8_t size);
		virtual void drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmpData, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);

		// Transaction API not used by GFX
		void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

		void displaySleep(bool enterSleep);
		void setGamma(ILI9341_GAMMA gamma);
		void set3DGammaEnable(bool onoff);
		void setPositiveGamma(uint8_t *);
		void setNegativeGamma(uint8_t *);
		// void setDigitalGamma(uint8_t*);  Not implemented.
		void setIFControl(ILI9341_IFCTRL_WEMODE weMode, uint8_t EPF, uint8_t MDT, ILI9341_IFCTRL_ENDIAN endian, ILI9341_IFCTRL_DM dmMode, ILI9341_IFCTRL_RM rmMode, ILI9341_IFCTRL_RIM rimMode);

		uint8_t readcommand8(uint8_t reg, uint8_t index = 0);
	};
#ifdef STD_SDK
}
#endif

#endif // _ADAFRUIT_ILI9341H_
