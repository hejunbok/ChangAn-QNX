/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
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



/*
 *    dummy.c
 *      The primary interface into the dummy codec.
 */

struct dummy_context;
#define  MIXER_CONTEXT_T struct dummy_context

#include "mcasp.h"
#include <hw/i2c.h>

enum
{
	DUMMY_PAGESELECTOR 				= 0,
#define PAGE_SELECT                                 0
	DUMMY_SFT_RESET				 	= 1,
#define SFT_RESET                                   (1<<7)
	DUMMY_CODEC_SAMPLE_RATE_SELEC 	= 2,
	DUMMY_PLL_PROGRAM_REG_A			= 3,
#define REG_A_PLL_CTRL                              (1<<7)
#define REG_A_PLL_Q(i)                              (i<<3)
#define REG_A_PLL_P(i)                              (i<<0)
	DUMMY_PLL_PROGRAM_REG_B			= 4,
#define REG_B_PLL_J(i)                              (i<<2)
	DUMMY_PLL_PROGRAM_REG_C			= 5,
#define REG_C_PLL_D_MOST_BIT(i)                     ((i>>6) & 0xff) //0x36
	DUMMY_PLL_PROGRAM_REG_D			= 6,
#define REG_D_PLL_D_LEAST_BIT(i)                    ((i&0x3f)<<2)//0xb0
	DUMMY_CODEC_DATAPATH_SETUP		= 7,
#define RIGTH_DAC_DATAPATH_CTRL                     (1<<1)
#define LEFT_DAC_DATAPATH_CTRL                      (1<<3)
#define FS_SET                                      (1<<7)
	DUMMY_AUDIO_SER_DATA_INT_CTRL_A	= 8,
#define REG_A_BCLK_DIR_CTRL                         (1<<7)
#define REG_A_WCLK_DIR_CTRL                         (1<<6)
#define REG_A_SERIAL_DOUT_3STATE_CTRL               (1<<5)
	DUMMY_AUDIO_SER_DATA_INT_CTRL_B	= 9,
#define REG_B_BCLK_RATE_CTRL                        (1<<3)
#define REG_B_AUDIO_SERIAL_DATA_MODE                (1<<6)
	DUMMY_AUDIO_SER_DATA_INT_CTRL_C	= 10,
	DUMMY_AUDIO_CODEC_OVERFLOW_FLAG	= 11,
#define OVERFLOW_FLAG_PLL_R(i)                      (i<<0)
	DUMMY_AUDIO_CODEC_DFC				= 12, //DIGITAL FILTER CONTROL
	DUMMY_HEADSET_PRESS_DETECT_A		= 13,
	DUMMY_HEADSET_PRESS_DETECT_B		= 14,
	DUMMY_LEFT_ADC_PGA_GAIN_CTRL		= 15,
	DUMMY_RIGHT_ADC_PGA_GAIN_CTRL		= 16,
	DUMMY_MIC3LR_TO_LEFT_ADC_CTRL		= 17,
#define MIC3R_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX     (0xf<<0)
#define MIC3L_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX     (0xf<<4)
	DUMMY_MIC3LR_TO_RIGHT_ADC_CTRL	= 18,
#define MIC3R_INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX    (0xf<<0)
#define MIC3L_INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX    (0xf<<4)
	DUMMY_LINE1L_TO_LEFT_ADC_CTRL		= 19,
#define LINE1L_LEFT_ADC_CHANNEL_POWER_CTRL          (1<<2)
#define LINE1L_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX    (0xf<<3)
	DUMMY_LINE2L_TO_LEFT_ADC_CTRL		= 20,
	DUMMY_LINE1R_TO_LEFT_ADC_CTRL		= 21,
	DUMMY_LINE1R_TO_RIGHT_ADC_CTRL	= 22,
#define LINE1R__RIGHT_ADC_CHANNEL_POWER_CTRL        (1<<2)
#define LINE1R__INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX  (0xf<<3)
	DUMMY_LINE2R_TO_RIGHT_ADC_CTRL	= 23,
	DUMMY_LINE1L_TO_RIGHT_ADC_CTRL	= 24,
	DUMMY_MICBIAS_CTRL				= 25,
#define MICBIAS_OFF                                 (0<<6)
#define MICBIAS_20                                  (1<<6)
#define MICBIAS_25                                  (2<<6)
#define MICBIAS_AVDD                                (3<<6)
	DUMMY_LEFT_AGC_CTRL_A				= 26,
	DUMMY_LEFT_AGC_CTRL_B				= 27,
	DUMMY_LEFT_AGC_CTRL_C				= 28,
	DUMMY_RIGHT_AGC_CTRL_A			= 29,
	DUMMY_RIGHT_AGC_CTRL_B			= 30,
	DUMMY_RIGHT_AGC_CTRL_C			= 31,
	DUMMY_LEFT_AGC_GAIN				= 32,
	DUMMY_RIGHT_AGC_GAIN				= 33,
	DUMMY_LEFT_AGC_NOISE_GATE_DEBNC	= 34,
	DUMMY_RIGHT_AGC_NOISE_GATE_DEBNC	= 35,
	DUMMY_ADC_FLAG					= 36,
	DUMMY_DAC_POWER_DRIVER_CTRL		= 37,
#define LEFT_DAC_POWER_CTRL                         (1<<7)
#define RIGHT_DAC_POWER_CTRL                        (1<<6)
	DUMMY_HPOWER_DRIVER_CTRL			= 38,
	DUMMY_RESERVED_REG_1				= 39,
	DUMMY_HIGH_POWER_STAGE_CTRL		= 40,
	DUMMY_DAC_OUTPUT_SWITCH_CTRL		= 41,
#define RIGHT_DAC_OUT_SWITCHING_CTRL                (1<<4)
#define LEFT_DAC_OUT_SWITCHING_CTRL                 (1<<6)
	DUMMY_OUTPUT_DRIVER_POP_REDUCTION	= 42,
	DUMMY_LEFT_DAC_DIG_VOL_CTRL		= 43,
	DUMMY_RIGHT_DAC_DIG_VOL_CTRL		= 44,
	DUMMY_LINE2L_TO_HPLOUT_VOL_CTRL	= 45,
	DUMMY_PGA_L_TO_HPLOUT_VOL_CTRL	= 46,
	DUMMY_DAC_L1_TO_HPLOUT_VOL_CTRL	= 47,
#define HPLOUT_DAC_L1_OUT_ROUTING_CTR               (1<<7)
	DUMMY_LINE2R_TO_HPLOUT_VOL_CTRL	= 48,
	DUMMY_PGA_R_TO_HPLOUT_VOL_CTRL	= 49,
	DUMMY_DAC_R1_TO_HPLOUT_VOL_CTRL	= 50,
	DUMMY_HPLOUT_OUTPUT_LVL_CTRL		= 51,
#define HPLOUT_POWER_CTRL                           (1<<0)
#define HPLOUT_MUTE                                 (1<<3)
	DUMMY_LINE2L_TO_HPLCOM_VOL_CTRL	= 52,
	DUMMY_PGA_L_TO_HPLCOM_VOL_CTRL	= 53,
	DUMMY_DAC_L1_TO_HPLCOM_VOL_CTRL	= 54,
	DUMMY_LINE2R_TO_HPLCOM_VOL_CTRL	= 55,
	DUMMY_PGA_R_TO_HPLCOM_VOL_CTRL	= 56,
	DUMMY_DAC_R1_TO_HPLCOM_VOL_CTRL	= 57,
	DUMMY_HPLCOM_OUTPUT_LVL_CTRL		= 58,
	DUMMY_LINE2L_TO_HPROUT_VOL_CTRL	= 59,
	DUMMY_PGA_L_TO_HPROUT_VOL_CTRL	= 60,
	DUMMY_DAC_L1_TO_HPROUT_VOL_CTRL	= 61,
	DUMMY_LINE2R_TO_HPROUT_VOL_CTRL	= 62,
	DUMMY_PGA_R_TO_HPROUT_VOL_CTRL	= 63,
	DUMMY_DAC_R1_TO_HPROUT_VOL_CTRL	= 64,
#define HPROUT_DAC_R1_OUT_ROUTING_CTRL              (1<<7)
	DUMMY_HPROUT_OUTPUT_LVL_CTRL		= 65,
#define HPROUT_POWER_CTRL                           (1<<0)
#define HPROUT_MUTE                                 (1<<3)
	DUMMY_LINE2L_TO_HPRCOM_VOL_CTRL	= 66,
	DUMMY_PGA_L_HPRCOM_VOL_CTRL		= 67,
	DUMMY_DAC_L1_TO_HPRCOM_VOL_CTRL	= 68,
	DUMMY_LINE2R_TO_HPRCOM_VOL_CTRL	= 69,
	DUMMY_PGA_R_TO_HPRCOM_VOL_CTRL	= 70,
	DUMMY_DAC_R1_TO_HPRCOM_VOL_CTRL	= 71,
	DUMMY_HPRCOM_OUTPUT_LVL_CTRL		= 72,
	DUMMY_LINE2L_TO_MONOLOP_VOL_CTRL	= 73,
	DUMMY_PGA_L_MONOLOP_VOL_CTRL		= 74,
	DUMMY_DAC_L1_TO_MONOLOP_VOL_CTRL	= 75,
	DUMMY_LINE2R_TO_MONOLOP_VOL_CTRL	= 76,
	DUMMY_PGA_R_TO_MONOLOP_VOL_CTRL	= 77,
	DUMMY_DAC_R1_TO_MONOLOP_VOL_CTRL	= 78,
	DUMMY_MONOLOP_OUTPUT_LVL_CTRL		= 79,
	DUMMY_LINE2L_TO_LEFTLOP_VOL_CTRL	= 80,
	DUMMY_PGA_L_TO_LEFTLOP_VOL_CTRL	= 81,
	DUMMY_DAC_L1_TO_LEFTLOP_VOL_CTRL	= 82,
	DUMMY_LINE2R_TO_LEFTLOP_VOL_CTRL	= 83,
	DUMMY_PGA_R_TO_LEFTLOP_VOL_CTRL	= 84,
	DUMMY_DAC_R1_TO_LEFTLOP_VOL_CTRL	= 85,
	DUMMY_LEFTLOP_OUTPUT_LVL_CTRL		= 86,
#define LEFTLOP_LEFT_LOP_M_POWER_STATUS             (1<<0)
#define LEFTLOP_LEFT_LOP_M_MUTE                     (1<<3)
	DUMMY_LINE2L_TO_RIGHTLOP_VOL_CTRL	= 87,
	DUMMY_PGA_L_TO_RIGHTLOP_VOL_CTRL	= 88,
	DUMMY_DAC_L1_TO_RIGHTLOP_VOL_CTRL	= 89,
	DUMMY_LINE2R_TO_RIGHTLOP_VOL_CTRL	= 90,
	DUMMY_PGA_R_TO_RIGHTLOP_VOL_CTRL	= 91,
	DUMMY_DAC_R1_TO_RIGHTLOP_VOL_CTRL	= 92,
	DUMMY_RIGHTLOP_OUTPUT_LVL_CTRL	= 93,
#define RIGHTLOP_RIGHT_LOP_M_POWER_STATUS           (1<<0)
#define RIGHTLOP_RIGHT_LOP_M_MUTE                   (1<<3)
	DUMMY_MODULE_PWR_STAT				= 94,
	DUMMY_OUTPUT_DRIVER_SHORT_CRCT_DETEC_STAT	= 95,
	DUMMY_STICKY_INTERRUPT_FLAGS		=96,
	DUMMY_REALTIME_INTERRUPT_FLAGS	= 97,
	DUMMY_GPIO1_CTRL					= 98,
	DUMMY_GPIO2_CTRL					= 99,
	DUMMY_ADDITIONAL_GPIO_CTRL_A		= 100,
	DUMMY_ADDITIONAL_GPIO_CTRL_B		= 101,
#define CODEC_CLKIN_SOURCE_PLLDIV                   (0<<0)
#define CODEC_CLKIN_SOURCE_CLKDIV                   (1<<0)
	DUMMY_CLK_GENERATION_CTRL			= 102,
#define CLKDIV_IN_USE_BCLK                          (2<<6)
#define PLLCLK_USE_BCLK                             (2<<4)
};

typedef struct dummy_context
{
	ado_mixer_t *mixer;
	HW_CONTEXT_T *hwc;
	int fd;
	int num_of_codecs;
	int tx_voices;
	int rx_voices;
	int i2c_addr1;
	int i2c_addr2;
	int i2c_addr3;

	int sel_mcasp;
#define SEL_MCASP0 0
#define SEL_MCASP2 0
#define SEL_MCASP5 1

	int left_adc_pga_gain_ctrl[3];
	int right_adc_pga_gain_ctrl[3];

	int left_dac_vol_ctrl[3];
	int right_dac_vol_ctrl[3];

	uint8_t output_routing; // for McASP2 (hp or lineout)

	uint8_t	input_mux; /* 0: mic1, 1: mic2, 2: aux*/
#define MIC1_IN_INPUT   0
#define AUX_IN_INPUT    1
#define MIC2_IN_INPUT   2

#define SWITCH_LINEOUT  0
#define SWITCH_HPOUT    1
#define DAC_TO_LR1  (0x00)
#define DAC_TO_LR3  (0x50)
#define SWITCH_HP_LINE_MASK (0xf0)

	ado_mixer_delement_t *mic1_in;
	ado_mixer_delement_t *aux_in;
}
dummy_context_t;

/*
 If MCLK is an even multiple of sampling frequency, we do not need
 to turn on internal codec PLL, and we can use 'Q' to obtain desired
 frequency:
			fs = (MCLK) / (128 * Q)

 If MCLK is not an even multiple of sampling frequency, we need to
 use internal PLL and obtain desired frequency using P,R,K=J.D using
 the following formula:

			fs = (MCLK * K * R) / (2048 * P)
			where K = J.D (J is the integer portion of K, D is the fraction)

 For example: Given MCLK=24.576MHz, Fs=44.1KHz, and assuming R=1, we have:

 P = 2 [10MHz <= (MCLK/P) <= 20MHz]

 K = (44100 * 2048 * 2)/(24576000) = 7.35
 Therefore, J = 7 [4 <= J <= 55]
			D = 3500

 Finally, the above calculation meet the final requirement:
 80MHz <= (MCLK * K * R / P) <= 110MHz


 */

#define NUM_CLOCK_ENTRIES 3
static uint32_t codec_pll_dividers[NUM_CLOCK_ENTRIES][6] = {
                             /* MCLK      Q  P  R  J  D */
	/* mclk = 11.2896 MHz */   {11289600, 0, 1, 1, 8, 0},
	/* mclk = 20.0214 MHz */   {20021400, 0, 1, 1, 4, 5110},
	/* mclk = 45.1584 MHz */   {45158400, 8, 0, 0, 0, 0},
};

#define MCLK_INDEX  0
#define PLL_Q_INDEX 1
#define PLL_P_INDEX 2
#define PLL_R_INDEX 3
#define PLL_J_INDEX 4
#define PLL_D_INDEX 5

static int32_t pcm_devices[1] = {
	0
};

static snd_mixer_voice_t stereo_voices[2] = {
	{SND_MIXER_VOICE_LEFT, 0},
	{SND_MIXER_VOICE_RIGHT, 0}
};

static struct snd_mixer_element_volume1_range output_range[1] = {
	{0, 127, -6350, 0}
};

static struct snd_mixer_element_volume1_range input_range[1] = {
	{0, 119, 0, 5950}
};

static uint8_t dummy_rd(MIXER_CONTEXT_T * dummy, uint8_t regaddr, uint8_t slave_addr)
{
	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} dummy_rd_data;

	/*Read the Registers Current Value */
	dummy_rd_data.buf[0] = regaddr;
	dummy_rd_data.hdr.send_len = 1;
	dummy_rd_data.hdr.recv_len = 1;

	dummy_rd_data.hdr.slave.addr = slave_addr;
	dummy_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	dummy_rd_data.hdr.stop = 1;

	if (devctl(dummy->fd, DCMD_I2C_SENDRECV, &dummy_rd_data, sizeof (dummy_rd_data), NULL))
	{
		ado_error("Failed to write to codec: %s\n", strerror(errno));
	}

	ado_debug(DB_LVL_MIXER, "DUMMY Codec read reg=%x data=%x (%d) ,slave_addr = %x", regaddr, dummy_rd_data.buf[0],
			  dummy_rd_data.buf[0], slave_addr);
	return dummy_rd_data.buf[0];
}

#if 0	//only for aic3106
static void dummy_wr(MIXER_CONTEXT_T * dummy, uint8_t regaddr, uint8_t data, uint8_t slave_addr)
{
	struct send_recv
	{
		i2c_sendrecv_t hdr;
		uint8_t buf[2];
	} dummy_rd_data;

	/*Read the Registers Current Value */
	dummy_rd_data.buf[0] = regaddr;
	dummy_rd_data.buf[1] = data;
	dummy_rd_data.hdr.send_len = 2;
	dummy_rd_data.hdr.recv_len = 1;

	dummy_rd_data.hdr.slave.addr = slave_addr;
	dummy_rd_data.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	dummy_rd_data.hdr.stop = 1;

	if (devctl(dummy->fd, DCMD_I2C_SENDRECV, &dummy_rd_data, sizeof (dummy_rd_data), NULL))
	{
		ado_error("Failed to write to codec: %s\n", strerror(errno));
	}

	ado_debug(DB_LVL_MIXER, "DUMMY Codec write reg=%x data=%x (%d), slave_addr = %x", regaddr, data, data, slave_addr);
	/* Wait and then do dumby reads to ensure the codec write is applied */
	delay(5);
	dummy_rd(dummy, regaddr, slave_addr);
	dummy_rd(dummy, regaddr, slave_addr);
	return;
}
#else
static void dummy_wr(MIXER_CONTEXT_T * dummy, uint8_t regaddr, uint8_t data, uint8_t slave_addr)
{
	ado_debug(DB_LVL_MIXER, "dummy dummy_wr");
	return;
}
#endif

static int32_t
dummy_output_vol_control(MIXER_CONTEXT_T * dummy, ado_mixer_delement_t * element, uint8_t set,
						   uint32_t * vol, void *instance_data)
{
	int32_t altered = 0;

	if(set)
	{
		altered = 	(vol[0] != (127 - (dummy->left_dac_vol_ctrl[0] & 0x7f))) 	||
		(vol[1] != (127 - (dummy->right_dac_vol_ctrl[0] & 0x7f)));
		if( (dummy->num_of_codecs == 3) && !altered)
		{
			altered = 	(vol[3] != (127 - (dummy->left_dac_vol_ctrl[1] & 0x7f))) 	||
			(vol[4] != (127 - (dummy->right_dac_vol_ctrl[1] & 0x7f))) 	||
			(vol[2] != (127 - (dummy->left_dac_vol_ctrl[2] & 0x7f))) 	||
			(vol[5] != (127 - (dummy->right_dac_vol_ctrl[2] & 0x7f)));
		}

		// vol[0] and vol[1] for dummy #1
		dummy->left_dac_vol_ctrl[0] &= 0x80;
		dummy->left_dac_vol_ctrl[0] |= 127 - (vol[0] & 0x7f);
		dummy->right_dac_vol_ctrl[0] &= 0x80;
		dummy->right_dac_vol_ctrl[0] |= 127 - (vol[1] & 0x7f);
		dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[0], dummy->i2c_addr1);
		dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[0], dummy->i2c_addr1);

		if(dummy->num_of_codecs == 3)
		{
			// vol[3] and vol[4] for dummy #2
			dummy->left_dac_vol_ctrl[1] &= 0x80;
			dummy->left_dac_vol_ctrl[1] |= 127 - (vol[3] & 0x7f);
			dummy->right_dac_vol_ctrl[1] &= 0x80;
			dummy->right_dac_vol_ctrl[1] |= 127 - (vol[4] & 0x7f);
			dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[1], dummy->i2c_addr2);
			dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[1], dummy->i2c_addr2);

			// vol[2] and vol[5] for dummy #3
			dummy->left_dac_vol_ctrl[2] &= 0x80;
			dummy->left_dac_vol_ctrl[2] |= 127 - (vol[2] & 0x7f);
			dummy->right_dac_vol_ctrl[2] &= 0x80;
			dummy->right_dac_vol_ctrl[2] |= 127 - (vol[5] & 0x7f);
			dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[2], dummy->i2c_addr3);
			dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[2], dummy->i2c_addr3);
		}
	}
	else
	{
		vol[0] = 127 - (dummy->left_dac_vol_ctrl[0] & 0x7f);
		vol[1] = 127 - (dummy->right_dac_vol_ctrl[0] & 0x7f);
		if(dummy->num_of_codecs == 3)
		{
			vol[3] = 127 - (dummy->left_dac_vol_ctrl[1] & 0x7f);
			vol[4] = 127 - (dummy->right_dac_vol_ctrl[1] & 0x7f);
			vol[2] = 127 - (dummy->left_dac_vol_ctrl[2] & 0x7f);
			vol[5] = 127 - (dummy->right_dac_vol_ctrl[2] & 0x7f);
		}
	}
	return (altered);
}

static int32_t
dummy_output_mute_control(MIXER_CONTEXT_T * dummy, ado_mixer_delement_t * element, uint8_t set,
							uint32_t * val, void *instance_data)
{
	int32_t altered = 0;
	if (set)
	{

		if(dummy->num_of_codecs == 1)
		{
			altered = 	val[0] != ( ((dummy->left_dac_vol_ctrl[0] & 0x80)>>7) |
								   ((dummy->right_dac_vol_ctrl[0] & 0x80)>>6) );
		}
		else if(dummy->num_of_codecs == 3)
		{
			altered = 	val[0] != ( ((dummy->left_dac_vol_ctrl[0] & 0x80)>>7) |
								   ((dummy->right_dac_vol_ctrl[0] & 0x80)>>6)|
								   ((dummy->left_dac_vol_ctrl[1] & 0x80)>>4) |
								   ((dummy->right_dac_vol_ctrl[1] & 0x80)>>3)|
								   ((dummy->left_dac_vol_ctrl[2] & 0x80)>>5) |
								   ((dummy->right_dac_vol_ctrl[2] & 0x80)>>2) );
		}
		// bits 0 and 1 for dummy #1
		dummy->left_dac_vol_ctrl[0] &= 0x7f;
		dummy->left_dac_vol_ctrl[0] |= (val[0] & SND_MIXER_CHN_MASK_FRONT_LEFT)<<7;
		dummy->right_dac_vol_ctrl[0] &= 0x7f;
		dummy->right_dac_vol_ctrl[0] |= (val[0] & SND_MIXER_CHN_MASK_FRONT_RIGHT)<<6;
		dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[0], dummy->i2c_addr1);
		dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[0], dummy->i2c_addr1);

		if(dummy->num_of_codecs == 3)
		{
			// bits 3 and 4 for dummy #2
			dummy->left_dac_vol_ctrl[1] &= 0x7f;
			dummy->left_dac_vol_ctrl[1] |= (val[0] & SND_MIXER_CHN_MASK_REAR_LEFT)<<4;
			dummy->right_dac_vol_ctrl[1] &= 0x7f;
			dummy->right_dac_vol_ctrl[1] |= (val[0] & SND_MIXER_CHN_MASK_REAR_RIGHT)<<3;
			dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[1], dummy->i2c_addr2);
			dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[1], dummy->i2c_addr2);

			// bits 2 and 5 for dummy #3
			dummy->left_dac_vol_ctrl[2] &= 0x7f;
			dummy->left_dac_vol_ctrl[2] |= (val[0] & SND_MIXER_CHN_MASK_FRONT_CENTER)<<5;
			dummy->right_dac_vol_ctrl[2] &= 0x7f;
			dummy->right_dac_vol_ctrl[2] |= (val[0] & SND_MIXER_CHN_MASK_WOOFER)<<2;
			dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[2], dummy->i2c_addr2);
			dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[2], dummy->i2c_addr2);
		}
	}
	else
	{
		if(dummy->num_of_codecs == 1)
		{
			val[0] =  	(dummy->left_dac_vol_ctrl[0] & 0x80)>>7 	|
			(dummy->right_dac_vol_ctrl[0] & 0x80)>>6;
		}
		else
		{
			val[0] =  	(dummy->left_dac_vol_ctrl[0] & 0x80)>>7 	|
			(dummy->right_dac_vol_ctrl[0] & 0x80)>>6 |
			(dummy->left_dac_vol_ctrl[1] & 0x80)>>4 |
			(dummy->right_dac_vol_ctrl[1] & 0x80)>>3 |
			(dummy->left_dac_vol_ctrl[2] & 0x80)>>5 |
			(dummy->right_dac_vol_ctrl[2] & 0x80)>>2;
		}
	}
	return (altered);
}

static int32_t
dummy_hp_get(MIXER_CONTEXT_T * dummy, ado_dswitch_t * dswitch, snd_switch_t * cswitch, void *instance_data)
{
	cswitch->type = SND_SW_TYPE_BOOLEAN;
#if 0
	if(dummy_rd(dummy, DUMMY_DAC_OUTPUT_SWITCH_CTRL, dummy->i2c_addr1) & SWITCH_HP_LINE_MASK)
		cswitch->value.enable = SWITCH_LINEOUT;
	else
		cswitch->value.enable = SWITCH_HPOUT;
#endif
	cswitch->value.enable = SWITCH_LINEOUT;
	return (0);
}

static int32_t
dummy_hp_set(MIXER_CONTEXT_T * dummy, ado_dswitch_t * dswitch, snd_switch_t * cswitch, void *instance_data)
{
	int32_t altered = 0;

	if((cswitch->value.enable == SWITCH_LINEOUT) && (dummy->output_routing != SWITCH_LINEOUT))
	{
		// selected LINEOUT
		dummy_wr(dummy, DUMMY_DAC_OUTPUT_SWITCH_CTRL, (RIGHT_DAC_OUT_SWITCHING_CTRL | LEFT_DAC_OUT_SWITCHING_CTRL), dummy->i2c_addr1);
		dummy->output_routing = SWITCH_LINEOUT;
		altered = 1;
	}
	else if ((cswitch->value.enable == SWITCH_HPOUT) && (dummy->output_routing != SWITCH_HPOUT))
	{
		// selected HPOUT
		dummy_wr(dummy, DUMMY_DAC_OUTPUT_SWITCH_CTRL, 0x00, dummy->i2c_addr1);
		dummy->output_routing = SWITCH_HPOUT;
		altered = 1;
	}

	return altered;
}
static int32_t
dummy_mux_control(MIXER_CONTEXT_T * dummy, ado_mixer_delement_t * element, uint8_t set,ado_mixer_delement_t ** inelements, void *instance_data)
{
	int32_t altered = 0;
	uint32_t tmp;

	if (set)
	{
		if(dummy->hwc->cap_aif.active)
			return (EBUSY);						 /* Cannot switch while capture is active */

		if(inelements[0] == dummy->mic1_in)
			tmp = MIC1_IN_INPUT;
		else
			tmp = AUX_IN_INPUT;
		altered = dummy->input_mux != tmp;

		dummy->input_mux = tmp;

		if(MIC1_IN_INPUT == dummy->input_mux) // Mic
		{
			// disconnect Line1L and Line1R from Left and Right ADC
			dummy_wr(dummy, DUMMY_LINE1L_TO_LEFT_ADC_CTRL,(LINE1L_LEFT_ADC_CHANNEL_POWER_CTRL | LINE1L_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX) , dummy->i2c_addr1);
			dummy_wr(dummy, DUMMY_LINE1R_TO_RIGHT_ADC_CTRL, (LINE1R__RIGHT_ADC_CHANNEL_POWER_CTRL | LINE1R__INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX), dummy->i2c_addr1);

			// connect MIC3L to Left ADC and MIC3R to right ADC
			dummy_wr(dummy, DUMMY_MIC3LR_TO_LEFT_ADC_CTRL, MIC3R_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX, dummy->i2c_addr1);
			dummy_wr(dummy, DUMMY_MIC3LR_TO_RIGHT_ADC_CTRL, MIC3L_INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX, dummy->i2c_addr1);
		}
		else // Aux input
		{
			// disconnect MIC3L to Left ADC and MIC3R to right ADC
			dummy_wr(dummy, DUMMY_MIC3LR_TO_LEFT_ADC_CTRL,(MIC3R_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX | MIC3L_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX), dummy->i2c_addr1);
			dummy_wr(dummy, DUMMY_MIC3LR_TO_RIGHT_ADC_CTRL,(MIC3R_INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX | MIC3L_INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX), dummy->i2c_addr1);

			// connect Line1L and Line1R from Left and Right ADC
			dummy_wr(dummy, DUMMY_LINE1L_TO_LEFT_ADC_CTRL, LINE1L_LEFT_ADC_CHANNEL_POWER_CTRL, dummy->i2c_addr1);
			dummy_wr(dummy, DUMMY_LINE1R_TO_RIGHT_ADC_CTRL, LINE1R__RIGHT_ADC_CHANNEL_POWER_CTRL, dummy->i2c_addr1);
		}
	}
	else
	{
		if(MIC1_IN_INPUT == dummy->input_mux)
			inelements[0] = inelements[1] = dummy->mic1_in;
		else
			inelements[0] = inelements[1] =dummy->aux_in;
	}
	return (altered);
}


static int32_t
dummy_input_vol_control(MIXER_CONTEXT_T * dummy, ado_mixer_delement_t * element, uint8_t set,uint32_t * vol, void *instance_data)
{
	int32_t altered = 0;

	if(set)
	{
		altered = ((vol[0] != (dummy->left_adc_pga_gain_ctrl[0] & 0x7f)) ||
				   (vol[1] != (dummy->right_adc_pga_gain_ctrl[0] & 0x7f)));

		dummy->left_adc_pga_gain_ctrl[0] &= 0x80;
		dummy->left_adc_pga_gain_ctrl[0] |= (vol[0] & 0x7f);
		dummy->right_adc_pga_gain_ctrl[0] &= 0x80;
		dummy->right_adc_pga_gain_ctrl[0] |= (vol[1] & 0x7f);

		dummy_wr(dummy, DUMMY_LEFT_ADC_PGA_GAIN_CTRL, dummy->left_adc_pga_gain_ctrl[0], dummy->i2c_addr1);
		dummy_wr(dummy, DUMMY_RIGHT_ADC_PGA_GAIN_CTRL, dummy->right_adc_pga_gain_ctrl[0], dummy->i2c_addr1);
	}
	else
	{
		vol[0] = dummy->left_adc_pga_gain_ctrl[0] & 0x7f;
		vol[1] = dummy->right_adc_pga_gain_ctrl[0] & 0x7f;
	}

	return (altered);
}

static int32_t
dummy_input_mute_control(MIXER_CONTEXT_T * dummy, ado_mixer_delement_t * element, uint8_t set,
						   uint32_t * val, void *instance_data)
{
	int32_t altered = 0;

	if(set)
	{
		altered = val[0] != (  ((dummy->left_adc_pga_gain_ctrl[0] & 0x80)>>7) |
							 ((dummy->right_adc_pga_gain_ctrl[0] & 0x80)>>6) );

		dummy->left_adc_pga_gain_ctrl[0] &= 0x7f;
		dummy->left_adc_pga_gain_ctrl[0] |= (val[0] & SND_MIXER_CHN_MASK_FRONT_LEFT)<<7;
		dummy->right_adc_pga_gain_ctrl[0] &= 0x7f;
		dummy->right_adc_pga_gain_ctrl[0] |= (val[0] & SND_MIXER_CHN_MASK_FRONT_RIGHT)<<6;

		dummy_wr(dummy, DUMMY_LEFT_ADC_PGA_GAIN_CTRL, dummy->left_adc_pga_gain_ctrl[0], dummy->i2c_addr1);
		dummy_wr(dummy, DUMMY_RIGHT_ADC_PGA_GAIN_CTRL, dummy->right_adc_pga_gain_ctrl[0], dummy->i2c_addr1);
	}
	else
	{
		val[0] = (dummy->left_adc_pga_gain_ctrl[0] & 0x80)>>7 |
		(dummy->right_adc_pga_gain_ctrl[0] & 0x80)>>6;
	}
	return (altered);
}

/* Required for compatibility with Audioman
 * This switch is called by audio manager to ask deva to send the current HW status, i.e., whether headset is connected
 */
static int32_t
dummy_mixer_audioman_refresh_set(MIXER_CONTEXT_T * hw_ctx, ado_dswitch_t * dswitch, snd_switch_t * cswitch,
								void *instance_data)
{
	return (EOK);
}

static int32_t
dummy_mixer_audioman_refresh_get(MIXER_CONTEXT_T * hw_ctx, ado_dswitch_t * dswitch, snd_switch_t * cswitch,
								void *instance_data)
{
	/* Always return disabled as this switch does not maintain state */
	cswitch->type = SND_SW_TYPE_BOOLEAN;
	cswitch->value.enable = 0;
	return 0;
}

static int32_t build_dummy_mixer(MIXER_CONTEXT_T * dummy, ado_mixer_t * mixer)
{
	int error = 0;

	ado_mixer_delement_t *play_vol;
	ado_mixer_delement_t *play_mute;
	ado_mixer_delement_t *play_out;
	ado_mixer_dgroup_t *play_grp;

	ado_mixer_delement_t *input_mux = NULL;

	ado_mixer_delement_t *input_vol = NULL;
	ado_mixer_delement_t *input_mute = NULL;

	ado_mixer_dgroup_t *linein_grp;
	ado_mixer_dgroup_t *micin_grp;
	ado_mixer_dgroup_t *igain_grp;

	ado_mixer_delement_t *pcm_out, *pcm_in = NULL;

//	if(SEL_MCASP2 == dummy->sel_mcasp)
	if(SEL_MCASP0 == dummy->sel_mcasp)
	{
		/* ################ */
		/* the OUTPUT GROUP */
		/* ################ */

		// pcm out
		if (!error && (pcm_out = ado_mixer_element_pcm1(mixer, SND_MIXER_ELEMENT_PLAYBACK,SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
			error++;

		// volume
		if (!error && (play_vol = ado_mixer_element_volume1(mixer, "PCM Volume", 2,output_range, dummy_output_vol_control, (void *) NULL, NULL)) == NULL)
			error++;

		// route pcm_out to volume
		if (!error && ado_mixer_element_route_add(mixer, pcm_out, play_vol) != 0)
			error++;

		// mute
		if (!error && (play_mute = ado_mixer_element_sw1(mixer, "HP Mute", 2,dummy_output_mute_control, (void *) NULL, NULL)) == NULL)
			error++;

		// route volume to mute
		if (!error && ado_mixer_element_route_add(mixer, play_vol, play_mute) != 0)
			error++;

		// ouput
		if (!error && (play_out = ado_mixer_element_io(mixer, "PCM_OUT", SND_MIXER_ETYPE_OUTPUT, 0, 2, stereo_voices)) == NULL)
			error++;
		if (!error && ado_mixer_element_route_add(mixer, play_mute, play_out) != 0)
			error++;

		if (!error && (play_grp = ado_mixer_playback_group_create(mixer, SND_MIXER_PCM_OUT,SND_MIXER_CHN_MASK_STEREO, play_vol, play_mute)) == NULL)
			error++;

		if(!error &&
		   ado_mixer_switch_new(mixer, "Headphone Select", SND_SW_TYPE_BOOLEAN, 0, dummy_hp_get, dummy_hp_set, NULL, NULL) == NULL)
			error++;

		/* ################ */
		/* the INPUT GROUP */
		/* ################ */

		// Input mux component
		if(!error && (input_mux = ado_mixer_element_mux1(mixer, "Input Mux", 0, 2, dummy_mux_control, NULL, NULL)) == NULL)
			error++;

		// Input gain group
		if(!error && (input_vol = ado_mixer_element_volume1(mixer, "Input Volume", 2,input_range, dummy_input_vol_control, (void*) NULL, NULL)) == NULL)
			error++;

		// route mux output to volume element
		if(!error && ado_mixer_element_route_add(mixer, input_mux, input_vol) != 0)
			error++;

		if(!error && (input_mute = ado_mixer_element_sw1(mixer, "Input Mute", 2,dummy_input_mute_control, NULL, NULL)) == NULL)
			error++;

		// route vol to mute
		if(!error && ado_mixer_element_route_add(mixer, input_vol, input_mute) != 0)
			error++;

		// create input gain group
		if(!error && (igain_grp = ado_mixer_capture_group_create(mixer, SND_MIXER_GRP_IGAIN, SND_MIXER_CHN_MASK_STEREO,input_vol, input_mute, NULL, NULL)) == NULL)
			error++;

		// Linein group
		if(!error && (dummy->aux_in = ado_mixer_element_io(mixer, "LINEIN", SND_MIXER_ETYPE_INPUT, 0, 2, stereo_voices)) == NULL)
			error++;

		// route linein io to mux input
		if(!error && ado_mixer_element_route_add(mixer, dummy->aux_in, input_mux) != 0)
			error++;

		// create linein group
		if(!error && (linein_grp = ado_mixer_capture_group_create(mixer, SND_MIXER_LINE_IN,SND_MIXER_CHN_MASK_STEREO, NULL, NULL, input_mux, dummy->aux_in)) == NULL)
			error++;

		// Mic group
		if(!error && (dummy->mic1_in = ado_mixer_element_io(mixer, "MICIN", SND_MIXER_ETYPE_INPUT, 0, 2, stereo_voices)) == NULL)
			error++;

		// route mic io to mux input
		if(!error && ado_mixer_element_route_add(mixer, dummy->mic1_in, input_mux) != 0)
			error++;

		// create mic group
		if(!error && (micin_grp = ado_mixer_capture_group_create(mixer, SND_MIXER_MIC_IN,SND_MIXER_CHN_MASK_STEREO, NULL, NULL, input_mux, dummy->mic1_in)) == NULL)
			error++;

		// PCM component
		if(!error && (pcm_in = ado_mixer_element_pcm1(mixer, SND_MIXER_ELEMENT_CAPTURE, SND_MIXER_ETYPE_CAPTURE1, 1, &pcm_devices[0])) == NULL)
			error++;

		// route mute to pcm
		if(!error && ado_mixer_element_route_add(mixer, input_mute, pcm_in) != 0)
			error++;

		/* ######## */
		/* Switches */
		/* ######## */

		// Audioman support
		if (!error && ado_mixer_switch_new(mixer, "Audioman Refresh", SND_SW_TYPE_BOOLEAN, 0, dummy_mixer_audioman_refresh_get,
											dummy_mixer_audioman_refresh_set, NULL, NULL) == NULL)
			error++;

		return (!error ? 0 : -1);
	}
	return (0);
}


static ado_mixer_reset_t dummy_reset;
static int dummy_reset(MIXER_CONTEXT_T * dummy)
{
	int i, j;
	uint8_t regVal;
	int ind;

	for(j=0, i = dummy->i2c_addr1; i <= (dummy->i2c_addr1 + dummy->num_of_codecs - 1); i++, j++)
	{
		dummy_wr(dummy, DUMMY_PAGESELECTOR, PAGE_SELECT, i);		// select Page 0
		dummy_wr(dummy, DUMMY_SFT_RESET, SFT_RESET, i); 		// rest codec

		if( MASTER == dummy->hwc->clk_mode)
		{
			// BCLK and WCLK as input, slave mode, Place DOUT in high impedance when valid data is not being sent
			dummy_wr(dummy, DUMMY_AUDIO_SER_DATA_INT_CTRL_A, REG_A_SERIAL_DOUT_3STATE_CTRL, i);

			/*
			* fS(ref) = (PLLCLK_IN × K × R) / (2048 × P), where
			* P = 1, 2, 3,…, 8
			* R = 1, 2, …, 16
			* K = J.D
			* J = 1, 2, 3, …, 63
			* D = 0000, 0001, 0002, 0003, …, 9998, 9999
			*
			* CLKDIV_IN uses BCLK; PLLCLK_IN uses BCLK
			* Here we assume the BCLK is 2.8224MHz
			* 44.1KHz = (PLLCLK_IN x K x R) / (2048 x P) = (2.8224 x 32 x 1) / (2048 x 1)
			*/
			// CODEC_CLKIN Source Selection: PLLDIV_OUT
			dummy_wr(dummy, DUMMY_ADDITIONAL_GPIO_CTRL_B, CODEC_CLKIN_SOURCE_PLLDIV, i);
			dummy_wr(dummy, DUMMY_CLK_GENERATION_CTRL, (CLKDIV_IN_USE_BCLK | PLLCLK_USE_BCLK), i);
			dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_B, 32, i);  // J=32
			dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_C, 0 , i);  // 00: MSB of D
			dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_D, 0, i);   // 00: LSB of D
			dummy_wr(dummy, DUMMY_AUDIO_CODEC_OVERFLOW_FLAG, 1, i);   // R=1
			dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_A, (REG_A_PLL_CTRL | 1), i);  // Enable PLL, P = 1
		}
		else
		{ // McASP is SLAVE -> Codec is Master

			// BCLK and WCLK as output, master mode, Place DOUT in high impedance when valid data is not being sent
			dummy_wr(dummy, DUMMY_AUDIO_SER_DATA_INT_CTRL_A,(REG_A_BCLK_DIR_CTRL | REG_A_WCLK_DIR_CTRL | REG_A_SERIAL_DOUT_3STATE_CTRL), i);

			for(ind=0; ind < NUM_CLOCK_ENTRIES; ind++)
			{
				if(dummy->hwc->mclk == codec_pll_dividers[ind][0])
				{
					// found the correct mclk

					if(0 == codec_pll_dividers[ind][PLL_Q_INDEX])
					{
						//Q=0 means we need to enable PLL and use the values for P,R,J and D
						dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_A,(REG_A_PLL_CTRL | REG_A_PLL_P(codec_pll_dividers[ind][PLL_P_INDEX])), i);		//Enable PLL, P=2
						dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_B, REG_B_PLL_J(codec_pll_dividers[ind][PLL_J_INDEX]), i);						// J=7
						dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_C, REG_C_PLL_D_MOST_BIT(codec_pll_dividers[ind][PLL_D_INDEX]) , i);			// D=3500
						dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_D, REG_D_PLL_D_LEAST_BIT(codec_pll_dividers[ind][PLL_D_INDEX]) , i);			// D=3500
						dummy_wr(dummy, DUMMY_AUDIO_CODEC_OVERFLOW_FLAG, OVERFLOW_FLAG_PLL_R(codec_pll_dividers[ind][PLL_R_INDEX]) , i);	// R=1

					}
					else
					{
						// A non-zero value for Q means the do not need to use the PLL
						dummy_wr(dummy, DUMMY_PLL_PROGRAM_REG_A,REG_A_PLL_Q(codec_pll_dividers[ind][PLL_Q_INDEX]), i);		// Set Q value
					}
					break;

				}
			}
			if(ind == NUM_CLOCK_ENTRIES)
			{
				ado_error("dummy: invalid MCLK value (%d)", dummy->hwc->mclk);
				return(-1);
			}
		}


		// use DSP mode for num_of_codecs > 1 and Left Justified for num_of_codecs =1
		// 16-bit word
		if(dummy->num_of_codecs > 1)
			dummy_wr(dummy, DUMMY_AUDIO_SER_DATA_INT_CTRL_B, (REG_B_BCLK_RATE_CTRL | REG_B_AUDIO_SERIAL_DATA_MODE), i);
		else
			dummy_wr(dummy, DUMMY_AUDIO_SER_DATA_INT_CTRL_B, (0x00), i);
		/**
		 * NOTE: offset are still not behaving properly for McASP5
		 * offset: 0 for dummy#1, 32 for dummy#2 and 64 for dummy#3
		 */
		if(dummy->num_of_codecs > 1)
			dummy_wr(dummy, DUMMY_AUDIO_SER_DATA_INT_CTRL_C, (((j)<<6)+1), i);

		////////////
		// INPUTS
		///////////

		//if(SEL_MCASP2 == dummy->sel_mcasp)
		if(SEL_MCASP0 == dummy->sel_mcasp)
		{
			// connect MIC3L to Left ADC and MIC3R to right ADC
			dummy_wr(dummy, DUMMY_MIC3LR_TO_LEFT_ADC_CTRL, MIC3R_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX, i);
			dummy_wr(dummy, DUMMY_MIC3LR_TO_RIGHT_ADC_CTRL, MIC3L_INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX, i);

			// disconnect Line1L and Line1R from Left and Right ADC (can connect through mux)
			dummy_wr(dummy, DUMMY_LINE1L_TO_LEFT_ADC_CTRL, LINE1L_INPUT_LEVEL_CTRL_LEFT_ADC_PGA_MIX, i);
			dummy_wr(dummy, DUMMY_LINE1R_TO_RIGHT_ADC_CTRL, LINE1R__INPUT_LEVEL_CTRL_RIGHT_ADC_PGA_MIX, i);

		}
		else if(SEL_MCASP5 == dummy->sel_mcasp)
		{

			// LINE1L to Left ADC: fully differential & volume (0db)
			dummy_wr(dummy, DUMMY_LINE1L_TO_LEFT_ADC_CTRL, (1<<7) | (0<<2),  i);

			// LINE1R to Right ADC: fully differential & volume (0db), power-on
			dummy_wr(dummy, DUMMY_LINE1R_TO_RIGHT_ADC_CTRL, (1<<7) | (0<<2),  i);
		}

		// turn on microphone biasing
		dummy_wr(dummy, DUMMY_MICBIAS_CTRL, MICBIAS_AVDD, i);

		// set Left and Right ADC to (29.5 db (50%))
		dummy->left_adc_pga_gain_ctrl[j] = 0x3b;
		dummy->right_adc_pga_gain_ctrl[j] = 0x3b;
		dummy_wr(dummy, DUMMY_LEFT_ADC_PGA_GAIN_CTRL, dummy->left_adc_pga_gain_ctrl[j], i);
		dummy_wr(dummy, DUMMY_RIGHT_ADC_PGA_GAIN_CTRL, dummy->right_adc_pga_gain_ctrl[j], i);

#if 0
		// Turn on Left and Right ADC
		regVal = dummy_rd(dummy, DUMMY_LINE1L_TO_LEFT_ADC_CTRL, i);
		dummy_wr(dummy, DUMMY_LINE1L_TO_LEFT_ADC_CTRL, regVal | (1<<2), i);
		regVal = dummy_rd(dummy, DUMMY_LINE1R_TO_RIGHT_ADC_CTRL, i);
		dummy_wr(dummy, DUMMY_LINE1R_TO_RIGHT_ADC_CTRL, regVal | (1<<2), i);
#endif

		////////////
		// OUTPUTS
		///////////

		// set output vol to 100% (0db)
		dummy->left_dac_vol_ctrl[j] = 0x0;
		dummy->right_dac_vol_ctrl[j] = 0x0;
		dummy_wr(dummy, DUMMY_LEFT_DAC_DIG_VOL_CTRL, dummy->left_dac_vol_ctrl[j], i);
		dummy_wr(dummy, DUMMY_RIGHT_DAC_DIG_VOL_CTRL, dummy->right_dac_vol_ctrl[j], i);

		// Select DAC output to DAC_L3/R3 path (direct from DAC to LOP)
		dummy_wr(dummy, DUMMY_DAC_OUTPUT_SWITCH_CTRL, (RIGHT_DAC_OUT_SWITCHING_CTRL | LEFT_DAC_OUT_SWITCHING_CTRL), i);

		// LEFT/LOP/M Vol Ctrl (0db) + not muted + powered on
		dummy_wr(dummy, DUMMY_LEFTLOP_OUTPUT_LVL_CTRL, (LEFTLOP_LEFT_LOP_M_POWER_STATUS | LEFTLOP_LEFT_LOP_M_MUTE), i);
		dummy_wr(dummy, DUMMY_RIGHTLOP_OUTPUT_LVL_CTRL, (RIGHTLOP_RIGHT_LOP_M_POWER_STATUS | RIGHTLOP_RIGHT_LOP_M_MUTE), i);

		//if(SEL_MCASP2 == dummy->sel_mcasp)
		if(SEL_MCASP0 == dummy->sel_mcasp)
		{
			// select DAC_L1 path to HPLOUT and DAC_R1 to HPROUT
			dummy_wr(dummy, DUMMY_DAC_L1_TO_HPLOUT_VOL_CTRL, HPLOUT_DAC_L1_OUT_ROUTING_CTR, i);
			dummy_wr(dummy, DUMMY_DAC_R1_TO_HPROUT_VOL_CTRL, HPROUT_DAC_R1_OUT_ROUTING_CTRL, i);

			// HPL/ROUT Vol Ctrl (0db) + not muted + powered on
			dummy_wr(dummy, DUMMY_HPLOUT_OUTPUT_LVL_CTRL, (HPLOUT_POWER_CTRL | HPLOUT_MUTE), i);
			dummy_wr(dummy, DUMMY_HPROUT_OUTPUT_LVL_CTRL, (HPROUT_POWER_CTRL | HPROUT_MUTE), i);
		}

		// Power up Left and Right DAC
		dummy_wr(dummy, DUMMY_DAC_POWER_DRIVER_CTRL, (LEFT_DAC_POWER_CTRL | RIGHT_DAC_POWER_CTRL), i);

	}
	return (0);
}


static ado_mixer_destroy_t dummy_destroy;
static int dummy_destroy(MIXER_CONTEXT_T * dummy)
{
	int i;
	ado_debug(DB_LVL_MIXER, "destroying DUMMY Codec");

	for(i = dummy->i2c_addr1; i <= (dummy->i2c_addr1 + dummy->num_of_codecs - 1); i++)
		dummy_wr(dummy, DUMMY_SFT_RESET, SFT_RESET, i);
	close(dummy->fd);
	ado_free(dummy);

	return (0);
}

/**
 * This function is responsible for configuring the dummy chip's
 * sample rates
 */
int
dummy_set_sample_rate(MIXER_CONTEXT_T *dummy)
{
	uint32_t	fdiv;
	int i;
	fdiv = 0;

	for(i = dummy->i2c_addr1; i <= (dummy->i2c_addr1 + dummy->num_of_codecs - 1); i++)
	{
		dummy_wr(dummy, DUMMY_CODEC_SAMPLE_RATE_SELEC, fdiv,i);
		// Left and Right DAC datapath control + sampling frequency
		dummy_wr(dummy, DUMMY_CODEC_DATAPATH_SETUP,(RIGTH_DAC_DATAPATH_CTRL | LEFT_DAC_DATAPATH_CTRL |FS_SET), i); //44.1 kHz
	}
	return 0;
}

int
codec_mixer(ado_card_t * card, HW_CONTEXT_T * hwc)
{
	dummy_context_t *dummy;
	int32_t status;

	ado_debug(DB_LVL_MIXER, "initializing DUMMYA Codec");

	if ((dummy = (dummy_context_t *) ado_calloc(1, sizeof (dummy_context_t))) == NULL)
	{
		ado_error("dummy: no memory %s", strerror(errno));
		return (-1);
	}
	if ((status = ado_mixer_create(card, "dummy", &hwc->mixer, dummy)) != EOK)
	{
		ado_free(dummy);
		return (status);
	}
	dummy->mixer = hwc->mixer;
	dummy->hwc = hwc;
	dummy->output_routing = SWITCH_LINEOUT; // valid for McASP2, ignored otherwise
	dummy->input_mux = MIC1_IN_INPUT;


	// determine which mcasp is being used
	if(hwc->mcasp_baseaddr == MCASP0_BASEADDR)
		dummy->sel_mcasp = SEL_MCASP0;
	else if(hwc->mcasp_baseaddr == MCASP2_BASEADDR)
		dummy->sel_mcasp = SEL_MCASP2;
	else if(hwc->mcasp_baseaddr == MCASP5_BASEADDR)
		dummy->sel_mcasp = SEL_MCASP5;

	if(hwc->i2c_dev == 0xff)
	{
		ado_debug(DB_LVL_DRIVER, "No i2c device selected (will assume Mcasp1 --> i2c2)");
		if(SEL_MCASP0 == dummy->sel_mcasp)
			//hwc->i2c_dev = 1;//just for test
			hwc->i2c_dev = 2;
		else if(SEL_MCASP2 == dummy->sel_mcasp)
			hwc->i2c_dev = 0;
		else if(SEL_MCASP5 == dummy->sel_mcasp)
			hwc->i2c_dev = 2;
	}

	switch(hwc->i2c_dev)
	{
		case 0:
			if ((dummy->fd = open("/dev/i2c0", O_RDWR)) < 0)
			{
				ado_error("dummy: could not open i2c device %s", strerror(errno));
				ado_free(dummy);
				return (-1);
			}
			break;
		case 1:
			if ((dummy->fd = open("/dev/i2c1", O_RDWR)) < 0)
			{
				ado_error("dummy: could not open i2c device %s", strerror(errno));
				ado_free(dummy);
				return (-1);
			}
			break;
		case 2:
			if ((dummy->fd = open("/dev/i2c2", O_RDWR)) < 0)
			{
				ado_error("dummy: could not open i2c device %s", strerror(errno));
				ado_free(dummy);
				return (-1);
			}
			break;
		default:
			break;
	}

	dummy->tx_voices = hwc->play_strm.voices;
	dummy->rx_voices = hwc->cap_aif.cap_strm[0].voices;
	dummy->num_of_codecs = dummy->tx_voices>>1; // each codes supports two channels

	dummy->i2c_addr1 = hwc->codec_i2c_addr;
	dummy->i2c_addr2 = dummy->i2c_addr1 + 1; // will be ignored for McASP2
	dummy->i2c_addr3 = dummy->i2c_addr2 + 1; // will be ingnored for McASP2

	// reset codec(s)
	if(dummy_reset(dummy) == -1)
    {
		ado_error("dummy: could not initialize codec");
		close(dummy->fd);
		ado_free(dummy);
		return (-1);
    }

	// set sample rate
	dummy_set_sample_rate(dummy);

	if (build_dummy_mixer(dummy, dummy->mixer))
	{
		close(dummy->fd);
		ado_free(dummy);
		return (-1);
	}

	ado_mixer_set_reset_func(dummy->mixer, dummy_reset);
	ado_mixer_set_destroy_func(dummy->mixer, dummy_destroy);

	/* setup mixer controls for pcm  */
	ado_pcm_chn_mixer(hwc->pcm, ADO_PCM_CHANNEL_PLAYBACK,
					  hwc->mixer, ado_mixer_find_element(hwc->mixer, SND_MIXER_ETYPE_PLAYBACK1, SND_MIXER_ELEMENT_PLAYBACK, 0),
					  ado_mixer_find_group(hwc->mixer, SND_MIXER_PCM_OUT, 0));

	ado_pcm_chn_mixer(hwc->cap_aif.cap_strm[0].pcm, ADO_PCM_CHANNEL_CAPTURE,
					  hwc->mixer, ado_mixer_find_element(hwc->mixer, SND_MIXER_ETYPE_CAPTURE1, SND_MIXER_ELEMENT_CAPTURE, 0),
					  ado_mixer_find_group(hwc->mixer, SND_MIXER_GRP_IGAIN, 0));

	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/nto/arm/dll.le.j6_dummy.v7/dummy.c $ $Rev: 759887 $")
#endif
