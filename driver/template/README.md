输入“lsmod”命令即可查看当前系统中存在的模块



查看当前系统中所有的设备

```
cat /proc/devices
```



驱动加载成功需要在/dev 目录下创建一个与之对应的设备节点文件，应用程序就是通过操作这个设备节点文件来完成对具体设备的操作。输入如下命令创建/dev/chrdevbase 这个设备节点文件：

```
mknod /dev/chrdevbase c 200 0
```

chrdevbase是设备名，“200”是设备的主设备号，“0”是设备的次设备号。



设备操作测试

```
./chrdevbaseApp /dev/chrdevbase 1
./chrdevbaseApp /dev/chrdevbase 2
```

