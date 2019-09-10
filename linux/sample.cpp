//
// SPI LCD test program
// Written by Larry Bank
// demo written for the Waveshare 1.3" 240x240 IPS LCD "Hat"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bb_spi_lcd.h"
#include <armbianio.h>

int main(int argc, char *argv[])
{
int i;

	i = AIOInitBoard("Raspberry Pi");
	if (i == 0) // problem
	{
		printf("Error in AIOInit(); check if this board is supported\n");
		return 0;
	}
	i = spilcdInit(LCD_ST7789_NOCS, 0, 1, 3250000, -1, 22, 13, 18, -1,-1,-1);
	if (i == 0)
	{
		spilcdFill(0, 1);
		spilcdWriteString(0,0,(char *)"Hello World!", 0x1f,0,FONT_NORMAL, 1);
		printf("Successfully initialized bb_spi_lcd library\n");
		printf("Press ENTER to quit\n");
		getchar();
		spilcdShutdown();
	}
	else
	{
		printf("Unable to initialize the spi_lcd library\n");
	}
   return 0;
} /* main() */
