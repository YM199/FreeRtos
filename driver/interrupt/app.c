/************************************
 *
 * 文件名：app.c
 *
 * 文件描述：key应用
 * 
 * 创建人：Yang Mou，2022 年  1 月 21 日
 * 
 * *********************************/
#include "include/app.h"

int main( int argc, char *argv[] )
{
    int fd, ret;
    char *filename;
    unsigned int cmd;
    unsigned int arg;
    unsigned char str[100];

    if( argc != 2 )
    {
        /*参数错误*/
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }

    filename = argv[1];

    fd = open( filename, O_RDWR );
    if( fd < 0 )
    {
        /*无法打开filename*/
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }

    while( 1 )
    {
        printf( "input CMD: " );
        ret = scanf( "%d", &cmd );
        if( ret != 1 )
        {
            gets( str );
        }

        if( 1 == cmd) /* 关闭LED */
        {
            cmd  = CLOSE_CMD;
        }
        else if( 2 == cmd ) /*打开LED*/
        {
            cmd = OPEN_CMD;
        }
        else if( 3 == cmd ) /*设置周期值*/
        {
            cmd = SETPERIOD_CMD;
            printf("Input Timer Period: ");
            ret = scanf("%d", &arg);
            if( ret != 1 )
            {
                gets(str);
            }
        }

        ioctl( fd, cmd, arg );
    }

    close( fd );

    return 0;
}
