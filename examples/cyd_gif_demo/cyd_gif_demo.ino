//
// CYD (Cheap Yellow Display) GIF example
//
#include <bb_captouch.h>
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#define GIF_NAME earth_128x128
#include "earth_128x128.h"

uint8_t *pFrameBuffer;

// Define one of these depending on your device
// CYD_35C = 320x480 3.5" cap touch
// CYD_28C = 240x320 (any size)
// CYD_128 = 240x240 round 1.28" ESP32-C3
#define CYD_35C
//#define CYD_128C
//#define CYD_28C

// 3.5" 320x480 LCD w/capacitive touch
#ifdef CYD_35C
#define LCD DISPLAY_CYD_35
#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25
#endif

#ifdef CYD_28C
// 2.8" ESP32 LCD board with the GT911 touch controller
#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25
#define LCD DISPLAY_CYD
#endif

#ifdef CYD_128C
// 1.28" ESP32-C3 round LCD board with the CST816D touch controller
#define TOUCH_SDA 4
#define TOUCH_SCL 5
#define TOUCH_INT 0
#define TOUCH_RST 1
#define QWIIC_SDA 21
#define QWIIC_SCL 20
#define LCD DISPLAY_CYD_128
#endif
AnimatedGIF gif;
BBCapTouch bbct;
BB_SPI_LCD lcd;
int iOffX, iOffY;
//
// Draw callback from GIF decoder
//
// called once for each line of the current frame
// MCUs with very little RAM would have to test for disposal methods, transparent pixels
// and translate the 8-bit pixels through the palette to generate the final output.
// The code for MCUs with enough RAM is much simpler because the AnimatedGIF library can
// generate "cooked" pixels that are ready to send to the display
//
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0) { // set the memory window when the first line is rendered
    lcd.setAddrWindow(iOffX + pDraw->iX, iOffY + pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  // For all other lines, just push the pixels to the display
  lcd.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth, DRAW_TO_LCD | DRAW_WITH_DMA);
} /* GIFDraw() */

void setup() {
  Serial.begin(115200);

  gif.begin(BIG_ENDIAN_PIXELS);
  lcd.begin(LCD);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  lcd.setFont(FONT_12x16);
  lcd.setCursor(0, 0);
  lcd.println("GIF + Touch Test");
  lcd.println("Touch to pause/unpause");
  delay(3000);
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
} /* setup() */

void loop() {
  int w, h;

  if (gif.open((uint8_t *)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", w, h);
    pFrameBuffer = (uint8_t *)heap_caps_malloc(w*(h+2), MALLOC_CAP_8BIT);
  while (1) {
      gif.setDrawType(GIF_DRAW_COOKED); // we want the library to generate ready-made pixels
      gif.setFrameBuf(pFrameBuffer);
      iOffX = (lcd.width() - w)/2;
      iOffY = (lcd.height() - h)/2;
      while (gif.playFrame(true, NULL)) {
        TOUCHINFO ti;
        if (bbct.getSamples(&ti)) { // a touch event
          delay(500);
          bbct.getSamples(&ti); // get release event
          while (!bbct.getSamples(&ti)) {};
          delay(50);
          bbct.getSamples(&ti); // get release event
        }
      }
      gif.reset();
    }
  } // while (1)
} /* loop() */
