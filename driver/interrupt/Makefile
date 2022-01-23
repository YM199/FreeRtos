KERNELDIR := /home/yangmou/Desktop/IMX6ULL/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga/
CURRENT_PATH := $(shell pwd)
VPATH := include
DEVICE_NAME := interrupt
obj-m := $(DEVICE_NAME).o

target: kernel_module app

kernel_module: $(DEVICE_NAME).h
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

app: app.h
	arm-linux-gnueabihf-gcc app.c -o app

.PHONY: clean copy
copy:
	cp app $(DEVICE_NAME).ko /home/yangmou/linux/nfs/rootfs/yangmou/
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	rm app
