#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stepper.h"
#include "../atmega/requests.h"

#define NUM_ROTATIONS 500
#define SLEEP_TIME 1000

int steps[] = {1, 5, 4, 6, 2, 10, 8, 9};
#define STEPS_SIZE (sizeof(steps)/sizeof(steps[0]))

int main(int argc, char **argv)
{
    int turn, x;
    usb_dev_handle *handle;

    usb_init();
    handle = stepper_connect();

    if (!handle) {
        printf("DISCONNECTED\n");
        exit(1);
    }

    for (turn = 0; turn < NUM_ROTATIONS; turn++) {
        for (x = 0; x < STEPS_SIZE; x++) {
            stepper_set_x(handle, steps[x]);
            stepper_set_y(handle, steps[x]);
            usleep(SLEEP_TIME);
        }
        usleep(SLEEP_TIME);
    }
    stepper_set_x(handle, 0);
    stepper_set_y(handle, 0);
    stepper_disconnect(handle);
    return 0;
}
