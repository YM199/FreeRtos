#include "App.h"

static char usrdata[] = {"usr data!"};

int main( int argc, char *argv[] )
{
    int fd, retvalue;
    char *filename;
    char readbuf[100], writebuf[100];

    if( argc != 3 )
    {
        printf("Error Usage!\r\n");
        return -1;
    }

    filename = argv[1];

    fd = open( filename, O_RDWR );
    if( fd < 0 )
    {
        printf("Can't open file %s\r\n", filename);
        return -1;
    }

    if( 1 == atoi( argv[2] ) )
    {
        retvalue = read( fd, readbuf, 50 );
        if( retvalue < 0 )
        {
            printf( "read file %s failed!\r\n", filename );
        }
        else
        {
            printf( "read data: %s\r\n", readbuf );
        }
    }
    if( 2 == atoi( argv[2] ) )
    {
        memcpy( writebuf, usrdata, sizeof( usrdata ) );
        retvalue = write( fd, writebuf, 50 );
        if( retvalue < 0 )
        {
            printf( "write file %s failed!\r\n", filename );
        }
    } 

    retvalue == close(fd);
    if(retvalue < 0)
    {
        printf( "close %s error\r\n", filename );
    }

    return 0;
}