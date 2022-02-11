#include "include/app.h"

/*函数名称: main
* 函数功能: ./app /dev/miscbeep 1打开蜂鸣器  ./app /dev/miscbeep 0关闭蜂鸣器
*/
int main( int argc, char **argv )
{
    int fd, retvalue;
    char *filename;
    unsigned char databuf[1];

    debug( argc != 3, -1 ); /*输出参数不对*/

    filename = argv[1];

    fd = open( filename, O_RDWR );
    debug( fd < 0, -1 );

    databuf[0] = atoi( argv[2] );
    retvalue = write( fd, databuf, sizeof( databuf ) );
    debug( retvalue < 0, -1 );

    retvalue = close( fd );
    debug( retvalue < 0, -1 );

    return 0;
}