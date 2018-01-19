#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <hw/i2c.h>
#include <hw/inout.h>
#include "dra74x-gpio.h"

#define DRA74X_GPIO_REGSIZE        	0x400

#define MSG(x...) 					printf(x)
#define I2C_DEV_BUS   				"/dev/i2c4"
#define I2C_DEV_ADR   				0x42
#define I2C_DEV_SPEED 				400000
#define MAX_PACKET_SIZE 		 	128

typedef struct {
  i2c_send_t head;
  char buf[MAX_PACKET_SIZE];
} gesture_write_t;

typedef struct {
  i2c_recv_t head;
  char buf[MAX_PACKET_SIZE];
} gesture_read_only_t;

typedef struct {
  i2c_sendrecv_t head;
  char buf[MAX_PACKET_SIZE];
} gesture_read_t;

//Gesture enable mask
unsigned char buf_enable_gesture_all[MAX_PACKET_SIZE] =  {
	/* Header **/
	0x10, 0x00, 0x00, 0xA2,
	/* Message ID **/
	0x85, 0x00,
	/* Reserved **/
	0x00, 0x00,
	/* Argument0 **/
	0x7F, 0x00, 0x00, 0x00,
	/* Argument1 **/
	0x7F, 0x00, 0x00, 0x00
};

//Data output enable mask
unsigned char buf_enable_data_output_gesture[MAX_PACKET_SIZE] =  {
	/* Header **/
	0x10, 0x00, 0x00, 0xA2,
	/* Message ID **/
	0xA0, 0x00,
	/* Reserved **/
	0x00, 0x00,
	/* Argument0 **/
	0x1A, 0x00, 0x00, 0x00,
	/* Argument1 **/
	0x3F, 0x18, 0x00, 0x00
};

//lock data output mask
unsigned char buf_output_lock_gesture_data[MAX_PACKET_SIZE] =  {
	/* Header **/
	0x10, 0x00, 0x00, 0xA2,
	/* Message ID **/
	0xA1, 0x00,
	/* Reserved **/
	0x00, 0x00,
	/* Argument0 **/
	0x1A, 0x00, 0x00, 0x00,
	/* Argument1 **/
	0x3F, 0x18, 0x00, 0x00
};

//Data outout request mask
unsigned char buf_request_flick_data[MAX_PACKET_SIZE] =  {
	/* Header **/
	0x10, 0x00, 0x00, 0xA2,
	/* Message ID **/
	0xA2, 0x00, 0x00, 0x00,
	/* Reserved **/
	0x1A, 0x00, 0x00, 0x00,
	/* para **/
	0x3F, 0x18, 0x00, 0x00
};

static inline void sr32(unsigned addr, unsigned start_bit, unsigned num_bits, unsigned value)
{
	unsigned tmp, msk = 0;
	msk = 1 << num_bits;
	--msk;
	tmp = in32(addr) & ~(msk << start_bit);
	tmp |= value << start_bit;
	out32(addr, tmp);
}

void gpio_init(uintptr_t gpio1_regbase, uintptr_t gpio3_regbase)
{
	sr32(gpio1_regbase + GPIO_OE, 24, 1, 1);
	sr32(gpio1_regbase + GPIO_DATAIN, 24, 1, 1); 		//set input
	sr32(gpio3_regbase + GPIO_OE, 20, 1, 0); 		//set output

	//set 3_20 to output,high
	sr32(gpio3_regbase + GPIO_OE, 20, 1, 0);
	sr32(gpio3_regbase + GPIO_DATAOUT, 20, 1, 0);
	usleep(5000);
	sr32(gpio3_regbase + GPIO_DATAOUT, 20, 1, 1);

	//set 1_24 to input
	sr32(gpio1_regbase + GPIO_OE, 24, 1, 1); //set GPIO1_24 OE to OFF(input)
}

/* GPIO03-20   MCLR(Reset) **/
void gesture_reset(uintptr_t gpio3_regbase)
{
	sr32(gpio3_regbase + GPIO_DATAOUT, 20, 1, 0); //set low
	usleep(50000);
	sr32(gpio3_regbase + GPIO_DATAOUT, 20, 1, 1); //set high
	usleep(50000);
	/* Delay 200ms waitting GESTID library start **/
	usleep(200000);
}

/* GPIO01-24   TS **/
int get_transfer_status(uintptr_t gpio1_regbase)
{
	unsigned tmp = 1, msk = 0;
	msk = 1 << 1;
	--msk;
	tmp = (in32(gpio1_regbase + GPIO_DATAIN) >> 24) & msk;

	return tmp;
}

static void m_string_cpy(char *str1, char *str2, int length)
{
	int i = 0;
	for(i=0;i<length;i++)
	{
		*str1 == *str2;
		str1 ++;
		str2 ++;
	}
}

int IIC_init(int speed)
{
	int fd = -1;
	int ret = 0;

	fd = open(I2C_DEV_BUS, O_RDWR);

	if(fd <= 0)
	{
		MSG("open fail %d\n", fd);
		return fd;
	}
	else
	{
		MSG("%s open success with fd:%d\n", I2C_DEV_BUS, fd);
	}

	if(ret = devctl(fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof(speed), NULL))
	{
		MSG("Set bus speed fail %d\n", ret);
		return ret;
	}

	return fd;
}

int IIC_read_only(int fd, gesture_read_only_t *read_data)
{
	int ret = 0;
	if(ret = devctl(fd, DCMD_I2C_RECV, read_data, sizeof(*read_data), NULL))
	{
		MSG("Fail to ioctl for DCMD_I2C_SENDRECV, errno=%d\n", ret);
		return ret;
	}
}

int IIC_read(int fd, gesture_read_t *read_data)
{
	int ret = 0;
	if(ret = devctl(fd, DCMD_I2C_SENDRECV, read_data, sizeof(*read_data), NULL))
	{
		MSG("Fail to ioctl for DCMD_I2C_SENDRECV, errno=%d\n", ret);
		return ret;
	}
}

int IIC_write(int fd, gesture_write_t *write_data)
{
	int ret = 0;
	if(ret = devctl(fd, DCMD_I2C_SEND, write_data, sizeof(*write_data), NULL))
	{
		MSG("Fail to ioctl for DCMD_I2C_SEND, errno=%d\n", ret);
		return ret;
	}
}

void clear_buffer(char *buffer)
{
	int i = 0;
	for(i=0;i<MAX_PACKET_SIZE;i++)
	{
		*buffer = 0xFF;
		buffer ++;
	}
}

int main(void)
{
	/* Init IIC read/write data struct **/
	gesture_write_t gesture_write_data;
	gesture_read_t gesture_read_data;
	gesture_read_only_t gesture_read_data_only;
	int is_circle = 0;
	int z_position_value = -1;
	int count = 0;

	int fd;
	int speed = I2C_DEV_SPEED;
	int ts_line_level = 1;
	int i = 0;
	unsigned char airwheel_counter =0;
	unsigned char old_airwheel_counter =0;
	unsigned char real_airwheel_counter =0;
	unsigned char ui_airwheel_counter =0;
	unsigned char counter_circle_increase =0;
	unsigned char counter_circle_decrease =0;

	/* Remap GPIO **/
	unsigned	gpio1_reglen, gpio3_reglen;	/* GPIO regs access*/
	unsigned	gpio1_physbase, gpio3_physbase;
	uintptr_t	gpio1_regbase, gpio3_regbase;

	/* Enable IO capability.*/
	if (ThreadCtl(_NTO_TCTL_IO, NULL) == -1) {
		printf("ThreadCtl: error");
		return -1;
	}

	gpio1_reglen = DRA74X_GPIO_REGSIZE;
	gpio1_physbase = DRA74X_GPIO1_BASE;
	gpio1_regbase = mmap_device_io(gpio1_reglen, gpio1_physbase);
	if (gpio1_regbase == (uintptr_t)MAP_FAILED) {
		printf("%s: mmap_device_io failed for GPIO1", __FUNCTION__);
		return -1;
	}
	gpio3_reglen = DRA74X_GPIO_REGSIZE;
	gpio3_physbase = DRA74X_GPIO3_BASE;
	gpio3_regbase = mmap_device_io(gpio3_reglen, gpio3_physbase);
	if (gpio3_regbase == (uintptr_t)MAP_FAILED) {
		printf("%s: mmap_device_io failed for GPIO3", __FUNCTION__);
		return -1;
	}
	/* End of remap GPIO **/

	printf("start\n");
	/* Init gesture chip control gpio **/
	gpio_init(gpio1_regbase, gpio3_regbase);

	printf("gpio_init finished\n");
	/* Reset gesture chip **/
	gesture_reset(gpio3_regbase);

	printf("gesture_reset finished\n");
	/* Init IIC **/
	fd = IIC_init(speed);
	if(fd <= 0)
	{
		printf("IIC_init failed\n");
	}
	printf("IIC_init finished\n");
	sleep(1);



	gesture_write_data.head.slave.fmt = I2C_ADDRFMT_7BIT;
	gesture_write_data.head.slave.addr = I2C_DEV_ADR;
	gesture_write_data.head.len = 0x10;
	gesture_write_data.head.stop = 1;

	gesture_read_data_only.head.slave.fmt = I2C_ADDRFMT_7BIT;
	gesture_read_data_only.head.slave.addr = I2C_DEV_ADR;
	gesture_read_data_only.head.len = 0x10;
	gesture_read_data_only.head.stop = 1;

	gesture_read_data_only.head.len = 0x10;

	//enable flick and circle gesture
	for(i=0;i<gesture_write_data.head.len;i++)
	{
		gesture_write_data.buf[i] = buf_enable_gesture_all[i];
	}

	IIC_write(fd, &gesture_write_data);
	while (1) {
		while(ts_line_level)
		{
			ts_line_level = get_transfer_status(gpio1_regbase);
		}
		ts_line_level = 1;
		clear_buffer(gesture_read_data_only.buf);
		IIC_read_only(fd, &gesture_read_data_only);
		usleep(200);
		if(gesture_read_data_only.buf[3] == 0x15){
			if(gesture_read_data_only.buf[6] != 0 || gesture_read_data_only.buf[7] != 0)
			{
				for(i=0;i<gesture_read_data_only.head.len;i++)
				{
					printf("0x%x ", (unsigned char)gesture_read_data_only.buf[i]);
				}
				printf("\ncommand send error 111111111111111111111\n");
				return 0;
			}
			else
			{
				break;
			}
		}
	}

	//enable all gesture data output mask
	for(i=0;i<gesture_write_data.head.len;i++)
	{
		gesture_write_data.buf[i] = buf_enable_data_output_gesture[i];
	}

	IIC_write(fd, &gesture_write_data);
	while (1) {
		while(ts_line_level)
		{
			ts_line_level = get_transfer_status(gpio1_regbase);
		}
		ts_line_level = 1;
		printf("Reading...........\n");
		clear_buffer(gesture_read_data_only.buf);
		IIC_read_only(fd, &gesture_read_data_only);
		usleep(200);
		if(gesture_read_data_only.buf[3] == 0x15){
			if(gesture_read_data_only.buf[6] != 0 || gesture_read_data_only.buf[7] != 0)
			{
				for(i=0;i<gesture_read_data_only.head.len;i++)
				{
					printf("0x%x ", (unsigned char)gesture_read_data_only.buf[i]);
				}
				printf("\ncommand send error 2222222222222222222222222\n");
				return 0;
			}
			else
			{
				break;
			}
		}
	}

	//lock gesture data
	for(i=0;i<gesture_write_data.head.len;i++)
	{
		gesture_write_data.buf[i] = buf_output_lock_gesture_data[i];
	}
	IIC_write(fd, &gesture_write_data);
	while (1) {
		while(ts_line_level)
		{
			ts_line_level = get_transfer_status(gpio1_regbase);
		}
		ts_line_level = 1;
		clear_buffer(gesture_read_data_only.buf);
		IIC_read_only(fd, &gesture_read_data_only);
		usleep(200);
		if(gesture_read_data_only.buf[3] == 0x15){
			if(gesture_read_data_only.buf[6] != 0 || gesture_read_data_only.buf[7] != 0)
			{
				for(i=0;i<gesture_read_data_only.head.len;i++)
				{
					printf("0x%x ", (unsigned char)gesture_read_data_only.buf[i]);
				}
				printf("\ncommand send error 333333333333333333333333\n");
				return 0;
			}
			else
			{
				break;
			}
		}
	}

	printf("While loop start\n");

	gesture_read_data_only.head.len = 0x14;
	//Request gesture data mask
	for(i=0;i<gesture_write_data.head.len;i++)
	{
		gesture_write_data.buf[i] = buf_request_flick_data[i];
	}

	IIC_write(fd, &gesture_write_data);
	while (1) {
		while(ts_line_level)
		{
			ts_line_level = get_transfer_status(gpio1_regbase);
		}
		ts_line_level = 1;
		clear_buffer(gesture_read_data_only.buf);
		IIC_read_only(fd, &gesture_read_data_only);
		usleep(200);
		if(gesture_read_data_only.buf[3] == 0x15){
			if(gesture_read_data_only.buf[6] != 0 || gesture_read_data_only.buf[7] != 0)
			{
				for(i=0;i<gesture_read_data_only.head.len;i++)
				{
					printf("0x%x ", (unsigned char)gesture_read_data_only.buf[i]);
				}
				printf("\ncommand send error 444444444444444444444\n");
				return 0;
			}
			else
			{
				break;
			}
		}
	}

	while(1)
	{
		while(ts_line_level)
		{
			ts_line_level = get_transfer_status(gpio1_regbase);
		}

		/* Read chip register data to check if movement is changed. **/
		IIC_read_only(fd, &gesture_read_data_only);
		/* Wait 200us to assure that MGC3X30 released TS line **/
		usleep(200);

		z_position_value = ((unsigned char)gesture_read_data_only.buf[15] << 8) + (unsigned char)gesture_read_data_only.buf[14];
		if(z_position_value >= 4000)
		{
			/* gesture can be recognized **/
			count = 0;
			system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture_tip show &");
		}
		else
		{
			count ++;
			if(count > 1500)
			{
				count = 0;
				system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture_tip hide &");
				system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture none &");
				continue;
			}
		}

		if((unsigned char)gesture_read_data_only.buf[8] == 0x03)
		{
			printf("-------------------------> LEFT\n");
			system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture left &");
		}
		else if((unsigned char)gesture_read_data_only.buf[8] == 0x02)
		{
			printf("-------------------------> RIGHT\n");
			system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture right &");
		}
		else if ((unsigned char)gesture_read_data_only.buf[8] == 0x04)
		{
			printf("-------------------------> UP\n");
			system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture up &");
		}
		else if((unsigned char)gesture_read_data_only.buf[8] == 0x05)
		{
			printf("-------------------------> DOWN\n");
			system("/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture down &");
		}
		else if((unsigned char)(gesture_read_data_only.buf[7])&0x02)
		{
		   airwheel_counter =( unsigned char ) (gesture_read_data_only.buf[12] >> 5)*32 +( unsigned char) (gesture_read_data_only.buf[12]  & 0x1F);

		   if((counter_circle_increase == 0x0) &&(counter_circle_decrease == 0x0))
		   {
			   if ((0x14<airwheel_counter)&&(airwheel_counter<0xF0))
			   {
				   real_airwheel_counter = airwheel_counter / 2 + 7;
			   }
			   else if	(0x14>=airwheel_counter)
			   {
				   real_airwheel_counter = 0x14;
			   }
			   else if (0xF0<=airwheel_counter)
			   {
				   real_airwheel_counter =0x78;
			   }
			   else
			   {
			   }
		   }

		   if( 0x78==real_airwheel_counter)
		   {
			  if((airwheel_counter>old_airwheel_counter)&&(airwheel_counter>0xF0))
			  {
				  counter_circle_increase = 0x1;
			  }
			  else if( (0x1==counter_circle_increase)&&(airwheel_counter<=0xF0)&&(airwheel_counter>=0xD8)&&(old_airwheel_counter>airwheel_counter)&&((old_airwheel_counter-airwheel_counter)<20))
			  {
				  counter_circle_increase = 0x0;
			  }
			  else
			  {

			  }
		   }

		   if( 0x14==real_airwheel_counter)
		   {
			  if((airwheel_counter<old_airwheel_counter)&&(airwheel_counter<0x14))
			  {
				  counter_circle_decrease = 0x1;
			  }
			  else if( (0x1==counter_circle_decrease&&(airwheel_counter<=0x30))&&(airwheel_counter>=0x14)&&(old_airwheel_counter<airwheel_counter)&&((airwheel_counter-old_airwheel_counter)<0x14))
			  {
				  counter_circle_decrease = 0x0;
			  }
			  else
			  {

			  }
		   }
		  printf("real_airwheel_counter =%d,airwheel_counter =%d,old_airwheel_counter =%d,counter_circle_increase =%d,counter_circle_decrease =%d\n",real_airwheel_counter,airwheel_counter,old_airwheel_counter,counter_circle_increase,counter_circle_decrease);
		   old_airwheel_counter = airwheel_counter;

		   if((0x14<=real_airwheel_counter)&&(real_airwheel_counter<=0x78)&&(counter_circle_increase==0x0)&&(counter_circle_decrease==0x0))
		   {
			  ui_airwheel_counter = real_airwheel_counter - 0x14;

		//	   printf("----------------->>>>>%d",real_airwheel_counter);
			   char cmd[100] = "/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control gesture countercoockwise ";
			   sprintf(cmd,"/qtcar/bin/write_str2pps /pps/hinge-tech/gesture_control clockwise %d &",ui_airwheel_counter);
			   system(cmd);
		   }
		   else
		   {
			   continue;
		   }

		}
	}
}
