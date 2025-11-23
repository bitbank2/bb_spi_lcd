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
#define DC_PIN 9
#define RESET_PIN -1
#define CS_PIN 7
#define LED_PIN 13
#define LCD_TYPE LCD_ST7789
#else
// Pin definitions for Adafruit PiTFT HAT
// GPIO 25 = Pin 22
#define DC_PIN 25
// GPIO 27 = Pin 13
#define RESET_PIN -1
// GPIO 8 = Pin 24
#define CS_PIN 8
// GPIO 24 = Pin 18
#define LED_PIN -1
#define LCD_TYPE LCD_ILI9341
#endif

BB_SPI_LCD lcd;
static uint8_t ucBuffer[4096];

int main(int argc, char *argv[])
{
int i;

// int spilcdInit(int iLCDType, int bFlipRGB, int bInvert, int bFlipped, int32_t iSPIFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin);
    i = lcd.begin(LCD_TYPE, FLAGS_NONE, 62500000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, -1,-1,-1);
    if (i == 0)	{
	lcd.setRotation(90);
	lcd.fillScreen(0);
	lcd.setTextColor(TFT_BLUE, TFT_BLACK);
	for (i=0; i<30; i++) {
	    lcd.drawStringFast((char *)"Hello World!", 0, i*8, FONT_8x8);
	}
	printf("Successfully initialized bb_spi_lcd library\n");
	printf("Press ENTER to quit\n");
	getchar();
	//lcd.spilcdShutdown(&lcd);
    } else {
	printf("Unable to initialize the spi_lcd library\n");
    }
   return 0;
} /* main() */
