# 非阻塞IO实验

## 应用层

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

---

select

```c
int ret;
fd_set readfds;
struct timeval timeout;
/*设置超时时间*/
timerout.tv_sec = 0;      /*秒*/
timerout.tv_usec = 500000;/*微秒*/

fd = open( "/dev/xxx_dev", O_RDWR | 0_NONBLOCK ); /*O_NONBLOCK表示已非阻塞方式打开设备*/

FD_ZERO( &readfds ); /*所有位清0*/
FD_SET( fd, &readfds ); /*某位置1，将fd加入到集合中*/

ret = select( fd + 1, readfds, NULL, NULL, &timeout );
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

---

poll函数本质上和select函数没有太大区别，但poll函数没有最大文件描述符限制。

```c
int poll(
          struct pollfd fds,
          nfds_t nfds, /*要监视的文件描述符的数量*/
          int timeout  /*超时时间，单位为ms*/
        )
```

> ```c
> struct pollfd
> {
>   int fd;        /* 文件描述符 */
>   short events;  /* 请求的事件 */
>   short revents; /* 返回的事件 */
> };
> ```
>
> fd是要监视的文件描述符
>
> events是要监视的事件:
>
> - POLLIN 有数据可以读取。
> - POLLPRI 有紧急的数据需要读取。
> - POLLOUT 可以写数据。
> - POLLERR 指定的文件描述符发生错误。
> - POLLHUP 指定的文件描述符挂起。
> - POLLNVAL 无效的请求。
> - POLLRDNORM 等同于 POLLIN。
> - 等
>
> revents是返回的事件，在events中请求的任何事件都可能在revents中返回。

```c
int ret;
int fd; /*要监视的文件描述符*/
struct pollfd fds;

fd = open( filename, O_RDWR | O_NONBLOCK ); /*非阻塞访问*/

fds.fd = fd;
fds.events = POLLIN; /*监视数据是否可以读取*/

ret = poll( &fds, 1, 500 );
if( ret )
{
    /*读取数据*/
}
else if( 0 == ret )
{
    /*超时*/
}
else if( ret < 0 )
{
    /*错误*/
}
```

> 我理解的是poll函数如果没有监视到对应的事件，就会返回一个超时条件码( 可以做对应的超时处理 )，然后进行监视。
> 经过测试，poll函数确实是不断的轮询调用的，不需要while函数辅助。

---

epoll函数用在大规模的并发服务器上面，暂未研究。

---

> 非阻塞模式并不需要等待队列，是根据驱动层的poll函数返回值来判断的，所以非阻塞模式需要构建驱动层的poll函数，阻塞模式不需要poll函数

```c
unsigned int irq_poll( struct file *filp, struct poll_table_struct *wait )
{
    unsigned int mask = 0;
    struct irq_dev *dev = ( struct irq_dev * )filp->private_data;

    if( atomic_read( &dev->releasekey ) ) /*按键按下并且松开*/
    {
        mask =  POLLIN | POLLRDNORM; /*有数据可以读取*/
    }

    return mask; /*该函数的返回值就是应用层获取到的返回值*/
}
```
