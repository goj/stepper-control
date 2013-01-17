# Name: Makefile
# Project: hid-custom-rq example
# Author: Christian Starkjohann
# Creation Date: 2008-04-06
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id$


# Concigure the following definitions according to your system.
# This Makefile has been tested on Mac OS X, Linux and Windows.

# Use the following 3 lines on Unix (uncomment the framework on Mac OS X):
USBFLAGS = `libusb-config --cflags`
USBLIBS = `libusb-config --libs`

NAME = camera-control

STD_OBJECTS = opendevice.o stepper.o camera-control.o
MOCK_OBJECTS = opendevice.o stepper-mock.o camera-control.o

CC		= gcc
CFLAGS	= $(CPPFLAGS) $(USBFLAGS) -O -g -Wall
LIBS	= $(USBLIBS)

all: camera-control mock-camera-control

.c.o:
	$(CC) $(CFLAGS) -c $<

camera-control: $(STD_OBJECTS)
	$(CC) -o $@ $(STD_OBJECTS) $(LIBS)

mock-camera-control: $(MOCK_OBJECTS)
	$(CC) -o $@ $(MOCK_OBJECTS) $(LIBS)

clean:
	rm -f *.o $(PROGRAM)
