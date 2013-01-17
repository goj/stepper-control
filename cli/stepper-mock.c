#include <stdio.h>
#include <string.h>
#include "stepper.h"

void coffemaker_init_usb() {
}

usb_dev_handle* stepper_connect() {
    return (usb_dev_handle*) 0xDE71CE;
}

void stepper_status(usb_dev_handle* hndl, int *x, int *y) {
    *x = 42;
    *y = 7;
}

void stepper_debug(usb_dev_handle* hndl, char* buf, int buf_size) {
    strcpy(buf, "fake debug info");
}

void stepper_set_x(usb_dev_handle* hndl, int x) {
    printf("setting fake X: %d\n", x);
}

void stepper_set_y(usb_dev_handle* hndl, int y) {
    printf("setting fake Y: %d\n", y);
}

void stepper_disconnect(usb_dev_handle* hndl) {
}
