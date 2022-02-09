#include "include/app.h"

int main(int argc, char **argv)
{
    int fd, retval;
    char *filename;

    unsigned char databuf[2];

    debug( argc != 3, -1 ); /*输入的参数数目不对*/

    filename = argv[1];

    fd =  open( filename, O_RDWR );
    debug( fd < 0, -1 ); /*设备打开失败*/

    databuf[0] = atoi( argv[2] );
    retval = write( fd, databuf, sizeof( databuf ) );
    debug( retval < 0, -1 ); /*写入失败*/

    retval = close( fd );
    debug( retval < 0, -1 ); /*设备关闭失败*/

    return 0;
}