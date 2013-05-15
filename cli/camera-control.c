#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stepper.h"
#include "../atmega/requests.h"

#define USLEEP_TIME 10000

// This program is sorta-kinda Simonyi - Hungarian.
// The convention through this code is that _msk suffix for variable
// stands for bit masks (values of the steps array) and _idx suffix
// stands for the indices. Suffix-free is some abstract position
// that (unlike the indices) doesn't wrap.

// half-step method
int steps[] = {1, 5, 4, 6, 2, 10, 8, 9};
#define STEPS_SIZE (sizeof(steps)/sizeof(steps[0]))
#define MAX_STEP (STEPS_SIZE - 1)

#define INC 1
#define DEC -1

void usage(char *name) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s help ......... print this message and exit.\n", name);
    fprintf(stderr, "  %s off .......... turn off the stepper engines.\n", name);
    fprintf(stderr, "  %s turn dx dy ... turn the camera by (dx, dy) and hold.\n", name);
}

#ifdef DEBUG
void show_position(char axis, int pos) {
    int i;
    for (i = 0; i < STEPS_SIZE; i++)
        putchar(i == pos ? axis : '.');
    putchar('\n');
}
#else
#define show_position(axis, pos)
#endif

int idx_from_msk(int msk) {
    int idx;
    for (idx = 0; idx < STEPS_SIZE; idx++) {
        if (steps[idx] == msk)
            return idx;
    }
    return 0; // garbage in - (safe) garbage out
}

void get_xy_indices(usb_dev_handle *handle, int *x_idx, int *y_idx) {
    int x_msk, y_msk;
    stepper_status(handle, NULL, &x_msk, &y_msk);
    assert(x_idx && y_idx);
    *x_idx = idx_from_msk(x_msk);
    *y_idx = idx_from_msk(y_msk);
}

int idx_add(int idx, int dir) {
    if (dir == DEC && idx == 0)
        return MAX_STEP;
    if (dir == INC && idx == MAX_STEP)
        return 0;
    return idx + dir;
}

void turn_camera(usb_dev_handle *handle, int dx, int dy) {
    int x_idx, y_idx;
    get_xy_indices(handle, &x_idx, &y_idx);

    // make x and y non-negative and always grow (dx,dy > 0),
    // reflect true direction in *_dir variables.
    int x = 0, y = 0;
    int x_dir = INC, y_dir = INC;
    if (dx < 0) {
        x_dir = DEC;
        dx = -dx;
    }
    if (dy < 0) {
        y_dir = DEC;
        dy = -dy;
    }

    // Now (x,y) grows from (0,0) towards (dx,dy).
    // If (y/x > dy/dx), then we are over the intended
    // slope and we should increase x in the next step.
    // sign(y/x - dy/dx) == sign(y*dx - x*dy)
    // Let's call (y*dx - x*dy) slope_error and update
    // it in each step.
    int slope_err = 0;
    for (;;) {
        if (x < dx && slope_err >= 0) {
            ++x;
            slope_err -= dy;
            x_idx = idx_add(x_idx, x_dir);
            show_position('X', x_idx);
            stepper_set_x(handle, steps[x_idx]);
        } else if (y < dy && slope_err <= 0) {
            ++y;
            slope_err += dx;
            y_idx = idx_add(y_idx, y_dir);
            show_position('Y', y_idx);
            stepper_set_y(handle, steps[y_idx]);
        } else {
            break;
        }
        usleep(USLEEP_TIME);
    }
}

int main(int argc, char **argv)
{
    usb_dev_handle *handle;

    if (argc <= 1 || !strcmp(argv[1], "help")) {
        usage(argv[0]);
        exit(0);
    }

    usb_init();
    handle = stepper_connect();

    if (!handle) {
        printf("DISCONNECTED\n");
        exit(1);
    }

    if (argc == 2 && !strcmp(argv[1], "off")) {
        stepper_set_x(handle, 0);
        stepper_set_y(handle, 0);
    } else if (argc == 4 && !strcmp(argv[1], "turn")) {
        turn_camera(handle, atoi(argv[2]), atoi(argv[3]));
    } else {
        usage(argv[0]);
    }

    stepper_disconnect(handle);
    return 0;
}
