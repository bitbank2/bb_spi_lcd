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
#ifndef __SS_OLED_H__
enum {
  FONT_NORMAL=0,
  FONT_LARGE,
  FONT_SMALL,
  FONT_STRETCHED
};
#endif

// Proportional font data taken from Adafruit_GFX library
/// Font data stored PER GLYPH
#ifndef _ADAFRUIT_GFX_H
typedef struct {
  uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
  uint8_t width;         ///< Bitmap dimensions in pixels
  uint8_t height;        ///< Bitmap dimensions in pixels
  uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
  int8_t xOffset;        ///< X dist from cursor pos to UL corner
  int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
  uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
  GFXglyph *glyph;  ///< Glyph array
  uint8_t first;    ///< ASCII extents (first char)
  uint8_t last;     ///< ASCII extents (last char)
  uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;
#endif // _ADAFRUIT_GFX_H

typedef enum
{
 MODE_DATA = 0,
 MODE_COMMAND
} DC_MODE;

#if defined(_LINUX_) && defined(__cplusplus)
extern "C" {
#endif

// Sets the D/C pin to data or command mode
void spilcdSetMode(int iMode);
//
// Provide a small temporary buffer for use by the graphics functions
//
void spilcdSetTXBuffer(uint8_t *pBuf, int iSize);

//
// Choose the gamma curve between 2 choices (0/1)
// ILI9341 only
//
int spilcdSetGamma(int iMode);

// Initialize the library
int spilcdInit(int iLCDType, int bFlipRGB, int bInvert, int bFlipped, int32_t iSPIFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);

//
// Initialize the touch controller
//
int spilcdInitTouch(int iType, int iChannel, int iSPIFreq);

//
// Set touch calibration values
// These are the minimum and maximum x/y values returned from the sensor
// These values are used to normalize the position returned from the sensor
//
void spilcdTouchCalibration(int iminx, int imaxx, int iminy, int imaxy);

//
// Shut down the touch interface
//
void spilcdShutdownTouch(void);

//
// Read the current touch values
// values are normalized to 0-1023 range for x and y
// returns: -1=not initialized, 0=nothing touching, 1=good values
//
int spilcdReadTouchPos(int *pX, int *pY);

// Turns off the display and frees the resources
void spilcdShutdown(void);

// Fills the display with the byte pattern
int spilcdFill(unsigned short usPattern, int bRender);

//
// Draw a rectangle and optionally fill it
// With the fill option, a color gradient will be created
// between the top and bottom lines going from usColor1 to usColor2
//
void spilcdRectangle(int x, int y, int w, int h, unsigned short usColor1, unsigned short usColor2, int bFill, int bRender);

//
// Reset the scroll position to 0
//
void spilcdScrollReset(void);

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
void spilcdScroll(int iLines, int iFillColor);

//
// Draw a NxN tile scaled 150% in both directions
int spilcdDrawTile150(int x, int y, int iTileWidth, int iTileHeight, unsigned char *pTile, int iPitch, int bRender);

// Draw a NxN tile
int spilcdDrawTile(int x, int y, int iTileWidth, int iTileHeight, unsigned char *pTile, int iPitch, int bRender);

// Draw a 16x16 tile with variable cols/rows removed
int spilcdDrawMaskedTile(int x, int y, unsigned char *pTile, int iPitch, int iColMask, int iRowMask, int bRender);

// Draw a NxN tile scaled to 2x width, 1.5x height with pixel averaging
int spilcdDrawScaledTile(int x, int y, int cx, int cy, unsigned char *pTile, int iPitch, int bRender);

int spilcdDraw53Tile(int x, int y, int cx, int cy, unsigned char *pTile, int iPitch, int bRender);

// Draw a 16x16 tile as 16x13 (with priority to non-black pixels)
int spilcdDrawRetroTile(int x, int y, unsigned char *pTile, int iPitch, int bRender);

// Draw a 16x16 tile scaled to 16x14 with pixel averaging
int spilcdDrawSmallTile(int x, int y, unsigned char *pTile, int iPitch, int bRender);

// Write a text string to the display at x (column 0-83) and y (row 0-5)
int spilcdWriteString(int x, int y, char *szText, int iFGColor, int iBGColor, int iFontSize, int bRender);

// Write a text string of 8x8 characters
// quickly to the LCD with a single data block write.
// This reduces the number of SPI transactions and speeds it up
// This function only allows the FONT_NORMAL and FONT_SMALL sizes
// 
int spilcdWriteStringFast(int x, int y, char *szText, unsigned short usFGColor, unsigned short usBGColor, int iFontSize);
//
// Draw a string in a proportional font you supply
//
int spilcdWriteStringCustom(GFXfont *pFont, int x, int y, char *szMsg, uint16_t usFGColor, uint16_t usBGColor, int bBlank);
//
// Get the width and upper/lower bounds of text in a custom font
//
void spilcdGetStringBox(GFXfont *pFont, char *szMsg, int *width, int *top, int *bottom);

// Sets a pixel to the given color
// Coordinate system is pixels, not text rows (0-239, 0-319)
int spilcdSetPixel(int x, int y, unsigned short usPixel, int bRender);

// Set the software orientation
int spilcdSetOrientation(int iOrientation);

// Draw an ellipse with X and Y radius
void spilcdEllipse(int32_t centerX, int32_t centerY, int32_t radiusX, int32_t radiusY, unsigned short color, int bFilled, int bRender);
//
// Draw a line between 2 points using Bresenham's algorithm
// 
void spilcdDrawLine(int x1, int y1, int x2, int y2, unsigned short usColor, int bRender);
//
// Public wrapper function to write data to the display
//
void spilcdWriteDataBlock(uint8_t *pData, int iLen, int bRender);
//
// Position the "cursor" to the given
// row and column. The width and height of the memory
// 'window' must be specified as well. The controller
// allows more efficient writing of small blocks (e.g. tiles)
// by bounding the writes within a small area and automatically
// wrapping the address when reaching the end of the window
// on the curent row
//
void spilcdSetPosition(int x, int y, int w, int h, int bRender);
//
// Draw a 4, 8 or 16-bit Windows uncompressed bitmap onto the display
// Pass the pointer to the beginning of the BMP file
// Optionally stretch to 2x size
// returns -1 for error, 0 for success
//
int spilcdDrawBMP(uint8_t *pBMP, int iDestX, int iDestY, int bStretch, int iTransparent, int bRender);

//
// Show part or all of the back buffer on the display
// Used after delayed rendering of graphics
//
void spilcdShowBuffer(int x, int y, int cx, int cy);
//
// Returns the current backbuffer address
//
uint16_t * spilcdGetBuffer(void);
//
// Allocate the back buffer for delayed rendering operations
//
int spilcdAllocBackbuffer(void);
//
// Free the back buffer
//
void spilcdFreeBackbuffer(void);
//
// Draw a 1-bpp pattern into the backbuffer with the given color and translucency
// 1 bits are drawn as color, 0 are transparent
// The translucency value can range from 1 (barely visible) to 32 (fully opaque)
//
void spilcdDrawPattern(uint8_t *pPattern, int iSrcPitch, int iDestX, int iDestY, int iCX, int iCY, uint16_t usColor, int iTranslucency);
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
#define LCD_ORIENTATION_NATIVE 1
#define LCD_ORIENTATION_ROTATED 2

enum {
   LCD_INVALID=0,
   LCD_ILI9341,
   LCD_HX8357,
   LCD_ST7735R, // 128x160
   LCD_ST7735S, // 80x160 with offset of 24,0
   LCD_ST7735S_B, // 80x160 with offset of 26,2
   LCD_SSD1331,
   LCD_SSD1351,
   LCD_ILI9342,
   LCD_ST7789,  // 240x240
   LCD_ST7789_135, // 135x240
   LCD_ST7789_NOCS, // 240x240 without CS, vertical offset of 80, MODE3
   LCD_SSD1283A // 132x132
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
