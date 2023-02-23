# GPIO on Odroid

General Purpose Input Output (GPIO) pins are what allow Single Board Computers (SBCs) like Odroid, Raspberry Pi, and others to operate a myriad of additional custom hardware, and being able interface with these pins is crutial to being able to write dirvers that allow the devices to correctly connect to and interact with a host SBC.
While extremely useful, there has been a relative lack of information of how to go about setting up such an interface on specific boards.
Some more popular boards like the Raspberry Pi are quite prolific and have examples, however these often fail to translate to boards such as odroids, which feature a different SoC, and since GPIO is a low level interface, the APIs are different.
This repo seeks to provide an easy to follow set of instructions for odroid boards specifically.

## The Problem

There does exist an "easier" way to use GPIO pins than the method present here, however this method is considered depreciated, and who knows how long support for it will remain in the kernel.
This legacy method involves finding a overall GPIO number (for example, on an odroid XU4, gpx1.2 is GPIO 18) and referencing the pin via this number.
In the new method, the setup is more complicated, but more robust, and goes as follows:

1. A device is created within the Device Tree that maps all the required gpio pins to specific names
2. A platform device is created in a kernel module that pulls a gpio descriptor
3. ....
4. Profit.

There are two key files included in this repo, in addition the documentation, .gitignore, and the makefile:

1. mydevice.dts provides the device tree overlay for adding a custom device to the device tree.
2. test_driver.c contains the code for a kernel module that drives this device.

Both of these files have a documentation page that goes over them in detail.
It is also important to note, the setup used for these examples is specifically an Odroid-XU4 with the ubuntu 22.04 provided by HardKernel with the 5.4.228-412 Kernel.
Make sure to read the documentation, especially regarding the device tree to make sure that the code matches your setup properly.

Happy Hacking!
