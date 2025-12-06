//
// bb_spi_lcd Linux specific functions
//
#ifndef __BB_LINUX_IO__
#define __BB_LINUX_IO__

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifndef __MEM_ONLY__
#include <fcntl.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#ifndef OUTPUT
#define false 0
#define true 1
#define PROGMEM
#define memcpy_P memcpy
#define OUTPUT 1
#define INPUT 0
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0
#define pgm_read_byte(a) (*(uint8_t*)(a))
#endif // !OUTPUT
#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif
//uint8_t *pDMA, ucTXBuf[4096];
//static uint8_t transfer_is_done = true;
extern volatile int bSetPosition;
static int iGPIOChip;
struct gpiod_chip *chip = NULL;
#ifdef GPIOD_API
struct gpiod_line *lines[64];
#else
struct gpiod_line_request *lines[64];
#endif
#endif // !__MEM_ONLY__

extern uint8_t u8BW, u8WR, u8RD, u8DC, u8CS, u8CMD;
static int spi_fd; // SPI handle
static uint8_t gpio_bit_zero; // bit zero of parallel GPIO pins
volatile uint32_t *gpio;
volatile uint32_t *set_reg, *clr_reg, *sel_reg;
#define GPIO_BLOCK_SIZE 4*1024
static uint32_t u32Speed;
// N.B. the default buffer size is 4K
#define RPI_DMA_SIZE 4096 
// wrapper/adapter functions to make the code work on Linux
int digitalRead(int iPin)
{
#ifdef __MEM_ONLY__
    (void)iPin;
#else
    if (lines[iPin] == 0) return 0;
#ifdef GPIOD_API // 1.x (old) API
    return gpiod_line_get_value(lines[iPin]);
#else // 2.x (new)
    return gpiod_line_request_get_value(lines[iPin], iPin) == GPIOD_LINE_VALUE_ACTIVE;
#endif
#endif // __MEM_ONLY__
} /* digitalRead() */

void digitalWrite(int iPin, int iState)
{
#ifdef __MEM_ONLY__
    (void)iPin; (void)iState;
#else
    if (lines[iPin] == 0) return;
#ifdef GPIOD_API // old 1.6 API
    gpiod_line_set_value(lines[iPin], iState);
#else // new 2.x API
   gpiod_line_request_set_value(lines[iPin], iPin, (iState) ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE);
#endif
#endif // __MEM_ONLY__
} /* digitalWrite() */

void pinMode(int iPin, int iMode)
{
#ifdef __MEM_ONLY__
    (void)iPin; (void)iMode;
#else
#ifdef GPIOD_API // old 1.6 API
   if (chip == NULL) {
       char szTemp[32];
       snprintf(szTemp, sizeof(szTemp), "gpiochip%d", iGPIOChip);
       chip = gpiod_chip_open_by_name(szTemp);
   }
   lines[iPin] = gpiod_chip_get_line(chip, iPin);
   if (iMode == OUTPUT) {
       gpiod_line_request_output(lines[iPin], CONSUMER, 0);
   } else if (iMode == INPUT_PULLUP) {
       gpiod_line_request_input_flags(lines[iPin], CONSUMER, GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP);
   } else { // plain input
       gpiod_line_request_input(lines[iPin], CONSUMER);
   }
#else // new 2.x API
   struct gpiod_line_settings *settings;
   struct gpiod_line_config *line_cfg;
   struct gpiod_request_config *req_cfg;
   char szTemp[32];
   snprintf(szTemp, sizeof(szTemp), "/dev/gpiochip%d", iGPIOChip);
   //printf("opening %s\n", szTemp);
   chip = gpiod_chip_open(szTemp);
   if (!chip) {
        printf("chip open failed\n");
           return;
   }
   settings = gpiod_line_settings_new();
   if (!settings) {
        printf("line_settings_new failed\n");
           return;
   }
   gpiod_line_settings_set_direction(settings, (iMode == OUTPUT) ? GPIOD_LINE_DIRECTION_OUTPUT : GPIOD_LINE_DIRECTION_INPUT);
   line_cfg = gpiod_line_config_new();
   if (!line_cfg) return;
   gpiod_line_config_add_line_settings(line_cfg, (const unsigned int *)&iPin, 1, settings);
   req_cfg = gpiod_request_config_new();
   gpiod_request_config_set_consumer(req_cfg, CONSUMER);
   lines[iPin] = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
   gpiod_request_config_free(req_cfg);
   gpiod_line_config_free(line_cfg);
   gpiod_line_settings_free(settings);
   gpiod_chip_close(chip);
#endif
#endif // __MEM_ONLY__
} /* pinMode() */

static void delay(int iMS)
{
#ifdef __MEM_ONLY__
    (void)iMS;
#else
  usleep(iMS * 1000);
#endif
} /* delay() */

static void delayMicroseconds(int iMS)
{
#ifdef __MEM_ONLY__
    (void)iMS;
#else
  usleep(iMS);
#endif
} /* delayMicroseconds() */

void linux_spi_write(uint8_t *pBuf, int iLen, uint32_t iSPISpeed)
{
#ifdef __MEM_ONLY__
    (void)pBuf; (void)iLen; (void)iSPISpeed;
#else
struct spi_ioc_transfer spi;
   memset(&spi, 0, sizeof(spi));
   while (iLen) { // max 64k transfers (default is 4k)
       int j = iLen;
       if (j > RPI_DMA_SIZE) j = RPI_DMA_SIZE;
       spi.tx_buf = (unsigned long)pBuf;
       spi.len = j;
       spi.speed_hz =iSPISpeed;
       //spi.cs_change = 1;
       spi.bits_per_word = 8;
       ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
       iLen -= j;
       pBuf += j;
   }
#endif
} /* linux_spi_write() */

void linux_spi_init(int iMISOPin, int iMOSIPin, int iCLKPin)
{
#ifdef __MEM_ONLY__
    (void)iMISOPin; (void)iMOSIPin; (void)iCLKPin;
#else
    iGPIOChip = iMISOPin;
    char szTemp[32];
    snprintf(szTemp, sizeof(szTemp), "/dev/spidev%d.%d", iMOSIPin, iCLKPin);
    spi_fd = open(szTemp, O_RDWR);
    if (spi_fd <= 0) {
	    printf("Error opening %s\n", szTemp);
    }
#endif
} /* linux_spi_init() */

/**
 * Wait N CPU cycles (ARM CPU only)
 * On the RPI Zero 2W, 1 microsecond ˜= 463 wait loops
 */
static void wait_cycles(unsigned int n)
{
    if(n) while(n--) { asm volatile("nop"); }
}
//
// Write parallel data to the Raspberry Pi (1/2/3/4) GPIO registers
//
void linux_parallel_write(uint8_t *pData, int len, int iMode)
{
#ifdef __MEM_ONLY__
    (void)pData; (void)len; (void)iMode;
#else
        const uint32_t DATA_BIT_0 = gpio_bit_zero;
        uint32_t c, e;
        const uint32_t u32WR = 1 << u8WR;
        const uint32_t xor_mask = (0xff << DATA_BIT_0);
        const uint32_t xor_mask2 = (xor_mask | u32WR);

        *clr_reg = (1 << u8CS); // activate CS
        if (iMode == 0) /*MODE_DATA*/ { // DC high
             *set_reg = (1 << u8DC);
        } else {
             *clr_reg = (1 << u8DC); // DC low
        }
        //wait_cycles(u32Speed);

        for (int i=0; i<len; i++) {
            c = *pData++;
            e = c << DATA_BIT_0;
            *clr_reg = (e ^ xor_mask2); // set 0 bits and WR low
            *set_reg = e; // set 1 bits
	    // The latch timing is lopsided because the data setup time
	    // and rising edge of the write signal needs a little more time
	    // compared to the falling edge of the signal
            wait_cycles(u32Speed);
            *set_reg = u32WR; // clock high
            wait_cycles(u32Speed/2);
        } // for i
        *set_reg = (1 << u8CS); // de-activate CS
#endif
} /* linux_parallel_write() */

//
// Configure a GPIO pin on the Raspberry Pi as an OUTPUT
//
void set_gpio_output(int pin) {
#ifdef __MEM_ONLY__
    (void)pin;
#else
    // The pin in GPIOSEL0 goes from 0-9 and next pins 10-19
    // is on reg GPIOSEL1 and so on
    int reg = pin / 10;
    int shift = (pin % 10) * 3;
    // Clear the 3 bits for the pin and set it to 001 (output)
    gpio[reg] = (gpio[reg] & ~(7 << shift)) | (1 << shift);
#endif
} /* set_gpio_output() */
//
// Use parallel GPIO to bit bang QSPI protocol
//
void linux_qspi_init(uint32_t u32Freq, uint8_t u8Bit0, uint8_t u8RST, uint8_t u8CS, uint8_t u8CLK)
{
#ifdef __MEM_ONLY__
    (void)u32Freq; (void)u8Bit0;
#else
   int mem_fd;
   void *gpio_map;
   uint32_t u32GPIO_BASE;

   gpio_bit_zero = u8Bit0;
   u8WR = u8CLK;
   u32Speed = u32Freq; // delay amount for parallel data too
   // Determine if we're on a RPI 2/3 or 4 based on the RAM size
   struct sysinfo info;
   sysinfo(&info);
//printf("ram size = %d\n", info.totalram);
   if (info.totalram < 950000000) { // must be Zero2W or RPI 3B
//       printf("RPI 2/3\n");
       u32GPIO_BASE = 0x3f000000+0x00200000;
   } else { // RPI 4B
//       printf("RPI 4B\n");
       u32GPIO_BASE = 0xfe000000+0x00200000;
   }
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("Failed to open /dev/mem, try running as root");
        exit(EXIT_FAILURE);
    }
    // Map GPIO memory to our address space
    gpio_map = mmap(
        NULL,                 // Any address in our space will do
        GPIO_BLOCK_SIZE,      // Map length = 4K
        PROT_READ | PROT_WRITE, // Enable reading & writing to mapped memory
        MAP_SHARED,           // Shared with other processes
        mem_fd,               // File descriptor for /dev/mem
        u32GPIO_BASE          // Offset to GPIO peripheral
    );
    close(mem_fd);
    if (gpio_map == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    // volatile pointer to prevent compiler optimizations
    gpio = (volatile uint32_t *)gpio_map;
    sel_reg = gpio;
    set_reg = &gpio[7];
    clr_reg = &gpio[10];

#endif
} /* linux_qspi_init() */

//
// Send bytes (pixels) on bits 0-3
//
void linux_qspi_send_bytes(uint8_t *pData, int iLen)
{
const uint32_t u32Mask = (0xf << gpio_bit_zero);
uint32_t u32, u32Not;

    for (int i=0; i<iLen; i++) {
        u32 = *pData++;
        u32Not = ~u32;
        // high nibble first
        *clr_reg = (1 << u8WR); // clock low
        *set_reg = ((u32 << (gpio_bit_zero-4)) & u32Mask); // high nibble
        *clr_reg = ((u32Not << (gpio_bit_zero-4)) & u32Mask);
        wait_cycles(1);
        *set_reg = (1 << u8WR); // clock high
        wait_cycles(1);
        *clr_reg = (1 << u8WR); // clock low
        *set_reg = ((u32 << gpio_bit_zero) & u32Mask); // low nibble
        *clr_reg = ((u32Not << gpio_bit_zero) & u32Mask);
        wait_cycles(1);
        *set_reg = (1 << u8WR); // clock high
    }
} /* linux_qspi_send_bytes() */

//
// Send a byte (command or parameter)
// on bit 0 only
//
void linux_qspi_send_byte(uint8_t u8)
{
    for (int i=0; i<8; i++) {
        *clr_reg = (1 << u8WR); // clock low
        if (u8 & 0x80) { // msb first
            *set_reg = (1 << gpio_bit_zero);
        } else {
            *clr_reg = (1 << gpio_bit_zero);
        }
        u8 <<= 1;
        wait_cycles(1);
        *set_reg = (1 << u8WR); // clock high
        wait_cycles(1); 
    }
} /* linux_qspi_send_byte() */

void linux_qspi_send_cmd(uint8_t u8CMD, uint8_t *pParams, int iLen)
{
    *clr_reg = (1 << u8CS) | (1 << u8WR); // CLK & CS low
    linux_qspi_send_byte(0x02); // send command byte on data bit 0
    linux_qspi_send_byte(0); // 2nd 8 bits of address
    linux_qspi_send_byte(u8CMD); // 3rd 8 bits of address
    linux_qspi_send_byte(0); // final 8 bits of address
    for (int i=0; i<iLen; i++) {
        linux_qspi_send_byte(pParams[i]); // send all command parameters on bit 0 also
    }
    *set_reg = (1 << u8CS); // CS high = deactivate bus
} /* linux_qspi_send_cmd() */

void linux_qspi_send_data(uint8_t *pData, int iLen)
{
    *clr_reg = (1 << u8CS) | (1 << u8WR); // CLK+CS low
    linux_qspi_send_byte(0x32); // send command byte on data bit 0
    linux_qspi_send_byte(0); // 2nd 8 bits of address
    if (bSetPosition) { // first data after a setPosition
        linux_qspi_send_byte(0x2c);
        bSetPosition = 0;
    } else { // all data after...
        linux_qspi_send_byte(0x3c);
    }
    linux_qspi_send_byte(0); // final 8 bits of address
    linux_qspi_send_bytes(pData, iLen); // send 4 bits of pixel data
    *set_reg = (1 << u8CS); // CS high = deactivate bus
} /* linux_qspi_send_data() */
//
// Map the RPI GPIO registers into virtual memory
// and initialize the correct pins as output
//
void linux_parallel_init(uint32_t u32Freq, uint8_t u8Bit0)
{
#ifdef __MEM_ONLY__
    (void)u32Freq; (void)u8Bit0;
#else
   int mem_fd;
   void *gpio_map;
   uint32_t u32GPIO_BASE;

   gpio_bit_zero = u8Bit0;
   u32Speed = u32Freq; // delay amount for parallel data too
   // Determine if we're on a RPI 2/3 or 4 based on the RAM size
   struct sysinfo info;
   sysinfo(&info);
//printf("ram size = %d\n", info.totalram);
   if (info.totalram < 950000000) { // must be Zero2W or RPI 3B
//       printf("RPI 2/3\n");
       u32GPIO_BASE = 0x3f000000+0x00200000;
   } else { // RPI 4B
//       printf("RPI 4B\n");
       u32GPIO_BASE = 0xfe000000+0x00200000;
   }
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("Failed to open /dev/mem, try running as root");
        exit(EXIT_FAILURE);
    }
    // Map GPIO memory to our address space
    gpio_map = mmap(
        NULL,                 // Any address in our space will do
//        0x150010,
        GPIO_BLOCK_SIZE,      // Map length = 4K
        PROT_READ | PROT_WRITE, // Enable reading & writing to mapped memory
        MAP_SHARED,           // Shared with other processes
        mem_fd,               // File descriptor for /dev/mem
        u32GPIO_BASE          // Offset to GPIO peripheral
    );
    close(mem_fd);
    if (gpio_map == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    // volatile pointer to prevent compiler optimizations
    gpio = (volatile uint32_t *)gpio_map;
//    gpio[0x5401] = 0; // disable UART/SPI1/SPI2

    sel_reg = gpio;
    set_reg = &gpio[7];
    clr_reg = &gpio[10];

    set_gpio_output(u8CS);
    set_gpio_output(13); // RESET
    *set_reg = (1 << 13); // RESET disabled
    set_gpio_output(u8DC);
    set_gpio_output(u8WR);
    for (int i=14; i<22; i++) { // data pins
        set_gpio_output(i);
    }
#endif
} /* linux_parallel_init() */
#endif // __BB_LINUX_IO__
