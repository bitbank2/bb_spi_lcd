//
// ESP32-S3 SIMD optimized code
// Written by Larry Bank
// Copyright (c) 2025 BitBank Software, Inc.
// Project started March 21, 2025
//
#if defined (ARDUINO_ARCH_ESP32) && !defined(NO_SIMD)
#if __has_include ("dsps_fft2r_platform.h")
#include "dsps_fft2r_platform.h"
#if (dsps_fft2r_sc16_aes3_enabled == 1)
	.text
	.align 4

    .type   s3_blur_be, @function
    .global s3_blur_be

# Blur a row of RGB565 big-endian pixels with a 3x3 filter
# The pixel count must be a multiple of 4
#                                   A2            A3            A4        A5
# Call as void s3_blur_be(uint16_t *pSrc, uint16_t *pDest, uint32_t count, uint32_t pitch, uint32_t *pMasks);

 s3_blur_be:
  entry    a1,16

# load the pixel mask into Q0 and the rounding value into Q1
  ee.vldbc.32.ip q0,a6,4
  ee.vldbc.32.ip q1,a6,0

  movi.n a11,0
  sub  a11,a11,a5           # negative pitch value in a11
  movi.n a6,8
  movi.n a7,1
  movi.n a8,2
  movi.n a9,4               # prepare shift amounts
  movi.n a10,16

.blur_loop_be:

  wsr.sar    a7             # SAR (shift amount register) = 1
  wur.sar_byte a9           # SAR_BYTE register set to 4 bytes of shift

  add a2,a2,a11             # move source pointer to line above
  ee.vld.l.64.ip  q2,a2,8   # load 8 RGB565 src (line above) pixels into q2
  ee.vld.h.64.ip  q2,a2,-8  # do it as 2 reads because it will be unaligned
  add a2,a2,a5              # reset to current line
  mv.qr q7,q2               # big endian data, need to swap the byte order
  ee.vunzip.8 q7,q2         # swap the byte order to be big-endian
  ee.vzip.8 q2,q7

  mv.qr q3,q2
# prepare top left pixel
  ee.vzip.16 q2,q3          # double the RGB565 pixels in preparation
  ee.andq q2,q2,q0          # mask 8 doubled pixels with 07e0f81f
  ee.andq q3,q3,q0
  mv.qr q4,q2               # top left pixel starts sum in Q4

# top center pixel
  ee.src.q q7,q2,q3         # shift the register pair down by 1 double pixel
  ee.vsl.32 q7,q7           # times 2
  ee.vadds.s32 q4,q4,q7     # add to sum

# top right pixel
  wur.sar_byte a6           # set shift to 8 bytes
  ee.src.q q7,q2,q3         # shift down another pixel
  ee.vadds.s32 q4,q4,q7     # add to sum

# middle row
  ee.vld.l.64.ip  q2,a2,8   # load 8 RGB565 src (current line) pixels into q2
  ee.vld.h.64.ip  q2,a2,-8  # do it as 2 reads because it will be unaligned
  mv.qr q7,q2               # big endian data, need to swap the byte order
  ee.vunzip.8 q7,q2         # swap the byte order to be big-endian
  ee.vzip.8 q2,q7

  mv.qr q3,q2
# prepare middle left pixel
  ee.vzip.16 q2,q3          # double the RGB565 pixels in preparation
  ee.andq q2,q2,q0
  ee.andq q3,q3,q0          # mask 8 doubled pixels with 07e0f81f
  ee.vsl.32 q7,q2           # times 2
  ee.vadds.s32 q4,q4,q7     # add to sum

# center (current) pixel
  wsr.sar    a8             # shift amount = 2
  wur.sar_byte a9           # shift byte amount = 4
  ee.src.q q7,q2,q3
  ee.vsl.32 q7,q7           # times 4
  ee.vadds.s32 q4,q4,q7     # add to sum

# middle right pixel
  wur.sar_byte a6           # shift byte amount = 8
  wsr.sar   a7              # shift amount = 1
  ee.src.q q7,q2,q3
  ee.vsl.32 q7,q7           # times 2
  ee.vadds.s32 q4,q4,q7     # add to sum

# bottom row
  add a2,a2,a5
  ee.vld.l.64.ip  q2,a2,8   # load 8 RGB565 src (line below) pixels into q2
  ee.vld.h.64.ip  q2,a2,0   # do it as 2 reads because it will be unaligned
  add a2,a2,a11             # a2 points to current row again
  mv.qr q7,q2               # big endian data, need to swap the byte order
  ee.vunzip.8 q7,q2         # swap the byte order to be big-endian
  ee.vzip.8 q2,q7

  mv.qr q3,q2
# prepare bottom left pixel
  ee.vzip.16 q2,q3          # double the RGB565 pixels in preparation
  ee.andq q2,q2,q0
  ee.andq q3,q3,q0          # mask 8 doubled pixels with 07e0f81f
  ee.vadds.s32 q4,q4,q2     # add to sum

# bottom middle pixel
  wsr.sar    a7             # shift amount = 1
  wur.sar_byte a9           # shift byte amount = 4
  ee.src.q q7,q2,q3
  ee.vsl.32 q7,q7           # times 2
  ee.vadds.s32 q4,q4,q7     # add to sum

# bottom right pixel
  wur.sar_byte a6           # set shift to 8 bytes
  ee.src.q q7,q2,q3         # shift down another pixel
  ee.vadds.s32 q4,q4,q7     # add to sum

  ee.vadds.s32 q4,q4,q1     # add rounding factor
  wsr.sar   a9              # shift amount to 4
  ee.vsr.32 q4,q4           # divide by 16
  ee.andq q4,q4,q0          # mask back to 07e0f81f
  wsr.sar   a10             # shift amount to 16
  ee.vsr.32 q5,q4           # shift down upper half
  ee.orq q4,q4,q5           # combine upper and lower halves
  mv.qr q7,q4
  ee.vunzip.16 q7,q4        # get even 16-bit values

  mv.qr q2,q7               # big endian data, need to swap the byte order
  ee.vunzip.8 q7,q2         # swap the byte order to be big-endian
  ee.vzip.8 q2,q7

  ee.movi.32.a q2,a12,0      # output will always be unaligned, so store
  s32i  a12,a3,0             # using 32-bit registers
  ee.movi.32.a q2,a12,1
  s32i  a12,a3,4 
  addi.n a3,a3,8

  addi.n a4,a4,-4           # decrement pixel count by 4
  bnez.n a4,.blur_loop_be

  # return value of 0
    retw.n

#endif // dsps_fft2r_sc16_aes3_enabled
#endif // __has_include
#endif // ESP32
