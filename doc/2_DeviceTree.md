# Setting Up a Custom Device in the Device Tree

The Device tree is part of the system's boot structure that tells the kernel what physical devices such as CPUs, memory, etc are connected to the chip, and how to interface with them.
Conveniently, we can create an Overlay that modifies the device tree that is already part of the officially distributed linux image.
This is percisely what the "mydevice.dts" file does.

## More About the Device Tree
But before we get into that, lets discuss the device tree a bit more.
The current version of the device tree is represented in the /proc directory, and we can use the device tree compiler to generate a readable file version of it with the following command:

```
$ dtc -I fs -O dts -o dt.dts /proc/device-tree
```

Note that this command does not need elevated privledges, it is simply reading the existing device tree and generating a text file that represents it.
In this command, we are telling it that the input is a file system (-I fs), and that we want it to convert the file system into a text "-O dts" and to write the text to a file (-o dt.dts), and finally, that the input file system is /proc/device-tree.

The devide tree is quite large, however the parts that are of specific concern to us are the pinctrl nodes represented as ```pinctrl@xxxxxxxx```.
The [GPIO register map](https://wiki.odroid.com/odroid-xu4/software/gpio_register_map) provided by hardkernel shows that there are three key memory blocks containing the registers for operating the GPIO pins, specifically at:

1. 0x13400000
2. 0x14010000
3. 0x03860000

Therefore, there are three key pinctrl nodes that we need to be concerned with:

1. pinctrl@13400000
2. pinctrl@14010000
3. pinctrl@03860000

Under each of these nodes are two types of child nodes, device configuration nodes, and pin controller nodes.
The pin controller nodes are named after the GPIO ports (e.g. gpx1) and have a parameter ```gpio-controller``` as the first property.
The first thing to do to set up a custom device is to identify the GPIO pins that are needed to creat the interface, in this example, the pin gpx1-2 will be used.
Therfore the first step is to find the gpx1 pin controller node.
For gpx1, per the register map, should be located under pinctrl@13400000.
Once you have found this node, find the phandle property.
This porperty is a unique identifier for each node that we will need later to reference it.
It will be a hex number in angle brackets like

```
phandle = <0xfa>
```

Along with the phandle for the pin controller, you will also need the phandle for the pinctrl node.
Once you have obtained these numbers, you can proceed to write your own overlay that adds a custom device to the tree!

# The Device Tree Overlay

A device tree overlay allows modifications to be made to an existing device tree.
The overlay consists of fragments of a device tree, each fragment has a target and an overlay that is to be applied to the target.
To create the overlay, you start with a .dts following format:

```
/dts-v1/;
/plubin/;

/ {
    fragment@0 {
        target = <0x01>;
        __overlay__ {
            newproperty = "newvalue";
        };
    };
};
```

To use the GPIO pins, we need to create two new nodes, one under the pinctrl node for the relevant memory block that configures the pins, and another under the root node that adds our device to the tree.
The two fragments that do this are in ```mydevice.dts```.
The first fragment ads the pin configurations to the pinctrl node, therefore its target should be the phandle for the pinctrl node that we are using.
If it is not (I am not certain how consistent the phandles are between devices, but I am going to guess not very), make sure to change the value of the target to the correct phandle.
As for the overlay, lets discuss it!

### Configuration Node

First, a name is provided for the config (in the example this is "pinctrl_mydevice") as well as a reference to the new node we are adding (mydevice_conf).
This reference will not appear in the final tree, but we can use it as a placeholder for the phandle of the node, which is assigned automatically.
In the device, there are properties, the first of which is ```samsung,pins```.
This property lists the pins that you plan to use in the form "gp*-#" where the * is the port letter, and # is the pin number.
The other required value is samsun,pin-function, this is the default function of the pin (i.e. input, output, etc).
For more information on other properties you can set, see the documentation [here](https://www.kernel.org/doc/Documentation/devicetree/bindings/pinctrl/samsung-pinctrl.txt).

### Custom Device

For the second fragment, a new device is being added to the root, so ```target-path``` is used instead of ```target```.
First, the new devide is named, given a compatibility.
The standard for the compatibility is the form "manufacturer,part" as this ensures that compatibility is unique.
Each device is also given a status, and a list of pin configuration nodes, in this case, the reference from the configuration node established earlier is used.
Finally, we list the gpio pins as a triplet:

```
custpinname-gpio = <pincontroller pinnumber flags>;
```

In the provided example, the pin being considered is gpx1-2, therfore the first value in the triplet is the phandle for the gpx1 node.
The second parameter is the pin number, in the proved example, this is 2 since it is pin 2 in gpx1.
The final parameter are a seriese of flags, most importantly, the pin can be set to be active high, or active low.
For more information on these flags, see the documentation [here](https://www.kernel.org/doc/Documentation/devicetree/bindings/gpio/gpio.txt)

## Applying the Overlay

Now that the overlay has been written, it needs to be applied to the device tree.
To do this, it must first be complied into a device tree blob with the following command:

```
$ dtc -@ -I dts -O dtb -o mydevice.dtbo mydevice.dts
```

This command uses the device tree compiler, and tells it to compile the file ```mydevice.dts```  which is a text file (-I dts), it is to emit symbols (-@, this allows the overlay plugin to be loaded), the output will be a blob (-O dtb) and will be saved to ```mydevice.dtbo```.
The output file can then be copied to the overlays directory in the boot directory (in the ubuntu image, this is located at "/media/boot/overlays").
Finally, u-boot needs to be told to apply the overlay, this can be done with the ```config.ini``` file in the boot directory.
Simply add a new profile in the overlays section:

```
[overlay_mydevice]
overlays="mydevice"
```

And set this profile to be loaded by setting ```overlay_profile=mydevice```.
For more information on this process, see the odroid wiki page [here](https://wiki.odroid.com/common/application_note/software/device_tree_overlay).
Now you are ready to reboot your system.
Beware that if you did something wrong, a faulty device tree can cause the system to fail to boot, however the beauty of overlays is that it is easy to undo.
Simply mount your sdcard (or emmc) to another machine, go to the boot directory and reset the profile to ```overlay_profile=```, after this, you system should boot normally.
If youare able to get the system to boot, you can check the device tree using the method discussed at the beginning to verify your overlay was applied correctly to your device tree!

Once you have successfully added your new, custom device to the device tree, it is time to write a kernel to use it!
