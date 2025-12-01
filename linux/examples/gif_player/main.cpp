#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h>
#include <time.h>
AnimatedGIF gif;
BB_SPI_LCD lcd;
#define RADXA

#ifdef RADXA
#define LCD_TYPE LCD_ST7789
#define DC_PIN 48
#define RESET_PIN -1
#define CS_PIN 55
#define LED_PIN 56
#define GPIO_CHIP 4
#define SPI_NUM 12
#else
// definitions for parallel 3.5" Kumon LCD shield
uint8_t u8DataPins[8] = {14,15,16,17,18,19,20,21};
#define LCD_TYPE LCD_ILI9488
#define RESET_PIN 13
#define RD_PIN -1
#define WR_PIN 27
#define CS_PIN 22
#define DC_PIN 4
#define BUS_WIDTH 8
#endif // RADXA

int MilliTime()
{
int iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000*res.tv_sec + res.tv_nsec/1000000;

    return iTime;
} /* MilliTime() */

int main(int argc, char *argv[])
{
  int iTime, iFrame;
  int w, h;
  uint8_t *pStart;
  int iLoops = 1;
  if (argc != 2 && argc != 3) {
      printf("bb_spi_lcd gif_player\nUsage: gif_player <file> <optional loop count>\n");
      return -1;
  }
#ifdef RADXA
  lcd.begin(LCD_TYPE, FLAGS_NONE, 30000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, GPIO_CHIP, SPI_NUM, 0);
#else
  lcd.beginParallel(LCD_TYPE, FLAGS_SWAP_RB, RESET_PIN, RD_PIN, WR_PIN, CS_PIN
, DC_PIN, BUS_WIDTH, u8DataPins, 18); // last parameter is the delay cycles for parallel data output
#endif // RADXA
  lcd.setRotation(90);
  lcd.fillScreen(0);

  if (argc == 3) iLoops = atoi(argv[2]);

  gif.begin(BIG_ENDIAN_PIXELS);
  iFrame = 0;
  if (gif.open(argv[1], NULL)) {
      w = gif.getCanvasWidth();
      h = gif.getCanvasHeight();
      printf("Successfully opened GIF; Canvas size = %d x %d\n", w, h);
      gif.setFrameBuf((uint8_t*)malloc(w*h*3));
      pStart = gif.getFrameBuf();
      pStart += w*h; // point to RGB565 pixels
      gif.setDrawType(GIF_DRAW_COOKED); // fully prepare pixels
      iFrame = 0;
      iTime = MilliTime();
      for (int i=0; i<iLoops; i++) {
          while (gif.playFrame(false, NULL, NULL)) {
	      lcd.setAddrWindow(0,0,w,h);
	      //i = MilliTime();
	      lcd.pushPixels((uint16_t *)pStart, w * h);
	      //i = MilliTime() - i;
	      iFrame++;
          } // while decoding frames
	  gif.reset();
      } // fore each loop
      iTime = MilliTime() - iTime;
      printf("%d frames in %d ms\n", iFrame, iTime);
  }
  usleep(1000000); // wait a second before erasing the display
  lcd.fillScreen(0);
  return 0;
} /* main () */

