#include "i2c_io.h"
#include "audio_hero_api.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <hw/i2c.h>

//int audio_recv(const unsigned char *addr, const int addr_size, unsigned char *data, const int data_num)
//{
	//return 0;
//}
//int audio_send(unsigned char *data, const int data_num)
//{
	//return 0;
//}

#define MSG(x...) printf(x)
//#define I2C_CHANNEL   "/dev/i2c-1"
//#define DSP_ADDRESS   (0x63)

/* This is the structure as used in the I2C_RDWR ioctl call */
//struct i2c_rdwr_ioctl_data {
   //struct i2c_msg __user *msgs;    /* pointers to i2c_msgs */
  // __u32 nmsgs;                    /* number of i2c_msgs */
//};

//static int i2c_fd = -1;

int i2c_open()
{
#if 0
   if (i2c_fd != -1)
   {
      return 0;
   }

   i2c_fd = open(I2C_CHANNEL, O_RDWR);
   if (i2c_fd < 0)
   {
      ALOGE("Failed to open i2c device.\n");
      return -1;
   }
#endif
   return 0;
}

void i2c_close()
{
#if 0
   if (i2c_fd > 0)
   {
      close(i2c_fd);
      i2c_fd = -1;
   }
#endif
}

int i2c_write(const uint8_t* data, uint8_t len)
{
#if 0
   if (i2c_fd < 0)
   {
      MSG("I2C device is not opened.\n");
      return -1;
   }

   struct i2c_msg msgs;
   struct i2c_rdwr_ioctl_data write_data;
   int ret = 0;

   ioctl(i2c_fd, I2C_TIMEOUT, 2);
   ioctl(i2c_fd, I2C_RETRIES, 1);

   msgs.len = len;
   msgs.flags = 0;
   msgs.addr = DSP_ADDRESS;
   msgs.buf = (uint8_t*)data;
   write_data.msgs = &msgs;
   write_data.nmsgs = 1;

   ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long)&write_data);
   if (ret < 0)
   {
      MSG("Error during I2C_RDWR ioctl with error code:%d.\n", ret);
      return -1;
   }

   return 0;
#endif
   return audio_send((unsigned char*)data, len);
}

int i2c_read(const uint8_t* subaddr, uint8_t subaddr_len, uint8_t* data, uint8_t len)
{
#if 0
   if (i2c_fd < 0)
   {
      MSG("I2C device is not opened.\n");
      return -1;
   }

   struct i2c_msg msgs[2];
   struct i2c_rdwr_ioctl_data read_data;
   int ret = 0;

   ioctl(i2c_fd, I2C_TIMEOUT, 2);
   ioctl(i2c_fd, I2C_RETRIES, 1);

   msgs[0].len = subaddr_len;
   msgs[0].flags = 0;
   msgs[0].addr = DSP_ADDRESS;
   msgs[0].buf = (uint8_t*)subaddr;

   msgs[1].len = len;
   msgs[1].flags = I2C_M_RD;
   msgs[1].addr = DSP_ADDRESS;
   msgs[1].buf = (uint8_t*)data;

   read_data.msgs = msgs;
   read_data.nmsgs = 2;

   ret = ioctl(i2c_fd, I2C_RDWR, (unsigned long)&read_data);
   if (ret < 0)
   {
      MSG("Error during I@C_RDWR ioctl with error code:%d.\n", ret);
      return -1;
   }

   return 0;
#endif

   return audio_recv((const unsigned char*)subaddr, subaddr_len, (unsigned char*)data, len);
}

int i2c_write_high(uint8_t subaddr, const uint8_t* data, uint8_t len)
{
   if (!data)
   {
      MSG("%s: the data is NULL pointer.\n", __FUNCTION__);
      return -1;
   }

   uint8_t buffer[len + 1];

   buffer[0] = subaddr;
   memcpy(buffer + 1, data, len);

   return i2c_write(buffer, len + 1);
}

int i2c_read_high(uint8_t subaddr, uint8_t* data, uint8_t len)
{
   if (!data)
   {
      MSG("%s: the data is NULL pointer.\n", __FUNCTION__);
      return -1;
   }

   uint8_t data_of_subaddr[1] = {subaddr};

   return i2c_read(data_of_subaddr, 1, data, len);
}

int i2c_read_back_high(uint8_t subaddr, uint8_t* data, uint8_t len)
{
   if (!data)
   {
      MSG("%s: the data is NULL pointer.\n", __FUNCTION__);
      return -1;
   }

   uint8_t data_of_subaddr[2] = {0xE0, subaddr};

   return i2c_read(data_of_subaddr, 2, data, len);
}

