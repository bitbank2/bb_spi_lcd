//
// Generic SPI display example
// This example shows how to use bb_spi_lcd if you are in possession
// of a generic SPI LCD that isn't part of a pre-configured system.
// For example, if you have a Feather board and connect it to a
// ST7735 LCD sold separately
//
// Written by Larry Bank
// Copyright (c) 2025 BitBank Software, Inc.
// bitbank@pobox.com
//
#include <bb_spi_lcd.h>
BB_SPI_LCD lcd;

// Pins for the Keyboard Featherwing with a FeatherS3 board controlling it
#define CS_PIN 1
#define DC_PIN 3
#define RESET_PIN -1
#define LED_PIN -1
#define MISO_PIN MISO
#define MOSI_PIN MOSI
#define CLK_PIN SCK

void setup()
{
  // The begin method prototype:
  // begin(int iType, int iFlags, int iFreq, int iCSPin, int iDCPin, int iResetPin, int iLEDPin, int iMISOPin, int iMOSIPin, int iCLKPin)
  // For unused pins, pass -1
  lcd.begin(LCD_ILI9341, FLAGS_NONE, 40000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, MISO_PIN, MOSI_PIN, CLK_PIN);
  lcd.setRotation(90);
  // The default orientation is portrait. To set landscape mode use setRotation(270)
  lcd.fillScreen(TFT_BLACK); // the default is to not keep a local copy of the framebuffer since it's large
  lcd.setTextColor(TFT_GREEN); // all functions will send data to the display and be visible immediately
  lcd.setFont(FONT_12x16);
  lcd.println("Generic SPI display");
  lcd.println("Using bb_spi_lcd");
}

void loop()
{
}

