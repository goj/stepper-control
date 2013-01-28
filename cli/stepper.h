#ifndef LIBSTEPPER_H_DHLAZNOI
#define LIBSTEPPER_H_DHLAZNOI

#include <usb.h>

int stepper_init_usb();
usb_dev_handle* stepper_connect();
int stepper_status(usb_dev_handle*, int*, int*, int*);
int stepper_debug(usb_dev_handle*, char*, int);
int stepper_set_x(usb_dev_handle*, int);
int stepper_set_y(usb_dev_handle*, int);
int stepper_disconnect(usb_dev_handle*);

#endif /* end of include guard: LIBSTEPPER_H_DHLAZNOI */

