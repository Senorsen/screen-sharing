
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <jpeglib.h>
#include "config.h"
#include "pointer.h"
#include "imgconv.h"

void output_jpeg(unsigned int width, unsigned int height, unsigned char *rgbdata, char *filename, int quality) {
    FILE *fp = fopen(filename, "wb");
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, 1);
    jpeg_start_compress(&cinfo, 1);

    JSAMPROW row_pointer[1];
    unsigned char *pBuf = (unsigned char *) malloc(3 * width);
    row_pointer[0] = pBuf;
    while (cinfo.next_scanline < cinfo.image_height) {
        memcpy(pBuf, rgbdata + 3 * cinfo.next_scanline * width, 3 * width);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    free(pBuf);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);
}


int capture_desktop(unsigned int *width, unsigned int *height, unsigned char *yuvdata) {
    Window desktop;
    Display *disp;
    XImage *img;

    int screen_width, screen_height;

    disp = XOpenDisplay(NULL);      // Connect to a local display

    if (disp == NULL) {
        return -1;
    }
    desktop = RootWindow(disp, 0);  // Point to the root window
    if (desktop == 0) {
        return -1;
    }
    screen_width = DisplayWidth(disp, 0);
    screen_height = DisplayHeight(disp, 0);
    screen_width = SCREEN_WIDTH;
    screen_height = SCREEN_HEIGHT;
    img = XGetImage(disp, desktop, SCREEN_LEFT, SCREEN_TOP, screen_width, screen_height, AllPlanes, ZPixmap);

    unsigned int size = sizeof(unsigned char) * img->width * img->height * 1.5 * 3;
    unsigned char *rgbdata = (unsigned char *) malloc(size);
    yuvdata = (unsigned char *) malloc(size);

    rgba2rgb(img->width, img->height, img->data, rgbdata);
    rgb2yuv420p(img->width, img->height, rgbdata, yuvdata);
    free(rgbdata);

    XDestroyImage(img);
    XCloseDisplay(disp);
    return 0;
}

int main(int argc, char *argv[]) {
    unsigned int width, height;
    unsigned char *yuvdata;
    capture_desktop(&width, &height, yuvdata);
    return 0;
}

