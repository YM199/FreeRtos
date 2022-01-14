申请设备号：

```
申请
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)
int register_chrdev_region(dev_t from, unsigned count, const char *name)
注销
void unregister_chrdev_region(dev_t from, unsigned count)
```



添加字符设备：

```
添加
int cdev_add(struct cdev *p, dev_t dev, unsigned count)
删除
void cdev_del(struct cdev *p)
```



**自动创建设备节点**

创建和删除类

```
创建
struct class *class_create (struct module *owner, const char *name)
删除
void class_destroy(struct class *cls);
```

创建设备

```
创建
struct device *device_create(struct class *class,...)
卸载
void device_destroy(struct class *class, dev_t devt)
```



输入“lsmod”命令即可查看当前系统中存在的模块

查看当前系统中所有的设备

```
cat /proc/devices
```





**设备树**

到/proc/device-tree/查看对应的设备树节点
