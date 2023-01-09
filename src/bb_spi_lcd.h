#ifndef SPI_LCD_H
#define SPI_LCD_H
//
// SPI_LCD using the SPI interface
// Copyright (c) 2017-2019 Larry Bank
// email: bitbank@pobox.com
// Project started 4/25/2017
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// these are defined the same in the OLED library
#ifndef _LINUX_
#include <Arduino.h>
#include <Print.h>
#if defined( ARDUINO_M5Stick_C ) || defined (ARDUINO_M5STACK_Core2)
#include <Wire.h>
#endif
#endif // _LINUX_

#if !defined( __SS_OLED_H__ ) && !defined( __ONEBITDISPLAY__ )
enum {
   FONT_6x8 = 0,
   FONT_8x8,
   FONT_12x16,
   FONT_16x16,
   FONT_16x32,
   FONT_COUNT
};
#endif

// Drawing function render flags
#define DRAW_TO_RAM     1
#define DRAW_TO_LCD     2
#define DRAW_WITH_DMA   4

//
// Data callback function for custom (non-SPI) LCDs
// e.g. 8/16-bit parallel/8080
// The last parameter can be MODE_DATA or MODE_COMMAND
// CS toggle must be handled by the callback function
//
typedef void (*DATACALLBACK)(uint8_t *pData, int len, int iMode);

//
// Reset callback function for custom (non-SPI) LCDs
// e.g. 8/16-bit parallel/8080
// Use it to prepare the GPIO lines and reset the display
//
typedef void (*RESETCALLBACK)(void);

// Proportional font data taken from Adafruit_GFX library
/// Font data stored PER GLYPH
#if !defined( _ADAFRUIT_GFX_H ) && !defined( _GFXFONT_H_ )
#define _GFXFONT_H_
typedef struct {
  uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
  uint8_t width;         ///< Bitmap dimensions in pixels
  uint8_t height;        ///< Bitmap dimensions in pixels
  uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
  int8_t xOffset;        ///< X dist from cursor pos to UL corner
  int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

#ifndef TFT_BLACK
#define TFT_BLACK 0x0000
#define TFT_GREEN 0x07e0
#define TFT_RED 0xf800
#define TFT_BLUE 0x001f
#define TFT_CYAN 0x07ff
#define TFT_YELLOW 0xffe0
#define TFT_MAGENTA 0xf81f
#define TFT_WHITE 0xffff
#define TFT_GREY 0x5AEB
#define TFT_ORANGE 0xbbc0
#endif

/// Data stored for FONT AS A WHOLE
typedef struct {
  uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
  GFXglyph *glyph;  ///< Glyph array
  uint8_t first;    ///< ASCII extents (first char)
  uint8_t last;     ///< ASCII extents (last char)
  uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;
#endif // _ADAFRUIT_GFX_H

// Structure holding an instance of a display
typedef struct tagSPILCD
{
   int iLCDType, iLCDFlags; // LCD display type and flags
   int iOrientation; // current orientation
   int iScrollOffset, bScroll;
   int iWidth, iHeight; // native direction size
   int iCurrentWidth, iCurrentHeight; // rotated size
   int iCSPin, iCLKPin, iMOSIPin, iDCPin, iResetPin, iLEDPin;
   int32_t iSPISpeed, iSPIMode; // SPI settings
   int iScreenPitch, iOffset, iMaxOffset; // display RAM values
   int iColStart, iRowStart, iMemoryX, iMemoryY; // display oddities
   uint8_t *pBackBuffer;
   int iWindowX, iWindowY, iCurrentX, iCurrentY; // for RAM operations
   int iWindowCX, iWindowCY;
   int iCursorX, iCursorY; // for text operations
   int iFont, iWrap, iFG, iBG, iAntialias;
   GFXfont *pFont;
   int iOldX, iOldY, iOldCX, iOldCY; // to optimize spilcdSetPosition()
   RESETCALLBACK pfnResetCallback;
   DATACALLBACK pfnDataCallback;
} SPILCD;

#ifdef __cplusplus
class BB_SPI_LCD : public Print
{
  public:
    BB_SPI_LCD() {memset(&_lcd, 0, sizeof(_lcd));}
    int createVirtual(int iWidth, int iHeight);
    int freeVirtual(void);
    int captureArea(int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h, uint16_t *pPixels, int bSwap565 = 1);
    int merge(uint16_t *pSrc, uint16_t usTrans, int bSwap565);
    int begin(int iStandardType);
    int begin(int iType, int iFlags, int iFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);
    int beginParallel(int iType, int iFlags, uint8_t RST_PIN, uint8_t RD_PIN, uint8_t WR_PIN, uint8_t CS_PIN, uint8_t DC_PIN, int iBusWidth, uint8_t *data_pins);
    void setRotation(int iAngle);
    uint8_t getRotation(void);
    void fillScreen(int iColor);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillRect(int x, int y, int w, int h, int iColor);
    void setTextColor(int iFG, int iBG = -1);
    void setCursor(int x, int y);
    void setAddrWindow(int x, int y, int w, int h);
    void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    int16_t getCursorX(void);
    int16_t getCursorY(void);
    bool allocBuffer(void);
    void * getBuffer(void);
    SPILCD * getLCDStruct(void);
    void freeBuffer(void);
    void setTextSize(int iSize) {}; // empty for now
    void setFont(int iFont);
    void setScroll(bool bScroll);
    void setAntialias(bool bAntialias);
    void setFreeFont(const GFXfont *pFont);
    int16_t height(void);
    int16_t width(void);
    void display(void);
    void pushImage(int x, int y, int w, int h, uint16_t *pixels, int iFlags = DRAW_TO_LCD);
    void pushPixels(uint16_t *pixels, int count);
    void drawString(const char *pText, int x, int y, int size=-1);
    void drawString(String text, int x, int y, int size=-1);
    void drawStringFast(const char *szText, int x, int y, int size = -1);
    int drawBMP(const uint8_t *pBMP, int iDestX, int iDestY, int bStretch = 0, int iTransparent = -1, int iFlags = DRAW_TO_LCD);
    void drawLine(int x1, int y1, int x2, int y2, int iColor);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color);
    void drawPattern(uint8_t *pPattern, int iSrcPitch, int iDestX, int iDestY, int iCX, int iCY, uint16_t usColor, int iTranslucency);
    using Print::write;
    virtual size_t write(uint8_t);

  private:
    SPILCD _lcd;
}; // class BB_SPI_LCD
#endif // __cplusplus

// Parallel LCD support functions
void ParallelDataInit(uint8_t RD_PIN, uint8_t WR_PIN, uint8_t CS_PIN, uint8_t DC_PIN, int iBusWidth, uint8_t *data_pins, int iSwapColor);
void ParallelReset(void);
void ParallelDataWrite(uint8_t *pData, int len, int iMode);

enum
{
    DISPLAY_TINYPICO_IPS_SHIELD=1,
    DISPLAY_TINYPICO_EXPLORER_SHIELD,
    DISPLAY_WIO_TERMINAL,
    DISPLAY_TEENSY_ILI9341,
    DISPLAY_M5STACK_STICKC,
    DISPLAY_M5STACK_STICKCPLUS,
    DISPLAY_M5STACK_CORE2,
    DISPLAY_RANKIN_COLORCOIN,
    DISPLAY_RANKIN_SENSOR,
    DISPLAY_RANKIN_POWER,
    DISPLAY_T_DONGLE_S3,
    DISPLAY_T_DISPLAY_S3,
    DISPLAY_T_DISPLAY,
    DISPLAY_T_QT,
    DISPLAY_TUFTY2040,
    DISPLAY_KUMAN_35,
    DISPLAY_KUMAN_24,
    DISPLAY_MAKERFABS_S3,
    DISPLAY_M5STACK_ATOMS3,
    DISPLAY_COUNT
};
#if !defined(BITBANK_LCD_MODES)
#define BITBANK_LCD_MODES
typedef enum
{
 MODE_DATA = 0,
 MODE_COMMAND
} DC_MODE;
#endif

// Initialization flags
#define FLAGS_NONE    0
#define FLAGS_SWAP_RB 1
#define FLAGS_INVERT  2
#define FLAGS_BITBANG 4
#define FLAGS_FLIPX   8
#define FLAGS_SWAP_COLOR 16
#define FLAGS_MEM_RESTART 32

#if defined(_LINUX_) && defined(__cplusplus)
extern "C" {
#endif

// Sets the D/C pin to data or command mode
void spilcdSetMode(SPILCD *pLCD, int iMode);
//
// Set the text cursor position in pixels
//
void spilcdSetCursor(SPILCD *pLCD, int x, int y);
//
// Provide a small temporary buffer for use by the graphics functions
//
void spilcdSetTXBuffer(uint8_t *pBuf, int iSize);

//
// Choose the gamma curve between 2 choices (0/1)
// ILI9341 only
//
int spilcdSetGamma(SPILCD *pLCD, int iMode);

// Initialize the library
int spilcdInit(SPILCD *pLCD, int iLCDType, int iFlags, int32_t iSPIFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);

//
// Initialize the touch controller
//
int spilcdInitTouch(SPILCD *pLCD, int iType, int iChannel, int iSPIFreq);

//
// Set touch calibration values
// These are the minimum and maximum x/y values returned from the sensor
// These values are used to normalize the position returned from the sensor
//
void spilcdTouchCalibration(SPILCD *pLCD, int iminx, int imaxx, int iminy, int imaxy);

//
// Shut down the touch interface
//
void spilcdShutdownTouch(SPILCD *pLCD);

//
// Read the current touch values
// values are normalized to 0-1023 range for x and y
// returns: -1=not initialized, 0=nothing touching, 1=good values
//
int spilcdReadTouchPos(SPILCD *pLCD, int *pX, int *pY);

// Turns off the display and frees the resources
void spilcdShutdown(SPILCD *pLCD);

// Fills the display with the byte pattern
int spilcdFill(SPILCD *pLCD, unsigned short usPattern, int bRender);

//
// Draw a rectangle and optionally fill it
// With the fill option, a color gradient will be created
// between the top and bottom lines going from usColor1 to usColor2
//
void spilcdRectangle(SPILCD *pLCD, int x, int y, int w, int h, unsigned short usColor1, unsigned short usColor2, int bFill, int bRender);

//
// Reset the scroll position to 0
//
void spilcdScrollReset(SPILCD *pLCD);

// Configure a GPIO pin for input
// Returns 0 if successful, -1 if unavailable
int spilcdConfigurePin(int iPin);

// Read from a GPIO pin
int spilcdReadPin(int iPin);

//
// Scroll the screen N lines vertically (positive or negative)
// This is a delta which affects the current hardware scroll offset
// If iFillcolor != -1, the newly exposed lines will be filled with that color
//
void spilcdScroll(SPILCD *pLCD, int iLines, int iFillColor);

// Write a text string to the display at x (column 0-83) and y (row 0-5)
int spilcdWriteString(SPILCD *pLCD, int x, int y, char *szText, int iFGColor, int iBGColor, int iFontSize, int bRender);

// Write a text string of 8x8 characters
// quickly to the LCD with a single data block write.
// This reduces the number of SPI transactions and speeds it up
// This function only allows the FONT_NORMAL and FONT_SMALL sizes
// 
int spilcdWriteStringFast(SPILCD *pLCD, int x, int y, char *szText, unsigned short usFGColor, unsigned short usBGColor, int iFontSize, int iFlags);
//
// Draw a string in a proportional font you supply
//
int spilcdWriteStringCustom(SPILCD *pLCD, GFXfont *pFont, int x, int y, char *szMsg, uint16_t usFGColor, uint16_t usBGColor, int bBlank, int iFlags);
//
// Draw a string in a proportional font with antialiasing
//
int spilcdWriteStringAntialias(SPILCD *pLCD, GFXfont *pFont, int x, int y, char *szMsg, uint16_t usFGColor, uint16_t usBGColor, int iFlags);
//
// Get the width and upper/lower bounds of text in a custom font
//
void spilcdGetStringBox(GFXfont *pFont, char *szMsg, int *width, int *top, int *bottom);

// Sets a pixel to the given color
// Coordinate system is pixels, not text rows (0-239, 0-319)
int spilcdSetPixel(SPILCD *pLCD, int x, int y, unsigned short usPixel, int bRender);

// Set the software orientation
int spilcdSetOrientation(SPILCD *pLCD, int iOrientation);

// Draw an ellipse with X and Y radius
void spilcdEllipse(SPILCD *pLCD, int32_t centerX, int32_t centerY, int32_t radiusX, int32_t radiusY, unsigned short color, int bFilled, int bRender);
//
// Draw a line between 2 points using Bresenham's algorithm
// 
void spilcdDrawLine(SPILCD *pLCD, int x1, int y1, int x2, int y2, unsigned short usColor, int bRender);
int spilcdDraw53Tile(SPILCD *pLCD, int x, int y, int cx, int cy, unsigned char *pTile, int iPitch, int iFlags);
int spilcdDrawSmallTile(SPILCD *pLCD, int x, int y, unsigned char *pTile, int iPitch, int iFlags);
int spilcdDrawTile(SPILCD *pLCD, int x, int y, int iTileWidth, int iTileHeight, unsigned char *pTile, int iPitch, int iFlags);
int spilcdDrawTile150(SPILCD *pLCD, int x, int y, int iTileWidth, int iTileHeight, unsigned char *pTile, int iPitch, int iFlags);
int spilcdDrawRetroTile(SPILCD *pLCD, int x, int y, unsigned char *pTile, int iPitch, int iFlags);
int spilcdDrawMaskedTile(SPILCD *pLCD, int x, int y, unsigned char *pTile, int iPitch, int iColMask, int iRowMask, int iFlags);
//
// Public wrapper function to write data to the display
//
void spilcdWriteDataBlock(SPILCD *pLCD, uint8_t *pData, int iLen, int iFlags);
void spilcdWritePixelsMasked(SPILCD *pLCD, int x, int y, uint8_t *pData, uint8_t *pMask, int iCount, int iFlags);
void spilcdWriteCommand(SPILCD *pLCD, unsigned char c);
int spilcdIsDMABusy(void);
uint8_t * spilcdGetDMABuffer(void);
//
// Position the "cursor" to the given
// row and column. The width and height of the memory
// 'window' must be specified as well. The controller
// allows more efficient writing of small blocks (e.g. tiles)
// by bounding the writes within a small area and automatically
// wrapping the address when reaching the end of the window
// on the curent row
//
void spilcdSetPosition(SPILCD *pLCD, int x, int y, int w, int h, int bRender);
//
// Draw a 4, 8 or 16-bit Windows uncompressed bitmap onto the display
// Pass the pointer to the beginning of the BMP file
// Optionally stretch to 2x size
// returns -1 for error, 0 for success
//
int spilcdDrawBMP(SPILCD *pLCD, uint8_t *pBMP, int iDestX, int iDestY, int bStretch, int iTransparent, int bRender);

//
// Give bb_spi_lcd two callback functions to talk to the LCD
// useful when not using SPI or providing an optimized interface
//
void spilcdSetCallbacks(SPILCD *pLCD, RESETCALLBACK pfnReset, DATACALLBACK pfnData);

//
// Show part or all of the back buffer on the display
// Used after delayed rendering of graphics
//
void spilcdShowBuffer(SPILCD *pLCD, int x, int y, int cx, int cy, int iFlags);
//
// Returns the current backbuffer address
//
uint16_t * spilcdGetBuffer(SPILCD *pLCD);
//
// Set the back buffer
//
void spilcdSetBuffer(SPILCD *pLCD, void *pBuffer);
//
// Allocate the back buffer for delayed rendering operations
//
int spilcdAllocBackbuffer(SPILCD *pLCD);
//
// Free the back buffer
//
void spilcdFreeBackbuffer(SPILCD *pLCD);
//
// Draw a 1-bpp pattern into the backbuffer with the given color and translucency
// 1 bits are drawn as color, 0 are transparent
// The translucency value can range from 1 (barely visible) to 32 (fully opaque)
//
void spilcdDrawPattern(SPILCD *pLCD, uint8_t *pPattern, int iSrcPitch, int iDestX, int iDestY, int iCX, int iCY, uint16_t usColor, int iTranslucency);
//
// Rotate a 1 or 16-bpp image around a given center point
// valid angles are 0-359
//
void spilcdRotateBitmap(uint8_t *pSrc, uint8_t *pDest, int iBpp, int iWidth, int iHeight, int iPitch, int iCenterX, int iCenterY, int iAngle);
//
// Treat the LCD as a 240x320 portrait-mode image
// or a 320x240 landscape mode image
// This affects the coordinate system and rotates the
// drawing direction of fonts and tiles
//
enum {
  LCD_ORIENTATION_0=0,
  LCD_ORIENTATION_90,
  LCD_ORIENTATION_180,
  LCD_ORIENTATION_270
};

enum {
   LCD_INVALID=0,
   LCD_ILI9341, // 240x320
   LCD_ILI9225, // 176x220
   LCD_HX8357, // 320x480
   LCD_ST7735R, // 128x160
   LCD_ST7735S, // 80x160 with offset of 24,0
   LCD_ST7735S_B, // 80x160 with offset of 26,2
   LCD_SSD1331,
   LCD_SSD1351,
   LCD_ILI9342, // 320x240 IPS
   LCD_ST7789, // 240x320
   LCD_ST7789_240,  // 240x240
   LCD_ST7789_135, // 135x240
   LCD_ST7789_NOCS, // 240x240 without CS, vertical offset of 80, MODE3
   LCD_ST7789_172, // 172x320
   LCD_ST7789_280, // 240x280
   LCD_SSD1283A, // 132x132
   LCD_ILI9486, // 320x480
   LCD_ILI9488, // 320x480
   LCD_GC9A01, // 240x240 round
   LCD_GC9107, // 128x128 tiny (0.85")
   LCD_VIRTUAL, // memory-only display
   LCD_VALID_MAX
};

// Errors returned by various drawing functions
enum {
  BB_ERROR_SUCCESS=0, // no error
  BB_ERROR_INV_PARAM, // invalid parameter
  BB_ERROR_NO_BUFFER, // no backbuffer defined
  BB_ERROR_SMALL_BUFFER // SPI data buffer too small
};

// touch panel types
#define TOUCH_XPT2046 1

#if defined(_LINUX_) && defined(__cplusplus)
}
#endif

#endif // SPI_LCD_H
