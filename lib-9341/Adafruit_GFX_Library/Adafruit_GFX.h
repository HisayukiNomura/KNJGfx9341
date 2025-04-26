#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H
#include "../misc/defines.h"

#ifdef STD_SDK
	// #include "../core/Arduino.h"
	#include "../misc/KNJGfx_struct.h"
	#include "../core/Print.h"
	#include "gfxfont.h"
struct KanjiData;
#else
	#if ARDUINO >= 100
		#include "Arduino.h"
		#include "Print.h"
	#else
		#include "WProgram.h"
	#endif
#endif

#ifdef STD_SDK

#else
	#include <Adafruit_I2CDevice.h>
	#include <Adafruit_SPIDevice.h>
#endif

/// A generic graphics superclass that can handle all sorts of drawing. At a
/// minimum you can subclass and provide drawPixel(). At a maximum you can do a
/// ton of overriding to optimize. Used for any/all Adafruit displays!

#ifdef STD_SDK
namespace ardPort {
#endif
	class GFXcanvas1;
	class GFXcanvas8;
	class GFXcanvas16;

	class Adafruit_GFX : public ardPort::core::Print {
	   protected:
		uint16_t convert8To565(uint8_t color8);

	   public:
		Adafruit_GFX(int16_t w, int16_t h);  // Constructor

		/**********************************************************************/
		/*!
		  @brief  Draw to the screen/framebuffer/etc.
		  Must be overridden in subclass.
		  @param  x    X coordinate in pixels
		  @param  y    Y coordinate in pixels
		  @param color  16-bit pixel color.
		*/
		/**********************************************************************/
		virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

		// TRANSACTION API / CORE DRAW API
		// These MAY be overridden by the subclass to provide device-specific
		// optimized code.  Otherwise 'generic' versions are used.
		virtual void startWrite(void);
		virtual void writePixel(int16_t x, int16_t y, uint16_t color);
		virtual void writePixel(XY xy, uint16_t color) { writePixel(xy.x, xy.y, color); }

		virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		virtual void writeFillRect(XYWH xywh, uint16_t color) { writeFillRect(xywh.x, xywh.y, xywh.w, xywh.h, color); }

		virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		virtual void writeFastVLine(XY xy, uint16_t h, uint16_t color) { writeFastVLine(xy.x, xy.y, h, color); }

		virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		virtual void writeFastHLine(XY xy, uint16_t w, uint16_t color) { writeFastHLine(xy.x, xy.y, w, color); }

		virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
		virtual void writeLine(XY xy0, XY xy1, uint16_t color) { writeLine(xy0.x, xy0.y, xy1.x, xy1.y, color); }

		virtual void endWrite(void);

		// CONTROL API
		// These MAY be overridden by the subclass to provide device-specific
		// optimized code.  Otherwise 'generic' versions are used.
		virtual void setRotation(uint8_t r);
		virtual void invertDisplay(bool i);

		// BASIC DRAW API
		// These MAY be overridden by the subclass to provide device-specific
		// optimized code.  Otherwise 'generic' versions are used.

		// It's good to implement those, even if using transaction API
		virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		virtual void drawFastVLine(XY xy, uint16_t h, uint16_t color) { drawFastVLine(xy.x, xy.y, h, color); }
		virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		virtual void drawFastHLine(XY xy, uint16_t w, uint16_t color) { drawFastHLine(xy.x, xy.y, w, color); }
		virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		virtual void fillRect(XYWH xywh, uint16_t color) { fillRect(xywh.x, xywh.y, xywh.w, xywh.h, color); }

		virtual void fillScreen(uint16_t color);

		// Optional and probably not necessary to change
		virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
		virtual void drawLine(XY xy0, XY xy1, uint16_t color) { drawLine(xy0.x, xy0.y, xy1.x, xy1.y, color); }

		virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
		virtual void drawRect(XYWH xywh, uint16_t color) { drawRect(xywh.x, xywh.y, xywh.w, xywh.h, color); }

		// These exist only with Adafruit_GFX (no subclass overrides)
		void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
		void drawCircle(XY xy, int16_t r, uint16_t color) { drawCircle(xy.x, xy.y, r, color); }

		void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
		void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
		void fillCircle(XY xy, int16_t r, uint16_t color) { fillCircle(xy.x, xy.y, r, color); }
		void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
		void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
		void drawTriangle(XY xy0, XY xy1, XY xy2, uint16_t color) { drawTriangle(xy0.x, xy0.y, xy1.x, xy1.y, xy2.x, xy2.y, color); }
		void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
		void fillTriangle(XY xy0, XY xy1, XY xy2, uint16_t color) { fillTriangle(xy0.x, xy0.y, xy1.x, xy1.y, xy2.x, xy2.y, color); }
		void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,  int16_t radius, uint16_t color);
		void drawRoundRect(XYWH xywh, int16_t radius, uint16_t color) { drawRoundRect(xywh.x, xywh.y, xywh.w, xywh.h, radius, color); }
		void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
		void fillRoundRect(XYWH xywh, int16_t radius, uint16_t color) { fillRoundRect(xywh.x, xywh.y, xywh.w, xywh.h, radius, color); }

		void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
		void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg);
		void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,uint16_t color);
		void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,uint16_t color, uint16_t bg);
		void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,int16_t h, uint16_t color);
		void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h);
		void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
		void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);
		void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h);

		// ビットマップの描画
		void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
		void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
		void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);
		void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
		void drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h, uint16_t colorTransparent);
		void drawRGBBitmap(int16_t x, int16_t y, const uint16_t *pcolors, int16_t w, int16_t h, uint16_t colorTransparent) { drawRGBBitmap(x, y, (uint16_t *)pcolors, w, h, colorTransparent); }

		// 8bit バージョン
		void drawRGBBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h);
		void drawRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);

		void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
		void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);

		// GFXCanvas使用
		void drawRGBBitmap(int16_t x, int16_t y, GFXcanvas16 *);
		void drawRGBBitmap(int16_t x, int16_t y, GFXcanvas8 *);
		void drawRGBBitmap(int16_t x, int16_t y, GFXcanvas1 *);

		// 日本語表示用
		// virtual void drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t* bmpData, uint16_t color, uint16_t bg, uint8_t size);
		
		virtual void drawChar(int16_t x, int16_t y, uint8_t w, uint8_t h, const uint8_t *bmpData, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);

		void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1,
						   int16_t *y1, uint16_t *w, uint16_t *h);
		void getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y,
						   int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
		void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1,
						   int16_t *y1, uint16_t *w, uint16_t *h);
		void setTextSize(uint8_t s);
		void setTextSize(uint8_t sx, uint8_t sy);

		void setFont(const GFXfont *f = NULL);
		// 漢字フォントを使用する場合はこっちが呼び出される
		void setFont(const KanjiData *a_pKanjiData, const uint8_t *a_pBmpData);

		/**********************************************************************/
		/*!
		  @brief  Set text cursor location
		  @param  x    X coordinate in pixels
		  @param  y    Y coordinate in pixels
		*/
		/**********************************************************************/
		void setCursor(int16_t x, int16_t y) {
			cursor_x = x;
			cursor_y = y;
		}

		/**********************************************************************/
		/*!
		  @brief   Set text font color with transparant background
		  @param   c   16-bit 5-6-5 Color to draw text with
		  @note    For 'transparent' background, background and foreground
				   are set to same color rather than using a separate flag.
		*/
		/**********************************************************************/
		void setTextColor(uint16_t c) {
			textcolor = textbgcolor = c;
		}
		/// @brief テキストの背景色を設定
		/// @param bg 背景色
		void setBackgroundColor(uint16_t bg) {
			textbgcolor = bg;
		}

		/**********************************************************************/
		/*!
		  @brief   Set text font color with custom background color
		  @param   c   16-bit 5-6-5 Color to draw text with
		  @param   bg  16-bit 5-6-5 Color to draw background/fill with
		*/
		/**********************************************************************/
		void setTextColor(uint16_t c, uint16_t bg) {
			textcolor = c;
			textbgcolor = bg;
		}

		/**********************************************************************/
		/*!
		@brief  Set whether text that is too long for the screen width should
				automatically wrap around to the next line (else clip right).
		@param  w  true for wrapping, false for clipping
		*/
		/**********************************************************************/
		void setTextWrap(bool w) {
			wrap = w;
		}

		/**********************************************************************/
		/*!
		  @brief  Enable (or disable) Code Page 437-compatible charset.
				  There was an error in glcdfont.c for the longest time -- one
				  character (#176, the 'light shade' block) was missing -- this
				  threw off the index of every character that followed it.
				  But a TON of code has been written with the erroneous
				  character indices. By default, the library uses the original
				  'wrong' behavior and old sketches will still work. Pass
				  'true' to this function to use correct CP437 character values
				  in your code.
		  @param  x  true = enable (new behavior), false = disable (old behavior)
		*/
		/**********************************************************************/
		void cp437(bool x = true) {
			_cp437 = x;
		}

		using Print::write;

#if not defined(STD_SDK)
	/************************************************************************/
	/*!
	  @brief      Write a byte/character of data, used to support print()
	  @param      c  The 8-bit ascii character to write
	  @returns    Number of bytes written
	*/
	/************************************************************************/
	#if ARDUINO >= 100
		virtual size_t write(uint8_t);
	#else
		virtual void write(uint8_t);
	#endif
#else
	virtual size_t write(uint8_t);
	virtual size_t write(uint32_t);
#endif

		/************************************************************************/
		/*!
		  @brief      Get width of the display, accounting for current rotation
		  @returns    Width in pixels
		*/
		/************************************************************************/
		int16_t width(void) const { return _width; };

		/************************************************************************/
		/*!
		  @brief      Get height of the display, accounting for current rotation
		  @returns    Height in pixels
		*/
		/************************************************************************/
		int16_t height(void) const { return _height; }

		/************************************************************************/
		/*!
		  @brief      Get rotation setting for display
		  @returns    0 thru 3 corresponding to 4 cardinal rotations
		*/
		/************************************************************************/
		uint8_t getRotation(void) const { return rotation; }

		// get current cursor position (get rotation safe maximum values,
		// using: width() for x, height() for y)
		/************************************************************************/
		/*!
		  @brief  Get text cursor X location
		  @returns    X coordinate in pixels
		*/
		/************************************************************************/
		int16_t getCursorX(void) const { return cursor_x; }

		/************************************************************************/
		/*!
		  @brief      Get text cursor Y location
		  @returns    Y coordinate in pixels
		*/
		/************************************************************************/
		int16_t getCursorY(void) const { return cursor_y; };

	   protected:
		void charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx,
						int16_t *miny, int16_t *maxx, int16_t *maxy);
		int16_t WIDTH;         ///< This is the 'raw' display width - never changes
		int16_t HEIGHT;        ///< This is the 'raw' display height - never changes
		int16_t _width;        ///< Display width as modified by current rotation
		int16_t _height;       ///< Display height as modified by current rotation
		int16_t cursor_x;      ///< x location to start print()ing text
		int16_t cursor_y;      ///< y location to start print()ing text
		uint16_t textcolor;    ///< 16-bit background color for print()
		uint16_t textbgcolor;  ///< 16-bit text color for print()
		uint8_t textsize_x;    ///< Desired magnification in X-axis of text to print()
		uint8_t textsize_y;    ///< Desired magnification in Y-axis of text to print()
		uint8_t rotation;      ///< Display rotation (0 thru 3)
		bool wrap;             ///< If set, 'wrap' text at right edge of display
		bool _cp437;           ///< If set, use correct CP437 charset (default is off)
		GFXfont *gfxFont;      ///< Pointer to special font
	};

	/// A simple drawn button UI element
	class Adafruit_GFX_Button {
	   public:
		Adafruit_GFX_Button(void);
		// "Classic" initButton() uses center & size
		void initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w,
						uint16_t h, uint16_t outline, uint16_t fill,
						uint16_t textcolor, char *label, uint8_t textsize);
		void initButton(Adafruit_GFX *gfx, int16_t x, int16_t y, uint16_t w,
						uint16_t h, uint16_t outline, uint16_t fill,
						uint16_t textcolor, char *label, uint8_t textsize_x,
						uint8_t textsize_y);
		// New/alt initButton() uses upper-left corner & size
		void initButtonUL(Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w,
						  uint16_t h, uint16_t outline, uint16_t fill,
						  uint16_t textcolor, char *label, uint8_t textsize);
		void initButtonUL(Adafruit_GFX *gfx, int16_t x1, int16_t y1, uint16_t w,
						  uint16_t h, uint16_t outline, uint16_t fill,
						  uint16_t textcolor, char *label, uint8_t textsize_x,
						  uint8_t textsize_y);
		void drawButton(bool inverted = false);
		bool contains(int16_t x, int16_t y);

		/**********************************************************************/
		/*!
		  @brief    Sets button state, should be done by some touch function
		  @param    p  True for pressed, false for not.
		*/
		/**********************************************************************/
		void press(bool p) {
			laststate = currstate;
			currstate = p;
		}

		bool justPressed();
		bool justReleased();

		/**********************************************************************/
		/*!
		  @brief    Query whether the button is currently pressed
		  @returns  True if pressed
		*/
		/**********************************************************************/
		bool isPressed(void) { return currstate; };

	   private:
		Adafruit_GFX *_gfx;
		int16_t _x1, _y1;  // Coordinates of top-left corner
		uint16_t _w, _h;
		uint8_t _textsize_x;
		uint8_t _textsize_y;
		uint16_t _outlinecolor, _fillcolor, _textcolor;
		char _label[10];

		bool currstate, laststate;
	};

	/// A GFX 1-bit canvas context for graphics
	class GFXcanvas1 : public Adafruit_GFX {
	   public:
		GFXcanvas1(uint16_t w, uint16_t h, bool allocate_buffer = true);
		~GFXcanvas1(void);
		GFXcanvas1(const GFXcanvas1 *pSrc, bool allocate_buffer = true);  // コピーコンストラクタ

		void useBackgroundColor(uint16_t color);
		void disableBackgroundColor();
		void drawPixel(int16_t x, int16_t y, uint16_t color);
		void fillScreen(uint16_t color);
		void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		bool getPixel(int16_t x, int16_t y) const;
		GFXcanvas1 *deepCopy(const GFXcanvas1 *canvas);

		/**********************************************************************/
		/*!
		  @brief    Get a pointer to the internal buffer memory
		  @returns  A pointer to the allocated buffer
		*/
		/**********************************************************************/
		uint8_t *getBuffer(void) const { return buffer; }

	   protected:
		bool getRawPixel(int16_t x, int16_t y) const;
		void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		uint8_t *buffer;    ///< Raster data: no longer private, allow subclass access
		bool buffer_owned;  ///< If true, destructor will free buffer, else it will do
							///< nothing
	   public:
		bool isBackground;  /// 背景色を使用するかのフラグ
		bool bckColor;      /// 背景色。この色はビットマップ描画時に透過色となる。

	   private:
#ifdef __AVR__
		// Bitmask tables of 0x80>>X and ~(0x80>>X), because X>>Y is slow on AVR
		static const uint8_t PROGMEM GFXsetBit[], GFXclrBit[];
#endif
	};

	/// A GFX 8-bit canvas context for graphics
	class GFXcanvas8 : public Adafruit_GFX {
	   public:
		GFXcanvas8(uint16_t w, uint16_t h, bool allocate_buffer = true);
		~GFXcanvas8(void);
		GFXcanvas8(const GFXcanvas8 *pSrc, bool allocate_buffer = true);  // コピーコンストラクタ

		// 代入演算子
		GFXcanvas8 &operator=(const GFXcanvas8 &other) {
			if (this != &other) {  // 自己代入のチェック
				deepCopy(&other);
			}
			return *this;  // 自分自身を返す
		}

		void useTransparentColor(uint8_t color);
		void unUseTransparentColor();
		GFXcanvas8 *deepCopy(const GFXcanvas8 *canvas);
		void drawPixel(int16_t x, int16_t y, uint16_t color);
		void fillScreen(uint16_t color);
		void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		uint8_t getPixel(int16_t x, int16_t y) const;
		/**********************************************************************/
		/*!
		 @brief    Get a pointer to the internal buffer memory
		 @returns  A pointer to the allocated buffer
		*/
		/**********************************************************************/
		uint8_t *getBuffer(void) const { return buffer; }

	   protected:
		uint8_t getRawPixel(int16_t x, int16_t y) const;
		void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		uint8_t *buffer;    ///< Raster data: no longer private, allow subclass access
		bool buffer_owned;  ///< If true, destructor will free buffer, else it will do
							///< nothing
	   public:
		bool isBackground;  /// 背景色を使用するかのフラグ
		uint16_t bckColor;  /// 背景色。この色はビットマップ描画時に透過色となる。
	};

	///  A GFX 16-bit canvas context for graphics
	class GFXcanvas16 : public Adafruit_GFX {
	   public:
		GFXcanvas16(uint16_t w, uint16_t h, bool allocate_buffer = true);   // コンストラクタ
		~GFXcanvas16(void);                                                 // デストラクタ
		GFXcanvas16(const GFXcanvas16 *pSrc, bool allocate_buffer = true);  // コピーコンストラクタ

		// 代入演算子
		GFXcanvas16 &operator=(const GFXcanvas16 &other) {
			if (this != &other) {  // 自己代入のチェック
				deepCopy(&other);
			}
			return *this;  // 自分自身を返す
		}

		void useTransparentColor(uint16_t color);
		void unUseTransparentColor();
		GFXcanvas16 *deepCopy(const GFXcanvas16 *canvas);
		void copyRGBBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *picBuf, uint16_t bufWidth, uint16_t bufHeight);                                                                               // キャンバスに背景画像をコピー
		void copyRGBBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *picBuf, uint16_t bufWidth, uint16_t bufHeight) { copyRGBBitmap(x, y, w, h, (uint16_t *)picBuf, bufWidth, bufHeight); }  // キャンバスに背景画像をコピー
		void drawPixel(int16_t x, int16_t y, uint16_t color);
		void fillScreen(uint16_t color);
		void byteSwap(void);
		void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		uint16_t getPixel(int16_t x, int16_t y) const;
		/**********************************************************************/
		/*!
		  @brief    Get a pointer to the internal buffer memory
		  @returns  A pointer to the allocated buffer
		*/
		/**********************************************************************/
		uint16_t *getBuffer(void) const { return buffer; }
	
		void setBuffer(uint16_t* buf) { buffer = buf;} 
		
	   protected:
		uint16_t getRawPixel(int16_t x, int16_t y) const;
		void drawFastRawVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
		void drawFastRawHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
		uint16_t *buffer;   ///< Raster data: no longer private, allow subclass access
		bool buffer_owned;  ///< If true, destructor will free buffer, else it will do
							///< nothing
	   public:
		bool isBackground;  /// 背景色を使用するかのフラグ
		uint16_t bckColor;  /// 背景色。この色はビットマップ描画時に透過色となる。
	};
#ifdef STD_SDK
}
#endif

#endif  // _ADAFRUIT_GFX_H
