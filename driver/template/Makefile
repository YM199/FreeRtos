KERNELDIR := /home/yangmou/Desktop/IMX6ULL/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga/

CURRENT_PATH := $(shell pwd)
obj-m := chrdevbase.o

target: kernel_modules app

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

app:
	arm-linux-gnueabihf-gcc chrdevbaseApp.c -o chrdevbaseApp

copy:
	sudo cp chrdevbaseApp chrdevbase.ko /var/www/html

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean


