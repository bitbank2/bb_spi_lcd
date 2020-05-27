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

Features:<br>
---------<br>
- Supports the most popular display controllers (SSD1351, ST7735, ST7789, ILI9341, ILI9342, HX8357, ILI9468)<br>
- Supports display modes: 0/180 degree rotated, inverted, BGR or RGB color order<br>
- Supports 90/270 degree rotated operation (software does the rotation)<br>
- Optimized primitives for text, lines, rectangles, ellipses and bitmap drawing<br>
- Fast (50x) drawing of Adafruit format custom fonts with optional blanking mode to erase old data without flickering<br>
- Load and display 4 and 16-bit Windows BMP files<br>
- Deferred rendering allows quickly preparing a back buffer, then displaying it<br>
- Callbacks allow working with non-SPI displays (e.g. 8/16-bit parallel)<br>
<br>


If you find this code useful, please consider sending a donation to support my work

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

