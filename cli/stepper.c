/* Contains code from hid-custom-rq example by Christian Starkjohann */

#include <usb.h>
#include "stepper.h"
#include "opendevice.h"

#include "../atmega/requests.h"
#include "../atmega/usbconfig.h"

#define STATUS_MSG_SIZE 4

#define TIMEOUT 5000
#ifdef DEBUG
#define SHOW_WARNINGS 1
#else
#define SHOW_WARNINGS 0
#endif

static int out_request(usb_dev_handle *dev, int request, int value, char *bytes, int size);
static int in_request(usb_dev_handle *dev, int request, int value, char *bytes, int size);

usb_dev_handle* stepper_connect() {
    usb_dev_handle      *handle = NULL;
    const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID},
                        rawPid[2] = {USB_CFG_DEVICE_ID};
    char                vendor[] = {USB_CFG_VENDOR_NAME, 0},
                        product[] = {USB_CFG_DEVICE_NAME, 0};
    int                 len, vid, pid;

    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    /* The following function is in opendevice.c: */
    if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0) {
        return NULL;
    }
    /* Since we use only control endpoint 0, we don't need to choose a
     * configuration and interface. Reading device descriptor and setting a
     * configuration and interface is done through endpoint 0 after all.
     * However, newer versions of Linux require that we claim an interface
     * even for endpoint 0. Enable the following code if your operating system
     * needs it: */
#if 1
    int retries = 1, usbConfiguration = 1, usbInterface = 0;
    if(usb_set_configuration(handle, usbConfiguration) && SHOW_WARNINGS) {
        fprintf(stderr, "Warning: could not set configuration: %s\n", usb_strerror());
    }
    /* now try to claim the interface and detach the kernel HID driver on
     * Linux and other operating systems which support the call. */
    while((len = usb_claim_interface(handle, usbInterface)) != 0 && retries-- > 0){
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
        if(usb_detach_kernel_driver_np(handle, 0) < 0 && SHOW_WARNINGS) {
            fprintf(stderr, "Warning: could not detach kernel driver: %s\n", usb_strerror());
        }
#endif
    }
#endif
    return handle;
}

int stepper_status(usb_dev_handle* handle, int *no, int *x, int *y) {
    char bytes[STATUS_MSG_SIZE];
    int cnt = in_request(handle, REQ_GET_STATUS, 0, bytes, STATUS_MSG_SIZE);
    if (cnt != STATUS_MSG_SIZE)
        return cnt;
    if (no) *no = (bytes[0] << 8) | bytes[1];
    if (x) *x = bytes[2];
    if (y) *y = bytes[3];
    return 0;
}

int stepper_debug(usb_dev_handle* handle, char* buffer, int buf_size) {
    int cnt = in_request(handle, REQ_DEBUG, 0, buffer, buf_size);
    buffer[buf_size-1] = '\0';
    return cnt > 0 ? 0 : cnt;
}

int stepper_set_x(usb_dev_handle* handle, int x) {
    return out_request(handle, REQ_SET_X, x, "", 0);
}

int stepper_set_y(usb_dev_handle* handle, int y) {
    return out_request(handle, REQ_SET_Y, y, "", 0);
}

int stepper_disconnect(usb_dev_handle* handle) {
    return usb_close(handle);
}

static int usb_request(usb_dev_handle *dev, int request, int dir, int value, char *bytes, int size)
{
    int requesttype = USB_TYPE_VENDOR | USB_RECIP_DEVICE | dir;
    int cnt = usb_control_msg(dev, requesttype, request, value, 0, bytes, size, TIMEOUT);
    if(cnt < 0) {
        fprintf(stderr, "USB error: %s\n", usb_strerror());
    }
    return cnt;
}

static int out_request(usb_dev_handle *dev, int request, int value, char *bytes, int size) {
    return usb_request(dev, request, USB_ENDPOINT_OUT, value, bytes, size);
}

static int in_request(usb_dev_handle *dev, int request, int value, char *bytes, int size) {
    return usb_request(dev, request, USB_ENDPOINT_IN, value, bytes, size);
}
