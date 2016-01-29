#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
//#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

//#include <mach/hardware.h>
#include <mach/regs-gpio.h>
#include <mach/leds-gpio.h>

static struct resource	*switch0_irq;
static struct resource	*switch1_irq;
static struct resource	*switch2_irq;
static struct resource	*switch3_irq;

static int switch0=0;
static int switch1=0;
static int switch2=0;
static int switch3=0;

static irqreturn_t moboswitch0_irq(int irqno, void *param)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB(5), S3C2410_GPIO_OUTPUT);

	if(switch0%2)
		s3c2410_gpio_setpin(S3C2410_GPB(5), 1);
	else
		s3c2410_gpio_setpin(S3C2410_GPB(5), 0);
	switch0++;
}

static irqreturn_t moboswitch1_irq(int irqno, void *param)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB(6), S3C2410_GPIO_OUTPUT);

	if(switch1%2)
		s3c2410_gpio_setpin(S3C2410_GPB(6), 1);
	else
		s3c2410_gpio_setpin(S3C2410_GPB(6), 0);
	switch1++;
}

static irqreturn_t moboswitch2_irq(int irqno, void *param)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB(7), S3C2410_GPIO_OUTPUT);

	if(switch2%2)
		s3c2410_gpio_setpin(S3C2410_GPB(7), 1);
	else
		s3c2410_gpio_setpin(S3C2410_GPB(7), 0);
	switch2++;

}
static irqreturn_t moboswitch3_irq(int irqno, void *param)
{
	s3c2410_gpio_cfgpin(S3C2410_GPB(8), S3C2410_GPIO_OUTPUT);

	if(switch3%2)
		s3c2410_gpio_setpin(S3C2410_GPB(8), 1);
	else
		s3c2410_gpio_setpin(S3C2410_GPB(8), 0);
	switch3++;
}

static int led_switch_probe(struct platform_device *dev)
{
	struct s3c24xx_led_platdata *pdata = dev->dev.platform_data;
	struct s3c24xx_gpio_led *led;
	int ret;

	if(dev->id==0)
	{
	switch0_irq=platform_get_resource(dev, IORESOURCE_IRQ, 0);
	if (switch0_irq == NULL) {
			dev_err(dev, "no irq resource specified\n");
			ret = -ENOENT;
			goto err_map;
		}
	ret = request_irq(switch0_irq->start, moboswitch0_irq, IRQF_TRIGGER_RISING, dev->name, dev);
	}
	else if(dev->id==1)
	{
		switch1_irq=platform_get_resource(dev, IORESOURCE_IRQ, 1);
		if (switch1_irq == NULL) {
				dev_err(dev, "no irq resource specified\n");
				ret = -ENOENT;
				goto err_map;
			}
		ret = request_irq(switch1_irq->start, moboswitch1_irq, IRQF_TRIGGER_RISING, dev->name, dev);
	}
	else if(dev->id==2)
	{
		switch2_irq=platform_get_resource(dev, IORESOURCE_IRQ, 2);
		if (switch2_irq == NULL) {
				dev_err(dev, "no irq resource specified\n");
				ret = -ENOENT;
				goto err_map;
			}
		ret = request_irq(switch2_irq->start, moboswitch2_irq, IRQF_TRIGGER_RISING, dev->name, dev);
	}
	else if(dev->id==3)
	{
		switch3_irq=platform_get_resource(dev, IORESOURCE_IRQ, 3);
		if (switch3_irq == NULL) {
				dev_err(dev, "no irq resource specified\n");
				ret = -ENOENT;
				goto err_map;
			}
		ret = request_irq(switch3_irq->start, moboswitch3_irq, IRQF_TRIGGER_RISING, dev->name, dev);
	}
	else
		return 0;

	return 0;

err_map:

return ret;
}


static int led_switch_remove(struct platform_device *pdev)
{

	if(pdev->id==0&&switch0_irq)
	{
	switch0=0;
	free_irq(switch0_irq->start, dev);
	switch0_irq=NULL;
	}
	else if(pdev->id==1&&switch1_irq)
	{
	switch1=0;
	free_irq(switch1_irq->start, dev);
	switch1_irq=NULL;
	}
	else if(pdev->id==2&&switch2_irq)
	{
	switch2=0;
	free_irq(switch2_irq->start, dev);
	switch2_irq=NULL;
	}
	else if(pdev->id==3&&switch3_irq)
	{
	switch3=0;
	free_irq(switch3_irq->start, dev);
	switch3_irq=NULL;
	}
	else
		return 0;
	return 0;
}

static struct platform_driver led_switch_driver = {
	.probe		= led_switch_probe,
	.remove		= led_switch_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "s3c24xx_led",
	},
};


static int __init led_switch_init(void)
{
	return platform_driver_register(&led_switch_driver);
}

static int __exit led_switch_init(void)
{
	return platform_driver_register(&led_switch_driver);
}

module_init(led_switch_init);
module_exit(led_switch_exit);
