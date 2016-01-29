#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
//#include <linux/leds.h>
#include <linux/gpio.h>

//#include <mach/hardware.h>
#include <mach/regs-gpio.h>
#include <mach/leds-gpio.h>


static struct class *leds_class;

static int led_base_probe(struct platform_device *dev)
{
    struct s3c24xx_led_platdata *pdata = dev->dev.platform_data;

	if (pdata->flags & S3C24XX_LEDF_TRISTATE) {
		s3c2410_gpio_setpin(pdata->gpio, 0);
		s3c2410_gpio_cfgpin(pdata->gpio, S3C2410_GPIO_INPUT);
	} else {
		s3c2410_gpio_pullup(pdata->gpio, 0);
		s3c2410_gpio_setpin(pdata->gpio, 0);
		s3c2410_gpio_cfgpin(pdata->gpio, S3C2410_GPIO_OUTPUT);
	}

	return 0;

}


static int led_base_remove(struct platform_device *pdev)
{

	return 0;
}

static struct platform_driver led_base_driver = {
	.probe		= led_base_probe,
	.remove		= led_base_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "s3c24xx_led",
	},
};


static int __init led_base_init(void)
{
	return platform_driver_register(&led_base_driver);
}

static int __exit led_base_init(void)
{
	return platform_driver_register(&led_base_driver);
}

module_init(led_base_init);
module_exit(led_base_exit);
