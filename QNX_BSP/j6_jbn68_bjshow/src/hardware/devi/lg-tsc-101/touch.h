/*
 * $QNXLicenseC:
 * Copyright 2012, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */


#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <sys/time.h>
#include <hw/i2c.h>
#include <hw/inout.h>

//This is borrowed from startup to help with GPIO definitions
//TODO: include this from original path
#include <dra74x-gpio.h>

// Touch sreen interrupt (connected to GPIO1_02)
#define TOUCH_INT        1002

//Borrowed from init_intrinfo.c
#define GPIO_IRQ_BASE_VECTOR    1000

//TODO: find if we already have GPIO reg size defined somewhere. I could not find one.
//TRM says regsize = 408bytes hence defaulting to 1KB
#define DRA74X_GPIO1_REGSIZE        0x400

//This is wake up GPIO connected to tch hw via TLC59108IPW
#define TOUCH_WAKEUP_PIN	6
#define TOUCH_WAKE_HIGH   	0x10
#define TOUCH_WAKE_LOW    	0x00
#define TOUCH_WAKE_MASK    	0x30	//bit 5:4 controls pin6

// Driver state initialize
#define FLAG_INIT         0x1000
// Driver state reset
#define FLAG_RESET        0x2000

#define RELEASE_DELAY     100000000
#define INTR_DELAY        50

//Interrupt priority
#define PULSE_PRIORITY    21

//Interrupt pulse code
#define PULSE_CODE        1

#define ABS(x)  ((x) >= 0 ? (x) : (-(x)))

//Verbosity codes
#define LVL_INFO	0
#define LVL_VERBOSE	1
#define LVL_MAX		10


// I2C controller device pathname
#define TSC_I2C_DEVICE		"/dev/i2c0"

// IO Expander TLC59108IPW
//we are only interested in LED6 pin on this chip
#define TLC59108IPW_ADDRESS		0x40
#define TLC59108IPW_MODE1		0x00
#define TLC59108IPW_MODE2		0x01
#define TLC59108IPW_PWM2		0x04
#define TLC59108IPW_LEDOUT0		0x0C
#define TLC59108IPW_LEDOUT1		0x0D

// LG-TSC slave address
#define TSC_ADDRESS				0x18

// Speed of I2C communication with TSC. Currently use 100K - can try 400K later.
#define TSC_I2C_SPEED			100000


//===================== Starting here are the definitions related to LG touch screen controller =====================
//These are Touch Controller Device Registers (DA)
#define TCH_FIRM_UPDATE_R           0x04
#define TCH_CALIBRATION_W           0x05
#define TCH_CALIBRATION_CHECK_R     0x06
#define TCH_IDLE_CONTROL_W          0x11
#define TCH_FLASH_FIELD_R           0x12
#define TCH_IDLE_READ_R             0x17
#define TCH_FIRMWARE_ID_R           0xD0
#define TCH_TOUCH_DATA_LENGTH       0xF5
#define TCH_TOUCH_DATA              0xFA

//Register bit field define for registers TCH_IDLE_CONTROL_W and TCH_FLASH_FIELD_R
#define TCH_IDLE_CONTROL_MODE_ON    (1<<0)
#define TCH_IDLE_CONTROL_MODE_OFF   (0<<0)
#define TCH_IDLE_CONTROL_SET_FREQ(x)(x<<1)
#define TCH_IDLE_CONTROL_FREQ_MASK  (7<<1)
#define TCH_IDLE_CONTROL_SET_TIME(x)(x<<4)
#define TCH_IDLE_CONTROL_TIME_MASK  (7<<4)
#define TCH_IDLE_READ_MODE_MASK     (1<<0)
#define TCH_IDLE_READ_SET_FREQ(x)   (x<<1)
#define TCH_IDLE_READ_FREQ_MASK     (7<<1)
#define TCH_IDLE_READ_SET_TIME(x)   (x<<4)
#define TCH_IDLE_READ_TIME_MASK     (7<<4)

#define TCH_IDLE_MASK				0x01
#define TCH_IDLE_OFF				0x0

//idle time defines for registers TCH_IDLE_CONTROL_W and TCH_FLASH_FIELD_R
#define TCH_IDLE_TIME_1SEC          (0)
#define TCH_IDLE_TIME_2SEC          (1)
#define TCH_IDLE_TIME_3SEC          (2)
#define TCH_IDLE_TIME_4SEC          (3)
#define TCH_IDLE_TIME_5SEC          (4)
#define TCH_IDLE_TIME_6SEC          (5)
#define TCH_IDLE_TIME_7SEC          (6)
#define TCH_IDLE_TIME_8SEC          (7)
#define TCH_IDLE_TIME_9SEC          (8)
#define TCH_IDLE_TIME_10SEC         (9)
#define TCH_IDLE_TIME_11SEC         (10)
#define TCH_IDLE_TIME_12SEC         (11)
#define TCH_IDLE_TIME_13SEC         (12)
#define TCH_IDLE_TIME_14SEC         (13)
#define TCH_IDLE_TIME_15SEC         (14)
#define TCH_IDLE_TIME_16SEC         (15)

//idle frequencies for registers TCH_IDLE_CONTROL_W and TCH_FLASH_FIELD_R
#define TCH_IDLE_FREQ_10HZ          (0)
#define TCH_IDLE_FREQ_20HZ          (1)

//Possible values for register TCH_FIRM_UPDATE_R
#define TCH_FIRM_UPDATE_STATUS_CHECK    (4)
#define TCH_FIRM_UPDATE_BOOT_JUMP   	(0x88)

//Possible value for
#define TCH_FLASH_FIELD_VAL1        (0x55)  //check on other possible values

//This shows individual bit position inside 'struct tch_id'
#define TCH_ID_STATUS_ON            (1)
#define TCH_ID_STATUS_OFF           (0)

//This contains each touch point's status. 0:off(absent), 1:On(present)
#define FULL_TCH_ID(P) 	((uint16_t)(((P).byte_hi.id.id_hi & 0x30) << 4)|((P).byte_lo.id_lo))

//These defines are to help streamline the LG TSC code
#define GET_ABS_TCH_X(P)  	((uint16_t) (((P).x_hi << 8)|((P).x_lo)))
#define GET_ABS_TCH_Y(P)  	((uint16_t) (((P).y_hi << 8)|((P).y_lo)))
#define MAX_TOUCH_POINTS	(10)
#define MAX_TOUCH_DATA_LEN	(32)

#include <_pack1.h>

//The touch data packet is in below format:
struct a_tch_pos { //Read as: A touch position
	uint8_t  y_hi:4; //lower nibble
	uint8_t  x_hi:4; //higher nibble
	uint8_t  x_lo:8;
	uint8_t  y_lo:8;
};

struct next_byte {
	uint8_t  id_reserved2:4;
	uint8_t  id_hi:2;
	uint8_t  id_reserved1:2;
};

struct tch_id { //Read as: Touch ID
    union  {
        uint8_t  id_lo:8;
        }byte_lo;

	union {
		uint8_t             abyte;
	    struct next_byte   id;
	}byte_hi;
};

//This is a full size touch data struct. This assumes all 10 touch points are to be read and holds storage for such
//See sample from ppt for illustration
struct tch_data {
	struct tch_id 	touch_id;
	struct a_tch_pos 	touch_points[MAX_TOUCH_POINTS];
};

struct lg_tch_info {
	uint8_t idle_read;
	uint8_t fw_id;
	uint8_t calibration_check;
};

struct lg_tch_platform_data {
	//uint8_t x_line;
	//uint8_t y_line;
	uint16_t x_size;
	uint16_t y_size;
};

/* driver private data */
typedef struct _private_data
{
	int             	irq;    /* IRQ to attach to */
	int             	iid;    /* Interrupt ID */
	int             	irq_pc; /* IRQ pulse code */

	int             	chid;
	int             	coid;
	pthread_attr_t     pattr;
	pthread_mutex_t    mutex;
	struct sched_param param;
	struct sigevent    event;

	struct packet_abs 	tp[MAX_TOUCH_POINTS]; /* LG's TSC has 10 touch point */

	unsigned char		verbose;
	int					flags;
	unsigned 			irq_count;		/* This helps track when to toggle Wakeup pin*/

	/* GPIO regs */
    unsigned       	gpio1_reglen;	/* GPIO regs access*/
    unsigned       	gpio1_physbase;
	uintptr_t      	gpio1_regbase;

	/* I2C related stuff */
	char				*i2c;
	int					fd;
	unsigned int		speed;
	i2c_addr_t			slave;
	uint8_t             skip_idle_disable;    //Default (0) means: disable idle mode

	struct lg_tch_info info;				/* Holds touch controller's data*/
	struct lg_tch_platform_data pdata;

	/* Timer related stuff */
	timer_t         	timerid;
	struct itimerspec 	itime;
	uint8_t  			rotate;
} private_data_t;

#include <_packpop.h>

static int touch_init(input_module_t *module);
static int touch_devctrl(input_module_t *module, int event, void *ptr);
static int touch_reset(input_module_t *module);
static int touch_parm(input_module_t *module,int opt,char *optarg);
static int touch_shutdown(input_module_t *module, int delay);
static void *intr_thread( void *data, int argid );
static int read_reg(int fd, uint16_t reg, uint16_t len, uint8_t *val);
static int touch_write_reg( int fd, uint16_t reg, uint8_t  val);
static int touch_read_reg(int fd, uint16_t reg, uint8_t *val);
static int touch_read_message(private_data_t    *dp, struct tch_data *message);
static void touch_input_touchevent(private_data_t *dp, struct tch_data *message, input_module_t *up );
static int TLC59108IPW_write_reg( int fd, uint16_t reg, uint8_t val);
static int TLC59108IPW_read_reg(int fd, uint16_t reg, uint16_t len, uint8_t *val);
static int tlc_wakeup_ldc(private_data_t  *dp);
static int ldc3001_initialize(private_data_t	*dp);


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/lg-tsc-101/touch.h $ $Rev: 759883 $")
#endif
