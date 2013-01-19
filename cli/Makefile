# Initial version:
#     Name: Makefile
#     Project: hid-custom-rq example
#     Author: Christian Starkjohann
#     Creation Date: 2008-04-06
#     Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)

USBFLAGS = `libusb-config --cflags`
USBLIBS = `libusb-config --libs`

STD_OBJECTS = opendevice.o stepper.o
MOCK_OBJECTS = opendevice.o stepper-mock.o

CC		= gcc
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall -DDEBUG
LIBS	= $(USBLIBS)

all: camera-control mock-camera-control fast-rotate

%.o: %.c
	$(CC) $(CFLAGS) -c $<

camera-control: camera-control.o $(STD_OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

mock-camera-control: camera-control.o $(MOCK_OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

fast-rotate: fast-rotate.o $(STD_OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f *.o camera-control mock-camera-control fast-rotate
