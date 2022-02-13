# 中断

相比于正点原子的例程，删除了定时器，应用层代码，只保留了最原始的按键中断。按键中断不需要应用层代码，只需要加载了内核模块就能触发中断。

---
获取中断号:
> 中断号是硬件规定好的，我们只是读取出来

unsigned int irq_of_parse_and_map(struct device_node *dev,int index);
> index：索引号，interrupts 属性可能包含多条中断信息，通过 index 指定要获取的信息。

如果是GPIO的话，可以使用`int gpio_to_irq(unsigned int gpio)`函数来获取GPIO对应的中断号
> gpio：要获取的 GPIO 编号。
---
申请中断:
`request_irq`用于申请中断，该函数可能会导致睡眠，不能在中断上下文或者禁止睡眠的代码段中使用。它会激活中断，不需要手动使能

```C
int request_irq(unsigned int irq,irq_handler_t handler, unsigned long flags,const char *name, void *dev)
```

irq：要申请中断的中断号。
handler：中断处理函数，当中断发生以后就会执行此中断处理函数。
flags：中断标志。
name：中断名字。
dev：dev 会传递给中断处理函数 irq_handler_t 的第二个参数。
> void free_irq(unsigned int irq, void *dev)释放中断
