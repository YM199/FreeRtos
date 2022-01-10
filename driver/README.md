字符设备的注册和注销函数

```c
static inline int register_chrdev( unsigned int major, const char *name,const struct file_operations *fops );
static inline void unregister_chrdev(unsigned int major, const char *name);
```

地址映射

```
ioremap(cookie,size);
void iounmap (volatile void __iomem *addr);
```







输入“lsmod”命令即可查看当前系统中存在的模块

查看当前系统中所有的设备

```
cat /proc/devices
```
