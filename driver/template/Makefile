KERNELDIR := /home/yangmou/Desktop/IMX6ULL/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga/
CURRENT_PATH := $(shell pwd)
FILE_NAME := chrdev
obj-m := $(FILE_NAME).o

target: kernel_modules app

kernel_modules: $(FILE_NAME).h
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

app: App.h
	arm-linux-gnueabihf-gcc App.c -o App

copy:
	sudo cp App $(FILE_NAME).ko /var/www/html

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	-rm App


