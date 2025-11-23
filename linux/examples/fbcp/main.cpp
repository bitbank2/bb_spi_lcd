//
// LCD Monitor Demo
// written by Larry Bank Nov 23, 2025
// Copyright(c) 2025 BitBank Software, Inc.
//
#include <drm.h>
#include <drm_fourcc.h>
#include <drm_mode.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <unistd.h>
#include <arm_neon.h>
#include <string.h>
#include <stdio.h>
#include <bb_spi_lcd.h>
BB_SPI_LCD lcd;
uint8_t *pFramebuffer;
int center_x, center_y;
// Variable that keeps count on how much screen has been partially updated
int n = 0;

#define PIMORONI_HAT

#ifdef PIMORONI_HAT
#define DC_PIN 9
#define RESET_PIN -1
#define CS_PIN 7
#define LED_PIN 13
#define LCD_TYPE LCD_ST7789
#endif
//
// Convert the 16 or 32-bit RPI GUI desktop to 1-bit per pixel
//
void ConvertGUI(uint8_t *pGUI, int w, int h, int bpp)
{
int iPitch = lcd.width() * 2;

    for (int y=0; y<lcd.height(); y++) {
        uint16_t *d = (uint16_t *)&pFramebuffer[y * iPitch];
	if (bpp == 32) {
	uint8_t *s = &pGUI[y * w * 4];
        for (int x=0; x<lcd.width(); x+=8) { // work 8 pixels at a time
            uint8x8x4_t u8RGB32 = vld4_u8(s);
	    uint16x8_t r, g, b, temp;
	    // widen each color to 16-bits
	    b = vmovl_u8(u8RGB32.val[0]);
	    g = vmovl_u8(u8RGB32.val[1]);
	    r = vmovl_u8(u8RGB32.val[2]);
	    // shift to upper half
	    b = vshlq_n_u16(b, 8);
	    g = vshlq_n_u16(g, 8);
	    r = vshlq_n_u16(r, 8);
	    s += 32;
            temp = vsriq_n_u16(r, g, 5); // shift green elements right and insert red elements
            temp = vsriq_n_u16(temp, b, 11); // shift blue elements right and insert
	    temp = vreinterpretq_u16_u8(vrev16q_u8(vreinterpretq_u8_u16(temp))); // byte swap
            vst1q_u16(d, temp); // write 8 pixels
            d += 8;
	} // for x
	} else { // 16-bpp
	    uint8_t *s = &pGUI[y * w * 2];
	    for (int x=0; x<lcd.width(); x+=16) { // work 16 pixels at a time
                uint8x16_t pixels0 = vld1q_u8(s);
		s += 16;
		uint8x16_t pixels1 = vld1q_u8(s);
		s += 16;
		pixels0 = vrev16q_u8(pixels0); // byte swap
                pixels1 = vrev16q_u8(pixels1);
		vst1q_u8((uint8_t *)d, pixels0);
		d += 8;
		vst1q_u8((uint8_t *)d, pixels1);
		d += 8;
	    }
	} // 16-bpp
    } // for y
} /* ConvertGUI() */

int main(int argc, char *argv[])
{
int iFrame;
int err, drm_fd, prime_fd; // ret, retval = -1;
unsigned int i, card;
uint32_t fb_id, crtc_id;
drmModePlaneRes *plane_res;
drmModePlane *plane;
drmModeFB *fb;
char buf[256];
uint64_t has_dumb;
uint8_t *pGUI;

    lcd.begin(LCD_TYPE, FLAGS_NONE, 62500000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN, -1, -1, -1);
    lcd.setRotation(90);
    lcd.fillScreen(0);
    lcd.allocBuffer();
    pFramebuffer = (uint8_t*)lcd.getBuffer();

    for (card = 0; ; card++) {
        snprintf(buf, sizeof(buf), "/dev/dri/card%u", card);
        drm_fd = open(buf, O_RDWR | O_CLOEXEC);
        if (drm_fd < 0) {
            fprintf(stderr, "Could not open KMS/DRM device.\n");
            return -1;
        }
        if (drmGetCap(drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) >= 0 && has_dumb) {
            break;
	} else {
            close(drm_fd);
	}
    } // for card
    //drm_fd = open(buf, O_RDWR | O_CLOEXEC);
    if (drm_fd < 0) {
        fprintf(stderr, "Could not open KMS/DRM device.\n");
        return -1;
    }
    if (drmSetClientCap(drm_fd, DRM_CLIENT_CAP_ATOMIC, 1)) {
        fprintf(stderr, "Unable to set atomic cap.\n");
        close(drm_fd);
        return -1;
    }
    if (drmSetClientCap(drm_fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1)) {
        fprintf(stderr, "Unable to set universal planes cap.\n");
        close(drm_fd);
        return -1;
    }
    plane_res = drmModeGetPlaneResources(drm_fd);
    if (!plane_res) {
        fprintf(stderr, "Unable to get plane resources.\n");
        close(drm_fd);
        return -1;
    }
    for (i = 0; i < plane_res->count_planes; i++) {
        plane = drmModeGetPlane(drm_fd, plane_res->planes[i]);
        fb_id = plane->fb_id;
        crtc_id = plane->crtc_id;
        drmModeFreePlane(plane);
        if (fb_id != 0 && crtc_id != 0) break;
    }
    if (i == plane_res->count_planes) {
        fprintf(stderr, "No planes found\n");
        drmModeFreePlaneResources(plane_res); 
        close(drm_fd);
        return -1;
    }
    fb = drmModeGetFB(drm_fd, fb_id);
    if (!fb) {
        fprintf(stderr, "Failed to get framebuffer %" PRIu32 ": %s\n",
                       fb_id, strerror(errno));
        drmModeFreePlaneResources(plane_res);
        close(drm_fd);
        return -1;
    }
    err = drmPrimeHandleToFD(drm_fd, fb->handle, O_RDONLY, &prime_fd);
    if (err < 0) {
        fprintf(stderr, "Failed to retrieve prime handler: %s\n", strerror( -err ));
        drmModeFreePlaneResources(plane_res);
        drmModeFreeFB(fb);
        close(drm_fd);
        return -1;
    }
    pGUI = (uint8_t *)mmap(NULL, (fb->bpp >> 3) * fb->width * fb->height,
                      PROT_READ, MAP_PRIVATE, prime_fd, 0);
    if (pGUI == MAP_FAILED) {
        //ret = -errno;
        fprintf(stderr, "Unable to mmap prime buffer\n");
        drmModeFreePlaneResources(plane_res);
        drmModeFreeFB(fb);
        close(drm_fd);
        return -1;
    }

    iFrame = 0;
    printf("Starting video copying of fb: %dx%dx%d-bpp\n", fb->width, fb->height, fb->bpp);
    while (iFrame < 400) {
        iFrame++;
        ConvertGUI(pGUI, fb->width, fb->height, fb->bpp);
	lcd.display();
    } // while running

    munmap(pFramebuffer, (fb->bpp >> 3) * fb->width * fb->height);
    drmModeFreePlaneResources(plane_res);
    drmModeFreeFB(fb);
    close(drm_fd);
    // clear the display
    lcd.fillScreen(0);
    return 0;
} /* main () */

