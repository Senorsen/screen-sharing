
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>

#include <X11/Xlib.h>

void get_pointer(int *pointer_x, int *pointer_y) {
    int number_of_screens, i, result;
    Window *root_windows;
    Window window_returned;
    int root_x, root_y;
    int win_x, win_y;
    unsigned int mask_return;

    Display *display = XOpenDisplay(NULL);
    assert(display);
    number_of_screens = XScreenCount(display);
    fprintf(stderr, "There are %d screens available in this X session\n", number_of_screens);
    root_windows = malloc(sizeof(Window) * number_of_screens);
    for (i = 0; i < number_of_screens; i++) {
        root_windows[i] = XRootWindow(display, i);
    }
    for (i = 0; i < number_of_screens; i++) {
        result = XQueryPointer(display, root_windows[i], &window_returned,
                &window_returned, &root_x, &root_y, &win_x, &win_y,
                &mask_return);
        if (result == True) {
            break;
        }
    }
    if (result != True) {
        fprintf(stderr, "No mouse found.\n");
        return -1;
    }
    *pointer_x = root_x;
    *pointer_y = root_y;

    free(root_windows);
    XCloseDisplay(display);
}

