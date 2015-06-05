
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include <vpx/vpx_encoder.h>

#include "config.h"

void vpxenc_init() {
    vpx_codec_ctx_t codec;
    vpx_codec_enc_cfg_t cfg;
    int frame_count = 0;
    vpx_image_t raw;
    vpx_codec_err_t res;
}

