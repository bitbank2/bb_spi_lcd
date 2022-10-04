//
// Demo sketch to show how to use
// the old Arduino UNO LCD shields
// (8-bit parallel ILI9486/ILI9341)
// with the UM TinyS3 or FeatherS3
// written by Larry Bank 7/22/2022
// bitbank@pobox.com
//
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#include "matrix_320x176.h"

BB_SPI_LCD lcd;
AnimatedGIF gif;
int x_offset, y_offset;
uint16_t *pImage;

#define BUS_WIDTH 8
#define LCD_RD -1
#if defined(ARDUINO_TINYS3)
// TinyS3
#define LCD_WR 21
#define LCD_CS 43
#define LCD_DC 44
#define LCD_RST -1
uint8_t u8Pins[BUS_WIDTH] = {6,2,3,4,5,9,7,8}; // TinyS3
#elif defined(ARDUINO_TINYS2)
// TinyS2
#define LCD_WR 18
#define LCD_CS 43
#define LCD_DC 44
#define LCD_RST -1
uint8_t u8Pins[BUS_WIDTH] = {33,5,6,7,17,9,38,8}; // TinyS2
#elif defined(ARDUINO_FEATHERS3) || defined(ARDUINO_FEATHERS2)
// FeatherS3
#define LCD_WR 7
#define LCD_CS 38
#define LCD_DC 3
#define LCD_RST 33
#define SD_CS 1
uint8_t u8Pins[BUS_WIDTH] = {10,11,5,6,12,14,18,17}; // FeatherS3
#endif
//
// ** Select the correct LCD type here **
// The 240x320 2.x" shields use the ILI9341
// and the 320x480 3.x" shields use the ILI9486
// Some have variations which require inverting
// the colors or the X/Y direction, so enable those
// flags if needed
#define LCD_TYPE LCD_ILI9486
//#define LCD_TYPE LCD_ILI9341
//#define LCD_FLAGS FLAGS_NONE
#define LCD_FLAGS (FLAGS_SWAP_RB | FLAGS_FLIPX)
// The backlight is wired permanently on
#define LCD_BKLT -1

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t c, *s, ucTransparent;
    uint16_t *d, *usPalette;
    int x, iWidth, iHeight;

    if (pDraw->y == 0) { // first line, allocate a backing buffer
       iWidth = pDraw->iWidth; iHeight = pDraw->iHeight;
       if (iWidth > lcd.width()) iWidth = lcd.width();
       if (iHeight > lcd.height()) iHeight = lcd.height();
//       lcd.setAddrWindow(x_offset+pDraw->iX, y_offset+pDraw->iY, iWidth, iHeight);
      // Serial.printf("frame: %d, %d, %d, %d\n", pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight);
    }
    iWidth = pDraw->iWidth;
    if (iWidth > lcd.width())
       iWidth = lcd.width();
    usPalette = pDraw->pPalette;

    s = pDraw->pPixels;
    d = &pImage[(pDraw->y + pDraw->iY) * pDraw->iWidth];
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      memset(d, 0, pDraw->iWidth * 2); // set background color to black
    }
    ucTransparent = pDraw->ucTransparent;
    for (x=0; x<iWidth; x++)
    {
      c = *s++;
      if (c != ucTransparent)
        d[x] = usPalette[c];
    } // for x
    if (pDraw->y + pDraw->iY < lcd.height()) {
       lcd.setAddrWindow(x_offset+pDraw->iX, y_offset+pDraw->iY+pDraw->y, iWidth, 1);
      lcd.pushPixels(d, pDraw->iWidth);
    }
} /* GIFDraw() */

void playGIF(void)
{
  if (gif.open((uint8_t *)matrix_320x176, sizeof(matrix_320x176), GIFDraw))
  {
    x_offset = (lcd.width() - gif.getCanvasWidth())/2;
    if (x_offset < 0) x_offset = 0;
    y_offset = (lcd.height() - gif.getCanvasHeight())/2;
    if (y_offset < 0) y_offset = 0;
    pImage = (uint16_t *)malloc(gif.getCanvasWidth() * gif.getCanvasHeight() * 2);
    while (gif.playFrame(false, NULL))
    {
    }
    gif.close();
    free(pImage);
  } // if GIF opened
} /* playGIF() */

void setup() {
   Serial.begin(115200);
   // The parallel LCD function also works on RP2040 and Teensy 4.1 boards
   lcd.beginParallel(LCD_TYPE, LCD_FLAGS, LCD_RST, LCD_RD, LCD_WR, LCD_CS, LCD_DC, BUS_WIDTH, u8Pins);
   lcd.setRotation(90);
   lcd.fillScreen(0);
   lcd.setFont(FONT_12x16);
   lcd.setTextColor(0x7e0,0x0000);
   lcd.println("Starting GIF Demo...");
   gif.begin(GIF_PALETTE_RGB565_BE); // big endian pixels
   delay(3000);
   lcd.fillScreen(0);
} /* setup() */

void loop() {
  playGIF();
} /* loop() */
