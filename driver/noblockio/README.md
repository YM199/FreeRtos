# 非阻塞IO实验

当设备不可用（不会挂起）：

- 轮询等待：会立即向内核返回一个错误码，表示读取失败。应用程序会再次重新读取数据，这样一直重复循环直到读取成功。
- 直接放弃

```c
fd = open( "/dev/xxx_dev", O_RDWR | 0_NONBLOCK ); /*O_NONBLOCK表示已非阻塞方式打开设备*/
```

---

设备驱动程序需要提供非阻塞处理方式 (轮询)，当应用层调用`select`、`epoll`、`poll`函数时驱动程序中的poll函数就会执行。

```c
int select(
            int nfds, /*最大文件描述符+1*/
            fd_set *readfds,
            fd_set *writefds,
            fd_set *exceptfds,
            struct timeval *timeout
          )
```

关于最大文件描述符:#TODO: 待学习，代码里面就是fd+1。

- readfds: 监视文件是否可以读取，只要集合里面有一个文件可以读取`select`就返回一个大于0的值，如果没有文件可以读取，那么就会根据`timeout`参数来判断是否超时。设置为NULL表示不关心任何文件的读变换。
- writefds: 监视文件的写操作，与`readfds`同理。
- exceptfds: 监视文件的异常，与`readfds`同理。

```c
int ret;
fd_set readfds;
struct timeval timeout;
timerout.tv_sec = 0;
timerout.tv_usec = 500000;

ret = select( fd + 1, readfds, NULL, NULL, &timeout )
switch (ret)
{
    case 0: /* 超时 */
        /* 用户自定义超时处理 */
    break;

    case -1:/* 错误 */
        /* 用户自定义错误处理 */
    break;

    default:  /* 可以读取数据 */
        /*数据读取代码*/
    break;
}
```
