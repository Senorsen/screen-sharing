
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>
#include <vpx/vpx_codec.h>
#include <vpx/vpx_image.h>
#include <vpx/vpx_integer.h>

#include "config.h"
#include "vpxenc.h"

int vpxenc_init(int fps, unsigned int width, unsigned int height) {
    vpx_codec_ctx_t codec;
    vpx_codec_enc_cfg_t cfg;
    int frame_count = 0;
    vpx_image_t raw;
    vpx_codec_err_t res;
    const int bitrate = 200;
    int keyframe_interval = 400;
    const VpxInterface encoder = {"vp8", VP8_FOURCC, &vpx_codec_vp8_cx};
    VpxVideoInfo info = {0};
    info.codec_fourcc = encoder.fourcc;
    info.frame_width = width;
    info.frame_height = height;
    info.time_base.numerator = 1;
    info.time_base.denominator = fps;
    if (!vpx_img_alloc(&raw, VPX_IMG_FMT_I420, info.frame_width, info.frame_height, 1)) {
        printf("[ERROR] Failed to allocate image.\n");
        return 1;
    }
    res = vpx_codec_enc_config_default(encoder.codec_interface(), &cfg, 0);
    if (res) {
        printf("[ERROR] Failed to get default codec config.\n");
        return 2;
    }
    cfg.g_w = info.frame_width;
    cfg.g_h = info.frame_height;
    cfg.g_timebase.num = info.time_base.numerator;
    cfg.g_timebase.den = info.time_base.denominator;
    cfg.rc_target_bitrate = bitrate;
    cfg.g_error_resilient = 0;
    
    if (vpx_codec_enc_init(&codec, encoder.codec_interface(), &cfg, 0)) {
        printf("[ERROR] Failed to initialize encoder.\n");
        return 3;
    }

    return 0;
}

void vpx_img_read(vpx_image_t *img, unsigned char *yuvdata) {
    // from WebM project, libvpx git repo, tools_common.c
    // with slight changes.
    int plane;

    for (plane = 0; plane < 3; ++plane) {
        unsigned char *buf = img->planes[plane];
        const int stride = img->stride[plane];
        const int w = vpx_img_plane_width(img, plane) *
            ((img->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
        const int h = vpx_img_plane_height(img, plane);
        int y;

        for (y = 0; y < h; ++y) {
            memcpy(buf, yuvdata, w);
            buf += stride;
        }
    }
}

int vpx_img_plane_width(const vpx_image_t *img, int plane) {
    if (plane > 0 && img->x_chroma_shift > 0)
        return (img->d_w + 1) >> img->x_chroma_shift;
    else
        return img->d_w;
}

int vpx_img_plane_height(const vpx_image_t *img, int plane) {
    if (plane > 0 &&  img->y_chroma_shift > 0)
        return (img->d_h + 1) >> img->y_chroma_shift;
    else
        return img->d_h;
}

