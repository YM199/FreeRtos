#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "test_app.h"
#include <sys/ioctl.h>

#include <linux/fb.h>
#include <sys/mman.h>

/**
 * @brief 
 * 
 * @param fbmem 
 * @param line_length 
 * @param bpp 
 * @param x 
 * @param y 
 * @param color 
 */
void lcd_show_pixel(unsigned char *fbmem, int line_length, int bpp, int x, int y, unsigned int color)
{
    unsigned char *p8 = (unsigned char *)(fbmem + y * line_length + x * bpp / 8);
    unsigned short *p16 = (unsigned short *)p8;
    unsigned int *p32 = (unsigned int *)p8;

    unsigned int red, green, blue;
    switch (8)
    {
    case 8:
        *p8 = (unsigned char)(color & 0xf);
        break;
    case 16:
        red = (color >> 16) & 0xff;
        green = (color >> 8) & 0xff;
        blue = (color >> 0) & 0xff;
        *p16 = (unsigned short)( ((red >> 3) << 11) | ((green >> 2) << 5) | ((blue >> 3) << 0) );
        break;
    case 32:
        *p32 = color;
        break;
    default:
        printf("can not support &d bpp\r\n");
        break;
    }   
}

int main(void)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    unsigned int *temp;
    unsigned int color;
    unsigned char *fbp = 0;
    int fp = 0;
    int x,y;

    fp = open("/dev/fb0", O_RDWR);
    if (fp < 0)
    {
        printf("Error : Can not open framebuffer device/n");
        exit(1);
    }

    if (ioctl(fp, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed infomation\r\n");
        exit(2);
    }

    if (ioctl(fp, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information\r\n");
        exit(3);
    }
    printf("The mem is :%d\n", finfo.smem_len);
    printf("The line_length is :%d\n", finfo.line_length);
    printf("The xres is :%d\n", vinfo.xres);
    printf("The yres is :%d\n", vinfo.yres);
    printf("bits_per_pixel is :%d\n", vinfo.bits_per_pixel);

    fbp = (unsigned char *) mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
    if ((int)fbp == -1)
    {
        printf("Error: fail to map framebuffer device to memory\r\n");
        exit(4);
    }

    /*红色*/
    color = 0xff << 16;
    for (y = 0; y < vinfo.yres; y++)
    {
        for (x = 0; x < vinfo.xres; x++)
        {
            lcd_show_pixel(fbp, finfo.line_length, vinfo.bits_per_pixel, x, y, color);
        }
    }

    usleep(1000*2000);
    /*显示图片*/
    temp = (unsigned int *)test_picture;
    for (y = 0; y < vinfo.yres; y++)
    {
      for(x = 0; x < vinfo.xres; x++)
      {
        lcd_show_pixel(fbp, finfo.line_length, vinfo.bits_per_pixel, x, y, *temp);
        temp++;
      }
    }

    munmap(fbp, finfo.smem_len);
    close(fp);
    return 0;
}