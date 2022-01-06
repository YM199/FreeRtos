**一、修改 Makefile，添加编译器**

修改：

```makefile
ARCH ?= arm
CROSS_COMPILE ?= /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
```

**二、busybox 中文字符支持**

> 打开文件 busybox-1.29.0/libbb/printable_string.c

找到函数 printable_string：



注释掉下面这个两行代码

```c
 31         /*if (c >= 0x7f)
 32             break;*/
     
 45             /*if (c < ' ' || c >= 0x7f)*/
 46             if( c < ' ')
 47                 *d = '?';

```

> 打开文件 busybox-1.29.0/libbb/unicode.c

找到函数：unicode_conv_to_printable2

修改如下：

```
1022                 /**d++ = (c >= ' ' && c < 0x7f) ? c : '?';*/
1023                 *d++ = (c >= ' ') ? c : '?';
1024                 src++;

1031                 /*if (c < ' ' || c >= 0x7f)*/
1032                 if(c < ' ')
1033                     *d = '?';

```

**三、配置 busybox**

先使用默认配置来配置一下 busybox：

```
make defconfig
```

令打开图形化配置界面：

```
make menuconfig
```

配置路径：

```
Location: 
	-> Settings 
		-> Build static binary (no shared libs)                   不要选中
		-> vi-style line editing commands                         选中
		

```





```
Location: 
	-> Linux Module Utilities
		-> Simplified modutils									 不要选中
```



```
Location: 
	-> Linux System Utilities 
 		-> mdev (16 kb) //确保下面的全部选中，默认都是选中的
```



```
Location: 
 	-> Settings
 		-> Support Unicode                                              //选中
			-> Check $LC_ALL, $LC_CTYPE and $LANG environment variables //选中
```



**四、编译 busybox**

```
make   #编译                                   
make install CONFIG_PREFIX=/home/yangmou/linux/nfs/rootfs   #安装
```



**五、向根文件系统添加 lib 库**

> 安装路径下

```
mkdir lib
```

进入如下路径对应的目录：

```
/usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc/lib

cp *so* *.a /home/yangmou/linux/nfs/rootfs/lib/ -d
```



将 rootfs/lib 中的 ld-linux-armhf.so.3 文件删除掉

```
rm ld-linux-armhf.so.3
```

重新复制：

```
cp ld-linux-armhf.so.3 /home/yangmou/linux/nfs/rootfs/lib/
```



进入：

```
cd /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/lib

cp *so* *.a /home/yangmou/linux/nfs/rootfs/lib/ -d
```



**向 rootfs 的“usr/lib”目录添加库文件**

在 rootfs 的 usr 目录下创建一个名为 lib 的目录，然后：



```
cd /usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/lib

cp *so* *.a /home/yangmou/linux/nfs/rootfs/usr/lib/ -d
```

会报多次指定源文件的警告，不用理会，复制成功了的



**六、创建其他文件夹**

```
mkdir dev proc mnt sys tmp root
```



**七、根文件系统初步测试**





**八、完善根文件系统**

创建/etc/init.d/rcS 文件（chmod 777 rcS）

```
#!/bin/sh
PATH=/sbin:/bin:/usr/sbin:/usr/bin
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/usr/lib
runlevel=S
umask 022
export PATH LD_LIBRARY_PATH runlevel

mount -a
mkdir /dev/pts
mount -t devpts devpts /dev/pts

echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s
```

创建/etc/fstab 文件

```
#<file system> <mount point> <type> <options> <dump> <pass>
proc /proc proc defaults 0 0
tmpfs /tmp tmpfs defaults 0 0
sysfs /sys sysfs defaults 0 0
```



创建/etc/inittab 文件

```
#etc/inittab
::sysinit:/etc/init.d/rcS
console::askfirst:-/bin/sh
::restart:/sbin/init
::ctrlaltdel:/sbin/reboot
::shutdown:/bin/umount -a -r 7 ::shutdown:/sbin/swapoff -a
```



新建文件/etc/resolv.conf

```
nameserver 114.114.114.114
nameserver 192.168.101.1
```





**注意事项：**

在打包根文件系统时，执行如下命令

```
cd rootfs/
tar -vcjf rootfs.tar.bz2 *
```

