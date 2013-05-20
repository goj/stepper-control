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
    fprintf(stderr, "  %s turn X Y ... turn the camera by (X, Y) and hold.\n", name);
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

void turn_camera(usb_dev_handle *handle, int X, int Y) {
    int x_idx, y_idx;
    get_xy_indices(handle, &x_idx, &y_idx);

    // make x and y non-negative and always grow (X,Y > 0),
    // reflect true direction in *_dir variables.
    int x = 0, y = 0;
    int x_dir = INC, y_dir = INC;
    if (X < 0) {
        x_dir = DEC;
        X = -X;
    }
    if (Y < 0) {
        y_dir = DEC;
        Y = -Y;
    }

    // Now (x,y) grows from (0,0) towards (X,Y).
    // If (y/x > Y/X), then we are over the intended
    // slope and we should increase x in the next step.
    //
    // Note that y/x > Y/X <=> y/x - Y/X > 0
    // and sign(y/x - Y/X) == sign(y*X - x*Y)
    // Let's call (y*X - x*Y) slope_err and update
    // it in each step.
    int slope_err = 0;

    // As we want the angular speed of each stepper to be
    // constant we choose to sleep on the x change if X>Y
    // and on the y change otherwise. This way steps in
    // the "less popular" direction occur "in parallel"
    // with the steps in "more popular" one.
    // We choose the sleep time so that we have a constant
    // speed in taxi metric no matter the direction.
    int x_sleep, y_sleep;
    if (X > Y) {
        x_sleep = USLEEP_TIME * (X+Y) / X;
        y_sleep = 0;
    } else {
        y_sleep = USLEEP_TIME * (X+Y) / Y;
        x_sleep = 0;
    }
    for (;;) {
        if (x < X && slope_err >= 0) {
            ++x;
            slope_err -= Y;
            x_idx = idx_add(x_idx, x_dir);
            show_position('X', x_idx);
            stepper_set_x(handle, steps[x_idx]);
            usleep(x_sleep);
        } else if (y < Y && slope_err < 0) {
            ++y;
            slope_err += X;
            y_idx = idx_add(y_idx, y_dir);
            show_position('Y', y_idx);
            stepper_set_y(handle, steps[y_idx]);
            usleep(y_sleep);
        } else {
            break;
        }
    }
    stepper_rest(handle);
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
