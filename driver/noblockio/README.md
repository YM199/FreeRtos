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

> 该函数能够监听文件描述符小于nfds的所有文件。

参数列表:

- `int nfds`: 表示待监听的集合里的最大文件描述符的值 + 1。
- `fd_set *readfds`、`fd_set *writefds`、`fd_set *exceptfds`三个集合分别存放需要监听读、写、异常三个操作的文件描述符( 如果为NULL表示不监听该事件 )。
- `struct timeval *timeout`表示超时时间，设为0则立刻扫描并返回，设为NULL则永远等待，直到有文件描述符就绪。

返回值:

- 负值: select错误
- 正值: 集合中的某些文件可读写或出错
- 0: 等待超时，没有可读写或错误的文件

---

定义好fd_set变量后调用如下宏进行操作:

```c
void FD_ZERO( fd_set *set );          /*将fd_set变量所有位清0*/
void FD_SET( int fd, fd_set *set );   /*将fd_set变量的某个位置1，也就是向fd_set添加一个文件描述符fd*/
void FD_CLR( int fd, fd_set *set );   /*将fd_set变量的某个位清0，也就是在fd_set删除一个文件描述符fd*/
int  FD_ISSET( int fd, fd_set *set ); /*测试文件描述符fd是否在fd_set变量中*/
```

```c
int ret;
fd_set readfds;
struct timeval timeout;
/*设置超时时间*/
timerout.tv_sec = 0;      /*秒*/
timerout.tv_usec = 500000;/*微秒*/

FD_ZERO( &readfds ); /*所有位清0*/
FD_SET( fd, &readfds ); /*某位置1，将fd加入到集合中*/

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
