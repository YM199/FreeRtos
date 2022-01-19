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
    unsigned char keyvalue;

    if( argc != 2 )
    {
         debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
         debug( "Error Usage!\r\n" );
         return -1;
    }

    filename = argv[1];

    /*打开key驱动*/
    fd = open(filename, O_RDWR);
    if( fd < 0 )
    {
        debug( "FILE: %s, LINE: %d:\r\n", __FILE__, __LINE__ );
        debug( "file %s open failed!\r\n", argv[1] );       
        return -1;
    }
    
    while( 1 )
    {
        read( fd, &keyvalue, sizeof( keyvalue ) );
        if( keyvalue == KEY0VALUE )
        {
            debug("KEY0 Press, value = %#X\r\n", keyvalue);/* 按下 */
        }
    }



    retvalue = close(fd);
    if( retvalue < 0 )
    {
        debug( "FILE: %s, LINE: %d", __FILE__, __LINE__ );
        debug( "file %s close failed!\r\n", argv[1] );
        return -1;
    }

    return 0;
}