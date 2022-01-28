#include "include/app.h"

int main( int argc, char **argv )
{
    int fd;
    int ret = 0;
    int data = 0;
    char *filename;
    fd_set readfds;
    struct timeval timeout;
    debug( argc < 2 );


    filename = argv[1];

    fd = open( filename, O_RDWR | O_NONBLOCK); /*非阻塞访问*/
    debug( fd < 0 );


    while( 1 )
    {
	    FD_ZERO(&readfds);
	    FD_SET(fd, &readfds);
	    /* 构造超时时间 */
	    timeout.tv_sec = 0;
	    timeout.tv_usec = 500000; /* 500ms */
	    ret = select(fd + 1, &readfds, NULL, NULL, &timeout);
	    switch (ret)
        {
		    case 0: 	/* 超时 */
			/* 用户自定义超时处理 */
			break;

		    case -1:	/* 错误 */
			/* 用户自定义错误处理 */
			break;

		    default:  /* 可以读取数据 */
			    if(FD_ISSET(fd, &readfds))
                {
				    ret = read(fd, &data, sizeof(data));
				    if (ret < 0)
                    {
					    /* 读取错误 */
				    }
                    else
                    {
					    if (data)
						    printf("key value=%d\r\n", data);
				    }
			    }
			break;
	    }
    }
    close( fd );
    return 0;
}