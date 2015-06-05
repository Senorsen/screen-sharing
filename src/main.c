
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/time.h>
#include <X11/Xlib.h>
#include <jpeglib.h>
#include "config.h"
#include "pointer.h"
#include "imgconv.h"

XImage *lastimg;
int newest;
int update_rect[1024][2];

int generate_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

int capture_desktop() {
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
    screen_width = SCREEN_WIDTH;
    screen_height = SCREEN_HEIGHT;
    img = XGetImage(disp, desktop, SCREEN_LEFT, SCREEN_TOP, screen_width, screen_height, AllPlanes, ZPixmap);

    XDestroyImage(img);
    XCloseDisplay(disp);
    return 0;
}

int main(int argc, char *argv[]) {
    capture_desktop();
    return 0;
}

