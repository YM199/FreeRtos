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
    int fd;
    int ret = 0;
    char *filename;
    unsigned char data;
    if(argc != 2)
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }
    filename = argv[1];
    fd = open( filename, O_RDWR );
    if( fd < 0 )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
    }
    
    while( 1 )
    {
        ret = read( fd, &data, sizeof( data ) );
        if ( ret < 0 )
        {

        }
        else
        {
            if( data )
            {
                debug( "key value: %d\n", data);
            }
        }
    }
    return 0;
}
