//
// Group5 compressed image test
//
#include <bb_spi_lcd.h>
#include "smiley.h"
BB_SPI_LCD lcd;

const uint16_t u16Color[8] = {TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE, TFT_GREY, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};
void setup()
{
  float f;
  int i;

  lcd.begin(DISPLAY_CYD_543);
  lcd.fillScreen(TFT_BLACK);
  // bb_spi_lcd can draw compressed graphics directly onto a display
  // without any local framebuffer
  for (i=0; i<50; i++) {
      f = 0.02f * (float)i;
      // Draw the smiley image scaled and in different colors
      lcd.drawG5Image(smiley, rand() % lcd.width(), rand() % lcd.height(), TFT_BLACK, u16Color[i & 7], f);
  }
} /* setup() */

void loop()
{

} /* loop() */
