//CORTEX-A8 LED TEST - 逆序流水灯

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "led8.h"

int main(void)
{
    int fd;

    // 打开端口设备
    if ((fd=open_port_device())<0)
        exit(0);
    else
    {
        // 配置GPE0端口控制寄存器
        if ((port_write(16,fd,GPE0CON_OFFSET,0x11111000,0xff))<0)
            exit(0);
        else 
            *(volatile unsigned char *)GPIO_DAT = 0xff; // 关闭所有LED
        
        // 配置GPE1端口控制寄存器
        if ((port_write(16,fd,GPE1CON_OFFSET,0x00000111,0xff))<0)
            exit(0);
        else 
            *(volatile unsigned char *)GPIO_DAT = 0xff; // 关闭所有LED
    }
    
    sleep(3); // 延时3秒
    
    // 主循环 - 逆序流水灯效果
    while(1)
    {
        // 控制LED8-LED6 (GPE1端口)
        if ((port_write(16,fd,GPE1CON_OFFSET,0x00000111,0xff))<0)
            exit(0);
        else 
        {
            // LED8点亮 (二进制1011, 第3位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xfb;
            printf("LED 8 点亮\n");
            sleep(1);
            
            // LED7点亮 (二进制1101, 第1位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xfd;
            printf("LED 7 点亮\n");
            sleep(1);
            
            // LED6点亮 (二进制1110, 第0位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xfe;
            printf("LED 6 点亮\n");
            sleep(1);
            
            *(volatile unsigned char *)GPIO_DAT = 0xff; // 关闭所有LED
        }

        // 控制LED5-LED1 (GPE0端口)
        if ((port_write(16,fd,GPE0CON_OFFSET,0x11111000,0xff))<0)
            exit(0);
        else 
        {
            // LED5点亮 (第7位为0)
            *(volatile unsigned char *)GPIO_DAT = 0x7f;
            printf("LED 5 点亮\n");
            sleep(1);
            
            // LED4点亮 (第6位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xbf;
            printf("LED 4 点亮\n");
            sleep(1);
            
            // LED3点亮 (第5位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xdf;
            printf("LED 3 点亮\n");
            sleep(1);
            
            // LED2点亮 (第4位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xef;
            printf("LED 2 点亮\n");
            sleep(1);
            
            // LED1点亮 (第3位为0)
            *(volatile unsigned char *)GPIO_DAT = 0xf7;
            printf("LED 1 点亮\n");
            sleep(1);
            
            *(volatile unsigned char *)GPIO_DAT = 0xff; // 关闭所有LED
        }
    }

    // 清理资源
    free_port_device();
    close_port_device(fd);
    return 0;
}

/* end of file */