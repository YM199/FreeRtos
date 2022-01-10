/************************************
 *
 * 文件名：app.c
 *
 * 文件描述：LED驱动测试app
 * 
 * 创建人：Yang Mou，2022 年  1 月 10 日
 * 
 * *********************************/

#include "include/app.h"

int main( int argc, char *argv[] )
{
    int fd, retvalue;
    char *filename;
    unsigned char databuf[1];

    if( argc != 3 )
    {
         debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
         debug( "Error Usage!\r\n" );
    }

    filename = argv[1];

    /*打开led驱动*/
    fd = open(filename, O_RDWR);
    if( fd < 0 )
    {
        debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
        debug( "file %s open failed!\r\n", argv[1] );       
        return -1;
    }
    
    databuf[0] = atoi( argv[2] );/* 要执行的操作：打开或关闭 */    

    /* 向/dev/led 文件写入数据 */
    retvalue = write( fd, databuf, sizeof( databuf ) );

    retvalue = close(fd);
    if( retvalue < 0 )
    {
        debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
        debug( "file %s close failed!\r\n", argv[1] );
        return -1;
    }

    return 0;
}