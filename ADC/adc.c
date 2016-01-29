#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <plat/regs-adc.h>
//#include <linux/wait.h>
//#include <linux/completion.h>
#include <linux/uaccess.h>
//#include <mach/map.h>

#define ADC_MINOR		234	/* Watchdog timer     */
#define SUCCESS          0

#define ADC_CHANNEL_AIN2  0x2<<3

int flag;//等待任务完成标志
unsigned long dat[2];//存放转换完成的数据
unsigned long adc_dat0;
static struct adc_device *adc_dev;


//声明等待队列
DECLARE_WAIT_QUEUE_HEAD(adc_wait);

struct adc_device
{
	struct platform_device	*pdev;
	struct clk		*clk;
	void __iomem		*regs;
	unsigned int		 prescale;
	int			 irq;
};

static ssize_t adc_read(struct file *file, char *buffer, size_t length, loff_t * offset)//设备读取函数
{
	  flag=0;
	  writel(1<<0,adc_dev->regs + S3C2410_ADCCON); //启动转换，转换完成后自动置0
	  wait_event_interruptible(adc_wait,flag);     //等待转换完成
	  copy_to_user(buffer, (char *)dat, sizeof(dat));
	  printk("read adc succeed!\n");

	  return 2;
}

static int adc_open(struct inode * inode,struct file * file) //打开设备函数
{
	printk("open adc succeed!\n");

	return SUCCESS;
}

static int adc_release(struct inode * inode,struct file * file) //关闭设备函数
{
	printk("release adc succeed!\n");
	return SUCCESS;
}


static const struct file_operations adc_fops = {
	.owner =	THIS_MODULE,
	.read =		adc_read,
	.open =		adc_open,
	.release =	adc_release,
};

static struct miscdevice adc_miscdev =
{
	.minor = ADC_MINOR,
    .name  = "s3c24xx-adc",
	.fops  = &adc_fops,
};


static irqreturn_t s3c_adc_irq(int irq, void *pw)
{
	struct adc_device *adc = pw;

    if(flag==0)
    {
        dat[0]=(readl(adc->regs + S3C2410_ADCDAT0) & 0x3ff );//读取转换完成的数据
        dat[1]=(readl(adc->regs + S3C2410_ADCDAT1) & 0x3ff );//读取转换完成的数据
        flag=1;
        wake_up_interruptible(&adc_wait);//唤醒等待其上的进程
        printk("Read value is %ld\t%ld\n",dat[0],dat[1]);
    }
    return IRQ_HANDLED;
}

static int adc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct adc_device *adc;
	struct resource *regs;
	int ret;

	adc = kzalloc(sizeof(struct adc_device), GFP_KERNEL);
	adc->pdev = pdev;
	adc->prescale = S3C2410_ADCCON_PRSCVL(49);

	adc->irq = platform_get_irq(pdev, 1);
	if (adc->irq <= 0) {
		dev_err(dev, "failed to get adc irq\n");
		ret = -ENOENT;
		goto err_alloc;
	}
	ret = request_irq(adc->irq, s3c_adc_irq, 0, dev_name(dev), adc);
	if (ret < 0) {
		dev_err(dev, "failed to attach adc irq\n");
		goto err_alloc;
	}

	adc->clk = clk_get(dev, "adc");
	if (IS_ERR(adc->clk)) {
		dev_err(dev, "failed to get adc clock\n");
		ret = PTR_ERR(adc->clk);
		goto err_irq;
	}

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
		dev_err(dev, "failed to find registers\n");
		ret = -ENXIO;
		goto err_clk;
	}

	adc->regs = ioremap(regs->start, resource_size(regs));
	if (!adc->regs) {
		dev_err(dev, "failed to map registers\n");
		ret = -ENXIO;
		goto err_clk;
	}

	clk_enable(adc->clk);

	writel(adc->prescale | S3C2410_ADCCON_PRSCEN | ADC_CHANNEL_AIN2,
	       adc->regs + S3C2410_ADCCON);


	dev_info(dev, "attached adc driver\n");

	platform_set_drvdata(pdev, adc);

	ret = misc_register(&adc_miscdev);
	if (ret) {
		dev_err(dev, "cannot register miscdev on minor=%d (%d)\n",
			ADC_MINOR, ret);
		goto err_clk;
	}

	adc_dev = adc;
	return 0;
err_clk:
	clk_put(adc->clk);

err_irq:
	free_irq(adc->irq, adc);

err_alloc:
	kfree(adc);
	return ret;
}

static int adc_remove(struct platform_device *pdev)
{
	struct adc_device *adc = platform_get_drvdata(pdev);

	iounmap(adc->regs);
	free_irq(adc->irq, adc);
	clk_disable(adc->clk);
	clk_put(adc->clk);
	kfree(adc);
	return 0;
}

static struct platform_driver adc_driver = {
	.probe		= adc_probe,
	.remove		= adc_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "s3c24xx-adc",
	},
};


static int __init adc_init(void)
{
	return platform_driver_register(&adc_driver);
}

static int __exit adc_exit(void)
{
	return platform_driver_register(&adc_driver);
}

module_init(adc_init);
module_exit(adc_exit);
