# Device Driver as a Kernel Module

Once the Device tree is set up properly, it is time to get to the fun part and write the kernel driver!
Information about the gpio consumer interface can be found [here](https://www.infradead.org/~mchehab/rst_conversion/driver-api/gpio/consumer.html), but the gist is pretty simple.
We create a platform device, and use it to get control of the gpio pins we set up previously in the device tree via the names we used in the custom device.
The key function here is the ```gpiod_get``` function, first we pass this function the device from out platform device, then the name of the pin we want which is the part before the "-gpio" or "-gpios" in the custom device set up in the device tree, and finally we can set the pin as an input or output immediately as we acquire it.
Once we have the pin descriptor, it can be used with the ```gpio_set_value``` and ```gpio_get_value``` functions or other interfaces as needed.

The other key node in the kernel module is setting the Module device table to include devices with the correct "compatability."
This is where the compatible property we set for the device comes into play, and why it needs to be unique.
