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

