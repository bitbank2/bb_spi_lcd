//
// Parallel LCD support for bb_spi_lcd
//

#include <Arduino.h>
#include <SPI.h>
#include <bb_spi_lcd.h>

static uint8_t u8WR, u8RD, u8DC, u8CS;

#ifdef ARDUINO_ARCH_RP2040
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
// This PIO code is Copyright (c) 2021 Pimoroni Ltd
// --------------- //
// st7789_parallel //
// --------------- //

#define st7789_parallel_wrap_target 0
#define st7789_parallel_wrap 1

static const uint16_t st7789_parallel_program_instructions[] = {
            //     .wrap_target
    0x6008, //  0: out    pins, 8         side 0
    0xb042, //  1: nop                    side 1
            //     .wrap
};
static const struct pio_program st7789_parallel_program = {
    .instructions = st7789_parallel_program_instructions,
    .length = 2,
    .origin = -1,
};

static inline pio_sm_config st7789_parallel_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + st7789_parallel_wrap_target, offset + st7789_parallel_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}
uint32_t parallel_sm;
PIO parallel_pio;
uint32_t parallel_offset;
uint32_t parallel_dma;
#endif // ARDUINO_ARCH_RP2040

void ParallelReset(void) {
    
} /* ParallelReset() */

void ParallelDataWrite(uint8_t *pData, int len, int iMode)
{
#ifdef ARDUINO_ARCH_RP2040
// Do everything with DMA since it's the simplest way to push the data
    while (dma_channel_is_busy(parallel_dma))
      ;
    gpio_put(u8DC, (iMode == MODE_DATA)); // DC pin (change after last DMA action completes)
    dma_channel_set_trans_count(parallel_dma, len, false);
    dma_channel_set_read_addr(parallel_dma, pData, true);

// If we didn't use the PIO state machine, this is how we would do it
// (I used this first before enabling the state machine code)
//  for (int i=0; i<len; i++) {
//     uint32_t c = pData[i];
//     if (c != old) {
//        gpio_clr_mask(u32Mask); // clear bits 14-21
//        gpio_set_mask(c << 14);
//        old = c;
//     }
//     digitalWrite(12, LOW); // toggle WR low to high to latch the data
//     digitalWrite(12, HIGH);
//  } // for i
//  gpio_put(10, 1); // deactivate CS
#endif // ARDUINO_ARCH_RP2040
} /* ParallelDataWrite() */
//
// Initialize the parallel bus info
//
void ParallelDataInit(uint8_t RD_PIN, uint8_t WR_PIN, uint8_t CS_PIN, uint8_t DC_PIN, int iBusWidth, uint8_t *data_pins)
{
    u8WR = WR_PIN;
    u8RD = RD_PIN;
    u8DC = DC_PIN;
    u8CS = CS_PIN;
    
#ifdef ARDUINO_ARCH_RP2040

// Set up GPIO for output mode
//  for (int i=10; i<=21; i++) { // I/O lines
//     pinMode(i, OUTPUT);
//  }
  pinMode(RD_PIN, OUTPUT); // RD
  digitalWrite(RD_PIN, HIGH); // RD deactivated
//  digitalWrite(12, HIGH); // WR deactivated
//  pinMode(10, OUTPUT); // CS
//  digitalWrite(10, HIGH); // CS deactivated
    gpio_set_function(DC_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(DC_PIN, GPIO_OUT);

    gpio_set_function(CS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 0); // CS always active
      parallel_pio = pio1;
      parallel_sm = pio_claim_unused_sm(parallel_pio, true);
      parallel_offset = pio_add_program(parallel_pio, &st7789_parallel_program);
      pio_gpio_init(parallel_pio, WR_PIN);
      for(int i = 0; i < 8; i++) {
        pio_gpio_init(parallel_pio, data_pins[0] + i); // NB: must be sequential GPIO numbers starting from D0
      }
      pio_sm_set_consecutive_pindirs(parallel_pio, parallel_sm, data_pins[0], 8, true);
      pio_sm_set_consecutive_pindirs(parallel_pio, parallel_sm, WR_PIN, 1, true);

      pio_sm_config c = st7789_parallel_program_get_default_config(parallel_offset);

      sm_config_set_out_pins(&c, data_pins[0], 8);
      sm_config_set_sideset_pins(&c, WR_PIN);
      sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
      sm_config_set_out_shift(&c, false, true, 8);
      sm_config_set_clkdiv(&c, 4);
      
      pio_sm_init(parallel_pio, parallel_sm, parallel_offset, &c);
      pio_sm_set_enabled(parallel_pio, parallel_sm, true);

      parallel_dma = dma_claim_unused_channel(true);
      dma_channel_config config = dma_channel_get_default_config(parallel_dma);
      channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
      channel_config_set_bswap(&config, false);
      channel_config_set_dreq(&config, pio_get_dreq(parallel_pio, parallel_sm, true));
      dma_channel_configure(parallel_dma, &config, &parallel_pio->txf[parallel_sm], NULL, 0, false);
#endif // ARDUINO_ARCH_RP2040
} /* ParallelDataInit() */
