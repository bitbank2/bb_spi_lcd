#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bb_spi_lcd.h>
#include <bb_captouch.h>
#include <time.h>
BBCapTouch bbct;
BB_SPI_LCD lcd;

#define LCD_CS 18
#define LCD_CLK 17
#define LCD_D0 13
#define LCD_D1 14
#define LCD_D2 15
#define LCD_D3 16
#define LCD_RST 19
// The SDA value passed to init() is really the Linux I2C bus number
#define TOUCH_SDA 1
#define TOUCH_SCL 0
#define TOUCH_INT 20
#define TOUCH_RST 21
// The delay between clock rise/fall
#define CLK_DELAY 1

const char *szNames[] = {"Unknown", "FT6x36", "GT911", "CST820", "CST226", "MXT144", "AXS15231"};

int main(int argc, char *argv[])
{
TOUCHINFO ti;
int iType;
int bDone = 0;

    lcd.beginQSPI(LCD_CO5300B, 0, LCD_CS, LCD_CLK, LCD_D0, LCD_D1, LCD_D2, LCD_D3, LCD_RST, CLK_DELAY);

    if (lcd.width() < lcd.height()) {
        lcd.setRotation(90);
    }
    lcd.fillScreen(TFT_BLACK);
    lcd.setFont(FONT_12x16);
    lcd.setTextColor(TFT_GREEN, TFT_BLACK);

    lcd.drawString("Probing I2C bus...",0,0);
    bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
    iType = bbct.sensorType();
    if (iType <= 0) {
        lcd.setTextColor(TFT_RED, TFT_BLACK);
        lcd.drawString("Error, exiting...",0,16);
        return -1;
    }
    lcd.drawString("Sensor found: ", 0,16);
    lcd.drawString(szNames[iType], -1, -1);
    lcd.drawString("Touch 2 spots to exit", 0,32);

    while (!bDone) {
        if (bbct.getSamples(&ti)) {
            if (ti.count == 2) bDone = 1; // exit with 2 finger touch
            lcd.fillRect(ti.x[0], ti.y[0], 3, 3, TFT_BLUE);
        } // if touch event happened
    } // while (1)
    lcd.fillScreen(TFT_BLACK);

    return 0;
} /* main () */

