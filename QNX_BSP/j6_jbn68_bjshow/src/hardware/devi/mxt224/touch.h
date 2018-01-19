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


// Touch sreen interrupt (GPIO0_31)
#define TOUCH_INT        0x31F

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


#define JITTER_DELTA        60


// I2C controller device pathname
#define TSC_I2C_DEVICE		"/dev/i2c0"

// IO Expander PCF8575 slave address
#define PCF8575_ADDRESS		0x21

// mXT224 slave address
#define TSC_ADDRESS		0x4A

// Speed of I2C communication with TSC. Currently use 100K - can try 400K later.
#define TSC_I2C_SPEED		100000

/* Version */
#define MXT224_VER_20		20
#define MXT224_VER_21		21
#define MXT224_VER_22		22

/* Slave addresses */
#define MXT224_APP_LOW		0x4a
#define MXT224_APP_HIGH		0x4b

/* Firmware */
#define MXT224_FW_NAME		"MXT224.fw"

/* Registers */
#define MXT224_FAMILY_ID	0x00
#define MXT224_VARIANT_ID	0x01
#define MXT224_VERSION		0x02
#define MXT224_BUILD		0x03
#define MXT224_MATRIX_X_SIZE	0x04
#define MXT224_MATRIX_Y_SIZE	0x05
#define MXT224_OBJECT_NUM	0x06
#define MXT224_OBJECT_START	0x07

#define MXT224_OBJECT_SIZE	6

/* Object types */
#define MXT224_DEBUG_DIAGNOSTIC	37
#define MXT224_GEN_MESSAGE	5
#define MXT224_GEN_COMMAND	6
#define MXT224_GEN_POWER	7
#define MXT224_GEN_ACQUIRE	8
#define MXT224_TOUCH_MULTI	9
#define MXT224_TOUCH_KEYARRAY	15
#define MXT224_TOUCH_PROXIMITY	23
#define MXT224_PROCI_GRIPFACE	20
#define MXT224_PROCG_NOISE	22
#define MXT224_PROCI_ONETOUCH	24
#define MXT224_PROCI_TWOTOUCH	27
#define MXT224_SPT_COMMSCONFIG	18	/* firmware ver 21 over */
#define MXT224_SPT_GPIOPWM	19
#define MXT224_SPT_SELFTEST	25
#define MXT224_SPT_CTECONFIG	28
#define MXT224_SPT_USERDATA	38	/* firmware ver 21 over */

/* MXT224_GEN_COMMAND field */
#define MXT224_COMMAND_RESET		0
#define MXT224_COMMAND_BACKUPNV		1
#define MXT224_COMMAND_CALIBRATE	2
#define MXT224_COMMAND_REPORTALL	3
#define MXT224_COMMAND_DIAGNOSTIC	5

/* MXT224_GEN_POWER field */
#define MXT224_POWER_IDLEACQINT		0
#define MXT224_POWER_ACTVACQINT		1
#define MXT224_POWER_ACTV2IDLETO	2

/* MXT224_GEN_ACQUIRE field */
#define MXT224_ACQUIRE_CHRGTIME		0
#define MXT224_ACQUIRE_TCHDRIFT		2
#define MXT224_ACQUIRE_DRIFTST		3
#define MXT224_ACQUIRE_TCHAUTOCAL	4
#define MXT224_ACQUIRE_SYNC		5
#define MXT224_ACQUIRE_ATCHCALST	6
#define MXT224_ACQUIRE_ATCHCALSTHR	7

/* MXT224_TOUCH_MULTI field */
#define MXT224_TOUCH_CTRL		0
#define MXT224_TOUCH_XORIGIN		1
#define MXT224_TOUCH_YORIGIN		2
#define MXT224_TOUCH_XSIZE		3
#define MXT224_TOUCH_YSIZE		4
#define MXT224_TOUCH_BLEN		6
#define MXT224_TOUCH_TCHTHR		7
#define MXT224_TOUCH_TCHDI		8
#define MXT224_TOUCH_ORIENT		9
#define MXT224_TOUCH_MOVHYSTI		11
#define MXT224_TOUCH_MOVHYSTN		12
#define MXT224_TOUCH_NUMTOUCH		14
#define MXT224_TOUCH_MRGHYST		15
#define MXT224_TOUCH_MRGTHR		16
#define MXT224_TOUCH_AMPHYST		17
#define MXT224_TOUCH_XRANGE_LSB		18
#define MXT224_TOUCH_XRANGE_MSB		19
#define MXT224_TOUCH_YRANGE_LSB		20
#define MXT224_TOUCH_YRANGE_MSB		21
#define MXT224_TOUCH_XLOCLIP		22
#define MXT224_TOUCH_XHICLIP		23
#define MXT224_TOUCH_YLOCLIP		24
#define MXT224_TOUCH_YHICLIP		25
#define MXT224_TOUCH_XEDGECTRL		26
#define MXT224_TOUCH_XEDGEDIST		27
#define MXT224_TOUCH_YEDGECTRL		28
#define MXT224_TOUCH_YEDGEDIST		29
#define MXT224_TOUCH_JUMPLIMIT		30	/* firmware ver 22 over */

/* MXT224_PROCI_GRIPFACE field */
#define MXT224_GRIPFACE_CTRL		0
#define MXT224_GRIPFACE_XLOGRIP		1
#define MXT224_GRIPFACE_XHIGRIP		2
#define MXT224_GRIPFACE_YLOGRIP		3
#define MXT224_GRIPFACE_YHIGRIP		4
#define MXT224_GRIPFACE_MAXTCHS		5
#define MXT224_GRIPFACE_SZTHR1		7
#define MXT224_GRIPFACE_SZTHR2		8
#define MXT224_GRIPFACE_SHPTHR1		9
#define MXT224_GRIPFACE_SHPTHR2		10
#define MXT224_GRIPFACE_SUPEXTTO	11

/* MXT224_PROCI_NOISE field */
#define MXT224_NOISE_CTRL		0
#define MXT224_NOISE_OUTFLEN		1
#define MXT224_NOISE_GCAFUL_LSB		3
#define MXT224_NOISE_GCAFUL_MSB		4
#define MXT224_NOISE_GCAFLL_LSB		5
#define MXT224_NOISE_GCAFLL_MSB		6
#define MXT224_NOISE_ACTVGCAFVALID	7
#define MXT224_NOISE_NOISETHR		8
#define MXT224_NOISE_FREQHOPSCALE	10
#define MXT224_NOISE_FREQ0		11
#define MXT224_NOISE_FREQ1		12
#define MXT224_NOISE_FREQ2		13
#define MXT224_NOISE_FREQ3		14
#define MXT224_NOISE_FREQ4		15
#define MXT224_NOISE_IDLEGCAFVALID	16

/* MXT224_SPT_COMMSCONFIG */
#define MXT224_COMMS_CTRL		0
#define MXT224_COMMS_CMD		1

/* MXT224_SPT_CTECONFIG field */
#define MXT224_CTE_CTRL			0
#define MXT224_CTE_CMD			1
#define MXT224_CTE_MODE			2
#define MXT224_CTE_IDLEGCAFDEPTH	3
#define MXT224_CTE_ACTVGCAFDEPTH	4
#define MXT224_CTE_VOLTAGE		5	/* firmware ver 21 over */

#define MXT224_VOLTAGE_DEFAULT		2700000
#define MXT224_VOLTAGE_STEP		10000

/* Define for MXT224_GEN_COMMAND */
#define MXT224_BOOT_VALUE		0xa5
#define MXT224_BACKUP_VALUE		0x55
#define MXT224_BACKUP_TIME		25	/* msec */
#define MXT224_RESET_TIME		65	/* msec */

#define MXT224_FWRESET_TIME		175	/* msec */


/* Touch status */
#define MXT224_SUPPRESS		(1 << 1)
#define MXT224_AMP		(1 << 2)
#define MXT224_VECTOR		(1 << 3)
#define MXT224_MOVE		(1 << 4)
#define MXT224_RELEASE		(1 << 5)
#define MXT224_PRESS		(1 << 6)
#define MXT224_DETECT		(1 << 7)

/* Touchscreen absolute values */
#define MXT224_MAX_XC		0x3ff
#define MXT224_MAX_YC		0x3ff
#define MXT224_MAX_AREA		0xff

/* Orient */
#define MXT224_NORMAL			0x0
#define MXT224_DIAGONAL			0x1
#define MXT224_HORIZONTAL_FLIP		0x2
#define MXT224_ROTATED_90_COUNTER	0x3
#define MXT224_VERTICAL_FLIP		0x4
#define MXT224_ROTATED_90		0x5
#define MXT224_ROTATED_180		0x6
#define MXT224_DIAGONAL_COUNTER		0x7

#define MXT224_MAX_FINGER		10

struct mxt224_info {
	uint8_t family_id;
	uint8_t variant_id;
	uint8_t version;
	uint8_t build;
	uint8_t matrix_xsize;
	uint8_t matrix_ysize;
	uint8_t object_num;
};

struct mxt224_object {
	uint8_t type;
	uint16_t start_address;
	uint8_t size;
	uint8_t instances;
	uint8_t num_report_ids;

	/* to map object and message */
	uint8_t max_reportid;
};

struct mxt224_message {
	uint8_t reportid;
	uint8_t message[7];
	uint8_t checksum;
};

struct mxt224_platform_data {
    unsigned int x_line;
    unsigned int y_line;
    unsigned int x_size;
    unsigned int y_size;
    unsigned int blen;
    unsigned int threshold;
    unsigned int voltage;
	unsigned char orient;
};

/* driver private data */
typedef struct _private_data
{
	int             irq;    /* IRQ to attach to */
	int             iid;    /* Interrupt ID */
	int             irq_pc; /* IRQ pulse code */

	int             chid;
	int             coid;
	pthread_attr_t       pattr;
	struct sched_param   param;
	struct sigevent      event;

	struct packet_abs tp;
	
	unsigned char	verbose;
	int		flags;

	unsigned        last_x, last_y;

	pthread_mutex_t mutex;

	/* I2C related stuff */
	char		*i2c;
	int		fd;
	unsigned int	speed;	
	i2c_addr_t	slave;				

	/*mxt224 */
	struct mxt224_platform_data pdata;
	struct mxt224_object *object_table;
	struct mxt224_info info;

	/* Timer related stuff */
	timer_t         timerid;
	struct itimerspec itime;

	int             intr_delay;
	int		bits;
	int        max_matrix_size;
	uint8_t  tsccmd_accex;
	uint8_t  tsccmd_accey;
	uint8_t  tsccmd_readx;
	uint8_t  tsccmd_ready;
	uint8_t  tsccmd_readz1;
	uint8_t  tsccmd_readz2;
	int      touch_threshold; 
	uint8_t  rotate;
} private_data_t;


static int touch_init(input_module_t *module);
static int touch_devctrl(input_module_t *module, int event, void *ptr);
static int touch_reset(input_module_t *module);
static int touch_parm(input_module_t *module,int opt,char *optarg);
static int touch_shutdown(input_module_t *module, int delay);
static void *intr_thread(void *data);

static int read_reg(int fd, uint16_t reg, uint16_t len, uint8_t *val);
static int mxt224_read_reg(int fd, uint16_t reg, uint8_t *val);
static int mxt224_write_reg( int fd, uint16_t reg, uint8_t  val);
static int mxt224_read_object_table(int fd, uint16_t reg, uint8_t *object_buf);
static struct mxt224_object * mxt224_get_object( private_data_t    *dp, uint8_t type);
static int mxt224_read_message(private_data_t    *dp, struct mxt224_message *message);
static int mxt224_read_object(private_data_t	*dp , uint8_t type, uint8_t offset, uint8_t *val);
static int mxt224_write_object(private_data_t	*dp, uint8_t type, uint8_t offset, uint8_t val);
static int mxt224_get_info(private_data_t	*dp);
static int mxt224_get_object_table(private_data_t	*dp);
static int mxt224_check_matrix_size(private_data_t	*dp);
static int mxt224_check_reg_init(private_data_t	*dp);
static int mxt224_object_writable(uint8_t  type);
static int mxt224_make_highchg(private_data_t	*dp);
static void mxt224_handle_pdata(private_data_t	*dp);
static int mxt224_initialize(private_data_t	*dp);
static void mxt224_dump_message(struct mxt224_message *message);
static void mxt224_input_touchevent(private_data_t *dp, struct mxt224_message *message, int id, input_module_t *up );


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devi/mxt224/touch.h $ $Rev: 699928 $")
#endif
