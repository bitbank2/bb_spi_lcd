//
// ESP32-S3 SIMD optimized code
// Written by Larry Bank
// Copyright (c) 2025 BitBank Software, Inc.
// Project started March 30, 2025
//
#if defined (ARDUINO_ARCH_ESP32) && !defined(NO_SIMD)
#if __has_include ("dsps_fft2r_platform.h")
#include "dsps_fft2r_platform.h"
#if (dsps_fft2r_sc16_aes3_enabled == 1)
	.text
	.align 4

    .type   s3_byteswap, @function
    .align 4
    .global s3_byteswap

# Fast big/little endian byte swap function
# The input pointer must be 16-byte aligned
# and the pixel count must be a multiple of 16
#
#                                        A2            A3        A4
# Call as void s3_byteswap(uint16_t *pSrc, uint16_t *pDest, int iPixelCount);
  s3_byteswap:
    .align 4
    entry    a1,16

.byteswap_loop:

  ee.vld.128.ip    q0,a2,16   # load 16 RGB565 src pixels into q0 & q1
  ee.vld.128.ip    q1,a2,16
  mv.qr q6,q0               # swap endian order
  mv.qr q7,q1
  ee.vunzip.8 q6,q0         # swap the byte order
  ee.vunzip.8 q7,q1
  ee.vzip.8 q0,q6
  ee.vzip.8 q1,q7

  ee.vst.128.ip q0,a3,16    # store 16 finished destination pixels
  ee.vst.128.ip q1,a3,16
  addi.n a4,a4,-16          # decrement pixel count by 16
  bnez.n a4,.byteswap_loop

  # return value of 0
    retw.n

#endif // dsps_fft2r_sc16_aes3_enabled
#endif // __has_include
#endif // ESP32
