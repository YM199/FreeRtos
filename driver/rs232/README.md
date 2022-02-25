# RS232

每个串口驱动都要定义一个 uart_driver。

注册uart_driver：int uart_register_driver(struct uart_driver *drv);

注销uart_driver: void uart_unregister_driver(struct uart_driver *drv);

---

添加uart_port: int uart_add_one_port(struct uart_driver *drv, struct uart_port *uport);

删除uart_port: int uart_remove_one_port(struct uart_driver *drv, struct uart_port *uport);

uart_port 中的 ops 成员变量很重要，因为 ops 包含了针对 UART 具体的驱动函数，Linux 系统收发数据最终调用的都是 ops 中的函数。