#include "include/app.h"

static int fd = 0;

/*================================================================
 * 函数名：sigio_signal_func
 * 功能：读取内核数据并打印
 * 备注：信号SIGNO的处理函数
================================================================*/
static void sigio_signal_func( int signum )
{
	int err = 0;
	unsigned int keyvalue = 0;

	err = read( fd, &keyvalue, sizeof( keyvalue ) );
	debug( err < 0,  );
	printf( "sigio signal! key value=%d\r\n", keyvalue );
}


int main( int argc, char **argv )
{
	int flags = 0;
	char *filename;

	debug( argc != 2, -1 );

	filename = argv[1];
	fd = open( filename, O_RDWR );
	debug( fd < 0, -1 );

	signal( SIGIO, sigio_signal_func ); /*设置信号SIGNO的处理函数*/

	fcntl( fd, F_SETOWN, getpid() ); /*将当前进程的进程号告诉内核*/

	/*开启异步通知*/
	flags = fcntl( fd, F_GETFL ); /*获取当前进程状态*/
	fcntl( fd, F_SETFL, flags | FASYNC ); /*开启当前进程的异步通知功能*/

	while(1)
	{
		sleep(2);
	}

	close( fd );

    return 0;
}