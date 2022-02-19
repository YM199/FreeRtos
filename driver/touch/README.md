# 多点电容触摸屏

## 多点电容触摸协议

>Multi-touch，简称 MT。

linux 内核中有一份文档详细的讲解了多点电容触摸屏协议，文档路径为：`Documentation/input/multi-touch-protocol.txt`。

MT 协议分为两种:

1. Type A: 适用于触摸点不能被区分或者追踪，此类型的设备上报原始数据(**此类型在实际使用中非常少**！)
2. Type B: 适用于有硬件追踪并能区分触摸点的触摸设备，此类型设备通过 slot 更新某一个触摸点的信息。

>Type B 和 Type A 相比最大的区别就是 Type B 可以区分出触摸点， 因此可以减少发送到用户空间的数据。

触摸点的信息通过一系列的 ABS_MT 事件(有的资料也叫消息)上报给 linux 内核，只有 ABS_MT 事件是用于多点触摸的。
>ABS_MT 事件定义在文件 include/uapi/linux/input.h 中。
>
>常用的事件如下:
>
>- ABS_MT_SLOT:        上报触摸点 ID
>- ABS_MT_POSITION_X:  上报触摸点的 (X,Y) 坐标信息
>- ABS_MT_POSITION_Y:  上报触摸点的 (X,Y) 坐标信息
>- ABS_MT_TRACKING_ID: 区分触摸点

### Type A

对于 TypeA 设备，内核驱动需要一次性将触摸屏上所有的触摸点信息全部上报。

对于 TypeA 类型的设备，通过`input_mt_sync()`函数来隔离不同的触摸点数据信息。
<details>
  <summary>void input_mt_sync(struct input_dev *dev)</summary>
  此函数只要一个参数，类型为 input_dev，用于指定具体的 input_dev 设备。input_mt_sync()函数会触发 SYN_MT_REPORT 事件，此事件会通知接收者获取当前触摸数据，并且准备接收下一个触摸点数据。
</details>

### Type B

Type B 使用 slot 协议区分具体的触摸点，slot 需要用到 ABS_MT_TRACKING_ID 消息，这个 ID 需要硬件提供，或者通过原始数据计算出来。

Type B 设备驱动需要给每个识别出来的触摸点分配一个 slot，后面使用这个 slot 来上报触摸点信息。可以通过 slot 的 ABS_MT_TRACKING_ID 来新增、替换或删除触摸点。一个非负数的 ID 表示一个有效的触摸点，-1 这个 ID 表示未使用 slot。一个以前不存在的 ID 表示这是一个新加的触摸点，一个 ID 如果再也不存在了就表示删除了。

对于 Type B 类型的设备，上报触摸点信息的时候需要通过`input_mt_slot()`函数区分是哪一个触摸点。
<details>
  <summary>void input_mt_slot(struct input_dev *dev, int slot)</summary>
  此函数有两个参数，第一个参数是 input_dev 设备，第二个参数 slot 用于指定当前上报的是哪个触摸点信息。input_mt_slot()函数会触发 ABS_MT_SLOT 事件，此事件会告诉接收者当前
  正在更新的是哪个触摸点(slot)的数据。
</details>

---

不管是哪个类型的设备，最终都要调用`input_sync()`函数来标识多点触摸信息传输完成，告诉接收者处理之前累计的所有消息，并且准备好下一次接收。

## tslib 移植

tslib 是一个开源的第三方库，用于触摸屏性能调试，使用电阻屏的时候一般使用 tslib 进行校准。虽然电容屏不需要校准，但是由于电容屏加工的原因，有的时候其不一定精准，因此有时候也需要进行校准。最主要的是 tslib 提供了一些其他软件，我们可以通过这些软件来测试触摸屏工作是否正常。最新版本的 tslib 已经支持了多点电容触摸屏，因此可以通过 tslib 来直观的测试多点电容触摸屏驱动，这个要比观看 eventX 原始数据方便的多。

### 获取 tslib 源码

获取 tslib 的源码，git 地址为 [tslib源码](https://github.com/kergoth/tslib)，目前最新的版本是 1.21。将压缩包发送到 ubuntu 中并解压，得到名为“tslib-1.21”的目录，此目录下就是 tslib 源码。

### 修改 tslib 源码所属用户

修改解压得到的 tslib-1.21 目录所属用户为当前用户，这一步一定要做！否则在稍后的编译中会遇到各种问题。我当前 ubuntu 的登录用户名为“zuozhongkai”，那么修改命令如下：

```shell
sudo chown zuozhongkai:zuozhongkai tslib-1.21 -R
```

### ubuntu 工具安装

请查看正点原子触摸屏章节

