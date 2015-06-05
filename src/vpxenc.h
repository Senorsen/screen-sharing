
#ifndef __VPXENC_H__
#define __VPXENC_H__

#include <vpx/vpx_codec.h>
#include <vpx/vpx_image.h>
#include <vpx/vpx_integer.h>

#define VP8_FOURCC (0x30385056)
#define VP9_FOURCC (0x30395056)
#define VP8_FOURCC_MASK (0x00385056)
#define VP9_FOURCC_MASK (0x00395056)

typedef struct VpxInterface {
    const char *const name;
    const uint32_t fourcc;
    vpx_codec_iface_t *(*const codec_interface)();
} VpxInterface;

struct VpxRational {
    int numerator;
    int denominator;
};

typedef struct {
    uint32_t codec_fourcc;
    int frame_width;
    int frame_height;
    struct VpxRational time_base;
} VpxVideoInfo;

int vpxenc_init(int fps, unsigned int width, unsigned int height);
void vpx_img_read(vpx_image_t *img, unsigned char *yuvdata);
int vpx_img_plane_width(const vpx_image_t *img, int plane);
int vpx_img_plane_height(const vpx_image_t *img, int plane);

#endif

