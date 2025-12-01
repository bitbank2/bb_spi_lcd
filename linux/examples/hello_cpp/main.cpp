//
// SPI LCD test program
// Written by Larry Bank
//
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bb_spi_lcd.h>

#define PIMORONI_HAT

#ifdef PIMORONI_HAT
#define MISO_PIN 0
#define MOSI_PIN 0
#define CLK_PIN 1
#define DC_PIN 9
#define RESET_PIN -1
#define CS_PIN 7
#define LED_PIN 13
#define LCD_TYPE LCD_ST7789
// For Radxa Dragon Q6A
// GPIO CHIP number
//#define MISO_PIN 4
// SPI bus number
//#define MOSI_PIN 12
//#define DC_PIN 48
//#define RESET_PIN -1
//#define CS_PIN 55
//#define LED_PIN 56
#else
// Pin definitions for Adafruit PiTFT HAT
uint8_t u8DataPins[8] = {14,15,16,17,18,19,20,21};
#define LCD_TYPE LCD_ILI9488
#define RESET_PIN 13
#define RD_PIN -1
#define WR_PIN 27
#define CS_PIN 22
#define DC_PIN 4
#define BUS_WIDTH 8
//#define DC_PIN 25
//#define RESET_PIN -1
//#define CS_PIN 8
//#define LED_PIN -1
//#define LCD_TYPE LCD_ILI9341
#endif

BB_SPI_LCD lcd;
static uint8_t ucBuffer[4096];
const uint16_t colors[8] = {TFT_BLACK, TFT_BLUE, TFT_GREEN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_CYAN, TFT_WHITE};
int main(int argc, char *argv[])
{
int i;

#ifdef PIMORONI_HAT
// int spilcdInit(int iLCDType, int bFlipRGB, int bInvert, int bFlipped, int32_t iSPIFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);
    i = lcd.begin(LCD_TYPE, FLAGS_NONE, 20000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, MISO_PIN, MOSI_PIN, CLK_PIN);
#else
    i = lcd.beginParallel(LCD_TYPE, FLAGS_NONE, RESET_PIN, RD_PIN, WR_PIN, CS_PIN
, DC_PIN, BUS_WIDTH, u8DataPins, 0);
#endif
    if (i == 0)	{
	lcd.setRotation(90);
	lcd.fillScreen(0);
        for (int j=0; j<200; j++) {
	    lcd.setTextColor(colors[j & 7], TFT_BLACK);
	    for (i=0; i<30; i++) {
	        lcd.drawStringFast((char *)"Hello World!", 0, i*8, FONT_8x8);
	    } // for i
        } // for j
	printf("Successfully initialized bb_spi_lcd library\n");
	printf("Press ENTER to quit\n");
	getchar();
	//lcd.spilcdShutdown(&lcd);
    } else {
	printf("Unable to initialize the spi_lcd library\n");
    }
   return 0;
} /* main() */
