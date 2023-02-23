/*
 * test_driver.c
 *
 * Author: LikeSmith
 * Date: FEBRUARY 2023
 * 
 * Basic test driver for linux kernel hacking :P
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LikeSmith");
MODULE_DESCRIPTION("A simple test driver");
MODULE_VERSION("1.0:0");

// gpio descriptors
static struct gpio_desc *my_gpio;

// device ids
static const struct of_device_id gpio_ids[] = {
    {.compatible = "mydevice", },
    { }
};

MODULE_DEVICE_TABLE(of, gpio_ids);

static int mydev_probe(struct platform_device *pdev)
{
    pr_info("Starting test driver....\n");

    // Acquire GPIO
    if (IS_ERR(my_gpio = gpiod_get(&(pdev->dev), "mydevice", GPIOD_OUT_HIGH)))
    {
        pr_err("[ERROR] Unable to get gpio descriptor (code %d)\n", (int)my_gpio);
        return -1;
    }

    pr_info("[INFO] mydevice has aquired gpio!\n");
    return 0;
}

static int mydev_remove(struct platform_device *pdev)
{
    gpiod_set_value(my_gpio, 0);
    gpiod_put(my_gpio);
    pr_info("[INFO] mydevice has released gpio!\n");
    return 0;
}

// platform driver
static struct platform_driver mydriver = {
    .probe = mydev_probe,
    .remove = mydev_remove,
    .driver = {
        .name = "mydriver gpio sample",
        .of_match_table = of_match_ptr(gpio_ids),
        .owner = THIS_MODULE,
    },
};

module_platform_driver(mydriver);
