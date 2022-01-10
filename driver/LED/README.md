插入内核模块之后：创建设备节点

```
mknod /dev/led c 200 0
```







```
./app /dev/led 1    点亮

./app /dev/led 0    熄灭
```

