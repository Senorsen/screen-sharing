
#ifndef __IMGCONV_H__
#define __IMGCONV_H__

void rgb2yuv420p(unsigned int width, unsigned int height, unsigned char *rgb, unsigned char *yuv420p);
void rgba2rgb(unsigned int width, unsigned int height, unsigned char *rgba, unsigned char *rgb);

#endif

