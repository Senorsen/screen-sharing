
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
#include "vpxif.h"

vpx_image_t *img;
unsigned int frame_count;
int keyframe_interval;
int bitrate;
int fps;
vpx_codec_ctx_t codec;

int vpxif_init(int cfg_fps, unsigned int width, unsigned int height) {
    vpx_codec_enc_cfg_t cfg;
    vpx_codec_err_t res;
    bitrate = 200;
    keyframe_interval = 400;
    frame_count = 0;
    fps = cfg_fps;
    const VpxInterface encoder = {"vp8", VP8_FOURCC, &vpx_codec_vp8_cx};
    VpxVideoInfo info = {0};
    info.codec_fourcc = encoder.fourcc;
    info.frame_width = width;
    info.frame_height = height;
    info.time_base.numerator = 1;
    info.time_base.denominator = fps;
    img = (vpx_image_t *) malloc(sizeof(vpx_image_t));
    if (!vpx_img_alloc(img, VPX_IMG_FMT_I420, info.frame_width, info.frame_height, 1)) {
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
        const char *detail = vpx_codec_error_detail(&codec);
        printf("    Err: %s\n", vpx_codec_error(&codec));
        return 3;
    }

    return 0;
}

void write_frame(unsigned char *vpxdata, const vpx_codec_cx_pkt_t *pkt) {
    mem_put_le32(vpxdata + 0, (int) pkt->data.frame.sz);
    mem_put_le32(vpxdata + 4, (int) (pkt->data.frame.pts & 0xFFFFFFFF));
    mem_put_le32(vpxdata + 8, (int) (pkt->data.frame.pts >> 32));
    memcpy(vpxdata + 12, pkt->data.frame.buf, pkt->data.frame.sz);
}

void encode_frame(vpx_codec_ctx_t *codec, vpx_image_t *img, int frame_index, int flags, unsigned char **vpxdata, void (*callback)(unsigned int, unsigned char *)) {
    int got_pkts = 0;
    vpx_codec_iter_t iter = NULL;
    const vpx_codec_cx_pkt_t *pkt = NULL;
    const vpx_codec_err_t res = vpx_codec_encode(codec, img, frame_index, 1, flags, VPX_DL_GOOD_QUALITY);
    if (res != VPX_CODEC_OK) {
        printf("[ERROR] Failed to encode frame");
        return;
    }
    while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL) {
        got_pkts = 1;
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
            const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
            // from ivf_write_frame_header
            unsigned int size = pkt->data.frame.sz + 12;
            *vpxdata = (unsigned char *) malloc(size);
            write_frame(*vpxdata, pkt);
            callback(size, *vpxdata);
        }
    }
}

void vpx_img_update(unsigned char *yuvdata, unsigned char **vpxdata, void (*callback)(unsigned int, unsigned char *)) {
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
    
    int flags = 0;
    if (keyframe_interval > 0 && frame_count % keyframe_interval == 0) {
        flags |= VPX_EFLAG_FORCE_KF;
    }
    encode_frame(&codec, img, frame_count++, flags, vpxdata, callback);

}

void vpxif_finish_up(unsigned char **vpxdata, void (*callback)(unsigned int, unsigned char *)) {
    encode_frame(&codec, NULL, 01, 0, vpxdata, callback);
    vpx_img_free(img);
    vpx_codec_destroy(&codec);
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

void mem_put_le32(unsigned char *mem, int val) {
    mem[0] = (val >>  0) & 0xff;
    mem[1] = (val >>  8) & 0xff;
    mem[2] = (val >> 16) & 0xff;
    mem[3] = (val >> 24) & 0xff;
}

