//
// Example showing how to use the blurSprite() method
// This function provides a fast 3x3 Gaussian filter to blur
// a source sprite into a destination
// On the ESP32-S3, this code is accelerated 400% by using
// the SIMD instructions (separate ASM code)
//
#include <bb_spi_lcd.h>
#include <PNGdec.h>
#include "bart_5clr.h"
PNG png;
BB_SPI_LCD lcd, sprite;
int x_off, y_off;

//
// Load the example image into a sprite
//
void PNGDraw(PNGDRAW *pDraw)
{
  if (pDraw->y < sprite.height()) {
    uint16_t *d = (uint16_t *)sprite.getBuffer();
    d += pDraw->y * sprite.width(); // point to the correct line of the framebuffer
    png.getLineAsRGB565(pDraw, d, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  }
} /* PNGDraw() */

void setup()
{
  int rc;
   lcd.begin(DISPLAY_CYD_4848/*DISPLAY_WS_AMOLED_18*/);
   lcd.fillScreen(TFT_BLACK);
// Load a PNG image into a sprite
  rc = png.openRAM((uint8_t *)bart_5clr, sizeof(bart_5clr), PNGDraw);
  if (rc == PNG_SUCCESS) {
    // Center it on the LCD
      x_off = (lcd.width() - png.getWidth())/2;
      y_off = (lcd.height() - png.getHeight())/2;
      // Create a sprite to hold the blurred image
      sprite.createVirtual(png.getWidth(), png.getHeight()); 
      png.decode(NULL, 0); // decode the image into sprite1
      png.close();
  } else {
    lcd.setTextColor(TFT_RED);
    lcd.setFont(FONT_12x16);
    lcd.println("Error opening PNG");
    while (1) {};
  }
}

void loop()
{
  int i;

  lcd.setTextColor(TFT_GREEN);
  lcd.setFont(FONT_12x16);
  lcd.setCursor(120, 4);
  lcd.print("Original");
  // Display the original image
  lcd.drawSprite(x_off, y_off, &sprite, 1.0f, 0xffffffff, DRAW_TO_LCD);
  delay(3000);

    long l;
    for (i=0; i<40; i++) {
      lcd.setCursor(100,4);
      // Each iteration causes the image blur to increase
      lcd.printf("Iteration: %d", i);
      l = micros();
      sprite.blurGaussian();
      l = micros() - l;
      lcd.drawSprite(x_off, y_off, &sprite, 1.0f, 0xffffffff, DRAW_TO_LCD);
    } // for i
    lcd.setCursor(70, lcd.height()-20);
    lcd.printf("blur time = %d us", (int)l);
    while (1) {};
}