KERNELDIR := /home/yangmou/Desktop/IMX6ULL/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga/
CURRENT_PATH := $(shell pwd)

obj-m := gt9147.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
	arm-linux-gnueabihf-gcc app.c -o app


.PHONY: clean copy
copy:
	cp app gt9147.ko /home/yangmou/linux/nfs/rootfs/yangmou/
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	rm app