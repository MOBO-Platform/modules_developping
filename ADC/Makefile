#obj-$(S3C2410_lLED_SWITCH) += adc.o



obj-m += adc.o
KDIR:= /usr/src/linux-2.6.32.61
PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -rf *.o
