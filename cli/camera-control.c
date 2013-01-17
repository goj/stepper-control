/* Hugely refactored code from hid-custom-rq example by Christian Starkjohann */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stepper.h"

static void usage(char *name)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s status ... get the device status\n", name);
    fprintf(stderr, "  %s set_x X .. set the X stepper\n", name);
    fprintf(stderr, "  %s set_y X .. set the Y stepper\n", name);
    fprintf(stderr, "  %s debug .... print debug info\n", name);
}

void print_status(int x, int y) {
    printf("engine status: X@%d, Y@%d\n", x, y);
}

int main(int argc, char **argv)
{
    usb_dev_handle *handle;

    if (argc < 2) {
        usage(argv[0]);
        return 0;
    }

    usb_init();
    handle = stepper_connect();
    if (!handle) {
        printf("DISCONNECTED\n");
        exit(1);
    }

    if (strcasecmp(argv[1], "status") == 0) {
        int x, y;
        stepper_status(handle, &x, &y);
        print_status(x, y);
    } else if (strcasecmp(argv[1], "set_x") == 0 && argc == 3) {
        stepper_set_x(handle, atoi(argv[2]));
    } else if (strcasecmp(argv[1], "set_y") == 0 && argc == 3) {
        stepper_set_y(handle, atoi(argv[2]));
    } else if(strcasecmp(argv[1], "debug") == 0) {
        const int buf_size = 256;
        char buf[buf_size];
        stepper_debug(handle, buf, buf_size);
        fputs(buf, stdout);
    } else{
        usage(argv[0]);
        exit(1);
    }
    stepper_disconnect(handle);
    return 0;
}
