/*!
 * @file Adafruit_SPITFT.cpp
 *
 * 
 * #mainpage Adafruit SPI TFT Displays (and some others)
 *
 * @section intro_sec Introduction
 *
 * Part of Adafruit's GFX graphics library. Originally this class was
 * written to handle a range of color TFT displays connected via SPI,
 * but over time this library and some display-specific subclasses have
 * mutated to include some color OLEDs as well as parallel-interfaced
 * displays. The name's been kept for the sake of older code.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!

 * @section dependencies Dependencies
 *
 * This library depends on
 * <a href="https://github.com/adafruit/Adafruit-GFX-Library">Adafruit_GFX</a>
 * being present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries,P
 * with contributions from the open source community.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */
#include "misc/defines.h"

// Not for ATtiny, at all
#if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny84__)

#include "Adafruit_GFX_Library/Adafruit_SPITFT.h"

#ifdef STD_SDK
using namespace ardPort;
using namespace ardPort::spi;

#endif

#if defined(__AVR__)
#if defined(__AVR_XMEGA__) // only tested with __AVR_ATmega4809__
#define AVR_WRITESPI(x) \
	for (SPI0_DATA = (x); (!(SPI0_INTFLAGS & _BV(SPI_IF_bp)));)
#elif defined(__LGT8F__)
#define AVR_WRITESPI(x)          \
	SPDR = (x);                  \
	asm volatile("nop");         \
	while ((SPFR & _BV(RDEMPT))) \
		;                        \
	SPFR = _BV(RDEMPT) | _BV(WREMPT)
#else
#define AVR_WRITESPI(x) for (SPDR = (x); (!(SPSR & _BV(SPIF)));)
#endif
#endif

#if defined(PORT_IOBUS)
// On SAMD21, redefine digitalPinToPort() to use the slightly-faster
// PORT_IOBUS rather than PORT (not needed on SAMD51).
#undef digitalPinToPort
#define digitalPinToPort(P) (&(PORT_IOBUS->Group[g_APinDescription[P].ulPort]))
#endif // end PORT_IOBUS

#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
// #pragma message ("GFX DMA IS ENABLED. HIGHLY EXPERIMENTAL.")
#include "wiring_private.h" // pinPeripheral() function
#include <Adafruit_ZeroDMA.h>
#include <malloc.h>			 // memalign() function
#define tcNum 2				 // Timer/Counter for parallel write strobe PWM
#define wrPeripheral PIO_CCL // Use CCL to invert write strobe

// DMA transfer-in-progress indicator and callback
static volatile bool dma_busy = false;
static void dma_callback(Adafruit_ZeroDMA *dma)
{
	dma_busy = false;
}

#if defined(__SAMD51__)
// Timer/counter info by index #
static const struct
{
	Tc *tc;	  // -> Timer/Counter base address
	int gclk; // GCLK ID
	int evu;  // EVSYS user ID
} tcList[] = {{TC0, TC0_GCLK_ID, EVSYS_ID_USER_TC0_EVU},
			  {TC1, TC1_GCLK_ID, EVSYS_ID_USER_TC1_EVU},
			  {TC2, TC2_GCLK_ID, EVSYS_ID_USER_TC2_EVU},
			  {TC3, TC3_GCLK_ID, EVSYS_ID_USER_TC3_EVU},
#if defined(TC4)
			  {TC4, TC4_GCLK_ID, EVSYS_ID_USER_TC4_EVU},
#endif
#if defined(TC5)
			  {TC5, TC5_GCLK_ID, EVSYS_ID_USER_TC5_EVU},
#endif
#if defined(TC6)
			  {TC6, TC6_GCLK_ID, EVSYS_ID_USER_TC6_EVU},
#endif
#if defined(TC7)
			  {TC7, TC7_GCLK_ID, EVSYS_ID_USER_TC7_EVU}
#endif
};
#define NUM_TIMERS (sizeof tcList / sizeof tcList[0]) ///< # timer/counters
#endif												  // end __SAMD51__

#endif // end USE_SPI_DMA

// Possible values for Adafruit_SPITFT.connection:
#define TFT_HARD_SPI 0 ///< Display interface = hardware SPI
#define TFT_SOFT_SPI 1 ///< Display interface = software SPI
#define TFT_PARALLEL 2 ///< Display interface = 8- or 16-bit parallel

// CONSTRUCTORS ------------------------------------------------------------

/*!
	@brief  Adafruit_SPITFTのデフォルトコンストラクタ
*/
Adafruit_SPITFT::Adafruit_SPITFT() : Adafruit_GFX()
{
}

/*!
	@brief   Adafruit_SPITFTのソフトウェア（ビットバン）SPI用コンストラクタ。
	@param   w     デフォルト回転設定（0）のディスプレイ幅（ピクセル単位）。
	@param   h     デフォルト回転設定（0）のディスプレイ高さ（ピクセル単位）。
	@param   cs    チップセレクト用のArduinoピン番号（未使用の場合は-1、CSをLowに固定）。
	@param   dc    データ/コマンド選択用のArduinoピン番号（必須）。
	@param   mosi  ビットバンSPI MOSI信号用のArduinoピン番号（必須）。
	@param   sck   ビットバンSPI SCK信号用のArduinoピン番号（必須）。
	@param   rst   ディスプレイリセット用のArduinoピン番号（オプション、未使用時は-1。MCUリセットと共用可能）。
	@param   miso  ビットバンSPI MISO信号用のArduinoピン番号（オプション、デフォルト-1、多くのディスプレイはSPIリード非対応）。
	@note    出力ピンは初期化されません。通常はサブクラスのbegin()関数を呼び出し、その中で本ライブラリのinitSPI()関数がピン初期化を行います。
*/

Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
								 int8_t mosi, int8_t sck, int8_t rst,
								 int8_t miso) : Adafruit_GFX(w, h),
												connection(TFT_SOFT_SPI),
												_rst(rst),
												_cs(cs),
												_dc(dc)
{
	swspi._sck = sck;
	swspi._mosi = mosi;
	swspi._miso = miso;
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(CORE_TEENSY)
#if !defined(KINETISK)
	dcPinMask = digitalPinToBitMask(dc);
	swspi.sckPinMask = digitalPinToBitMask(sck);
	swspi.mosiPinMask = digitalPinToBitMask(mosi);
#endif
	dcPortSet = portSetRegister(dc);
	dcPortClr = portClearRegister(dc);
	swspi.sckPortSet = portSetRegister(sck);
	swspi.sckPortClr = portClearRegister(sck);
	swspi.mosiPortSet = portSetRegister(mosi);
	swspi.mosiPortClr = portClearRegister(mosi);
	if (cs >= 0)
	{
#if !defined(KINETISK)
		csPinMask = digitalPinToBitMask(cs);
#endif
		csPortSet = portSetRegister(cs);
		csPortClr = portClearRegister(cs);
	}
	else
	{
#if !defined(KINETISK)
		csPinMask = 0;
#endif
		csPortSet = dcPortSet;
		csPortClr = dcPortClr;
	}
	if (miso >= 0)
	{
		swspi.misoPort = portInputRegister(miso);
#if !defined(KINETISK)
		swspi.misoPinMask = digitalPinToBitMask(miso);
#endif
	}
	else
	{
		swspi.misoPort = portInputRegister(dc);
	}
#else  // !CORE_TEENSY
	dcPinMask = digitalPinToBitMask(dc);
	swspi.sckPinMask = digitalPinToBitMask(sck);
	swspi.mosiPinMask = digitalPinToBitMask(mosi);
	dcPortSet = &(PORT->Group[g_APinDescription[dc].ulPort].OUTSET.reg);
	dcPortClr = &(PORT->Group[g_APinDescription[dc].ulPort].OUTCLR.reg);
	swspi.sckPortSet = &(PORT->Group[g_APinDescription[sck].ulPort].OUTSET.reg);
	swspi.sckPortClr = &(PORT->Group[g_APinDescription[sck].ulPort].OUTCLR.reg);
	swspi.mosiPortSet = &(PORT->Group[g_APinDescription[mosi].ulPort].OUTSET.reg);
	swspi.mosiPortClr = &(PORT->Group[g_APinDescription[mosi].ulPort].OUTCLR.reg);
	if (cs >= 0)
	{
		csPinMask = digitalPinToBitMask(cs);
		csPortSet = &(PORT->Group[g_APinDescription[cs].ulPort].OUTSET.reg);
		csPortClr = &(PORT->Group[g_APinDescription[cs].ulPort].OUTCLR.reg);
	}
	else
	{
		// No chip-select line defined; might be permanently tied to GND.
		// Assign a valid GPIO register (though not used for CS), and an
		// empty pin bitmask...the nonsense bit-twiddling might be faster
		// than checking _cs and possibly branching.
		csPortSet = dcPortSet;
		csPortClr = dcPortClr;
		csPinMask = 0;
	}
	if (miso >= 0)
	{
		swspi.misoPinMask = digitalPinToBitMask(miso);
		swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(miso));
	}
	else
	{
		swspi.misoPinMask = 0;
		swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(dc));
	}
#endif // end !CORE_TEENSY
#else  // !HAS_PORT_SET_CLR
	dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(dc));
	dcPinMaskSet = digitalPinToBitMask(dc);
	swspi.sckPort = (PORTreg_t)portOutputRegister(digitalPinToPort(sck));
	swspi.sckPinMaskSet = digitalPinToBitMask(sck);
	swspi.mosiPort = (PORTreg_t)portOutputRegister(digitalPinToPort(mosi));
	swspi.mosiPinMaskSet = digitalPinToBitMask(mosi);
	if (cs >= 0)
	{
		csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(cs));
		csPinMaskSet = digitalPinToBitMask(cs);
	}
	else
	{
		// No chip-select line defined; might be permanently tied to GND.
		// Assign a valid GPIO register (though not used for CS), and an
		// empty pin bitmask...the nonsense bit-twiddling might be faster
		// than checking _cs and possibly branching.
		csPort = dcPort;
		csPinMaskSet = 0;
	}
	if (miso >= 0)
	{
		swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(miso));
		swspi.misoPinMask = digitalPinToBitMask(miso);
	}
	else
	{
		swspi.misoPort = (PORTreg_t)portInputRegister(digitalPinToPort(dc));
		swspi.misoPinMask = 0;
	}
	csPinMaskClr = ~csPinMaskSet;
	dcPinMaskClr = ~dcPinMaskSet;
	swspi.sckPinMaskClr = ~swspi.sckPinMaskSet;
	swspi.mosiPinMaskClr = ~swspi.mosiPinMaskSet;
#endif // !end HAS_PORT_SET_CLR
#endif // end USE_FAST_PINIO
}

	/*!
		@brief   ハードウェアSPIを使用し、ボードのデフォルトSPIペリフェラルで動作するAdafruit_SPITFTのコンストラクタ。
		@param   w     デフォルト回転設定（0）のディスプレイ幅（ピクセル単位）。
		@param   h     デフォルト回転設定（0）のディスプレイ高さ（ピクセル単位）。
		@param   cs    チップセレクト用のArduinoピン番号（未使用の場合は-1、CSをLowに固定）。
		@param   dc    データ/コマンド選択用のArduinoピン番号（必須）。
		@param   rst   ディスプレイリセット用のArduinoピン番号（オプション、MCUリセットと共用可能、デフォルト-1は未使用）。
		@note    出力ピンは初期化されません。通常はサブクラスのbegin()関数を呼び出し、その中で本ライブラリのinitSPI()関数がピン初期化を行います。
	*/
	#if defined(ESP8266) // See notes below
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
								 int8_t rst) : Adafruit_GFX(w, h),
											   connection(TFT_HARD_SPI),
											   _rst(rst),
											   _cs(cs),
											   _dc(dc)
{
	hwspi._spi = &SPI;
}
	#else  // !ESP8266
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc,
								 int8_t rst) : Adafruit_SPITFT(w, h, &SPI, cs, dc, rst)
{
	// This just invokes the hardware SPI constructor below,
	// passing the default SPI device (&SPI).
}

	#endif // end !ESP8266

	#if !defined(ESP8266)
// ESP8266のコンパイラはこのコンストラクタで混乱します -- 
// SPIClassポインタ（引数#3）と通常の整数を区別できません。
// そのため、ESP8266ではこのバリアントは提供しません。
// デフォルトのハードウェアSPIペリフェラルか、ソフトウェアSPIのみ使用可能です。
// もしソフトウェアビットバンで動作する「仮想」SPIClassペリフェラルを作成する
// ライブラリがあったとしても、それはサポートされません。
/*!
	@brief   特定のSPIペリフェラルを使用するハードウェアSPI用Adafruit_SPITFTコンストラクタ
	@param   w         デフォルト回転（0）時のディスプレイ幅（ピクセル単位）
	@param   h         デフォルト回転（0）時のディスプレイ高さ（ピクセル単位）
	@param   spiClass  SPIClass型へのポインタ（例：&SPIや&SPI1）
	@param   cs        チップセレクト用Arduinoピン番号（未使用時は-1、CSをLowに固定）
	@param   dc        データ/コマンド選択用Arduinoピン番号（必須）
	@param   rst       ディスプレイリセット用Arduinoピン番号（オプション、MCUリセットと共用可能、デフォルト-1は未使用）
	@note    出力ピンはコンストラクタ内で初期化されません。
			 通常はサブクラスのbegin()関数を呼び出し、その中で本ライブラリのinitSPI()関数がピン初期化を行います。
			 ただし、独自にSERCOM SPIペリフェラルを構築した場合（SPIClassコンストラクタを呼び出した場合）は、
			 begin()関数およびpinPeripheral()を使ってMOSI, MISO, SCKピンのGPIO設定を手動で行う必要があります。
			 これはディスプレイ固有のbegin/init関数を呼ぶ前に実施してください。
			 （残念ながら避けられません）
*/
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, SPIClass *spiClass,
								 int8_t cs, int8_t dc, int8_t rst) : Adafruit_GFX(w, h),
																	 connection(TFT_HARD_SPI),
																	 _rst(rst),
																	 _cs(cs),
																	 _dc(dc)
{
	hwspi._spi = spiClass;
		#if defined(USE_FAST_PINIO)
			#if defined(HAS_PORT_SET_CLR)
				#if defined(CORE_TEENSY)
					#if !defined(KINETISK)
	dcPinMask = digitalPinToBitMask(dc);
					#endif
	dcPortSet = portSetRegister(dc);
	dcPortClr = portClearRegister(dc);
	if (cs >= 0)
	{
					#if !defined(KINETISK)
		csPinMask = digitalPinToBitMask(cs);
					#endif
		csPortSet = portSetRegister(cs);
		csPortClr = portClearRegister(cs);
	}
	else
	{ // see comments below
					#if !defined(KINETISK)
		csPinMask = 0;
					#endif
		csPortSet = dcPortSet;
		csPortClr = dcPortClr;
	}
				#else  // !CORE_TEENSY
	dcPinMask = digitalPinToBitMask(dc);
	dcPortSet = &(PORT->Group[g_APinDescription[dc].ulPort].OUTSET.reg);
	dcPortClr = &(PORT->Group[g_APinDescription[dc].ulPort].OUTCLR.reg);
	if (cs >= 0)
	{
		csPinMask = digitalPinToBitMask(cs);
		csPortSet = &(PORT->Group[g_APinDescription[cs].ulPort].OUTSET.reg);
		csPortClr = &(PORT->Group[g_APinDescription[cs].ulPort].OUTCLR.reg);
	}
	else
	{
		// No chip-select line defined; might be permanently tied to GND.
		// Assign a valid GPIO register (though not used for CS), and an
		// empty pin bitmask...the nonsense bit-twiddling might be faster
		// than checking _cs and possibly branching.
		csPortSet = dcPortSet;
		csPortClr = dcPortClr;
		csPinMask = 0;
	}
				#endif // end !CORE_TEENSY
			#else  // !HAS_PORT_SET_CLR
	dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(dc));
	dcPinMaskSet = digitalPinToBitMask(dc);
	if (cs >= 0)
	{
		csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(cs));
		csPinMaskSet = digitalPinToBitMask(cs);
	}
	else
	{
		// No chip-select line defined; might be permanently tied to GND.
		// Assign a valid GPIO register (though not used for CS), and an
		// empty pin bitmask...the nonsense bit-twiddling might be faster
		// than checking _cs and possibly branching.
		csPort = dcPort;
		csPinMaskSet = 0;
	}
	csPinMaskClr = ~csPinMaskSet;
	dcPinMaskClr = ~dcPinMaskSet;
			#endif // end !HAS_PORT_SET_CLR
		#endif // end USE_FAST_PINIO
}
	#endif // end !ESP8266

/*!
	@brief   Adafruit_SPITFTのパラレルディスプレイ接続用コンストラクタ。
	@param   w         デフォルト回転（0）時のディスプレイ幅（ピクセル単位）。
	@param   h         デフォルト回転（0）時のディスプレイ高さ（ピクセル単位）。
	@param   busWidth  tft16（ヘッダファイル内の列挙型）の場合は16ビットパラレル接続、それ以外は8ビット。
					   16ビットはまだ完全には実装・テストされていないため、現状は"tft8bitbus"を指定してください。
					   この引数はソフトウェアSPIとのコンストラクタの区別のために必要です。
					   8ビットアーキテクチャでは無視されます（PORTが8ビット幅のため）。
	@param   d0        データビット0用のArduinoピン番号（1以降は連続して割り当てられます）。
					   8（または16）ビットのデータピンは同じPORTレジスタ内で連続かつバイト（またはワード）アラインされている必要があります
					   （Arduinoのピン番号と一致しない場合があります）。
	@param   wr        ライトストローブ用のArduinoピン番号（必須）。
	@param   dc        データ/コマンド選択用のArduinoピン番号（必須）。
	@param   cs        チップセレクト用のArduinoピン番号（オプション、未使用時は-1、CSをLowに固定）。
	@param   rst       ディスプレイリセット用のArduinoピン番号（オプション、MCUリセットと共用可能、デフォルト-1は未使用）。
	@param   rd        リードストローブ用のArduinoピン番号（オプション、未使用時は-1）。
	@note    出力ピンは初期化されません。通常はサブクラスのbegin()関数を呼び出し、その中で本ライブラリのinitSPI()関数がピン初期化を行います。
			 （名称は誤解を招きますが、このライブラリは元々SPIディスプレイのみを扱っていましたが、最近パラレルにも対応したためです。
			 既存コードとの互換性維持のため名前はそのままです。）
*/
Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, tftBusWidth busWidth,
								 int8_t d0, int8_t wr, int8_t dc, int8_t cs,
								 int8_t rst, int8_t rd) : Adafruit_GFX(w, h),
														  connection(TFT_PARALLEL),
														  _rst(rst),
														  _cs(cs),
														  _dc(dc)
{
	tft8._d0 = d0;
	tft8._wr = wr;
	tft8._rd = rd;
	tft8.wide = (busWidth == tft16bitbus);
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(CORE_TEENSY)
	tft8.wrPortSet = portSetRegister(wr);
	tft8.wrPortClr = portClearRegister(wr);
#if !defined(KINETISK)
	dcPinMask = digitalPinToBitMask(dc);
#endif
	dcPortSet = portSetRegister(dc);
	dcPortClr = portClearRegister(dc);
	if (cs >= 0)
	{
#if !defined(KINETISK)
		csPinMask = digitalPinToBitMask(cs);
#endif
		csPortSet = portSetRegister(cs);
		csPortClr = portClearRegister(cs);
	}
	else
	{ // see comments below
#if !defined(KINETISK)
		csPinMask = 0;
#endif
		csPortSet = dcPortSet;
		csPortClr = dcPortClr;
	}
	if (rd >= 0)
	{ // if read-strobe pin specified...
#if defined(KINETISK)
		tft8.rdPinMask = 1;
#else // !KINETISK
		tft8.rdPinMask = digitalPinToBitMask(rd);
#endif
		tft8.rdPortSet = portSetRegister(rd);
		tft8.rdPortClr = portClearRegister(rd);
	}
	else
	{
		tft8.rdPinMask = 0;
		tft8.rdPortSet = dcPortSet;
		tft8.rdPortClr = dcPortClr;
	}
	// These are all uint8_t* pointers -- elsewhere they're recast
	// as necessary if a 'wide' 16-bit interface is in use.
	tft8.writePort = portOutputRegister(d0);
	tft8.readPort = portInputRegister(d0);
	tft8.dirSet = portModeRegister(d0);
	tft8.dirClr = portModeRegister(d0);
#else  // !CORE_TEENSY
	tft8.wrPinMask = digitalPinToBitMask(wr);
	tft8.wrPortSet = &(PORT->Group[g_APinDescription[wr].ulPort].OUTSET.reg);
	tft8.wrPortClr = &(PORT->Group[g_APinDescription[wr].ulPort].OUTCLR.reg);
	dcPinMask = digitalPinToBitMask(dc);
	dcPortSet = &(PORT->Group[g_APinDescription[dc].ulPort].OUTSET.reg);
	dcPortClr = &(PORT->Group[g_APinDescription[dc].ulPort].OUTCLR.reg);
	if (cs >= 0)
	{
		csPinMask = digitalPinToBitMask(cs);
		csPortSet = &(PORT->Group[g_APinDescription[cs].ulPort].OUTSET.reg);
		csPortClr = &(PORT->Group[g_APinDescription[cs].ulPort].OUTCLR.reg);
	}
	else
	{
		// No chip-select line defined; might be permanently tied to GND.
		// Assign a valid GPIO register (though not used for CS), and an
		// empty pin bitmask...the nonsense bit-twiddling might be faster
		// than checking _cs and possibly branching.
		csPortSet = dcPortSet;
		csPortClr = dcPortClr;
		csPinMask = 0;
	}
	if (rd >= 0)
	{ // if read-strobe pin specified...
		tft8.rdPinMask = digitalPinToBitMask(rd);
		tft8.rdPortSet = &(PORT->Group[g_APinDescription[rd].ulPort].OUTSET.reg);
		tft8.rdPortClr = &(PORT->Group[g_APinDescription[rd].ulPort].OUTCLR.reg);
	}
	else
	{
		tft8.rdPinMask = 0;
		tft8.rdPortSet = dcPortSet;
		tft8.rdPortClr = dcPortClr;
	}
	// Get pointers to PORT write/read/dir bytes within 32-bit PORT
	uint8_t dBit = g_APinDescription[d0].ulPin; // d0 bit # in PORT
	PortGroup *p = (&(PORT->Group[g_APinDescription[d0].ulPort]));
	uint8_t offset = dBit / 8; // d[7:0] byte # within PORT
	if (tft8.wide)
		offset &= ~1; // d[15:8] byte # within PORT
	// These are all uint8_t* pointers -- elsewhere they're recast
	// as necessary if a 'wide' 16-bit interface is in use.
	tft8.writePort = (volatile uint8_t *)&(p->OUT.reg) + offset;
	tft8.readPort = (volatile uint8_t *)&(p->IN.reg) + offset;
	tft8.dirSet = (volatile uint8_t *)&(p->DIRSET.reg) + offset;
	tft8.dirClr = (volatile uint8_t *)&(p->DIRCLR.reg) + offset;
#endif // end !CORE_TEENSY
#else  // !HAS_PORT_SET_CLR
	tft8.wrPort = (PORTreg_t)portOutputRegister(digitalPinToPort(wr));
	tft8.wrPinMaskSet = digitalPinToBitMask(wr);
	dcPort = (PORTreg_t)portOutputRegister(digitalPinToPort(dc));
	dcPinMaskSet = digitalPinToBitMask(dc);
	if (cs >= 0)
	{
		csPort = (PORTreg_t)portOutputRegister(digitalPinToPort(cs));
		csPinMaskSet = digitalPinToBitMask(cs);
	}
	else
	{
		// No chip-select line defined; might be permanently tied to GND.
		// Assign a valid GPIO register (though not used for CS), and an
		// empty pin bitmask...the nonsense bit-twiddling might be faster
		// than checking _cs and possibly branching.
		csPort = dcPort;
		csPinMaskSet = 0;
	}
	if (rd >= 0)
	{ // if read-strobe pin specified...
		tft8.rdPort = (PORTreg_t)portOutputRegister(digitalPinToPort(rd));
		tft8.rdPinMaskSet = digitalPinToBitMask(rd);
	}
	else
	{
		tft8.rdPort = dcPort;
		tft8.rdPinMaskSet = 0;
	}
	csPinMaskClr = ~csPinMaskSet;
	dcPinMaskClr = ~dcPinMaskSet;
	tft8.wrPinMaskClr = ~tft8.wrPinMaskSet;
	tft8.rdPinMaskClr = ~tft8.rdPinMaskSet;
	tft8.writePort = (PORTreg_t)portOutputRegister(digitalPinToPort(d0));
	tft8.readPort = (PORTreg_t)portInputRegister(digitalPinToPort(d0));
	tft8.portDir = (PORTreg_t)portModeRegister(digitalPinToPort(d0));
#endif // end !HAS_PORT_SET_CLR
#endif // end USE_FAST_PINIO
}

// end constructors -------
/**
 * @brief ソフトウェア（ビットバン）SPI用のAdafruit_SPITFTオブジェクトを初期化するヘルパー関数。デフォルトコンストラクタを使用したときはこれを使って初期化する
 * @param w    ディスプレイの幅（ピクセル単位）
 * @param h    ディスプレイの高さ（ピクセル単位）
 * @param cs   チップセレクト用のピン番号
 * @param dc   データ/コマンド選択用のピン番号
 * @param mosi MOSI信号用のピン番号
 * @param sck  SCK信号用のピン番号
 * @param rst  リセット用のピン番号
 * @param miso MISO信号用のピン番号
 *
 * @details
 * Adafruit_GFXの基本情報を初期化し、ソフトウェアSPI接続として設定します。
 * 各ピン番号をメンバ変数に格納します。
 */
void Adafruit_SPITFT::constructObject(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t mosi, int8_t sck, int8_t rst, int8_t miso)
{
	Adafruit_GFX::constructObject(w, h);
	connection = TFT_SOFT_SPI;
	_rst = rst;
	_cs = cs;
	_dc = dc;
	swspi._sck = sck;
	swspi._mosi = mosi;
	swspi._miso = miso;
}
/**
 * @brief ハードウェアSPI用のAdafruit_SPITFTオブジェクトを初期化するヘルパー関数。デフォルトコンストラクタを使用したときはこれを使って初期化する
 * @param w    ディスプレイの幅（ピクセル単位）
 * @param h    ディスプレイの高さ（ピクセル単位）
 * @param spiClass 使用するSPIClass型へのポインタ（例：&SPIや&SPI1）
 * @param dc   データ/コマンド選択用のピン番号
 * @param cs   チップセレクト用のピン番号
 * @param rst  リセット用のピン番号
 *
 * @details
 * Adafruit_GFXの基本情報を初期化し、ハードウェアSPI接続として設定します。
 * SPIインターフェースや各ピン番号をメンバ変数に格納します。
 */
void Adafruit_SPITFT::constructObject(uint16_t w, uint16_t h, SPIClass *spiClass, int8_t dc, int8_t cs, int8_t rst)
{
	Adafruit_GFX::constructObject(w, h);
	connection = TFT_HARD_SPI;
	_rst = rst;
	_cs = cs;
	_dc = dc;
	hwspi._spi = spiClass;
}

// CLASS MEMBER FUNCTIONS --------------------------------------------------

// begin() and setAddrWindow() MUST be declared by any subclass.

/*!
	@brief  TFTインターフェース用にマイコンのピンを初期化する関数。通常はサブクラスのbegin()関数から呼び出される。
	@param  freq     ハードウェアSPI使用時のSPI周波数。デフォルト（0）の場合はデバイス固有の値が使用される。
					 ソフトウェアSPIやパラレル接続時は無視される。
	@param  spiMode  ハードウェアSPI使用時のSPIモード。SPI.hで定義されているSPI_MODE0, SPI_MODE1, SPI_MODE2, SPI_MODE3のいずれかを指定する必要がある。
					 '0'などの数値を直接渡してはいけない（値が異なるため）。必ず定義を使うこと。
	@note   関数名は歴史的経緯でこのままだが、パラレル接続時にも呼ばれる。また、本来はprivateにできる関数だが、多くのクラス関数がpublicに配置されている。
*/

void Adafruit_SPITFT::initSPI(uint32_t freq, uint8_t spiMode)
{
	if (!freq)
		freq = DEFAULT_SPI_FREQ; // If no freq specified, use default

	// Init basic control pins common to all connection types
	if (_cs >= 0)
	{
		pinMode(_cs, OUTPUT);
		digitalWrite(_cs, HIGH); // Deselect
	}
	pinMode(_dc, OUTPUT);
	digitalWrite(_dc, HIGH); // Data mode

	if (connection == TFT_HARD_SPI)
	{
#if defined(SPI_HAS_TRANSACTION)
		hwspi.settings = SPISettings(freq, MSBFIRST, spiMode);
#else
		hwspi._freq = freq; // Save freq value for later
#endif

		hwspi._mode = spiMode; // Save spiMode value for later
		
		// hwspi._spi->begin() を呼び出すのは、variant.h に記載されている「確立済み」SPIインターフェースの場合のみ。
		// 独自実装の SERCOM SPI を使う場合は、begin() や pinPeripheral() の呼び出しを
		// 画面固有の begin/init 関数を呼ぶ前に、ユーザー自身のコードで必ず実行する。
		// その理由は、SPI::begin() が g_APinDescription[n].ulPinType に基づき pinPeripheral() を呼び出すためで、
		// 確立済みでない SPI インターフェースのピンでは常に PIO_DIGITAL などになってしまうため。
		// 一方で本来は PIO_SERCOM や PIO_SERCOM_ALT が必要。ピンやSERCOMごとにデバイスやバリアントで異なるため、
		// ここで一律に呼び出すことはできない。また、SPIデバイスはこの直後に初期化コマンドが続くため、
		// 事前にセットアップしておく必要がある。

		if (
#if !defined(SPI_INTERFACES_COUNT)
			1
#else
#if SPI_INTERFACES_COUNT > 0
			(hwspi._spi == &SPI)
#endif
#if SPI_INTERFACES_COUNT > 1
			|| (hwspi._spi == &SPI1)
#endif
#if SPI_INTERFACES_COUNT > 2
			|| (hwspi._spi == &SPI2)
#endif
#if SPI_INTERFACES_COUNT > 3
			|| (hwspi._spi == &SPI3)
#endif
#if SPI_INTERFACES_COUNT > 4
			|| (hwspi._spi == &SPI4)
#endif
#if SPI_INTERFACES_COUNT > 5
			|| (hwspi._spi == &SPI5)
#endif
#endif // end SPI_INTERFACES_COUNT
		)
		{
			hwspi._spi->begin();
		}
	}
	else if (connection == TFT_SOFT_SPI)
	{
		pinMode(swspi._mosi, OUTPUT);
		digitalWrite(swspi._mosi, LOW);
		pinMode(swspi._sck, OUTPUT);
		digitalWrite(swspi._sck, LOW);
		if (swspi._miso >= 0)
		{
			pinMode(swspi._miso, INPUT);
		}
	}
	else
	{ 	// TFT_PARALLEL
		// データピンの初期化処理。
		// d0ピンのみが渡されているため、他のデータピンを探すために
		// ピン記述リストを走査する。
		// これらのピンは同じPORT上にあり、次の7ビット（または15ビット）以内に連続している必要がある
		// （連続したPORTバイトまたはワードとして書き込みを行うため）。
#if defined(__AVR__)
	  // PORT registers are 8 bits wide, so just need a register match...
		for (uint8_t i = 0; i < NUM_DIGITAL_PINS; i++)
		{
			if ((PORTreg_t)portOutputRegister(digitalPinToPort(i)) ==
				tft8.writePort)
			{
				pinMode(i, OUTPUT);
				digitalWrite(i, LOW);
			}
		}
#elif defined(USE_FAST_PINIO)
#if defined(CORE_TEENSY)
		if (!tft8.wide)
		{
			*tft8.dirSet = 0xFF;	// Set port to output
			*tft8.writePort = 0x00; // Write all 0s
		}
		else
		{
			*(volatile uint16_t *)tft8.dirSet = 0xFFFF;
			*(volatile uint16_t *)tft8.writePort = 0x0000;
		}
#else  // !CORE_TEENSY
		uint8_t portNum = g_APinDescription[tft8._d0].ulPort, // d0 PORT #
			dBit = g_APinDescription[tft8._d0].ulPin,		  // d0 bit in PORT
			lastBit = dBit + (tft8.wide ? 15 : 7);
		for (uint8_t i = 0; i < PINS_COUNT; i++)
		{
			if ((g_APinDescription[i].ulPort == portNum) &&
				(g_APinDescription[i].ulPin >= dBit) &&
				(g_APinDescription[i].ulPin <= (uint32_t)lastBit))
			{
				pinMode(i, OUTPUT);
				digitalWrite(i, LOW);
			}
		}
#endif // end !CORE_TEENSY
#endif
		pinMode(tft8._wr, OUTPUT);
		digitalWrite(tft8._wr, HIGH);
		if (tft8._rd >= 0)
		{
			pinMode(tft8._rd, OUTPUT);
			digitalWrite(tft8._rd, HIGH);
		}
	}

	if (_rst >= 0)
	{
		// Toggle _rst low to reset
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(100);
		digitalWrite(_rst, LOW);
		delay(100);
		digitalWrite(_rst, HIGH);
		delay(200);
	}

#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
	if (((connection == TFT_HARD_SPI) || (connection == TFT_PARALLEL)) &&
		(dma.allocate() == DMA_STATUS_OK))
	{ // Allocate channel
		// The DMA library needs to alloc at least one valid descriptor,
		// so we do that here. It's not used in the usual sense though,
		// just before a transfer we copy descriptor[0] to this address.
		if (dptr = dma.addDescriptor(NULL, NULL, 42, DMA_BEAT_SIZE_BYTE, false,
									 false))
		{
			// Alloc 2 scanlines worth of pixels on display's major axis,
			// whichever that is, rounding each up to 2-pixel boundary.
			int major = (WIDTH > HEIGHT) ? WIDTH : HEIGHT;
			major += (major & 1);	// -> next 2-pixel bound, if needed.
			maxFillLen = major * 2; // 2 scanlines
			// Note to future self: if you decide to make the pixel buffer
			// much larger, remember that DMA transfer descriptors can't
			// exceed 65,535 bytes (not 65,536), meaning 32,767 pixels max.
			// Not that we have that kind of RAM to throw around right now.
			if ((pixelBuf[0] = (uint16_t *)malloc(maxFillLen * sizeof(uint16_t))))
			{
				// Alloc OK. Get pointer to start of second scanline.
				pixelBuf[1] = &pixelBuf[0][major];
				// Determine number of DMA descriptors needed to cover
				// entire screen when entire 2-line pixelBuf is used
				// (round up for fractional last descriptor).
				int numDescriptors = (WIDTH * HEIGHT + (maxFillLen - 1)) / maxFillLen;
				// DMA descriptors MUST be 128-bit (16 byte) aligned.
				// memalign() is considered obsolete but it's replacements
				// (aligned_alloc() or posix_memalign()) are not currently
				// available in the version of ARM GCC in use, but this
				// is, so here we are.
				if ((descriptor = (DmacDescriptor *)memalign(
						 16, numDescriptors * sizeof(DmacDescriptor))))
				{
					int dmac_id;
					volatile uint32_t *data_reg;

					if (connection == TFT_HARD_SPI)
					{
						// THIS IS AN AFFRONT TO NATURE, but I don't know
						// any "clean" way to get the sercom number from the
						// the SPIClass pointer (e.g. &SPI or &SPI1), which
						// is all we have to work with. SPIClass does contain
						// a SERCOM pointer but it is a PRIVATE member!
						// Doing an UNSPEAKABLY HORRIBLE THING here, directly
						// accessing the first 32-bit value in the SPIClass
						// structure, knowing that's (currently) where the
						// SERCOM pointer lives, but this ENTIRELY DEPENDS on
						// that structure not changing nor the compiler
						// rearranging things. Oh the humanity!

						if (*(SERCOM **)hwspi._spi == &sercom0)
						{
							dmac_id = SERCOM0_DMAC_ID_TX;
							data_reg = &SERCOM0->SPI.DATA.reg;
#if defined SERCOM1
						}
						else if (*(SERCOM **)hwspi._spi == &sercom1)
						{
							dmac_id = SERCOM1_DMAC_ID_TX;
							data_reg = &SERCOM1->SPI.DATA.reg;
#endif
#if defined SERCOM2
						}
						else if (*(SERCOM **)hwspi._spi == &sercom2)
						{
							dmac_id = SERCOM2_DMAC_ID_TX;
							data_reg = &SERCOM2->SPI.DATA.reg;
#endif
#if defined SERCOM3
						}
						else if (*(SERCOM **)hwspi._spi == &sercom3)
						{
							dmac_id = SERCOM3_DMAC_ID_TX;
							data_reg = &SERCOM3->SPI.DATA.reg;
#endif
#if defined SERCOM4
						}
						else if (*(SERCOM **)hwspi._spi == &sercom4)
						{
							dmac_id = SERCOM4_DMAC_ID_TX;
							data_reg = &SERCOM4->SPI.DATA.reg;
#endif
#if defined SERCOM5
						}
						else if (*(SERCOM **)hwspi._spi == &sercom5)
						{
							dmac_id = SERCOM5_DMAC_ID_TX;
							data_reg = &SERCOM5->SPI.DATA.reg;
#endif
#if defined SERCOM6
						}
						else if (*(SERCOM **)hwspi._spi == &sercom6)
						{
							dmac_id = SERCOM6_DMAC_ID_TX;
							data_reg = &SERCOM6->SPI.DATA.reg;
#endif
#if defined SERCOM7
						}
						else if (*(SERCOM **)hwspi._spi == &sercom7)
						{
							dmac_id = SERCOM7_DMAC_ID_TX;
							data_reg = &SERCOM7->SPI.DATA.reg;
#endif
						}
						dma.setPriority(DMA_PRIORITY_3);
						dma.setTrigger(dmac_id);
						dma.setAction(DMA_TRIGGER_ACTON_BEAT);

						// Initialize descriptor list.
						for (int d = 0; d < numDescriptors; d++)
						{
							// No need to set SRCADDR, DESCADDR or BTCNT --
							// those are done in the pixel-writing functions.
							descriptor[d].BTCTRL.bit.VALID = true;
							descriptor[d].BTCTRL.bit.EVOSEL = DMA_EVENT_OUTPUT_DISABLE;
							descriptor[d].BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_NOACT;
							descriptor[d].BTCTRL.bit.BEATSIZE = DMA_BEAT_SIZE_BYTE;
							descriptor[d].BTCTRL.bit.DSTINC = 0;
							descriptor[d].BTCTRL.bit.STEPSEL = DMA_STEPSEL_SRC;
							descriptor[d].BTCTRL.bit.STEPSIZE =
								DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
							descriptor[d].DSTADDR.reg = (uint32_t)data_reg;
						}
					}
					else
					{ // Parallel connection

#if defined(__SAMD51__)
						int dmaChannel = dma.getChannel();
						// Enable event output, use EVOSEL output
						DMAC->Channel[dmaChannel].CHEVCTRL.bit.EVOE = 1;
						DMAC->Channel[dmaChannel].CHEVCTRL.bit.EVOMODE = 0;

						// CONFIGURE TIMER/COUNTER (for write strobe)

						Tc *timer = tcList[tcNum].tc; // -> Timer struct
						int id = tcList[tcNum].gclk;  // Timer GCLK ID
						GCLK_PCHCTRL_Type pchctrl;

						// Set up timer clock source from GCLK
						GCLK->PCHCTRL[id].bit.CHEN = 0; // Stop timer
						while (GCLK->PCHCTRL[id].bit.CHEN)
							; // Wait for it
						pchctrl.bit.GEN = GCLK_PCHCTRL_GEN_GCLK0_Val;
						pchctrl.bit.CHEN = 1; // Enable
						GCLK->PCHCTRL[id].reg = pchctrl.reg;
						while (!GCLK->PCHCTRL[id].bit.CHEN)
							; // Wait for it

						// Disable timer/counter before configuring it
						timer->COUNT8.CTRLA.bit.ENABLE = 0;
						while (timer->COUNT8.SYNCBUSY.bit.STATUS)
							;

						timer->COUNT8.WAVE.bit.WAVEGEN = 2;	   // NPWM
						timer->COUNT8.CTRLA.bit.MODE = 1;	   // 8-bit
						timer->COUNT8.CTRLA.bit.PRESCALER = 0; // 1:1
						while (timer->COUNT8.SYNCBUSY.bit.STATUS)
							;

						timer->COUNT8.CTRLBCLR.bit.DIR = 1; // Count UP
						while (timer->COUNT8.SYNCBUSY.bit.CTRLB)
							;
						timer->COUNT8.CTRLBSET.bit.ONESHOT = 1; // One-shot
						while (timer->COUNT8.SYNCBUSY.bit.CTRLB)
							;
						timer->COUNT8.PER.reg = 6; // PWM top
						while (timer->COUNT8.SYNCBUSY.bit.PER)
							;
						timer->COUNT8.CC[0].reg = 2; // Compare
						while (timer->COUNT8.SYNCBUSY.bit.CC0)
							;
						// Enable async input events,
						// event action = restart.
						timer->COUNT8.EVCTRL.bit.TCEI = 1;
						timer->COUNT8.EVCTRL.bit.EVACT = 1;

						// Enable timer
						timer->COUNT8.CTRLA.reg |= TC_CTRLA_ENABLE;
						while (timer->COUNT8.SYNCBUSY.bit.STATUS)
							;

#if (wrPeripheral == PIO_CCL)
						// CONFIGURE CCL (inverts timer/counter output)

						MCLK->APBCMASK.bit.CCL_ = 1;		 // Enable CCL clock
						CCL->CTRL.bit.ENABLE = 0;			 // Disable to config
						CCL->CTRL.bit.SWRST = 1;			 // Reset CCL registers
						CCL->LUTCTRL[tcNum].bit.ENABLE = 0;	 // Disable LUT
						CCL->LUTCTRL[tcNum].bit.FILTSEL = 0; // No filter
						CCL->LUTCTRL[tcNum].bit.INSEL0 = 6;	 // TC input
						CCL->LUTCTRL[tcNum].bit.INSEL1 = 0;	 // MASK
						CCL->LUTCTRL[tcNum].bit.INSEL2 = 0;	 // MASK
						CCL->LUTCTRL[tcNum].bit.TRUTH = 1;	 // Invert in 0
						CCL->LUTCTRL[tcNum].bit.ENABLE = 1;	 // Enable LUT
						CCL->CTRL.bit.ENABLE = 1;			 // Enable CCL
#endif

						// CONFIGURE EVENT SYSTEM

						// Set up event system clock source from GCLK...
						// Disable EVSYS, wait for disable
						GCLK->PCHCTRL[EVSYS_GCLK_ID_0].bit.CHEN = 0;
						while (GCLK->PCHCTRL[EVSYS_GCLK_ID_0].bit.CHEN)
							;
						pchctrl.bit.GEN = GCLK_PCHCTRL_GEN_GCLK0_Val;
						pchctrl.bit.CHEN = 1; // Re-enable
						GCLK->PCHCTRL[EVSYS_GCLK_ID_0].reg = pchctrl.reg;
						// Wait for it, then enable EVSYS clock
						while (!GCLK->PCHCTRL[EVSYS_GCLK_ID_0].bit.CHEN)
							;
						MCLK->APBBMASK.bit.EVSYS_ = 1;

						// Connect Timer EVU to ch 0
						EVSYS->USER[tcList[tcNum].evu].reg = 1;
						// Datasheet recommends single write operation;
						// reg instead of bit. Also datasheet: PATH bits
						// must be zero when using async!
						EVSYS_CHANNEL_Type ev;
						ev.reg = 0;
						ev.bit.PATH = 2;				  // Asynchronous
						ev.bit.EVGEN = 0x22 + dmaChannel; // DMA channel 0+
						EVSYS->Channel[0].CHANNEL.reg = ev.reg;

						// Initialize descriptor list.
						for (int d = 0; d < numDescriptors; d++)
						{
							// No need to set SRCADDR, DESCADDR or BTCNT --
							// those are done in the pixel-writing functions.
							descriptor[d].BTCTRL.bit.VALID = true;
							// Event strobe on beat xfer:
							descriptor[d].BTCTRL.bit.EVOSEL = 0x3;
							descriptor[d].BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_NOACT;
							descriptor[d].BTCTRL.bit.BEATSIZE =
								tft8.wide ? DMA_BEAT_SIZE_HWORD : DMA_BEAT_SIZE_BYTE;
							descriptor[d].BTCTRL.bit.SRCINC = 1;
							descriptor[d].BTCTRL.bit.DSTINC = 0;
							descriptor[d].BTCTRL.bit.STEPSEL = DMA_STEPSEL_SRC;
							descriptor[d].BTCTRL.bit.STEPSIZE =
								DMA_ADDRESS_INCREMENT_STEP_SIZE_1;
							descriptor[d].DSTADDR.reg = (uint32_t)tft8.writePort;
						}
#endif // __SAMD51
					} // end parallel-specific DMA setup

					lastFillColor = 0x0000;
					lastFillLen = 0;
					dma.setCallback(dma_callback);
					return; // Success!
							// else clean up any partial allocation...
				} // end descriptor memalign()
				free(pixelBuf[0]);
				pixelBuf[0] = pixelBuf[1] = NULL;
			} // end pixelBuf malloc()
			  // Don't currently have a descriptor delete function in
			  // ZeroDMA lib, but if we did, it would be called here.
		} // end addDescriptor()
		dma.free(); // Deallocate DMA channel
	}
#endif // end USE_SPI_DMA
}

/*!
	@brief  初期化後にSPIクロック周波数を変更できるようにする
	@param  freq 設定したいSPIクロックの周波数。チップの仕様により、必ずしもこの値になるとは限りません。
*/
void Adafruit_SPITFT::setSPISpeed(uint32_t freq)
{
#if defined(SPI_HAS_TRANSACTION)
	hwspi.settings = SPISettings(freq, MSBFIRST, hwspi._mode);
#else
	hwspi._freq = freq; // Save freq value for later
#endif
}

/*!
	@brief  ディスプレイへコマンドやデータを送信する前に呼び出す関数。必要に応じてチップセレクトを行い、
	ハードウェアSPIかつトランザクション対応時はSPIトランザクションも開始する。全てのディスプレイタイプで
	必要。SPI専用の関数ではない。
*/
void Adafruit_SPITFT::startWrite(void)
{
	SPI_BEGIN_TRANSACTION();
	if (_cs >= 0)
		SPI_CS_LOW();
}

/*!
	@brief  ディスプレイへのコマンドやデータの送信後に呼び出す関数。
	@details コマンドやデータ送信後に呼び出してください。必要に応じてチップセレクトを解除し、
	ハードウェアSPIかつトランザクション対応時はSPIトランザクションも終了します。全ての
	ディスプレイタイプで必要です。SPI専用の関数ではありません。
*/
void Adafruit_SPITFT::endWrite(void)
{
	if (_cs >= 0)
		SPI_CS_HIGH();
	SPI_END_TRANSACTION();
}

// -------------------------------------------------------------------------
// 低レベルのグラフィックス操作群。
// これらの関数は、呼び出しの前後でチップセレクトやSPIトランザクション（startWrite(), endWrite()）が必要です。
// 高レベルのグラフィックスプリミティブ（例：円やテキスト描画）は、1回のトランザクション開始後に
// これらの関数を複数回呼び出してからトランザクションを終了する場合があります。
// 毎回トランザクションを開始するよりも効率的です。

/*!
	@brief  指定した座標に1ピクセル描画する（低レベル関数）。
			この関数単体では完結せず、事前にstartWrite()を呼び出しておく必要がある。
	@param  x      水平方向の座標（0 = 左端）。
	@param  y      垂直方向の座標（0 = 上端）。
	@param  color  16ビットRGB565フォーマットのピクセル色。
*/
void Adafruit_SPITFT::writePixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height))
	{
		setAddrWindow(x, y, 1, 1);
		SPI_WRITE16(color);
	}
}

/*!
	@brief  ピクセル配列内のバイト順を入れ替えます。リトルエンディアン⇔ビッグエンディアン変換を行います。
			下記のwritePixels()関数内で一部状況下で使用されますが、ユーザーコードでも役立つ場合があります。
	@param  src   16ビットピクセルバッファのソースアドレス。
	@param  len   バイトスワップするピクセル数。
	@param  dest  出力先アドレス（srcと異なる場合のみ指定）。NULL（デフォルト）またはsrcと同じアドレスの場合は、バッファをその場で上書きします。
*/
void Adafruit_SPITFT::swapBytes(uint16_t *src, uint32_t len, uint16_t *dest)
{
	if (!dest)
		dest = src; // NULL -> overwrite src buffer
	for (uint32_t i = 0; i < len; i++)
	{
		dest[i] = __builtin_bswap16(src[i]);
	}
}


/*!
	@brief  メモリ上のピクセル列をディスプレイに送信します。
	@param  colors     16ビットRGB565フォーマットのピクセル値配列へのポインタ。
	@param  len        colors配列内の要素数。
	@param  block      trueの場合（デフォルト）、DMA転送が完了するまで関数がブロックします。DMAが有効でない場合は無視されます。
	falseの場合、DMA転送開始直後に関数が即座に戻り、他のディスプレイ操作（または同じバスを共有する他のSPIデバイスの操作）
	を行う前にdmaWait()関数でDMA完了を待つ必要があります。
	@param  bigEndian  trueの場合、メモリ上のビットマップがビッグエンディアン（上位バイトが先）で格納されていることを
	示します。デフォルトはfalseで、多くのマイコンはリトルエンディアンのため、16ビットピクセル値はディスプレイに送る前に
	バイトスワップが必要です（ディスプレイはSPIや8ビットパラレル時にビッグエンディアンが多い）。アプリケーション側で最適化
	できる場合（例：uint16_t配列内の値がすでにビッグエンディアン順になっている場合）、特に非ブロッキングDMAモードでは高速化が可能です。
	@details 単体では完結せず、事前にstartWrite()およびsetAddrWindow()を呼び出しておく必要があります。
*/
void Adafruit_SPITFT::writePixels(uint16_t *colors, uint32_t len, bool block,
								  bool bigEndian)
{
	if (!len)
		return; // Avoid 0-byte transfers

	// avoid paramater-not-used complaints
	(void)block;
	(void)bigEndian;

#if defined(ESP32)
	if (connection == TFT_HARD_SPI)
	{
		if (!bigEndian)
		{
			hwspi._spi->writePixels(colors, len * 2); // Inbuilt endian-swap
		}
		else
		{
			hwspi._spi->writeBytes((uint8_t *)colors, len * 2); // Issue bytes direct
		}
		return;
	}
#elif defined(ARDUINO_NRF52_ADAFRUIT) && \
	defined(NRF52840_XXAA) // Adafruit nRF52 use SPIM3 DMA at 32Mhz
	if (!bigEndian)
	{
		swapBytes(colors, len); // convert little-to-big endian for display
	}
	hwspi._spi->transfer(colors, NULL, 2 * len); // NULL RX to avoid overwrite
	if (!bigEndian)
	{
		swapBytes(colors, len); // big-to-little endian to restore pixel buffer
	}

	return;
#elif defined(ARDUINO_ARCH_RP2040)
	spi_inst_t *pi_spi = hwspi._spi == &SPI ? spi0 : spi1;

	if (!bigEndian)
	{
		// switch to 16-bit writes
		hw_write_masked(&spi_get_hw(pi_spi)->cr0, 15 << SPI_SSPCR0_DSS_LSB,
						SPI_SSPCR0_DSS_BITS);
		spi_write16_blocking(pi_spi, colors, len);
		// switch back to 8-bit
		hw_write_masked(&spi_get_hw(pi_spi)->cr0, 7 << SPI_SSPCR0_DSS_LSB,
						SPI_SSPCR0_DSS_BITS);
	}
	else
	{
		spi_write_blocking(pi_spi, (uint8_t *)colors, len * 2);
	}
	return;
#elif defined(ARDUINO_ARCH_RTTHREAD)
	if (!bigEndian)
	{
		swapBytes(colors, len); // convert little-to-big endian for display
	}
	hwspi._spi->transfer(colors, 2 * len);
	if (!bigEndian)
	{
		swapBytes(colors, len); // big-to-little endian to restore pixel buffer
	}
	return;
#elif defined(USE_SPI_DMA) && \
	(defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
	if ((connection == TFT_HARD_SPI) || (connection == TFT_PARALLEL))
	{
		int maxSpan = maxFillLen / 2; // One scanline max
		uint8_t pixelBufIdx = 0;	  // Active pixel buffer number
#if defined(__SAMD51__)
		if (connection == TFT_PARALLEL)
		{
			// Switch WR pin to PWM or CCL
			pinPeripheral(tft8._wr, wrPeripheral);
		}
#endif // end __SAMD51__
		if (!bigEndian)
		{ // Normal little-endian situation...
			while (len)
			{
				int count = (len < maxSpan) ? len : maxSpan;

				// Because TFT and SAMD endianisms are different, must swap
				// bytes from the 'colors' array passed into a DMA working
				// buffer. This can take place while the prior DMA transfer
				// is in progress, hence the need for two pixelBufs.
				swapBytes(colors, count, pixelBuf[pixelBufIdx]);
				colors += count;

				// The transfers themselves are relatively small, so we don't
				// need a long descriptor list. We just alternate between the
				// first two, sharing pixelBufIdx for that purpose.
				descriptor[pixelBufIdx].SRCADDR.reg =
					(uint32_t)pixelBuf[pixelBufIdx] + count * 2;
				descriptor[pixelBufIdx].BTCTRL.bit.SRCINC = 1;
				descriptor[pixelBufIdx].BTCNT.reg = count * 2;
				descriptor[pixelBufIdx].DESCADDR.reg = 0;

				while (dma_busy)
					; // Wait for prior line to finish

				// Move new descriptor into place...
				memcpy(dptr, &descriptor[pixelBufIdx], sizeof(DmacDescriptor));
				dma_busy = true;
				dma.startJob(); // Trigger SPI DMA transfer
				if (connection == TFT_PARALLEL)
					dma.trigger();
				pixelBufIdx = 1 - pixelBufIdx; // Swap DMA pixel buffers

				len -= count;
			}
		}
		else
		{ // bigEndian == true
			// With big-endian pixel data, this can be handled as a single
			// DMA transfer using chained descriptors. Even full screen, this
			// needs only a relatively short descriptor list, each
			// transferring a max of 32,767 (not 32,768) pixels. The list
			// was allocated large enough to accommodate a full screen's
			// worth of data, so this won't run past the end of the list.
			int d, numDescriptors = (len + 32766) / 32767;
			for (d = 0; d < numDescriptors; d++)
			{
				int count = (len < 32767) ? len : 32767;
				descriptor[d].SRCADDR.reg = (uint32_t)colors + count * 2;
				descriptor[d].BTCTRL.bit.SRCINC = 1;
				descriptor[d].BTCNT.reg = count * 2;
				descriptor[d].DESCADDR.reg = (uint32_t)&descriptor[d + 1];
				len -= count;
				colors += count;
			}
			descriptor[d - 1].DESCADDR.reg = 0;

			while (dma_busy)
				; // Wait for prior transfer (if any) to finish

			// Move first descriptor into place and start transfer...
			memcpy(dptr, &descriptor[0], sizeof(DmacDescriptor));
			dma_busy = true;
			dma.startJob(); // Trigger SPI DMA transfer
			if (connection == TFT_PARALLEL)
				dma.trigger();
		} // end bigEndian

		lastFillColor = 0x0000; // pixelBuf has been sullied
		lastFillLen = 0;
		if (block)
		{
			while (dma_busy)
				; // Wait for last line to complete
#if defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO)
			if (connection == TFT_HARD_SPI)
			{
				// See SAMD51/21 note in writeColor()
				hwspi._spi->setDataMode(hwspi._mode);
			}
			else
			{
				pinPeripheral(tft8._wr, PIO_OUTPUT); // Switch WR back to GPIO
			}
#endif // end __SAMD51__ || ARDUINO_SAMD_ZERO
		}
		return;
	}
#endif // end USE_SPI_DMA

	// All other cases (bitbang SPI or non-DMA hard SPI or parallel),
	// use a loop with the normal 16-bit data write function:

	if (!bigEndian)
	{
		while (len--)
		{
			SPI_WRITE16(*colors++);
		}
	}
	else
	{
		// Well this is awkward. SPI_WRITE16() was designed for little-endian
		// hosts and big-endian displays as that's nearly always the typical
		// case. If the bigEndian flag was set, data is already in display's
		// order...so each pixel needs byte-swapping before being issued.
		// Rather than having a separate big-endian SPI_WRITE16 (adding more
		// bloat), it's preferred if calling function is smart and only uses
		// bigEndian where DMA is supported. But we gotta handle this...
		while (len--)
		{
			SPI_WRITE16(__builtin_bswap16(*colors++));
		}
	}
}

/*!
	@brief 直前の非ブロッキングwritePixels()呼び出しで開始されたDMA転送の完了を待ちます。
	@details  直前の非ブロッキングwritePixels()呼び出しで開始されたDMA転送の完了を待ちます。DMAが有効でない場合は何もしません。また、ブロッキングwritePixels()（デフォルト）を使用した場合は必要ありません。
*/
void Adafruit_SPITFT::dmaWait(void)
{
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
	while (dma_busy)
		;
#if defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO)
	if (connection == TFT_HARD_SPI)
	{
		// See SAMD51/21 note in writeColor()
		hwspi._spi->setDataMode(hwspi._mode);
	}
	else
	{
		pinPeripheral(tft8._wr, PIO_OUTPUT); // Switch WR back to GPIO
	}
#endif // end __SAMD51__ || ARDUINO_SAMD_ZERO
#endif
}
/*!
	@brief  DMA転送がアクティブかどうかを確認します。DMAが有効でない場合は常にfalseを返します。
	@return DMAが有効かつデータ送信中の場合はtrue、それ以外はfalse。
*/
bool Adafruit_SPITFT::dmaBusy(void) const
{
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
	return dma_busy;
#else
	return false;
#endif
}

/*!
	@brief  同じ色のピクセルを連続して描画します。
	@param  color  16ビットRGB565フォーマットのピクセル色。
	@param  len    描画するピクセル数。
	@details 単体では完結せず、事前にstartWrite()およびsetAddrWindow()を呼び出しておく必要があります。
*/
void Adafruit_SPITFT::writeColor(uint16_t color, uint32_t len)
{
	if (!len)
		return; // Avoid 0-byte transfers

	uint8_t hi = color >> 8, lo = color;

#if defined(ESP32) // ESP32 has a special SPI pixel-writing function...
	if (connection == TFT_HARD_SPI)
	{
#define SPI_MAX_PIXELS_AT_ONCE 32
#define TMPBUF_LONGWORDS (SPI_MAX_PIXELS_AT_ONCE + 1) / 2
#define TMPBUF_PIXELS (TMPBUF_LONGWORDS * 2)
		static uint32_t temp[TMPBUF_LONGWORDS];
		uint32_t c32 = color * 0x00010001;
		uint16_t bufLen = (len < TMPBUF_PIXELS) ? len : TMPBUF_PIXELS, xferLen,
				 fillLen;
		// Fill temp buffer 32 bits at a time
		fillLen = (bufLen + 1) / 2; // Round up to next 32-bit boundary
		for (uint32_t t = 0; t < fillLen; t++)
		{
			temp[t] = c32;
		}
		// Issue pixels in blocks from temp buffer
		while (len)
		{											 // While pixels remain
			xferLen = (bufLen < len) ? bufLen : len; // How many this pass?
			writePixels((uint16_t *)temp, xferLen);
			len -= xferLen;
		}
		return;
	}
#elif defined(ARDUINO_NRF52_ADAFRUIT) && \
	defined(NRF52840_XXAA) // Adafruit nRF52840 use SPIM3 DMA at 32Mhz
	// at most 2 scan lines
	uint32_t const pixbufcount = min(len, ((uint32_t)2 * width()));
	uint16_t *pixbuf = (uint16_t *)rtos_malloc(2 * pixbufcount);

	// use SPI3 DMA if we could allocate buffer, else fall back to writing each
	// pixel loop below
	if (pixbuf)
	{
		uint16_t const swap_color = __builtin_bswap16(color);

		// fill buffer with color
		for (uint32_t i = 0; i < pixbufcount; i++)
		{
			pixbuf[i] = swap_color;
		}

		while (len)
		{
			uint32_t const count = min(len, pixbufcount);
			writePixels(pixbuf, count, true, true);
			len -= count;
		}

		rtos_free(pixbuf);
		return;
	}
#elif defined(ARDUINO_ARCH_RTTHREAD)
	uint16_t pixbufcount;
	uint16_t *pixbuf;
	int16_t lines = height() / 4;
#define QUICKPATH_MAX_LEN 16
	uint16_t quickpath_buffer[QUICKPATH_MAX_LEN];

	do
	{
		pixbufcount = min(len, (lines * width()));
		if (pixbufcount > QUICKPATH_MAX_LEN)
		{
			pixbuf = (uint16_t *)rt_malloc(2 * pixbufcount);
		}
		else
		{
			pixbuf = quickpath_buffer;
		}
		lines -= 2;
	} while (!pixbuf && lines > 0);

	if (pixbuf)
	{
		uint16_t const swap_color = __builtin_bswap16(color);

		while (len)
		{
			uint16_t count = min(len, pixbufcount);
			// fill buffer with color
			for (uint16_t i = 0; i < count; i++)
			{
				pixbuf[i] = swap_color;
			}
			// Don't need to swap color inside the function
			// It has been done outside this function
			writePixels(pixbuf, count, true, true);
			len -= count;
		}
		if (pixbufcount > QUICKPATH_MAX_LEN)
		{
			rt_free(pixbuf);
		}
#undef QUICKPATH_MAX_LEN
		return;
	}
#else // !ESP32
#if defined(USE_SPI_DMA) && (defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO))
	if (((connection == TFT_HARD_SPI) || (connection == TFT_PARALLEL)) &&
		(len >= 16))
	{ // Don't bother with DMA on short pixel runs
		int i, d, numDescriptors;
		if (hi == lo)
		{ // If high & low bytes are same...
			onePixelBuf = color;
			// Can do this with a relatively short descriptor list,
			// each transferring a max of 32,767 (not 32,768) pixels.
			// This won't run off the end of the allocated descriptor list,
			// since we're using much larger chunks per descriptor here.
			numDescriptors = (len + 32766) / 32767;
			for (d = 0; d < numDescriptors; d++)
			{
				int count = (len < 32767) ? len : 32767;
				descriptor[d].SRCADDR.reg = (uint32_t)&onePixelBuf;
				descriptor[d].BTCTRL.bit.SRCINC = 0;
				descriptor[d].BTCNT.reg = count * 2;
				descriptor[d].DESCADDR.reg = (uint32_t)&descriptor[d + 1];
				len -= count;
			}
			descriptor[d - 1].DESCADDR.reg = 0;
		}
		else
		{
			// If high and low bytes are distinct, it's necessary to fill
			// a buffer with pixel data (swapping high and low bytes because
			// TFT and SAMD are different endianisms) and create a longer
			// descriptor list pointing repeatedly to this data. We can do
			// this slightly faster working 2 pixels (32 bits) at a time.
			uint32_t *pixelPtr = (uint32_t *)pixelBuf[0],
					 twoPixels = __builtin_bswap16(color) * 0x00010001;
			// We can avoid some or all of the buffer-filling if the color
			// is the same as last time...
			if (color == lastFillColor)
			{
				// If length is longer than prior instance, fill only the
				// additional pixels in the buffer and update lastFillLen.
				if (len > lastFillLen)
				{
					int fillStart = lastFillLen / 2,
						fillEnd = (((len < maxFillLen) ? len : maxFillLen) + 1) / 2;
					for (i = fillStart; i < fillEnd; i++)
						pixelPtr[i] = twoPixels;
					lastFillLen = fillEnd * 2;
				} // else do nothing, don't set pixels or change lastFillLen
			}
			else
			{
				int fillEnd = (((len < maxFillLen) ? len : maxFillLen) + 1) / 2;
				for (i = 0; i < fillEnd; i++)
					pixelPtr[i] = twoPixels;
				lastFillLen = fillEnd * 2;
				lastFillColor = color;
			}

			numDescriptors = (len + maxFillLen - 1) / maxFillLen;
			for (d = 0; d < numDescriptors; d++)
			{
				int pixels = (len < maxFillLen) ? len : maxFillLen, bytes = pixels * 2;
				descriptor[d].SRCADDR.reg = (uint32_t)pixelPtr + bytes;
				descriptor[d].BTCTRL.bit.SRCINC = 1;
				descriptor[d].BTCNT.reg = bytes;
				descriptor[d].DESCADDR.reg = (uint32_t)&descriptor[d + 1];
				len -= pixels;
			}
			descriptor[d - 1].DESCADDR.reg = 0;
		}
		memcpy(dptr, &descriptor[0], sizeof(DmacDescriptor));
#if defined(__SAMD51__)
		if (connection == TFT_PARALLEL)
		{
			// Switch WR pin to PWM or CCL
			pinPeripheral(tft8._wr, wrPeripheral);
		}
#endif // end __SAMD51__

		dma_busy = true;
		dma.startJob();
		if (connection == TFT_PARALLEL)
			dma.trigger();
		while (dma_busy)
			; // Wait for completion
			  // Unfortunately blocking is necessary. An earlier version returned
			  // immediately and checked dma_busy on startWrite() instead, but it
			  // turns out to be MUCH slower on many graphics operations (as when
			  // drawing lines, pixel-by-pixel), perhaps because it's a volatile
			  // type and doesn't cache. Working on this.
#if defined(__SAMD51__) || defined(ARDUINO_SAMD_ZERO)
		if (connection == TFT_HARD_SPI)
		{
			// SAMD51: SPI DMA seems to leave the SPI peripheral in a freaky
			// state on completion. Workaround is to explicitly set it back...
			// (5/17/2019: apparently SAMD21 too, in certain cases, observed
			// with ST7789 display.)
			hwspi._spi->setDataMode(hwspi._mode);
		}
		else
		{
			pinPeripheral(tft8._wr, PIO_OUTPUT); // Switch WR back to GPIO
		}
#endif // end __SAMD51__
		return;
	}
#endif // end USE_SPI_DMA
#endif // end !ESP32

	// All other cases (non-DMA hard SPI, bitbang SPI, parallel)...

	if (connection == TFT_HARD_SPI)
	{
#if defined(ESP8266)
		do
		{
			uint32_t pixelsThisPass = len;
			if (pixelsThisPass > 50000)
				pixelsThisPass = 50000;
			len -= pixelsThisPass;
			delay(1); // Periodic delay on long fills
			while (pixelsThisPass--)
			{
				hwspi._spi->write(hi);
				hwspi._spi->write(lo);
			}
		} while (len);
#elif defined(ARDUINO_ARCH_RP2040)
		spi_inst_t *pi_spi = hwspi._spi == &SPI ? spi0 : spi1;
		color = __builtin_bswap16(color);

		while (len--)
			spi_write_blocking(pi_spi, (uint8_t *)&color, 2);
#else // !ESP8266 && !ARDUINO_ARCH_RP2040
		while (len--)
		{
#if defined(__AVR__)
			AVR_WRITESPI(hi);
			AVR_WRITESPI(lo);
#elif defined(ESP32)
			hwspi._spi->write(hi);
			hwspi._spi->write(lo);
#else
			hwspi._spi->transfer(hi);
			hwspi._spi->transfer(lo);
#endif
		}
#endif // end !ESP8266
	}
	else if (connection == TFT_SOFT_SPI)
	{
#if defined(ESP8266)
		do
		{
			uint32_t pixelsThisPass = len;
			if (pixelsThisPass > 20000)
				pixelsThisPass = 20000;
			len -= pixelsThisPass;
			yield(); // Periodic yield() on long fills
			while (pixelsThisPass--)
			{
				for (uint16_t bit = 0, x = color; bit < 16; bit++)
				{
					if (x & 0x8000)
						SPI_MOSI_HIGH();
					else
						SPI_MOSI_LOW();
					SPI_SCK_HIGH();
					SPI_SCK_LOW();
					x <<= 1;
				}
			}
		} while (len);
#else // !ESP8266
		while (len--)
		{
#if defined(__AVR__)
			for (uint8_t bit = 0, x = hi; bit < 8; bit++)
			{
				if (x & 0x80)
					SPI_MOSI_HIGH();
				else
					SPI_MOSI_LOW();
				SPI_SCK_HIGH();
				SPI_SCK_LOW();
				x <<= 1;
			}
			for (uint8_t bit = 0, x = lo; bit < 8; bit++)
			{
				if (x & 0x80)
					SPI_MOSI_HIGH();
				else
					SPI_MOSI_LOW();
				SPI_SCK_HIGH();
				SPI_SCK_LOW();
				x <<= 1;
			}
#else  // !__AVR__
			for (uint16_t bit = 0, x = color; bit < 16; bit++)
			{
				if (x & 0x8000)
					SPI_MOSI_HIGH();
				else
					SPI_MOSI_LOW();
				SPI_SCK_HIGH();
				x <<= 1;
				SPI_SCK_LOW();
			}
#endif // end !__AVR__
		}
#endif // end !ESP8266
	}
	else
	{ // PARALLEL
		if (hi == lo)
		{
#if defined(__AVR__)
			len *= 2;
			*tft8.writePort = hi;
			while (len--)
			{
				TFT_WR_STROBE();
			}
#elif defined(USE_FAST_PINIO)
			if (!tft8.wide)
			{
				len *= 2;
				*tft8.writePort = hi;
			}
			else
			{
				*(volatile uint16_t *)tft8.writePort = color;
			}
			while (len--)
			{
				TFT_WR_STROBE();
			}
#endif
		}
		else
		{
			while (len--)
			{
#if defined(__AVR__)
				*tft8.writePort = hi;
				TFT_WR_STROBE();
				*tft8.writePort = lo;
#elif defined(USE_FAST_PINIO)
				if (!tft8.wide)
				{
					*tft8.writePort = hi;
					TFT_WR_STROBE();
					*tft8.writePort = lo;
				}
				else
				{
					*(volatile uint16_t *)tft8.writePort = color;
				}
#endif
				TFT_WR_STROBE();
			}
		}
	}
}

/*!
	@brief  ディスプレイに塗りつぶし矩形を描画します。

	@param  x      左上隅の水平座標。
	@param  y      左上隅の垂直座標。
	@param  w      矩形の幅（ピクセル単位、正なら右方向、負なら左方向）。
	@param  h      矩形の高さ（ピクセル単位、正なら下方向、負なら上方向）。
	@param  color  16ビットRGB565フォーマットの塗りつぶし色。
	@details 単体では完結せず、startWrite()の後に呼び出す必要があります。
			通常は高レベルのグラフィックスプリミティブから使用され、ユーザーコードが直接呼び出す必要はありません。
			ユーザーは通常、自己完結型のfillRect()を使うことになります。
			writeFillRect()は自身でクリッピングや矩形の範囲外判定を行います。
			より低レベルな実装についてはwriteFillRectPreclipped()を参照してください。
	@note   C言語の最適化の都合で、if分岐側を優先するため、このような深いネスト構造になっています。
			これにより、不要な分岐を避け、できるだけ早い段階で範囲外の矩形を除外します。
*/
void Adafruit_SPITFT::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
									uint16_t color)
{
	if (w && h)
	{ // Nonzero width and height?
		if (w < 0)
		{				// If negative width...
			x += w + 1; //   Move X to left edge
			w = -w;		//   Use positive width
		}
		if (x < _width)
		{ // Not off right
			if (h < 0)
			{				// If negative height...
				y += h + 1; //   Move Y to top edge
				h = -h;		//   Use positive height
			}
			if (y < _height)
			{ // Not off bottom
				int16_t x2 = x + w - 1;
				if (x2 >= 0)
				{ // Not off left
					int16_t y2 = y + h - 1;
					if (y2 >= 0)
					{ // Not off top
						// Rectangle partly or fully overlaps screen
						if (x < 0)
						{
							x = 0;
							w = x2 + 1;
						} // Clip left
						if (y < 0)
						{
							y = 0;
							h = y2 + 1;
						} // Clip top
						if (x2 >= _width)
						{
							w = _width - x;
						} // Clip right
						if (y2 >= _height)
						{
							h = _height - y;
						} // Clip bottom
						writeFillRectPreclipped(x, y, w, h, color);
					}
				}
			}
		}
	}
}

/*!
	@brief  ディスプレイに水平線を描画します。
	@details 端のクリッピングや範囲外判定を行います。単体では完結せず、startWrite()の後に呼び出す必要があります。
			通常は高レベルのグラフィックスプリミティブから使用され、ユーザーコードが直接呼び出す必要はありません。ユーザーは通常、自己完結型のdrawFastHLine()を使うことになります。
	@param  x      線分の始点の水平座標。
	@param  y      線分の始点の垂直座標。
	@param  w      線分の幅（ピクセル単位、正なら右方向、負なら左方向）。
	@param  color  16ビットRGB565フォーマットの線色。
*/
void inline Adafruit_SPITFT::writeFastHLine(int16_t x, int16_t y, int16_t w,
											uint16_t color)
{
	if ((y >= 0) && (y < _height) && w)
	{ // Y on screen, nonzero width
		if (w < 0)
		{				// If negative width...
			x += w + 1; //   Move X to left edge
			w = -w;		//   Use positive width
		}
		if (x < _width)
		{ // Not off right
			int16_t x2 = x + w - 1;
			if (x2 >= 0)
			{ // Not off left
				// Line partly or fully overlaps screen
				if (x < 0)
				{
					x = 0;
					w = x2 + 1;
				} // Clip left
				if (x2 >= _width)
				{
					w = _width - x;
				} // Clip right
				writeFillRectPreclipped(x, y, w, 1, color);
			}
		}
	}
}


/*!
	@brief  ディスプレイに垂直線を描画します。
	@details 端のクリッピングや範囲外判定を行います。単体では完結せず、startWrite()の後に呼び出す必要があります。
			通常は高レベルのグラフィックスプリミティブから使用され、ユーザーコードが直接呼び出す必要はありません。ユーザーは通常、自己完結型のdrawFastVLine()を使うことになります。
	@param  x      線分の始点の水平座標。
	@param  y      線分の始点の垂直座標。
	@param  h      線分の高さ（ピクセル単位、正なら下方向、負なら上方向）。
	@param  color  16ビットRGB565フォーマットの線色。
*/

void inline Adafruit_SPITFT::writeFastVLine(int16_t x, int16_t y, int16_t h,
											uint16_t color)
{
	if ((x >= 0) && (x < _width) && h)
	{ // X on screen, nonzero height
		if (h < 0)
		{				// If negative height...
			y += h + 1; //   Move Y to top edge
			h = -h;		//   Use positive height
		}
		if (y < _height)
		{ // Not off bottom
			int16_t y2 = y + h - 1;
			if (y2 >= 0)
			{ // Not off top
				// Line partly or fully overlaps screen
				if (y < 0)
				{
					y = 0;
					h = y2 + 1;
				} // Clip top
				if (y2 >= _height)
				{
					h = _height - y;
				} // Clip bottom
				writeFillRectPreclipped(x, y, 1, h, color);
			}
		}
	}
}

/*!
	@brief  writeFillRect() の下位レベル版です。
	@details この関数は、すべての引数が画面内であり、幅・高さが正で、矩形が画面外にはみ出さないことを前提とします。
	端のクリッピングや範囲外判定は一切行いません。高レベルのグラフィックスプリミティブ側でクリッピング処理を済ませておけば、
	下位レベル関数での不要な関数呼び出しやクリッピング処理を省略でき、効率化できます。
	@param  x      左上隅の水平座標。必ず画面内である必要があります。
	@param  y      左上隅の垂直座標。必ず画面内である必要があります。
	@param  w      矩形の幅（ピクセル単位）。必ず正で、画面外にはみ出さないこと。
	@param  h      矩形の高さ（ピクセル単位）。必ず正で、画面外にはみ出さないこと。
	@param  color  16ビットRGB565フォーマットの塗りつぶし色。
	@note   この関数は新規追加されたもので、現時点では矩形や水平・垂直線以外のグラフィックスプリミティブは最適に利用していません。
*/
inline void Adafruit_SPITFT::writeFillRectPreclipped(int16_t x, int16_t y,
													 int16_t w, int16_t h,
													 uint16_t color)
{
	setAddrWindow(x, y, w, h);
	writeColor(color, (uint32_t)w * h);
}

// -------------------------------------------------------------------------
// わずかに高レベルなグラフィックス操作群。
// 上記の 'write' 系関数と似ているが、これらは必要に応じて自前でチップセレクトやSPIトランザクション
// （startWrite(), endWrite()）を行う。通常は単独で使われるグラフィックスプリミティブであり、
// より高レベルなプリミティブから呼び出されるべきではない（パフォーマンス向上のため、上記の関数を使うべき）。

/*!
	@brief  指定座標に1ピクセル描画する（高レベル関数）。
	@details 必要に応じて自前でトランザクションを開始・終了する。（より低レベルなバリアントは writePixel(x, y, color) を参照）。
			この関数では端のクリッピングも行う。
	@param  x      水平方向の座標（0 = 左端）。
	@param  y      垂直方向の座標（0 = 上端）。
	@param  color  16ビットRGB565フォーマットのピクセル色。
*/
void Adafruit_SPITFT::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	// Clip first...
	if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height))
	{
		// THEN set up transaction (if needed) and draw...
		startWrite();
		setAddrWindow(x, y, 1, 1);
		SPI_WRITE16(color);
		endWrite();
	}
}

/*!
	@brief  ディスプレイに塗りつぶし矩形を描画します。
	@details 自己完結型で、必要に応じてトランザクションも自前で行います（下位レベルのバリアントはwriteFillRect()やwriteFillRectPreclipped()を参照）。
	ここで端のクリッピングや範囲外判定も行います。
	@param  x      左上隅の水平座標。
	@param  y      左上隅の垂直座標。
	@param  w      矩形の幅（ピクセル単位、正なら右方向、負なら左方向）。
	@param  h      矩形の高さ（ピクセル単位、正なら下方向、負なら上方向）。
	@param  color  16ビットRGB565フォーマットの塗りつぶし色。
	@note   この関数はwriteFillRect()の処理をほぼそのまま繰り返していますが、トランザクションの開始・終了を追加しています。
	これは、トランザクションを開始してwriteFillRect()を呼び出す方式では、矩形が範囲外で除外された場合でもトランザクションが
	発生してしまうためです。実際にはそれほど多くのコードではありません。
*/
void Adafruit_SPITFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
							   uint16_t color)
{
	if (w && h)
	{ // Nonzero width and height?
		if (w < 0)
		{				// If negative width...
			x += w + 1; //   Move X to left edge
			w = -w;		//   Use positive width
		}
		if (x < _width)
		{ // Not off right
			if (h < 0)
			{				// If negative height...
				y += h + 1; //   Move Y to top edge
				h = -h;		//   Use positive height
			}
			if (y < _height)
			{ // Not off bottom
				int16_t x2 = x + w - 1;
				if (x2 >= 0)
				{ // Not off left
					int16_t y2 = y + h - 1;
					if (y2 >= 0)
					{ // Not off top
						// Rectangle partly or fully overlaps screen
						if (x < 0)
						{
							x = 0;
							w = x2 + 1;
						} // Clip left
						if (y < 0)
						{
							y = 0;
							h = y2 + 1;
						} // Clip top
						if (x2 >= _width)
						{
							w = _width - x;
						} // Clip right
						if (y2 >= _height)
						{
							h = _height - y;
						} // Clip bottom
						startWrite();
						writeFillRectPreclipped(x, y, w, h, color);
						endWrite();
					}
				}
			}
		}
	}
}

/*!
	@brief  ディスプレイに水平線を描画します。
	@details 自己完結型で、必要に応じてトランザクションも自前で行います（下位レベルのバリアントはwriteFastHLine()を参照）。ここで端のクリッピングや範囲外判定も行います。
	@param  x      線分の始点の水平座標。
	@param  y      線分の始点の垂直座標。
	@param  w      線分の幅（ピクセル単位、正なら右方向、負なら左方向）。
	@param  color  16ビットRGB565フォーマットの線色。
	@note   この関数はwriteFastHLine()の処理をほぼそのまま繰り返していますが、
	トランザクションの開始・終了を追加しています。これは、トランザクションを開始してwriteFastHLine()
	を呼び出す方式では、線分が範囲外で除外された場合でもトランザクションが発生してしまうためです。
*/
void Adafruit_SPITFT::drawFastHLine(int16_t x, int16_t y, int16_t w,
									uint16_t color)
{
	if ((y >= 0) && (y < _height) && w)
	{ // Y on screen, nonzero width
		if (w < 0)
		{				// If negative width...
			x += w + 1; //   Move X to left edge
			w = -w;		//   Use positive width
		}
		if (x < _width)
		{ // Not off right
			int16_t x2 = x + w - 1;
			if (x2 >= 0)
			{ // Not off left
				// Line partly or fully overlaps screen
				if (x < 0)
				{
					x = 0;
					w = x2 + 1;
				} // Clip left
				if (x2 >= _width)
				{
					w = _width - x;
				} // Clip right
				startWrite();
				writeFillRectPreclipped(x, y, w, 1, color);
				endWrite();
			}
		}
	}
}

/*!
/*!
	@brief  ディスプレイに垂直線を描画します。
	@details 自己完結型で、必要に応じてトランザクションも自前で行います（下位レベルのバリアントはwriteFastVLine()を参照）。ここで端のクリッピングや範囲外判定も行います。
	@param  x      線分の始点の水平座標。
	@param  y      線分の始点の垂直座標。
	@param  h      線分の高さ（ピクセル単位、正なら下方向、負なら上方向）。
	@param  color  16ビットRGB565フォーマットの線色。
	@note   この関数はwriteFastVLine()の処理をほぼそのまま繰り返していますが、
	トランザクションの開始・終了を追加しています。これは、トランザクションを開始してwriteFastVLine()を呼び出す方式では、
	線分が範囲外で除外された場合でもトランザクションが発生してしまうためです。
*/
void Adafruit_SPITFT::drawFastVLine(int16_t x, int16_t y, int16_t h,
									uint16_t color)
{
	if ((x >= 0) && (x < _width) && h)
	{ // X on screen, nonzero height
		if (h < 0)
		{				// If negative height...
			y += h + 1; //   Move Y to top edge
			h = -h;		//   Use positive height
		}
		if (y < _height)
		{ // Not off bottom
			int16_t y2 = y + h - 1;
			if (y2 >= 0)
			{ // Not off top
				// Line partly or fully overlaps screen
				if (y < 0)
				{
					y = 0;
					h = y2 + 1;
				} // Clip top
				if (y2 >= _height)
				{
					h = _height - y;
				} // Clip bottom
				startWrite();
				writeFillRectPreclipped(x, y, 1, h, color);
				endWrite();
			}
		}
	}
}

/*!
	@brief  実質的にはwritePixel()にトランザクション処理を追加したもの。
	@detals 現在この関数はAdafruit公式のコードでは使われていないと思われます（古いBMP画像読み込み例で使われていた可能性があります）
	が、ユーザーコードで依存している場合に備えて残されています。非推奨（DEPRECATED）扱いです。
	@param  color  16ビットRGB565フォーマットのピクセル色。
*/
void Adafruit_SPITFT::pushColor(uint16_t color)
{
	startWrite();
	SPI_WRITE16(color);
	endWrite();
}

/*!
	@brief  指定した(x, y)位置に16ビット画像（565 RGB）を描画します。
	@param  x        左上隅の水平座標。
	@param  y        左上隅の垂直座標。
	@param  pcolors  表示する画像データのポインタ（ワード配列）。
	@param  w        ビットマップの幅（ピクセル単位）。
	@param  h        ビットマップの高さ（ピクセル単位）。
	@details 16ビット表示デバイス用で、色の減色は行いません。
			https://github.com/PaulStoffregen/ILI9341_t3 の実装を
			Marc MERLIN氏が移植したものです。examples/pictureEmbed
			で使い方例があります。
			2017年5月6日：関数名と引数が現行GFXライブラリとの互換性や
			過去実装との命名衝突回避のため変更されました。
			以前はdrawBitmap()という名前で引数順も異なっていました。
			この関数は自身でトランザクション開始・終了や端のクリッピング・
			範囲外除外も行います。
			(HN)このルーチンの工夫が面白い点として、ウインドウを指定して
			ピクセルデータを連続出力していますが、クリッピング処理も
			行っており、画像左上座標が負の場合でも画面内の必要部分だけ
			描画されます。そのため、pColorsで描画データの開始点を
			最初に計算し、一気に全データを出すのではなく、一行ずつ
			分割して出力しています。多少効率は落ちますが、画面外に
			画像が描画されてしまうことがなくなります。
*/
void Adafruit_SPITFT::drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h)
{
	int16_t x2, y2;					// Lower-right coord
	if ((x >= _width) ||			// Off-edge right
		(y >= _height) ||			// " top
		((x2 = (x + w - 1)) < 0) || // " left
		((y2 = (y + h - 1)) < 0))
		return; // " bottom

	int16_t bx1 = 0, by1 = 0, // Clipped top-left within bitmap
		saveW = w;			  // Save original bitmap width value
	if (x < 0)
	{ // Clip left
		w += x;
		bx1 = -x;
		x = 0;
	}
	if (y < 0)
	{ // Clip top
		h += y;
		by1 = -y;
		y = 0;
	}
	if (x2 >= _width)
		w = _width - x; // Clip right
	if (y2 >= _height)
		h = _height - y; // Clip bottom

	pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
	startWrite();
	setAddrWindow(x, y, w, h); // Clipped area

	while (h--)
	{							 // For each (clipped) scanline...
		writePixels(pcolors, w); // Push one (clipped) row
		pcolors += saveW;		 // Advance pointer by one full (unclipped) line
	}
	endWrite();
}

/*!
	@brief  デバイス依存の機能（描画ウインドウ）を使って、８ビットのビットマップ（透過なし）
			を描画する。8Bit色（RRRGGGBBフォーマット)は、RGB565に変換して描画される。
	@param  x        Top left corner horizontal coordinate.
	@param  y        Top left corner vertical coordinate.
	@param  pcolors  表示する画像データのポインタ。画像データはバイトマップ
	@param  w        Width of bitmap in pixels.
	@param  h        Height of bitmap in pixels.
*/
void Adafruit_SPITFT::drawRGBBitmap(int16_t x, int16_t y, uint8_t *pcolors, int16_t w, int16_t h)
{
	int16_t x2, y2;					// Lower-right coord
	if ((x >= _width) ||			// Off-edge right
		(y >= _height) ||			// " top
		((x2 = (x + w - 1)) < 0) || // " left
		((y2 = (y + h - 1)) < 0))
		return; // " bottom

	int16_t bx1 = 0, by1 = 0, // Clipped top-left within bitmap
		saveW = w;			  // Save original bitmap width value
	if (x < 0)
	{ // Clip left
		w += x;
		bx1 = -x;
		x = 0;
	}
	if (y < 0)
	{ // Clip top
		h += y;
		by1 = -y;
		y = 0;
	}
	if (x2 >= _width)
		w = _width - x; // Clip right
	if (y2 >= _height)
		h = _height - y; // Clip bottom

	pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
	startWrite();
	setAddrWindow(x, y, w, h); // Clipped area
	// 一行ずつ出力しているので、それに合わせる。
	//
	while (h--)
	{ // For each (clipped) scanline...
		uint16_t *cvtBuf = (uint16_t *)alloca(w * 2);
		uint16_t *pWk = cvtBuf;
		for (int i = 0; i < w; i++)
		{
			*pWk = convert8To565(*(pcolors + i));
			pWk++;
		}
		writePixels(cvtBuf, w); // Push one (clipped) row
		pcolors += saveW;		// Advance pointer by one full (unclipped) line
	}
	endWrite();
}

/*!
	@brief  DMAを使って、画面全体を描画する。
	@param  pcolors  表示する画像データのポインタ。画像データはワードマップ
	@note  没？　試してみたが、１画面の更新について、普通に描画するのより遅かった
			DMAでは、Little Endianにする必要があるので、最初に全画面分上位下位を入れ替えている。
			DMA 109.6 μs (上位下位のスワップをやめても、77.0 μs)
			通常描画  66.0 μs

*/
void Adafruit_SPITFT::drawDMABitmap(const uint16_t *pcolors)
{
	uint16_t chgEndien[WIDTH * HEIGHT];

	for (int i = 0; i < WIDTH * HEIGHT; i++)
	{
		chgEndien[i] = __builtin_bswap16(*(pcolors + i));
	}

	startWrite();
	setAddrWindow(0, 0, width(), height()); // Clipped area
	const uint dma_tx = dma_claim_unused_channel(true);
	dma_channel_config c = dma_channel_get_default_config(dma_tx);
	channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
	spi_inst_t *currentSPI = hwspi._spi == &SPI ? spi0 : spi1;
	channel_config_set_dreq(&c, spi_get_dreq(currentSPI, true));
	channel_config_set_write_increment(&c, false);
	volatile void *pWriteAddr = &spi_get_hw(currentSPI)->dr;
	dma_channel_configure(dma_tx, &c,
						  pWriteAddr,		  // write address
						  chgEndien,		  // read address
						  WIDTH * HEIGHT * 2, // element count (each element is of size transfer_data_size)
						  false);			  // don't start yet
	dma_start_channel_mask(1u << dma_tx);
	dma_channel_wait_for_finish_blocking(dma_tx); // DMA転送完了を待機
	dma_channel_unclaim(dma_tx);
	endWrite();
}

/// @brief デバイス依存の機能（描画ウインドウ）を使って、白黒ﾋﾞｯﾄﾏｯﾌﾟを表示する
/// @param x 表示するX座標
/// @param y 表示するY座標
/// @param pcolors 表示する画像データのポインタ。画像データはビットマップ。
/// @param w 表示する画像の幅
/// @param h 表示する画像の高さ

void Adafruit_SPITFT::drawBWBitmap(int16_t x, int16_t y, uint8_t *pcolors, int16_t w, int16_t h)
{
	int16_t x2, y2;					// Lower-right coord
	if ((x >= _width) ||			// Off-edge right
		(y >= _height) ||			// " top
		((x2 = (x + w - 1)) < 0) || // " left
		((y2 = (y + h - 1)) < 0))
		return; // " bottom

	int16_t bx1 = 0, by1 = 0, // Clipped top-left within bitmap
		saveW = w;			  // Save original bitmap width value
	if (x < 0)
	{ // Clip left
		w += x;
		bx1 = -x;
		x = 0;
	}
	if (y < 0)
	{ // Clip top
		h += y;
		by1 = -y;
		y = 0;
	}
	if (x2 >= _width)
		w = _width - x; // Clip right
	if (y2 >= _height)
		h = _height - y; // Clip bottom

	unsigned int saveWBytes = (saveW + 7) / 8;
	unsigned int bx1Bytes = (bx1 + 7) / 8;

	pcolors += by1 * saveWBytes + bx1Bytes; // Offset bitmap ptr to clipped top-left
	startWrite();
	setAddrWindow(x, y, w, h); // Clipped area
	// 一行ずつ出力しているので、それに合わせる。
	// これだと、縦長の矩形で効率が悪いと思うのだが。
	uint16_t Xbytes = ((w + 7) / 8);

	while (h--)
	{ // For each (clipped) scanline...
		uint16_t *cvtBuf = (uint16_t *)alloca(w * 2);
		uint16_t *pWk = cvtBuf;
		for (int i = 0; i < Xbytes; i++)
		{
			uint8_t bits = *(pcolors + i);
			for (int j = 0; j < 8; j++)
			{
				if (bits & 0x80)
				{
					*pWk = 0xFFFF; // 白
				}
				else
				{
					*pWk = 0x0000; // 黒
				}
				bits <<= 1;
				pWk++;
			}
		}
		writePixels(cvtBuf, w); // Push one (clipped) row
		pcolors += saveWBytes;	// Advance pointer by one full (unclipped) line
	}
	endWrite();
}

/*!
	@brief  透過色を指定してビットマップを描画する。
			透過色は、描画するビットマップの中に含まれる色で、描画しない色。
			この場合は、ウインドウを使えないので１ドットづつ書いていくしか方法がない。
	@param  x        Top left corner horizontal coordinate.
	@param  y        Top left corner vertical coordinate.
	@param  pcolors  表示する画像データのポインタ。画像データはワードマップ
	@param  w        Width of bitmap in pixels.
	@param  h        Height of bitmap in pixels.
	@param  colorTransparent  透過色
*/
void Adafruit_SPITFT::drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h, uint16_t colorTransparent)
{
	int16_t x2, y2;					// Lower-right coord
	if ((x >= _width) ||			// Off-edge right
		(y >= _height) ||			// " top
		((x2 = (x + w - 1)) < 0) || // " left
		((y2 = (y + h - 1)) < 0))
		return; // " bottom

	int16_t bx1 = 0, by1 = 0, // Clipped top-left within bitmap
		saveW = w;			  // Save original bitmap width value
	if (x < 0)
	{ // Clip left
		w += x;
		bx1 = -x;
		x = 0;
	}
	if (y < 0)
	{ // Clip top
		h += y;
		by1 = -y;
		y = 0;
	}
	if (x2 >= _width)
		w = _width - x; // Clip right
	if (y2 >= _height)
		h = _height - y; // Clip bottom

	pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
	startWrite();

	for (int iy = y; iy < (y + h); iy++)
	{
		for (int ix = x; ix < (x + w); ix++)
		{
			uint32_t pictIdx = (by1 + (iy - y)) * saveW + bx1 + (ix - x);
			uint16_t color = pcolors[pictIdx];
			if (color != colorTransparent)
			{
				drawPixel(ix, iy, color);
			}
		}
	}
	endWrite();
}

// -------------------------------------------------------------------------
// Miscellaneous class member functions that don't draw anything.

/*!
	@brief  ディスプレイの色反転を行います（ハードウェアが対応している場合）。
			この関数は自己完結型で、トランザクションのセットアップは不要です。
	@param  i  true = 反転表示、false = 通常表示。
*/
void Adafruit_SPITFT::invertDisplay(bool i)
{
	startWrite();
	writeCommand(i ? invertOnCommand : invertOffCommand);
	endWrite();
}

/*!
	@brief   8ビットの赤・緑・青の値から、'565' RGBフォーマット（赤5ビット、緑6ビット、青5ビット）の「パックされた」16ビットカラー値を返します。これは単なる数値演算であり、ハードウェアには一切アクセスしません。
	@param   red    8ビットの赤の明るさ（0 = 消灯、255 = 最大）。
	@param   green  8ビットの緑の明るさ（0 = 消灯、255 = 最大）。
	@param   blue   8ビットの青の明るさ（0 = 消灯、255 = 最大）。
	@return  「パックされた」16ビットカラー値（565フォーマット）。
*/
uint16_t Adafruit_SPITFT::color565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

/*!
	@brief   Adafruit_SPITFTのsendCommandは、コマンドとデータの送信をまとめて処理します。
	@param   commandByte       送信するコマンドバイト
	@param   dataBytes         送信するデータバイトへのポインタ
	@param   numDataBytes      送信するデータバイト数
*/
void Adafruit_SPITFT::sendCommand(uint8_t commandByte, uint8_t *dataBytes,
								  uint8_t numDataBytes)
{
	SPI_BEGIN_TRANSACTION();
	if (_cs >= 0)
		SPI_CS_LOW();

	SPI_DC_LOW();		   // Command mode
	spiWrite(commandByte); // Send the command byte

	SPI_DC_HIGH();
	for (int i = 0; i < numDataBytes; i++)
	{
		if ((connection == TFT_PARALLEL) && tft8.wide)
		{
			SPI_WRITE16(*(uint16_t *)dataBytes);
			dataBytes += 2;
		}
		else
		{
			spiWrite(*dataBytes); // Send the data bytes
			dataBytes++;
		}
	}

	if (_cs >= 0)
		SPI_CS_HIGH();
	SPI_END_TRANSACTION();
}

/*!
 @brief   Adafruit_SPITFTのSend Commandは、コマンドとデータの送信をまとめて処理します。
 @param   commandByte       送信するコマンドバイト
 @param   dataBytes         送信するデータバイトへのポインタ
 @param   numDataBytes      送信するデータバイト数
 */
void Adafruit_SPITFT::sendCommand(uint8_t commandByte, const uint8_t *dataBytes,
								  uint8_t numDataBytes)
{
	SPI_BEGIN_TRANSACTION();
	if (_cs >= 0)
		SPI_CS_LOW();

	SPI_DC_LOW();		   // Command mode
	spiWrite(commandByte); // Send the command byte

	SPI_DC_HIGH();
	for (int i = 0; i < numDataBytes; i++)
	{
		if ((connection == TFT_PARALLEL) && tft8.wide)
		{
			SPI_WRITE16(*(uint16_t *)dataBytes);
			dataBytes += 2;
		}
		else
		{
			spiWrite(pgm_read_byte(dataBytes++));
		}
	}

	if (_cs >= 0)
		SPI_CS_HIGH();
	SPI_END_TRANSACTION();
}

/*!
 @brief 16ビットパラレルディスプレイ向けにコマンドとデータの送信をまとめて処理します。
 @param  commandWord   送信するコマンドワード（16ビット）
 @param  dataBytes     送信するデータバイトへのポインタ
 @param  numDataBytes  送信するデータバイト数
 @details 現在はNT35510向けの特殊な実装になっており、コマンドは16ビット、データは8ビット値（16ビットバスでも上位バイトは常に0）
 として送信する必要があります。また、各転送ごとにアドレスの発行とインクリメントも必要なようです。

 */
void Adafruit_SPITFT::sendCommand16(uint16_t commandWord,
									const uint8_t *dataBytes,
									uint8_t numDataBytes)
{
	SPI_BEGIN_TRANSACTION();
	if (_cs >= 0)
		SPI_CS_LOW();

	if (numDataBytes == 0)
	{
		SPI_DC_LOW();			  // Command mode
		SPI_WRITE16(commandWord); // Send the command word
		SPI_DC_HIGH();			  // Data mode
	}
	for (int i = 0; i < numDataBytes; i++)
	{
		SPI_DC_LOW();			  // Command mode
		SPI_WRITE16(commandWord); // Send the command word
		SPI_DC_HIGH();			  // Data mode
		commandWord++;
		SPI_WRITE16((uint16_t)pgm_read_byte(dataBytes++));
	}

	if (_cs >= 0)
		SPI_CS_HIGH();
	SPI_END_TRANSACTION();
}

/*!
 @brief   ディスプレイ設定メモリ（RAMではない）から8ビットデータを読み出します。
		  この機能はほとんどドキュメント化・サポートされておらず、使用は推奨されません。
		  一部のサンプルで使われているため関数として残されています。
 @param   commandByte  データを読み出すコマンドレジスタ。
 @param   index  コマンド内のどのバイトを読み出すかのインデックス。
 @return  ディスプレイレジスタから読み出した8ビットの符号なしデータ。
*/
/**************************************************************************/
uint8_t Adafruit_SPITFT::readcommand8(uint8_t commandByte, uint8_t index)
{
	uint8_t result;
	startWrite();
	SPI_DC_LOW(); // Command mode
	spiWrite(commandByte);
	SPI_DC_HIGH(); // Data mode
	do
	{
		result = spiRead();
	} while (index--); // Discard bytes up to index'th
	endWrite();
	return result;
}

/*!
 @brief   ディスプレイレジスタから16ビットデータを読み出します。
		  16ビットパラレルディスプレイ専用です。
 @param   addr  アクセスするコマンド／レジスタ。
 @return  読み出した16ビット符号なしデータ。
*/
uint16_t Adafruit_SPITFT::readcommand16(uint16_t addr)
{
#if defined(USE_FAST_PINIO) // NOT SUPPORTED without USE_FAST_PINIO
	uint16_t result = 0;
	if ((connection == TFT_PARALLEL) && tft8.wide)
	{
		startWrite();
		SPI_DC_LOW(); // Command mode
		SPI_WRITE16(addr);
		SPI_DC_HIGH(); // Data mode
		TFT_RD_LOW();  // Read line LOW
#if defined(HAS_PORT_SET_CLR)
		*(volatile uint16_t *)tft8.dirClr = 0xFFFF;	  // Input state
		result = *(volatile uint16_t *)tft8.readPort; // 16-bit read
		*(volatile uint16_t *)tft8.dirSet = 0xFFFF;	  // Output state
#else												  // !HAS_PORT_SET_CLR
		*(volatile uint16_t *)tft8.portDir = 0x0000;  // Input state
		result = *(volatile uint16_t *)tft8.readPort; // 16-bit read
		*(volatile uint16_t *)tft8.portDir = 0xFFFF;  // Output state
#endif												  // end !HAS_PORT_SET_CLR
		TFT_RD_HIGH();								  // Read line HIGH
		endWrite();
	}
	return result;
#else
	(void)addr; // disable -Wunused-parameter warning
	return 0;
#endif // end !USE_FAST_PINIO
}

// -------------------------------------------------------------------------
// 最下層のハードウェア制御用関数群。
// 多くはインライン化され、#defineによって内容が変化します（通常は数命令程度）。
// それ以外のものはインライン化されず、やや複雑な処理を行います。

/*!
	@brief  ハードウェアSPIインターフェースを使用している場合に、SPIトランザクションを開始します。
	@details Arduinoプラットフォームの古いバージョン（SPIトランザクション追加前）では、代わりにSPIクロックやモードの設定を試みます。
			ハードウェアSPI以外の接続では何も行いません。
			この関数はチップセレクト操作を含みません。チップセレクトも含めた処理はstartWrite()を参照してください。
*/
inline void Adafruit_SPITFT::SPI_BEGIN_TRANSACTION(void)
{
	if (connection == TFT_HARD_SPI)
	{
#if defined(SPI_HAS_TRANSACTION)
		hwspi._spi->beginTransaction(hwspi.settings);
#else // No transactions, configure SPI manually...
#if defined(__AVR__) || defined(TEENSYDUINO) || defined(ARDUINO_ARCH_STM32F1)
		hwspi._spi->setClockDivider(SPI_CLOCK_DIV2);
#elif defined(__arm__)
		hwspi._spi->setClockDivider(11);
#elif defined(ESP8266) || defined(ESP32)
		hwspi._spi->setFrequency(hwspi._freq);
#elif defined(RASPI) || defined(ARDUINO_ARCH_STM32F1)
		hwspi._spi->setClock(hwspi._freq);
#endif
		hwspi._spi->setBitOrder(MSBFIRST);
		hwspi._spi->setDataMode(hwspi._mode);
#endif // end !SPI_HAS_TRANSACTION
	}
}

/*!
	@brief  ハードウェアSPIインターフェースを使用している場合にSPIトランザクションを終了します。
	@details 接続がハードウェアSPIでない場合や、ArduinoプラットフォームがSPIトランザクションに対応する以前
	のバージョンの場合は何もしません。この関数にはチップの非選択操作は含まれません。チップセレクトも含めた処理
	はendWrite()を参照してください。
*/
inline void Adafruit_SPITFT::SPI_END_TRANSACTION(void)
{
#if defined(SPI_HAS_TRANSACTION)
	if (connection == TFT_HARD_SPI)
	{
		hwspi._spi->endTransaction();
	}
#endif
}

/*!
	@brief  ディスプレイに8ビット値を1つ出力します。
	@details チップセレクト、トランザクション、データ/コマンド選択は事前に設定されている必要があります 
	― この関数はバイトの出力のみを行います。名称は現在の用途に正確ではありませんが、外部コードとの互換性維持のため残されています。
	ディスプレイ接続がパラレルの場合でも使用されます。
	@param  b  書き込む8ビット値。
*/
void Adafruit_SPITFT::spiWrite(uint8_t b)
{
	if (connection == TFT_HARD_SPI)
	{
#if defined(__AVR__)
		AVR_WRITESPI(b);
#elif defined(ESP8266) || defined(ESP32)
		hwspi._spi->write(b);
#elif defined(ARDUINO_ARCH_RP2040)
		spi_inst_t *pi_spi = hwspi._spi == &SPI ? spi0 : spi1;
		spi_write_blocking(pi_spi, &b, 1);
#else
		hwspi._spi->transfer(b);
#endif
	}
	else if (connection == TFT_SOFT_SPI)
	{
		for (uint8_t bit = 0; bit < 8; bit++)
		{
			if (b & 0x80)
				SPI_MOSI_HIGH();
			else
				SPI_MOSI_LOW();
			SPI_SCK_HIGH();
			b <<= 1;
			SPI_SCK_LOW();
		}
	}
	else
	{ // TFT_PARALLEL
#if defined(__AVR__)
		*tft8.writePort = b;
#elif defined(USE_FAST_PINIO)
		if (!tft8.wide)
			*tft8.writePort = b;
		else
			*(volatile uint16_t *)tft8.writePort = b;
#endif
		TFT_WR_STROBE();
	}
}

/*!
	@brief  ディスプレイにコマンドバイトを1つ出力します。
	@details チップセレクトやトランザクションは事前に設定されている必要があります 
	― この関数はデバイスをコマンドモードに設定し、バイトを送信した後、データモードに戻すだけです。
	対応する明示的なwriteData()関数はありません ― spiWrite()を使用してください。
	@param  cmd  書き込む8ビットコマンド。
*/
void Adafruit_SPITFT::writeCommand(uint8_t cmd)
{
	SPI_DC_LOW();
	spiWrite(cmd);
	SPI_DC_HIGH();
}

/*!
	@brief   ディスプレイから8ビット値を1つ読み出します。
	@details チップセレクトやトランザクションは事前に設定されている必要があります 
	― この関数はバイトの読み出しのみを行います。名称は現在の用途に正確ではありませんが、
	外部コードとの互換性維持のため残されています。ディスプレイ接続がパラレルの場合でも使用されます。
	@return  読み出した8ビットの符号なし値（MCUアーキテクチャがUSE_FAST_PINIO非対応の場合は常に0）。
*/
uint8_t Adafruit_SPITFT::spiRead(void)
{
	uint8_t b = 0;
	uint16_t w = 0;
	if (connection == TFT_HARD_SPI)
	{
		return hwspi._spi->transfer((uint8_t)0);
	}
	else if (connection == TFT_SOFT_SPI)
	{
		if (swspi._miso >= 0)
		{
			for (uint8_t i = 0; i < 8; i++)
			{
				SPI_SCK_HIGH();
				b <<= 1;
				if (SPI_MISO_READ())
					b++;
				SPI_SCK_LOW();
			}
		}
		return b;
	}
	else
	{ // TFT_PARALLEL
		if (tft8._rd >= 0)
		{
#if defined(USE_FAST_PINIO)
			TFT_RD_LOW(); // Read line LOW
#if defined(__AVR__)
			*tft8.portDir = 0x00; // Set port to input state
			w = *tft8.readPort;	  // Read value from port
			*tft8.portDir = 0xFF; // Restore port to output
#else							  // !__AVR__
			if (!tft8.wide)
			{ // 8-bit TFT connection
#if defined(HAS_PORT_SET_CLR)
				*tft8.dirClr = 0xFF; // Set port to input state
				w = *tft8.readPort;	 // Read value from port
				*tft8.dirSet = 0xFF; // Restore port to output
#else  // !HAS_PORT_SET_CLR
				*tft8.portDir = 0x00; // Set port to input state
				w = *tft8.readPort;	  // Read value from port
				*tft8.portDir = 0xFF; // Restore port to output
#endif // end HAS_PORT_SET_CLR
			}
			else
			{ // 16-bit TFT connection
#if defined(HAS_PORT_SET_CLR)
				*(volatile uint16_t *)tft8.dirClr = 0xFFFF; // Input state
				w = *(volatile uint16_t *)tft8.readPort;	// 16-bit read
				*(volatile uint16_t *)tft8.dirSet = 0xFFFF; // Output state
#else  // !HAS_PORT_SET_CLR
				*(volatile uint16_t *)tft8.portDir = 0x0000; // Input state
				w = *(volatile uint16_t *)tft8.readPort;	 // 16-bit read
				*(volatile uint16_t *)tft8.portDir = 0xFFFF; // Output state
#endif // end !HAS_PORT_SET_CLR
			}
			TFT_RD_HIGH(); // Read line HIGH
#endif // end !__AVR__
#else  // !USE_FAST_PINIO
			w = 0; // Parallel TFT is NOT SUPPORTED without USE_FAST_PINIO
#endif // end !USE_FAST_PINIO
		}
		return w;
	}
}

/*!
	@brief  ディスプレイに16ビット値を1つ出力します。
	@details チップセレクト、トランザクション、データ/コマンド選択は事前に設定されている必要があります 
	― この関数はワードの出力のみを行います。したがって「ワイド」（16ビット）パラレルディスプレイ専用です。
	@param  w  書き込む16ビット値。
*/
void Adafruit_SPITFT::write16(uint16_t w)
{
	if (connection == TFT_PARALLEL)
	{
#if defined(USE_FAST_PINIO)
		if (tft8.wide)
			*(volatile uint16_t *)tft8.writePort = w;
#else
		(void)w; // disable -Wunused-parameter warning
#endif
		TFT_WR_STROBE();
	}
}

/*!
	@brief  ディスプレイにコマンドワードを1つ出力します。
	@details チップセレクトやトランザクションは事前に設定されている必要があります 
	― この関数はデバイスをコマンドモードに設定し、ワードを送信した後、データモードに戻すだけです。「ワイド」（16ビット）パラレルディスプレイ専用です。
	@param  cmd  書き込む16ビットコマンド。
*/
void Adafruit_SPITFT::writeCommand16(uint16_t cmd)
{
	SPI_DC_LOW();
	write16(cmd);
	SPI_DC_HIGH();
}

/*!
	@brief  ディスプレイから16ビット値を1つ読み出します。
	@details チップセレクトやトランザクションは事前に設定されている必要があります 
	― この関数はワードの読み出しのみを行います。「ワイド」（16ビット）パラレルディスプレイ専用です。
	@return  読み出した16ビットの符号なし値（MCUアーキテクチャがUSE_FAST_PINIO非対応の場合は常に0）。
*/
uint16_t Adafruit_SPITFT::read16(void)
{
	uint16_t w = 0;
	if (connection == TFT_PARALLEL)
	{
		if (tft8._rd >= 0)
		{
#if defined(USE_FAST_PINIO)
			TFT_RD_LOW(); // Read line LOW
			if (tft8.wide)
			{ // 16-bit TFT connection
#if defined(HAS_PORT_SET_CLR)
				*(volatile uint16_t *)tft8.dirClr = 0xFFFF; // Input state
				w = *(volatile uint16_t *)tft8.readPort;	// 16-bit read
				*(volatile uint16_t *)tft8.dirSet = 0xFFFF; // Output state
#else														// !HAS_PORT_SET_CLR
				*(volatile uint16_t *)tft8.portDir = 0x0000; // Input state
				w = *(volatile uint16_t *)tft8.readPort;	 // 16-bit read
				*(volatile uint16_t *)tft8.portDir = 0xFFFF; // Output state
#endif														// end !HAS_PORT_SET_CLR
			}
			TFT_RD_HIGH(); // Read line HIGH
#else					   // !USE_FAST_PINIO
			w = 0; // Parallel TFT is NOT SUPPORTED without USE_FAST_PINIO
#endif					   // end !USE_FAST_PINIO
		}
	}
	return w;
}

/*!
	@brief  ソフトウェア（ビットバン）SPIのMOSIラインをHIGHに設定します。
*/
inline void Adafruit_SPITFT::SPI_MOSI_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
	*swspi.mosiPortSet = 1;
#else // !KINETISK
	*swspi.mosiPortSet = swspi.mosiPinMask;
#endif
#else  // !HAS_PORT_SET_CLR
	*swspi.mosiPort |= swspi.mosiPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(swspi._mosi, HIGH);
#endif // end !USE_FAST_PINIO
}

/*!
	@brief  ソフトウェア（ビットバン）SPIのMOSIラインをLOWに設定します。
*/
inline void Adafruit_SPITFT::SPI_MOSI_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
	*swspi.mosiPortClr = 1;
#else // !KINETISK
	*swspi.mosiPortClr = swspi.mosiPinMask;
#endif
#else  // !HAS_PORT_SET_CLR
	*swspi.mosiPort &= swspi.mosiPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(swspi._mosi, LOW);
#endif // end !USE_FAST_PINIO
}

/*!
	@brief  ソフトウェア（ビットバン）SPIのSCKラインをHIGHに設定します。
*/
inline void Adafruit_SPITFT::SPI_SCK_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
	*swspi.sckPortSet = 1;
#else // !KINETISK
	*swspi.sckPortSet = swspi.sckPinMask;
#endif
#else  // !HAS_PORT_SET_CLR
	*swspi.sckPort |= swspi.sckPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(swspi._sck, HIGH);
#endif // end !USE_FAST_PINIO
}

/*!
	@brief  ソフトウェア（ビットバン）SPIのSCKラインをLOWに設定します。
*/
inline void Adafruit_SPITFT::SPI_SCK_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
	*swspi.sckPortClr = 1;
#else // !KINETISK
	*swspi.sckPortClr = swspi.sckPinMask;
#endif
#else  // !HAS_PORT_SET_CLR
	*swspi.sckPort &= swspi.sckPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(swspi._sck, LOW);
#endif // end !USE_FAST_PINIO
}

/*!
	@brief  ソフトウェア（ビットバン）SPIのMISOラインの状態を読み取ります。
	@return  HIGHならtrue、LOWならfalse。
*/
inline bool Adafruit_SPITFT::SPI_MISO_READ(void)
{
#if defined(USE_FAST_PINIO)
#if defined(KINETISK)
	return *swspi.misoPort;
#else  // !KINETISK
	return *swspi.misoPort & swspi.misoPinMask;
#endif // end !KINETISK
#else  // !USE_FAST_PINIO
	return digitalRead(swspi._miso);
#endif // end !USE_FAST_PINIO
}


/*!
	@brief  ディスプレイに16ビット値を1つ出力します。
	@details チップセレクト、トランザクション、データ/コマンド選択は事前に設定されている必要があります 
	 この関数はワードの出力のみを行います。名称は8ビット版のspiWrite()と一貫性がありませんが、
	 外部コードとの互換性維持のためです。ディスプレイ接続がパラレルの場合でも使用されます。
	@param  w  書き込む16ビット値。
*/
void Adafruit_SPITFT::SPI_WRITE16(uint16_t w)
{
	if (connection == TFT_HARD_SPI)
	{
#if defined(__AVR__)
		AVR_WRITESPI(w >> 8);
		AVR_WRITESPI(w);
#elif defined(ESP8266) || defined(ESP32)
		hwspi._spi->write16(w);
#elif defined(ARDUINO_ARCH_RP2040)
		spi_inst_t *pi_spi = hwspi._spi == &SPI ? spi0 : spi1;
		w = __builtin_bswap16(w);
		spi_write_blocking(pi_spi, (uint8_t *)&w, 2);
#elif defined(ARDUINO_ARCH_RTTHREAD)
		hwspi._spi->transfer16(w);
#else
		// MSB, LSB because TFTs are generally big-endian
		hwspi._spi->transfer(w >> 8);
		hwspi._spi->transfer(w);
#endif
	}
	else if (connection == TFT_SOFT_SPI)
	{
		for (uint8_t bit = 0; bit < 16; bit++)
		{
			if (w & 0x8000)
				SPI_MOSI_HIGH();
			else
				SPI_MOSI_LOW();
			SPI_SCK_HIGH();
			SPI_SCK_LOW();
			w <<= 1;
		}
	}
	else
	{ // TFT_PARALLEL
#if defined(__AVR__)
		*tft8.writePort = w >> 8;
		TFT_WR_STROBE();
		*tft8.writePort = w;
#elif defined(USE_FAST_PINIO)
		if (!tft8.wide)
		{
			*tft8.writePort = w >> 8;
			TFT_WR_STROBE();
			*tft8.writePort = w;
		}
		else
		{
			*(volatile uint16_t *)tft8.writePort = w;
		}
#endif
		TFT_WR_STROBE();
	}
}

/*!
	@brief  ディスプレイに32ビット値を1つ出力します。
	@details チップセレクト、トランザクション、データ/コマンド選択は事前に設定されている必要があります 
	― この関数はロングワードの出力のみを行います。名称は8ビット版のspiWrite()と一貫性がありませんが、
	外部コードとの互換性維持のためです。ディスプレイ接続がパラレルの場合でも使用されます。
	@param  l  書き込む32ビット値。
*/
void Adafruit_SPITFT::SPI_WRITE32(uint32_t l)
{
	if (connection == TFT_HARD_SPI)
	{
#if defined(__AVR__)
		AVR_WRITESPI(l >> 24);
		AVR_WRITESPI(l >> 16);
		AVR_WRITESPI(l >> 8);
		AVR_WRITESPI(l);
#elif defined(ESP8266) || defined(ESP32)
		hwspi._spi->write32(l);
#elif defined(ARDUINO_ARCH_RP2040)
		spi_inst_t *pi_spi = hwspi._spi == &SPI ? spi0 : spi1;
		l = __builtin_bswap32(l);
		spi_write_blocking(pi_spi, (uint8_t *)&l, 4);
#elif defined(ARDUINO_ARCH_RTTHREAD)
		hwspi._spi->transfer16(l >> 16);
		hwspi._spi->transfer16(l);
#else
		hwspi._spi->transfer(l >> 24);
		hwspi._spi->transfer(l >> 16);
		hwspi._spi->transfer(l >> 8);
		hwspi._spi->transfer(l);
#endif
	}
	else if (connection == TFT_SOFT_SPI)
	{
		for (uint8_t bit = 0; bit < 32; bit++)
		{
			if (l & 0x80000000)
				SPI_MOSI_HIGH();
			else
				SPI_MOSI_LOW();
			SPI_SCK_HIGH();
			SPI_SCK_LOW();
			l <<= 1;
		}
	}
	else
	{ // TFT_PARALLEL
#if defined(__AVR__)
		*tft8.writePort = l >> 24;
		TFT_WR_STROBE();
		*tft8.writePort = l >> 16;
		TFT_WR_STROBE();
		*tft8.writePort = l >> 8;
		TFT_WR_STROBE();
		*tft8.writePort = l;
#elif defined(USE_FAST_PINIO)
		if (!tft8.wide)
		{
			*tft8.writePort = l >> 24;
			TFT_WR_STROBE();
			*tft8.writePort = l >> 16;
			TFT_WR_STROBE();
			*tft8.writePort = l >> 8;
			TFT_WR_STROBE();
			*tft8.writePort = l;
		}
		else
		{
			*(volatile uint16_t *)tft8.writePort = l >> 16;
			TFT_WR_STROBE();
			*(volatile uint16_t *)tft8.writePort = l;
		}
#endif
		TFT_WR_STROBE();
	}
}

/*!
	@brief  パラレル接続時のデータ書き込みで、WRラインをLOW→HIGHにします。
*/
inline void Adafruit_SPITFT::TFT_WR_STROBE(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
#if defined(KINETISK)
	*tft8.wrPortClr = 1;
	*tft8.wrPortSet = 1;
#else  // !KINETISK
	*tft8.wrPortClr = tft8.wrPinMask;
	*tft8.wrPortSet = tft8.wrPinMask;
#endif // end !KINETISK
#else  // !HAS_PORT_SET_CLR
	*tft8.wrPort &= tft8.wrPinMaskClr;
	*tft8.wrPort |= tft8.wrPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(tft8._wr, LOW);
	digitalWrite(tft8._wr, HIGH);
#endif // end !USE_FAST_PINIO
}

/*!
	@brief  パラレル接続時のデータ読み出しで、RDラインをHIGHにします。
*/
inline void Adafruit_SPITFT::TFT_RD_HIGH(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
	*tft8.rdPortSet = tft8.rdPinMask;
#else  // !HAS_PORT_SET_CLR
	*tft8.rdPort |= tft8.rdPinMaskSet;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(tft8._rd, HIGH);
#endif // end !USE_FAST_PINIO
}

/*!
	@brief  パラレル接続時のデータ読み出しで、RDラインをLOWにします。
*/
inline void Adafruit_SPITFT::TFT_RD_LOW(void)
{
#if defined(USE_FAST_PINIO)
#if defined(HAS_PORT_SET_CLR)
	*tft8.rdPortClr = tft8.rdPinMask;
#else  // !HAS_PORT_SET_CLR
	*tft8.rdPort &= tft8.rdPinMaskClr;
#endif // end !HAS_PORT_SET_CLR
#else  // !USE_FAST_PINIO
	digitalWrite(tft8._rd, LOW);
#endif // end !USE_FAST_PINIO
}

#endif // end __AVR_ATtiny85__ __AVR_ATtiny84__
