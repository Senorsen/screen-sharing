
#include "imgconv.h"

#define sR (unsigned char)(rgb[s])
#define sG (unsigned char)(rgb[s + 1])
#define sB (unsigned char)(rgb[s + 2])

void rgb2yuv420p(unsigned int width, unsigned int height, unsigned char *rgb, unsigned char *yuv420p) {
    unsigned int i = 0;
    unsigned int numpixels = width * height;
    unsigned int ui = numpixels;
    unsigned int vi = numpixels + numpixels / 4;
    unsigned int s = 0;

    for (int j = 0; j < height; j++) {
        for (int k = 0; k < width; k++) {
            s = j * width * 3 + k * 3;
            yuv420p[i] = (unsigned char)((66*sR + 129*sG + 25*sB + 128) >> 8) + 16;

            if (0 == j % 2 && 0 == k % 2) {
                yuv420p[ui++] = (unsigned char)((-38*sR - 74*sG + 112*sB + 128) >> 8) + 128;
                yuv420p[vi++] = (unsigned char)((112*sR - 94*sG - 18*sB + 128) >> 8) + 128;
            }
            i++;
        }
    }
}

void rgba2rgb(unsigned int width, unsigned int height, unsigned char *rgba, unsigned char *rgb) {
    unsigned int i = 0;
    unsigned int j = 0;
    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            rgb[j * width * 3 + i * 3] = rgba[j * width * 4 + i * 4 + 2];
            rgb[j * width * 3 + i * 3 + 1] = rgba[j * width * 4 + i * 4 + 1];
            rgb[j * width * 3 + i * 3 + 2] = rgba[j * width * 4 + i * 4];
        }
    }
}

