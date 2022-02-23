#include <stdio.h>
#include <sys/time.h>
#include <time.h>
int main(int argc, char **argv)
{
    time_t seconds; /*秒数*/

    seconds = time(NULL); /*从1970-01-01 00:00:00到现在的秒数*/

    /*以本地时间的字符串方式打印*/
    printf("%s\n",ctime(&seconds));

    return 0;
}