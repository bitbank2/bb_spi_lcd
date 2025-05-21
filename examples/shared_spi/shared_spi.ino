//
// Example showing how to share the SPI bus between the display and SD card
// This is a challenging situation because the SPI bus instance handle can be
// reset by the second device to be initialized (e.g. init the LCD, then the SD)
// This is resolved by initilizing the SPI bus outside of both libraries and sharing
// the instance / handle
//
// This example was written for the Waveshare ESP32-C6 1.47" LCD board
// It has a micro SD card slot on the back and a ST7789 172x320 LCD
// on the front. This example plays an animated GIF file read from
// the SD card
//
// N.B. A downside to using the shared SPI bus is that DMA is disabled since the Arduino SPI
// class doesn't support DMA and it is controlling the SPI bus handle
//
#include <bb_spi_lcd.h>
#include <AnimatedGIF.h> // https://github.com/bitbank2/AnimatedGIF.git
#include <SPI.h>
#include <SD.h>
AnimatedGIF gif;
BB_SPI_LCD lcd;
int iOffX, iOffY;
File file;
uint8_t *pFrameBuffer;
// These GPIOs are for the shared SPI bus
#define SPI_MOSI 6
#define SPI_SCK 7
#define SPI_MISO 5

// SD card slot
#define SD_CS 4
// LCD
#define TFT_CS 14
#define TFT_DC 15
#define TFT_RESET 21
#define TFT_LED 22

// Change this to the file you would like to display
#define gif_filename "/thisisfine_307x172.gif"
//
// Draw callback from GIF decoder
//
// called once for each line of the current frame
// MCUs with very little RAM would have to test for disposal methods, transparent pixels
// and translate the 8-bit pixels through the palette to generate the final output.
// The code for MCUs with enough RAM is much simpler because the AnimatedGIF library can
// generate "cooked" pixels that are ready to send to the display
//
void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0) { // set the memory window when the first line is rendered
    lcd.setAddrWindow(iOffX + pDraw->iX, iOffY + pDraw->iY, pDraw->iWidth, pDraw->iHeight);
  }
  // For all other lines, just push the pixels to the display
  lcd.pushPixels((uint16_t *)pDraw->pPixels, pDraw->iWidth);
} /* GIFDraw() */

// Callback functions to access a file on the SD card
void *myOpen(const char *filename, int32_t *size) {
  file = SD.open(filename);
  if (file) {
    *size = file.size();
  }
  return (void *)&file;
}

void myClose(void *pHandle) {
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
}
int32_t myRead(GIFFILE *pFile, uint8_t *buffer, int32_t length) {
//  File *f = (File *)pFile->fHandle;
//  return f->read(buffer, length);
    int32_t iBytesRead;
    iBytesRead = length;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < length)
       iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int32_t)f->read(buffer, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
}
int32_t mySeek(GIFFILE *pFile, int32_t iPosition)
{
//  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
//  i = micros() - i;
//  Serial.printf("Seek time = %d us\n", i);
  return pFile->iPos;
} /* GIFSeekFile() */

void setup()
{
   gif.begin(BIG_ENDIAN_PIXELS);
   pinMode(SD_CS, OUTPUT);
   digitalWrite(SD_CS, 1); // start with the SD card disabled
   SPI.end(); // make sure there isn't already an instance using the same pins
   SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, -1); // don't let it control the CS line of either device
   lcd.begin(LCD_ST7789_172, FLAGS_NONE, &SPI, TFT_CS, TFT_DC, TFT_RESET, TFT_LED);
   lcd.setRotation(90);
   lcd.fillScreen(TFT_BLACK);
   lcd.setTextColor(TFT_GREEN, TFT_BLACK);
   lcd.setFont(FONT_12x16);
   lcd.println("Shared SPI Test");
   if (!SD.begin(SD_CS, SPI, 20000000)) {
     lcd.println("Card Mount Failed");
   } else {
     lcd.println("Card Mount Succeeded");
   }
} /* setup() */

void loop()
{
  int w, h;
  if (gif.open(gif_filename, myOpen, myClose, myRead, mySeek, GIFDraw)) {
    w = gif.getCanvasWidth();
    h = gif.getCanvasHeight();
    iOffX = (lcd.width() - w)/2;
    iOffY = (lcd.height() - h)/2;
    lcd.printf("Canvas size: %dx%d\n", w, h);
    lcd.printf("Loop count: %d\n", gif.getLoopCount());
    delay(3000);
    lcd.fillScreen(TFT_BLACK);
    pFrameBuffer = (uint8_t *)malloc(w*(h+2));
    gif.setDrawType(GIF_DRAW_COOKED); // we want the library to generate ready-made pixels
    gif.setFrameBuf(pFrameBuffer);
    while (1) { // play forever
        while (gif.playFrame(true, NULL)) {
        }
        gif.reset();
    } // while (1)
  } else {// if GIF opened
    lcd.println("Error opening GIF");
    while (1) {
      vTaskDelay(1);
    }
  }
} /* loop() */
