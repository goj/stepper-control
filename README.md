Stepper control
===============

Control two stepper engines via the USB port.

Background
----------

Some Nikon cameras can be attached to your computer via the USB port and controlled this way.  It has some interesting uses, but hey, we can do better. Thanks to USB/IP project we can attach the camera to the OpenWRT router and do the outdoor photos from the comfort of our desk.  Nice thing for shooting birds at the feeder in winter. But what if you have two feeders? You need to move the camera. Enter this project.

License
-------

It's all covered by [GPLv2](http://www.gnu.org/licenses/gpl-2.0.html) (due to the V-USB dependency).

Credits
-------

This code is largely based on hid-custom-rq example project from V-USB.
The USB part of the circuit is based on [USB-Servo by Ronald Schaten](http://www.schatenseite.de/index.php?id=219&L=2).
