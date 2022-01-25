#include "include/app.h"

int main( int argc, char **argv )
{
    int fd;
    int ret = 0;
    int data = 0;
    char *filename;

    if( argc < 2 )
    {
        debug(  "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }

    filename = argv[1];

    fd = open( filename, O_RDWR );
    if( fd < 0 )
    {
        debug( "__FILE__: %s, __LINE__: %d\r\n", __FILE__, __LINE__ );
        return -1;
    }

    while( 1 )
    {
        ret = read( fd, &data, sizeof( data ) );/*读取数据到data中*/
        if( ret >= 0 )
        {
            if( data )
            {
                printf( "key value = %d\r\n", data );
                data = 0;
            }
        }
    }

    close( fd );
    return 0;
}