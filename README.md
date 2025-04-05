bb_spi_lcd (BitBank SPI Color LCD/OLED library)<br>
Project started 5/15/2017<br>
Copyright (c) 2017-2019 BitBank Software, Inc.<br>
Written by Larry Bank<br>
bitbank@pobox.com<br>
<br>
![bb_spi_lcd](/demo.jpg?raw=true "bb_spi_lcd")
<br>
The purpose of this code is to easily control color OLED/LCD
displays with a rich set of functions. The code can be built as
both an Arduino and Linux library. For Arduino, there are a wide variety
of target platforms and some don't have enough RAM to support every feature.
Specifically, AVR (Uno/Nano/Mega) don't have enough RAM to have
a back buffer. Without the back buffer, only text, pixel, rectangle and line drawing
are possible. With a back buffer, a host of other functions become available
such as ellipses, translucent/transparent rotating bitmaps, and transparent text.<br>

## Features
- Supports the most popular display controllers (SSD1351, ST7735, ST7789, ILI9225, ILI9341, ILI9342, HX8357, ILI9468)<br>
- 5 built in font sizes (6x8, 8x8, 12x16, 16x16, 16x32)
- Supports display modes: 0/90/180/270 degree rotated, inverted, BGR or RGB color order<br>
- Direct display drawing, backbuffer (RAM) drawing or both simultaneously<br>
- Display object structure (SPILCD) allows multiple simultaenous displays of different types to be controlled by a single MCU<br>
- Bit Bang option allows controlling SPI displays on any GPIO pins<br>
- Communication callback functions allows controlling parallel and custom connected displays<br>
- DMA on SAMD21, SAMD51 and ESP32 targets<br>
- Compiles on Arduino and Linux (e.g. Raspberry Pi)
- Optimized primitives for text, lines, rectangles, ellipses and bitmap drawing<br>
- Fast (50x) drawing of Adafruit format custom fonts with optional blanking mode to erase old data without flickering<br>
- 50% scaled drawing of Adafruit_GFX fonts with antialiasing
- Load and display 4, 8 and 16-bit Windows BMP files (including RLE)<br>
- Deferred rendering allows quickly preparing a back buffer, then displaying it<br>
- Callbacks allow working with non-SPI displays (e.g. 8/16-bit parallel)<br>
- Named display configurations of popular products (e.g. M5Stack Core2)
<br>

## Named Displays
The following displays are supported through a named constant. Their GPIO connections and (special) initialization sequence are built-in to bb_spi_lcd so that using them is as simple as:
lcd.begin(DISPLAY_CYD);<br>
Special init sequences such as those required for the M5Stack products are included too (PMIC setup). This means that you no longer need to use the M5 unified libraries to work with their products.

| Name | Description |
| --- | --- |
| `DISPLAY_PYBADGE_M4` | Adafruit's EdgeBadge / PyBadge |
| `DISPLAY_WIO_TERMINAL` | The original Seeed Studio SAMD51 Wio |
| `DISPLAY_TEENSY_ILI9341` | Teensy 4.x LCD shield |
| `DISPLAY_LOLIN_S3_MINI_PRO` | Small white PCB with 0.85" 128x128 LCD |
| `DISPLAY_M5STACK_STICKC` | The original Stick-C with 80x160 ST7735 |
| `DISPLAY_M5STACK_STICKCPLUS` | The newer Stick-C with 135x240 ST7789 |
| `DISPLAY_M5STACK_CORE2` | The M5Stack ESP32 Core2 w/320x240 ILI9342 |
| `DISPLAY_M5STACK_CORES3` | The ESP32-S3 w/320x240 ILI9342 |
| `DISPLAY_T_DONGLE_S3` | LilyGo ESP32-S3 USB dongle with 80x160 ST7735 |
| `DISPLAY_T_DISPLAY_S3` | LilyGo ESP32-S3 1.91" 170x320 ST7789 |
| `DISPLAY_T_DISPLAY_S3_PRO` | LilyGo ESP32-S3 2.23" 222x480 ST7796 |
| `DISPLAY_T_DISPLAY_S3_LONG` | LilyGo ESP32-S3 3.4" 180x640 |
| `DISPLAY_T_DISPLAY_S3_AMOLED` | LilyGo ESP32-S3 1.9" 536x240 AMOLED |
| `DISPLAY_T_DISPLAY_S3_AMOLED_164` | LilyGo ESP32-S3 1.64" 280x456 AMOLED |
| `DISPLAY_T_DISPLAY` | LilyGo original ESP32 135x240 ST7789 |
| `DISPLAY_T_QT` | LilyGo ESP32-S3 w/0.85" 128x128 |
| `DISPLAY_T_QT_C6` | LilyGo ESP32-C6 w/0.85" 128x128 |
| `DISPLAY_T_TRACK` | LilyGo ESP32-S3 126x294 AMOLED |
| `DISPLAY_TUFTY2040` | Pimoroni RP2040 w/2.4" parallel 240x320 ST7789 |
| `DISPLAY_MAKERFABS_S3` | MakerFabs 3.5" ESP32-S3 w/3.5" 320x480 ILI9488 16-bit parallel |
| `DISPLAY_M5STACK_ATOMS3` | M5Stack ESP32-S3 Atom w/0.85" 128x128 |
| `DISPLAY_WT32_SC01_PLUS` | ESP32-S3 w/3.5" 320x480 ST7796 8-bit parallel |
| `DISPLAY_CYD` | The original ESP32-2432S28R Cheap Yellow Display |
| `DISPLAY_CYD_2USB` | The updated 2.8" CYD with 2 USB ports |
| `DISPLAY_CYD_128` | ESP32-C3 1.28" round 240x240 |
| `DISPLAY_CYD_28C` | ESP32 2.8" 240x320 w/capacitive touch |
| `DISPLAY_CYD_24R` | ESP32 2.4" w/resistive touch |
| `DISPLAY_CYD_24C` | ESP32 2.4" w/capacitive touch |
| `DISPLAY_CYD_35` | ESP32 w/ILI9488 SPI |
| `DISPLAY_CYD_35R` | ESP32 w/ILI9488 SPI and resistive touch |
| `DISPLAY_CYD_22C` | ESP32 w/8-bit parallel ST7789 and capactive touch |
| `DISPLAY_CYD_543` | JC4827W543 4.3" ESP32-S3 w/QSPI 480x270 |
| `DISPLAY_CYD_535` | JC3248W535 3.5" ESP32-S3 w/320x480 |
| `DISPLAY_CYD_518` | 1.8" ESP32-S3 round 360x360 QSPI |
| `DISPLAY_CYD_700` | 7.0" ESP32-S3 800x480 ST7262 RGB panel |
| `DISPLAY_CYD_8048` | 4.3 and 5.5" 800x480 ESP32-S3 RGB panel |
| `DISPLAY_CYD_4848` | MakerFabs 4" 480x480 ESP32-S3 RGB panel |
| `DISPLAY_CYD_XIAO_ROUND` | Seeed Studio round LCD shield 240x240 |
| `DISPLAY_CYD_P4_1024x600` | Guition JC1060P470C ESP32-P4 1024x600 MIPI |
| `DISPLAY_WS_AMOLED_18` | Waveshare ESP32-S3 AMOLED Touch 1.8" |
| `DISPLAY_WS_ROUND_146` | Waveshare ESP32-S3 round 1.46" 412x412 |
| `DISPLAY_WS_AMOLED_241` | Waveshare ESP32-S3 2.41" 600x450 AMMOLED |
| `DISPLAY_WS_LCD_169` | Waveshare ESP32-S3 1.69 240x280 ST7789 |
| `DISPLAY_UM_480x480` | UnexpectedMaker ESP32-S3 4" 480x480 RGB Panel |

If you find this code useful, please consider sponsoring me here on Github or sending a donation to support my work

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

