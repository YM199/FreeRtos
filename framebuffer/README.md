# 设备树插件

如何看设备树是否插入成功？

只需要通过`find / -name "lcdif"`取查找相应的属性文件，如果能找到就代表插入成功

```
     fragment@0 {
         target = <&lcdif>;
         __overlay__ {

         };
     };
```

这个文件插入的是lcdif节点，直接`find / -name "lcdif"`查找文件位置