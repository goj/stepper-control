#include <stdio.h>
#include <string.h>
#include "stepper.h"

usb_dev_handle* stepper_connect() {
    return (usb_dev_handle*) 0xDE71CE;
}

static int request_no = 0;

int stepper_status(usb_dev_handle* hndl, int *no, int *x, int *y) {
    if (no) *no = ++request_no;
    if (x) *x = 42;
    if (y) *y = 7;
    return 0;
}

int stepper_debug(usb_dev_handle* hndl, char* buf, int buf_size) {
    strcpy(buf, "fake debug info");
    return 0;
}

int stepper_set_x(usb_dev_handle* hndl, int x) {
    printf("setting fake X: %d\n", x);
    return 0;
}

int stepper_set_y(usb_dev_handle* hndl, int y) {
    printf("setting fake Y: %d\n", y);
    return 0;
}

int stepper_disconnect(usb_dev_handle* hndl) {
    return 0;
}
