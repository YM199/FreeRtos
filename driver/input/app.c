#include "include/app.h"

static struct input_event inputevent;

int main( int argc, char **argv )
{
    int fd;
    int err = 0;
    char *filename;

    filename = argv[1];

    debug( argc != 2, -1 );

    fd = open( filename, O_RDWR );

    while( 1 )
    {
        err = read( fd, &inputevent, sizeof( inputevent ) );

        if( err > 0 )
        {
            switch( inputevent.type )
            {
                case EV_KEY:
                    if( inputevent.code < BTN_MISC ) /*键盘键值*/
                    {
                        printf( "key %d %s\r\n", inputevent.code,inputevent.value ? "press" : "release" );
                    }
                    else
                    {
                        printf( "button %d %s\r\n", inputevent.code,inputevent.value ? "press" : "release" );
                    }
                    break;
                /*其他事件类型*/
                case EV_REL:
                    break;
                case EV_ABS:
                    break;
                case EV_SW:
                    break;
            }
        }
        else
        {
            printf( "读取数据失败\r\n" );
        }
    }

    return 0;
}