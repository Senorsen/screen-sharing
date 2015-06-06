/**
 * Some code from libvpx project.
 *
 */

#ifndef __VPXIF_H__
#define __VPXIF_H__

#include <vpx/vpx_codec.h>
#include <vpx/vpx_image.h>
#include <vpx/vpx_integer.h>

#define VP8_FOURCC (0x30385056)
#define VP9_FOURCC (0x30395056)
#define VP8_FOURCC_MASK (0x00385056)
#define VP9_FOURCC_MASK (0x00395056)

void mem_put_le32(unsigned char *mem, int val);

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

int vpxif_init(int cfg_fps, unsigned int width, unsigned int height);
void vpx_img_read(unsigned char *yuvdata);
int vpx_img_plane_width(const vpx_image_t *img, int plane);
int vpx_img_plane_height(const vpx_image_t *img, int plane);

void vpx_img_update(unsigned char *yuvdata, unsigned char **vpxdata, void (*callback)(unsigned int, unsigned char *));
void vpxif_finish_up(unsigned char **vpxdata, void (*callback)(unsigned int, unsigned char *));


#endif

