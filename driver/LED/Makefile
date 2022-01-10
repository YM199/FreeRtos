KERNELDIR := /home/yangmou/Desktop/IMX6ULL/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga/
CURRENT_PATH := $(shell pwd)
VPATH := include
DEVICE_NAME := led
obj-m := $(DEVICE_NAME).o

target: kernel_module app

kernel_module: led.h
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

app: app.h
	arm-linux-gnueabihf-gcc app.c -o app

.PHONY: clean copy
copy:
	sudo cp App $(FILE_NAME).ko /var/www/html
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	rm app