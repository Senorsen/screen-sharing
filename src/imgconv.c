
#include "imgconv.h"

#define sR (unsigned char)(rgb[s+2])
#define sG (unsigned char)(rgb[s+1])
#define sB (unsigned char)(rgb[s+0])

void rgb2yuv420p(unsigned int width, unsigned int height, unsigned char *rgb, unsigned char *yuv420p) {
    unsigned int i = 0;
    unsigned int numpixels = width * height;
    unsigned int ui = numpixels;
    unsigned int vi = numpixels + numpixels / 4;
    unsigned int s = 0;

    for (int j = 0; j < height; j++) {
        for (int k = 0; k < width; k++) {
            yuv420p[i] = (unsigned char)((66*sR + 129*sG + 25*sB + 128) >> 8) + 16;

            if (0 == j % 2 && 0 == k % 2) {
                yuv420p[ui++] = (unsigned char)((-38*sR - 74*sG + 112*sB + 128) >> 8) + 128;
                yuv420p[vi++] = (unsigned char)((112*sR - 94*sG - 18*sB + 128) >> 8) + 128;
            }
            i++;
        }
    }
}

