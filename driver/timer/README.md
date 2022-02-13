# 内核定时器

Linux 内核使用 `timer_list` 表示内核定时器。内核定时器不是周期运行的，超时以后就会自动关闭，如果要实现周期性定时，需要在定时处理函数中重新开启定时器。

## init_timer 函数

`init_timer` 函数负责初始化 `timer_list` 变量

```C
void init_timer(struct timer_list *timer);
```

## add_timer 函数

`add_timer` 函数用于向 Linux 内核注册定时器，使用 `add_timer` 函数向内核注册定时器以后，定时器就会开始运行。

```C
void add_timer(struct timer_list *timer);
```

## 删除定时器

del_timer 函数用于删除一个定时器，不管定时器有没有被激活，都可以使用此函数删除。
> int del_timer(struct timer_list * timer)

del_timer_sync 函数是 del_timer 函数的同步版，会等待其他处理器使用完定时器再删除。
> int del_timer_sync(struct timer_list *timer)

## 修改定时值

mod_timer 函数用于修改定时值，**如果定时器还没有激活的话，mod_timer 函数会激活定时器**！
> int mod_timer(struct timer_list *timer, unsigned long expires)
