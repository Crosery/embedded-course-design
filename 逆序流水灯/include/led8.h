//CORTEX-A8 LED TEST

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define DEV_NAME "/dev/mem"

volatile unsigned int  *GPIO_DAT   = NULL;
volatile unsigned char *ADDR_START = NULL;	

//CONTROL
#define GPA0CON_OFFSET 0x0000
#define GPA1CON_OFFSET 0x0020
#define GPBCON_OFFSET 0x0040
#define GPC0CON_OFFSET 0x0060
#define GPC1CON_OFFSET 0x0080
#define GPD0CON_OFFSET 0x00A0
#define GPD1CON_OFFSET 0x00C0
#define GPE0CON_OFFSET 0x00E0
#define GPE1CON_OFFSET 0x0100
#define GPF0CON_OFFSET 0x0120
#define GPF1CON_OFFSET 0x0140
#define GPF2CON_OFFSET 0x0160
#define GPF3CON_OFFSET 0x0180
#define GPG0CON_OFFSET 0x01A0
#define GPG1CON_OFFSET 0x01C0
#define GPG2CON_OFFSET 0x01E0
#define GPG3CON_OFFSET 0x0200
#define GPICON_OFFSET  0x0220
#define GPJ0CON_OFFSET 0x0240
#define GPJ1CON_OFFSET 0x0260
#define GPJ2CON_OFFSET 0x0280
#define GPJ3CON_OFFSET 0x02A0
#define GPJ4CON_OFFSET 0x02C0

#define GPH2CON_OFFSET 0x0C40
#define GPH3CON_OFFSET 0x0C60
/*

//DATA
#define GPA0DAT_OFFSET 0x0004
#define GPA1DAT_OFFSET 0x0024
#define GPBDAT_OFFSET 0x0044
#define GPC0DAT_OFFSET 0x0064
#define GPC1DAT_OFFSET 0x0084
#define GPD0DAT_OFFSET 0x00A4
#define GPD1DAT_OFFSET 0x00C4
#define GPE0DAT_OFFSET 0x00E4
#define GPE1DAT_OFFSET 0x0104
#define GPF0DAT_OFFSET 0x0124
#define GPF1DAT_OFFSET 0x0144
#define GPF2DAT_OFFSET 0x0164
#define GPF3DAT_OFFSET 0x0184
#define GPG0DAT_OFFSET 0x01A4
#define GPG1DAT_OFFSET 0x01C4
#define GPG2DAT_OFFSET 0x01E4
#define GPG3DAT_OFFSET 0x0204
#define GPIDAT_OFFSET  0x0224
#define GPJ0DAT_OFFSET 0x0244
#define GPJ1DAT_OFFSET 0x0264
#define GPJ2DAT_OFFSET 0x0284
#define GPJ3DAT_OFFSET 0x02A4
#define GPJ4DAT_OFFSET 0x02C4

*/


int port_write( unsigned int n,					
			    unsigned int fd,
				volatile unsigned int ADDR_CON_OFFSET,
				volatile unsigned int GPIO_WR_CON,
				volatile unsigned int GPIO_WR_DAT
			  )
	{
	ADDR_START = (volatile unsigned char *)mmap(NULL,1024*n,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0xE0200000);
    if(ADDR_START == NULL)
    {
      printf("mmap err!\n");
      return -1;
    }
	*(volatile unsigned int *)(ADDR_START + ADDR_CON_OFFSET)=GPIO_WR_CON;
	GPIO_DAT = (volatile unsigned int *)(ADDR_START + ADDR_CON_OFFSET + 0x04);
	*(volatile unsigned char *)GPIO_DAT =GPIO_WR_DAT;
	return 0;
	}

int open_port_device(void)
{
	int fd;
	fd = open(DEV_NAME,O_RDWR);
	if(fd<0)
	{
      printf("Open device err!\n");
      return -1;
	}
	return fd;
}

int close_port_device(int fd)
{
close(fd);
return 0;
}

int free_port_device(void)
{
	munmap((void *)ADDR_START,1024*16);
	return 0;
}

/* end of file */
