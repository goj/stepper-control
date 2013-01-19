/* Hugely refactored code from hid-custom-rq example by Christian Starkjohann */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stepper.h"
#include "../atmega/requests.h"

static void usage(char *name)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s status ..... get the device status\n", name);
    fprintf(stderr, "  %s set_x X .... set the X stepper\n", name);
    fprintf(stderr, "  %s set_y X .... set the Y stepper\n", name);
    fprintf(stderr, "  %s debug ...... print debug info\n", name);
    fprintf(stderr, "  %s get_temp ... get the temperature\n", name);
}

void print_status(int request_no, int x, int y, int temp) {
    printf("Request %d\n", request_no);
    printf("Engine status: X=%d, Y=%d\n", x, y);
    printf("Temperature: %d℃\n", temp);
}

int main(int argc, char **argv)
{
    usb_dev_handle *handle;
    int err = 0;

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
        int request_no, x, y, temp;
        err = stepper_status(handle, &request_no, &x, &y, &temp);
        if (!err) {
            print_status(request_no, x, y, temp);
        }
    } else if (strcasecmp(argv[1], "set_x") == 0 && argc == 3) {
        err = stepper_set_x(handle, atoi(argv[2]));
    } else if (strcasecmp(argv[1], "set_y") == 0 && argc == 3) {
        err = stepper_set_y(handle, atoi(argv[2]));
    } else if(strcasecmp(argv[1], "debug") == 0) {
        const int buf_size = 256;
        char buf[buf_size];
        err = stepper_debug(handle, buf, buf_size);
        if (!err) {
            fputs(buf, stdout);
        }
    } else {
        usage(argv[0]);
    }
    if (err) {
        stepper_disconnect(handle);
        fprintf(stderr, "Unexpected error (code %d)\n", err);
        return err;
    }
    stepper_disconnect(handle);
    return 0;
}
