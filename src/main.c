
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/time.h>
#include <X11/Xlib.h>
#include <jpeglib.h>
#include "pointer.h"

XImage *lastimg;
int newest;
int update_rect[1024][2];

int generate_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

int output_jpeg(XImage *img, char *filename, int quality) {
    FILE *fp = fopen(filename, "wb");
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = img->width;
    cinfo.image_height = img->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, 1);
    jpeg_start_compress(&cinfo, 1);

    JSAMPROW row_pointer[1];    // Pointer to a single scan line
    unsigned char* pBuf = (unsigned char*)malloc(3*img->width);
    row_pointer[0] = pBuf;

    while(cinfo.next_scanline < cinfo.image_height) {
        int j = 0;
        for (int i = 0; i < img->width; i++) {
            *(pBuf+j) = *(img->data+cinfo.next_scanline*img->bytes_per_line+i*4+2);
            *(pBuf+j+1) = *(img->data+cinfo.next_scanline*img->bytes_per_line+i*4+1);
            *(pBuf+j+2) = *(img->data+cinfo.next_scanline*img->bytes_per_line+i*4);
            j += 3;
        }
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    free(pBuf);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);
    return 0;
}
int compare_image(XImage *new_image, XImage *old_image, int per_width, int per_height, int timestamp) {
    // a native way to calculate delta data
    int wblocks = new_image->width / per_width,
        hblocks = new_image->height / per_height,
        updates = 0;

    for (int i = 0; i < new_image->width; i += per_width) {
        int block_w = (i + per_width >= new_image->width) ? (new_image->width - i - 1) : per_width;
        for (int j = 0; j < new_image->height; j += per_height) {
            int block_h = (j + per_height >= new_image->height) ? (new_image->height - j - 1) : per_height;
            int flag = 0;
            for (int x = i; x < i + block_w; x++) {
                for (int y = j; y < j + block_h; y++) {
                    // TODO: not just compare RED but all the colors?
                    if (*(new_image->data + y * new_image->width * 4 + x * 4) != *(old_image->data + y * old_image->width * 4 + x * 4)) {
                        flag = 1;
                        break;
                    }
                }
                if (flag) break;
            }
            if (!flag) continue;

            update_rect[updates][0] = i;
            update_rect[updates][1] = j;
            XImage *img = (XImage *) calloc(1, sizeof(XImage));
            img->width = block_w;
            img->height = block_h;
            img->bytes_per_line = block_w * 4;
            img->data = (char *) calloc(1, sizeof(char) * 4 * block_w * block_h);
            for (int x = i; x < i + block_w; x++) {
                for (int y = j; y < j + block_h; y++) {
                    // maybe I can be more clever...
                    *(img->data+(y-j)*block_w*4+(x-i)*4) = *(new_image->data+y*(new_image->width)*4+x*4);
                    *(img->data+(y-j)*block_w*4+(x-i)*4+1) = *(new_image->data+y*(new_image->width)*4+x*4+1);
                    *(img->data+(y-j)*block_w*4+(x-i)*4+2) = *(new_image->data+y*(new_image->width)*4+x*4+2);
                }
            }
            char filename[255];
            sprintf(filename, "tmp/%d_%d.jpg", timestamp, updates);
            output_jpeg(img, filename, 40);
            free(img);
            updates++;
        }
    }
    return updates;
}
            
int capture_desktop(int timestamp, int need_update) {
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
    screen_width = DisplayWidth(disp,0);
    screen_height = DisplayHeight(disp,0);
    img = XGetImage(disp, desktop, 0, 0, screen_width, screen_height, AllPlanes, ZPixmap);
    // Processing...
    int updates = 0;
    if (lastimg != NULL) {
        updates = compare_image(img, lastimg, 200, 200, timestamp);
    }
    char filename[255];
    if (need_update) {
        sprintf(filename, "tmp/full_%d.jpg", timestamp);
        output_jpeg(img, filename, 30);
        newest = timestamp;
    }

    int pointer_x, pointer_y;
    get_pointer(&pointer_x, &pointer_y);

    char json[10240], json_updates[10240], tmps[10240];
    sprintf(json, "{\"newest\":%d,\"now\":%d,\"width\":%d,\"height\":%d,\"updates\":%d,\"pointer\":[%d,%d],\"diff\":[", newest, timestamp, screen_width, screen_height, updates, pointer_x, pointer_y);
    json_updates[0] = 0;
    for (int i = 0; i < updates; i++) {
        if (i > 0) strcat(json_updates, ",");
        sprintf(tmps, "[%d,%d]", update_rect[i][0], update_rect[i][1]);
        strcat(json_updates, tmps);
    }
    strcat(json, json_updates);
    sprintf(json, "%s]}", json);

    sprintf(filename, "tmp/%d.json", timestamp);
    FILE *fp = fopen(filename, "wb");
    fwrite(json, strlen(json), 1, fp);
    fclose(fp);
    sprintf(filename, "%d.json", timestamp);
    remove("tmp/current.json");
    symlink(filename, "tmp/current.json");
    printf("%9.d done with %d updates.\n", timestamp, updates);

    if (lastimg) XDestroyImage(lastimg);
    lastimg = img;
    XCloseDisplay(disp);
    return 0;
}

int main(int argc, char *argv[]) {
    lastimg = NULL;
    system("rm -rf tmp; mkdir -p tmp");
    memset(&update_rect, 0, sizeof(update_rect));
    int time = 0, lasttime, ts = 0, fullscreen = 0, fn = 0;
    
    while (1) {
        ts++;
        time = generate_timestamp();
        if (time - lasttime > 1) {
            // Save fullscreen every 5s.
            lasttime = time;
            fn++;
            fullscreen = fn;
        } else {
            fullscreen = 0;
        }
        capture_desktop(ts, fullscreen);
        usleep(200000);
    }
    return 0;
}

