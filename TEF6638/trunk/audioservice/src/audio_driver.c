#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <hw/i2c.h>
//#include <linux/i2c.h>

#include "audio_hero.h"

extern int audio_scaler_ctl ( const audio_ctl_parm* ctl, int gain );
extern int audio_mute_ctl ( const audio_ctl_parm* ctl, int is_unmute, int mute_time );
extern int audio_filter_ctl ( const audio_ctl_parm* ctl, audio_filter_parm* filter_parm );
extern int audio_chime_init ( void );
extern int audio_chime_generate ( audio_chime_type* chime, int loop );
extern int audio_chime_tone ( int tone );

int audio_config_mixer_ctls_with_output ( audio_physical_chnl_info* chnl, audio_physical_output* output );

//#define AUDIO_DEV                     "/dev/i2c1"
//#define AUDIO_ADDR                    0x60
#define I2C_DEV_BUS   "/dev/i2c2"
#define I2C_DEV_ADR   0x60
#define I2C_DEV_SPEED 100000
#define MAX_PACKET_SIZE 255
typedef struct {
  i2c_send_t head;
  char buf[MAX_PACKET_SIZE];
} audio_send_t;

typedef struct {
  i2c_sendrecv_t head;
  char buf[MAX_PACKET_SIZE];
} audio_sendrecv_t;

#define AUDIO_AUDIO_CFG_ADDR          0x3F
#define AUDIO_USE_CASE                0x30
#define AUDIO_SAMPLE_RATE             0x01

#define AUDIO_MODE_CFG_ADDR           0x00
#define AUDIO_MODE                    0x10
#define AUDIO_FREQ_M                  0x2A
#define AUDIO_FREQ_L                  0x12

#define AUDIO_STS_ADDR                0x00

#define AUDIO_PERIPHERAL_CFG_ADDR     0xA9
#define AUDIO_PERIPHERAL_ADC          0x28
#define AUDIO_PERIPHERAL_FRONT_DAC    0x33
#define AUDIO_PERIPHERAL_REAR_DAC     0x32
#define AUDIO_PERIPHERAL_HOST_OUT0    0x16
#define AUDIO_PERIPHERAL_HOST_OUT1    0x17
#define AUDIO_PERIPHERAL_ENABLE       0x00

#define AUDIO_PRIM_INPUT_ADDR         0x20

#define AUDIO_MUTE_STATE              0x00
#define AUDIO_UNMUTE_STATE            0x01

#define AUDIO_FADE_BALANCE_INDEX_OFFSET 0x07
#define AUDIO_TONE_INDEX_OFFSET       0x12

#define AUDIO_GEQ_GAIN_MIN             -12
#define AUDIO_GEQ_GAIN_MAX              12



static const char audio_cfg[]     = {AUDIO_USE_CASE | AUDIO_SAMPLE_RATE};
static const char mode_cfg[]      = {AUDIO_MODE, AUDIO_FREQ_M, AUDIO_FREQ_L};
static const char adc_cfg[]       = {AUDIO_PERIPHERAL_ADC, AUDIO_PERIPHERAL_ENABLE};
static const char dac_front_cfg[] = {AUDIO_PERIPHERAL_FRONT_DAC, AUDIO_PERIPHERAL_ENABLE};
static const char dac_rear_cfg[]  = {AUDIO_PERIPHERAL_REAR_DAC, AUDIO_PERIPHERAL_ENABLE};
static const char host_output0_cfg[] = {AUDIO_PERIPHERAL_HOST_OUT0, AUDIO_PHILIPS_I2S};
static const char host_output1_cfg[] = {AUDIO_PERIPHERAL_HOST_OUT1, AUDIO_PHILIPS_I2S};
static const int  channel_cfg     = 0x2026F3;
static const int  loudness_cfg    = AUDIO_EASYP_Loudf_StaticIndep;
static const char spk_pa1_cfg[]   = {(char)0x00, (char)0xff, (char)0xf1};
static const char spk_pa2_cfg[]   = {(char)0x01, (char)0xff, (char)0xf1};

static int audio_fd = 0;
static int audio_ret = 0;
static int audio_rdy = 0;

/*Data struct for module inlet*/
#define OPER1(name, subaddress) {subaddress, NULL},
static audio_module_inlet module_inlets[AUDIO_MODULE_INLETS_CNT] = {
  AUDIO_MODULE_INLET_TBL
};
#undef OPER1

/*Data struct for module outlet*/
#define OPER1(name, subaddress) {subaddress, 0},
static audio_module_outlet module_outlets[AUDIO_MODULE_OUTLETS_CNT] = {
  AUDIO_MODULE_OUTLET_TBL
};
#undef OPER1

/*Data struct for module*/
#undef OPER2
#define OPER2 ,
#define OPER1(name, feature, controls, inlet, outlet) audio_ctl_name name##_controls[] = {controls};
AUDIO_MODULE_TBL
#undef OPER1

#define OPER1(name, feature, controls, inlet, outlet) {feature, name##_controls, sizeof(name##_controls)/sizeof(name##_controls[0]), \
                                                         &(module_inlets[inlet]), &(module_outlets[outlet])},
const audio_module audio_modules[AUDIO_MODULES_CNT] = {
  AUDIO_MODULE_TBL
};
#undef OPER1

/*Data struct for module list of each physical channel*/
#define OPER1(name, modules) audio_module_name name##_modules[] = {modules};
AUDIO_PHYSICAL_CHNL_TBL
#undef OPER1

#define OPER1(name, modules) {name##_modules,(sizeof(name##_modules)/sizeof(name##_modules[0]))},
const audio_physical_chnl_module physical_chnl_modules[AUDIO_PHYSICAL_CHNLS_CNT] = {
  AUDIO_PHYSICAL_CHNL_TBL
};
#undef OPER1

/*Data struct for physical channel*/
audio_physical_chnl_info physical_chnl_info[AUDIO_PHYSICAL_CHNLS_CNT];

/*Data struct  input value*/
#define OPER1(name, pid, format, p_s_value, t_n_value) {name, pid, (int)format, p_s_value, t_n_value},

audio_physical_input physical_inputs[AUDIO_INPUTS_CNT] = {
  AUDIO_PHYSICAL_INPUT_TBL
};
#undef OPER1

/*Data struct for physical output*/
#define OPER1(name, inlet) {name, &module_inlets[inlet], NULL, NULL,{NULL},{NULL}},

audio_physical_output physical_outputs[AUDIO_OUTPUTS_CNT] = {
  AUDIO_PHYSICAL_OUTPUT_TBL
};
#undef OPER1

/*Data struct for control*/
#define OPER1(name, subaddress, count) int audio_##name##_subaddress[count] = {subaddress};
AUDIO_CTL_TBL
#undef OPER1

#define OPER1(name, subaddress, count) {name, audio_##name##_subaddress, count},
audio_ctl_parm audio_controls[AUDIO_CTL_CNT] = {
  AUDIO_CTL_TBL
};
#undef OPER1

/*Data struct for speaker*/
audio_spk speakers[AUDIO_SPKS_CNT] = {
  {AUDIO_FRONT_DAC_OUT, AUDIO_SPK_LEFT_CHNL},
  {AUDIO_FRONT_DAC_OUT, AUDIO_SPK_RIGHT_CHNL},
  {AUDIO_REAR_DAC_OUT, AUDIO_SPK_LEFT_CHNL},
  {AUDIO_REAR_DAC_OUT, AUDIO_SPK_RIGHT_CHNL},
};

/*Data struct for physical channel speaker*/
#define OPER1(physical_channel, speaker_name, speaker_channel) {physical_channel, speaker_name, speaker_channel},
audio_physical_chnl_spk physical_chanl_spk[] = {
  AUDIO_MAP_PHYSICAL_CHNL_TO_SPK_TBL
};

audio_fade_balance audio_fade_gain[15] = {
//Left   Right
  {0,     65 },
  {0,     45 },
  {0,     28 },
  {0,     15 },
  {0,     9  },
  {0,     5  },
  {0,     2  },
  {0,     0  },
  {2,     0  },
  {5,     0  },
  {9,     0  },
  {15,    0  },
  {28,    0  },
  {45,    0  },
  {65,    0  },
};

audio_fade_balance audio_balance_gain[15] = {
//Front  Rear
  {0,     65 },
  {0,     45 },
  {0,     28 },
  {0,     15 },
  {0,     9  },
  {0,     5  },
  {0,     2  },
  {0,     0  },
  {2,     0  },
  {5,     0  },
  {9,     0  },
  {15,    0  },
  {28,    0  },
  {45,    0  },
  {65,    0  },
};

int audio_tone[25] = {
  -12,
  -11,
  -10,
  -9,
  -8,
  -7,
  -6,
  -5,
  -4,
  -3,
  -2,
  -1,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
};

audio_filter_parm audio_geq_filter_parm[AUDIO_GEQ_BANDS_CNT] = {
  {AUDIO_FILTER_PEAK_NOTCH, 80, 100, 0, 7},
  {AUDIO_FILTER_PEAK_NOTCH, 500, 100, 0, 7},
  {AUDIO_FILTER_PEAK_NOTCH, 1000, 100, 0, 7},
  {AUDIO_FILTER_PEAK_NOTCH, 5000, 100, 0, 4},
  {AUDIO_FILTER_PEAK_NOTCH, 16000, 100, 0, 4},
};

int audio_preset_geq_gain[AUDIO_GEQ_MODES_CNT][AUDIO_GEQ_BANDS_CNT] = {
  {0, 0, 0, 0, 0},
  {4, -2, 4, 2, 2},
  {6, 0, 2, 0, 6},
  {2, 2, 0, 3, 4},
  {2, 0, 4, 0, 2},
  { -4, 2, 2, 2, -4},
  {0, 0, 0, 0, 0},
  {4, 6, 0, 1, 2},
};
////////////////////////////////////////////////////////////////
static int audio_open ( void )
{
  int speed = I2C_DEV_SPEED;

  if(audio_fd != 0) {
    MSG( "I2C busy %d\n", audio_fd );
    return -EBUSY;
  }

  if( (audio_fd = open( I2C_DEV_BUS, O_RDWR ) ) < 0 ) {
    MSG ( "open fail %d\n", audio_fd );
    return -ENODEV;
  }

  if ( audio_ret = devctl ( audio_fd, DCMD_I2C_SET_BUS_SPEED, &speed, sizeof ( speed ), NULL ) ) {
    MSG( "Set bus speed fail %d\n", audio_ret );
    return -EINVAL;
  }

  return 0;
}

static void audio_close ( void )
{
  if( audio_fd > 0 )
    close( audio_fd );
  audio_fd = 0;
}

///////////////////////////////////////////////////////////////////
int audio_data_conv ( int val, short int *audio_y_data )
{
  if ( audio_y_data == NULL ) {
    DBG ();
    return -EINVAL;
  }
  audio_y_data[0] = ( val & 0x00000FFF ) >> 1 ;
  audio_y_data[1] = ( val & 0x00FFF000 ) >> 12 ;
  return 0;
}

int audio_calc_x ( double val )
{
  int data =  val * 0x800000  + (  val < 0  ?  -0.5  : 0.5 );

  if ( data < -0x800000 )
    data = -0x800000;
  else if ( data > 0x7FFFFF )
    data = 0x7FFFFF;

  return data & 0xFFFFFF;
}

short int audio_calc_y ( double val )
{
  int data =  val * 0x800  + (  val < 0  ?  -0.5  : 0.5 );

  if ( data < -0x800 )
    data = -0x800;
  else if ( data > 0x7FF )
    data = 0x7FF;

  return data & 0x00000FFF;
}

///////////////////////////////////////////////////////////////////
int audio_recv ( const unsigned char *addr, const int addr_size, unsigned char *data, const int data_num )
{
  audio_sendrecv_t sendrecv_data;

  sendrecv_data.head.slave.fmt = I2C_ADDRFMT_7BIT;
  sendrecv_data.head.slave.addr = I2C_DEV_ADR;
  sendrecv_data.head.send_len = addr_size;
  sendrecv_data.head.recv_len = data_num;
  sendrecv_data.head.stop = 1;
  memcpy(sendrecv_data.buf,addr,addr_size);

  if(audio_fd == 0) {
    audio_open();
  }

  if ( audio_ret = devctl ( audio_fd, DCMD_I2C_SENDRECV, &sendrecv_data, sizeof ( sendrecv_data ), NULL ) ) {
    MSG ( "Fail to ioctl for DCMD_I2C_SENDRECV, errno=%d\n", audio_ret );
    return audio_ret;
  }

  memcpy(data,sendrecv_data.buf,data_num);

  return 0;
}

static int audio_read_x_reg ( const int addr, int *data, int data_num )
{
  if ( ( ( addr & AUDIO_ADDR_MASK ) != AUDIO_X_ADDR_SEG ) || ( data_num < 1 ) || ( data_num > 84 ) || ( data == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  int cnt = data_num * 3;
  unsigned char sub_addr[3], buf[cnt], *buf_ptr = buf;

  sub_addr[2] = addr;
  sub_addr[1] = addr >> 8;
  sub_addr[0] = addr >> 16;

  if ( audio_recv ( sub_addr, 3, buf, cnt ) == 0 ) {
    while ( data_num-- ) {
      cnt = ( *buf_ptr++ ) << 16;
      cnt += ( *buf_ptr++ ) << 8;
      *data++ = cnt + ( *buf_ptr++ );
    }
    return 0;
  } else return -EIO;

  return 0;
}

int audio_read_y_reg ( const int addr, short int *data, int data_num )
{
  if ( ( ( addr & AUDIO_ADDR_MASK ) != AUDIO_Y_ADDR_SEG ) || ( data_num < 1 ) || ( data_num > 126 ) || ( data == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  int cnt = data_num * 2;
  unsigned char sub_addr[3], buf[cnt], *buf_ptr = buf;

  sub_addr[2] = addr;
  sub_addr[1] = addr >> 8;
  sub_addr[0] = addr >> 16;

  if ( audio_recv ( sub_addr, 3, buf, cnt ) == 0 ) {
    while ( data_num-- ) {
      cnt = ( *buf_ptr++ ) << 8;
      *data++ = cnt + ( *buf_ptr++ );
    }
    return 0;
  } else return -EIO;

return 0;
}

int audio_send ( unsigned char *data, const int data_num )
{
  int i = 0;
  //for ( i = 0; i < data_num; i++ )
    //MSG ( "data[%d]:%x\n", i, data[i] );

  audio_send_t send_data;

  send_data.head.slave.fmt = I2C_ADDRFMT_7BIT;
  send_data.head.slave.addr = I2C_DEV_ADR;
  send_data.head.len = data_num;
  send_data.head.stop = 1;
  memcpy(send_data.buf,data,data_num);

  if(audio_fd == 0) {
    audio_open();
  }

  if ( audio_ret = devctl ( audio_fd, DCMD_I2C_SEND, &send_data, sizeof ( send_data ), NULL ) ) {
    MSG ( "Fail to ioctl for DCMD_I2C_SEND, errno=%d\n", audio_ret );
    return audio_ret;
  }

  return 0;
}

static int audio_write ( const char addr, const char *data, const int data_num )
{
  if ( ( data_num < 1 ) || ( data == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  unsigned char buf[data_num + 1];
  buf[0] = addr;
  memcpy ( &buf[1], data, data_num );

  return  audio_send ( buf, data_num + 1 ) ;
}

int audio_write_x_reg ( const int addr, const int *data, int data_num )
{
  if ( ( ( addr & AUDIO_ADDR_MASK ) != AUDIO_X_ADDR_SEG ) || ( data_num < 1 ) || ( data_num > 8 ) || ( data == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  unsigned char buf[data_num * 3 + 3], *buf_ptr = buf;

  *buf_ptr++ = addr >> 16;
  *buf_ptr++ = addr >> 8;
  *buf_ptr++ = addr;

  while ( data_num-- ) {
    *buf_ptr++ = *data >> 16;
    *buf_ptr++ = *data >> 8;
    *buf_ptr++ = *data++;
  }

  return audio_send ( buf, buf_ptr - buf ) ;
}

int audio_write_y_reg ( const int addr, short int *data, int data_num )
{
  if ( ( ( addr & AUDIO_ADDR_MASK ) != AUDIO_Y_ADDR_SEG ) || ( data_num < 1 ) || ( data_num > 12 ) || ( data == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  unsigned char buf[data_num * 2 + 3], *buf_ptr = buf;

  *buf_ptr++ = addr >> 16;
  *buf_ptr++ = addr >> 8;
  *buf_ptr++ = addr;

  while ( data_num-- ) {
    *buf_ptr++ = *data >> 8;
    *buf_ptr++ = *data++;
  }

  return audio_send ( buf, buf_ptr - buf );
}

int audio_write_y_reg_scratch ( const int addr, short int *data, int data_num )
{
  if ( ( ( addr & AUDIO_ADDR_MASK ) != AUDIO_Y_ADDR_SEG ) || ( data_num < 1 ) || ( data_num > 12 ) || ( data == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  int val;
  if ( audio_read_x_reg ( AUDIO_X_EasyP_Index, &val, 1 ) ) {
    DBG ();
    return -EIO;
  }

  if ( val ) {
    MSG ( "not ready\n" );
    return -EAGAIN;
  }

  if ( audio_write_y_reg ( AUDIO_Y_UpdatC_Coeff0, data, data_num ) ) {
    DBG ();
    return -EIO;
  }

  val = addr & ( ~AUDIO_ADDR_MASK );
  if ( audio_write_x_reg ( AUDIO_X_UpdatC_StartYaddr, &val, 1 ) ) {
    DBG ();
    return -EIO;
  }

  if ( audio_write_x_reg ( AUDIO_X_UpdatC_NrOfCoeff, &data_num, 1 ) ) {
    DBG ();
    return -EIO;
  }

  MSG ( "\n!!!need wait a moment!!!\n" );
  return 0;
}

int audio_init ( void )
{
  int cnt;
  unsigned char sts, sts_addr = AUDIO_STS_ADDR;

  if ( !audio_rdy ) {
    audio_open();
    if ( audio_fd <= 0 )
      goto err;

    //Set use case and sample rate
    if ( audio_write ( AUDIO_AUDIO_CFG_ADDR, audio_cfg, sizeof ( audio_cfg ) ) )
      goto err;

    //Change device to active mode
    if ( audio_write ( AUDIO_MODE_CFG_ADDR, mode_cfg, sizeof ( mode_cfg ) ) )
      goto err;

    //check power on
    cnt = 10;
    do {
      if ( cnt != 10 )
        usleep ( 5000 );
      if ( audio_recv ( &sts_addr, 1, &sts, sizeof ( sts ) ) )
        goto err;

#ifdef SIMULATION
      sts = 0;
#endif

    } while ( ( cnt-- ) && ( sts & 0x01 ) );

    //Enable AD converters
    if ( audio_write ( AUDIO_PERIPHERAL_CFG_ADDR, adc_cfg, sizeof ( adc_cfg ) ) )
      goto err;

    //Enable Front DACs
    if ( audio_write ( AUDIO_PERIPHERAL_CFG_ADDR, dac_front_cfg, sizeof ( dac_front_cfg ) ) )
      goto err;

    //Enable Rear DACs
    if ( audio_write ( AUDIO_PERIPHERAL_CFG_ADDR, dac_rear_cfg, sizeof ( dac_rear_cfg ) ) )
      goto err;

    // Write One Over Max Boost configuration for primary channel
    if ( audio_write_x_reg ( AUDIO_X_Vol_OneOverMaxBoostP, &channel_cfg, sizeof ( channel_cfg ) / sizeof ( int ) ) )
      goto err;

    //Set static loudness independent bass/treble
    if ( audio_write_x_reg ( AUDIO_X_EasyP_Index, &loudness_cfg, sizeof ( loudness_cfg ) / sizeof ( int ) ) )
      goto err;

    //Configure dsp GPIO_0 and GPIO_1 to enable SPK_PA1 & SPK_PA2
    //if ( audio_write ( 0xC0, spk_pa1_cfg, sizeof ( spk_pa1_cfg ) ) )
    //goto err;

    //if ( audio_write ( 0xC0, spk_pa2_cfg, sizeof ( spk_pa1_cfg ) ) )
    //goto err;

    audio_rdy = 1;
  }

  return 0;

err:
  audio_close();
  return -EAGAIN;
}

/*Init module inlet*/
int audio_init_module_inlet ( void )
{
  int index;

  for ( index = 0; index < AUDIO_MODULE_INLETS_CNT; index++ ) {
    module_inlets[index].module_outlet = NULL;
  }

  return 0;
}

/*Init module outlet*/
int audio_init_module_outlet ( void )
{
  int index;
  for ( index = 0; index < AUDIO_MODULE_OUTLETS_CNT; index++ ) {
    module_outlets[index].ref_cnt = 0;
  }

  return 0;
}

/*Connect specified inlet with specified outlet*/
int audio_connect_inlet_with_outlet ( audio_module_inlet* const inlet, audio_module_outlet* const outlet )
{
  if ( inlet == NULL || outlet == NULL ) {
    DBG ();
    return -EINVAL;
  }

  if ( inlet->module_outlet != outlet ) {
    MSG ( "Inlet:%x---Outlet:%x\n", inlet->subaddr, outlet->subaddr );
    if ( audio_write_x_reg ( inlet->subaddr, &outlet->subaddr, 1 ) ) {
      DBG ();
      return -EIO;
    }
  }

  outlet->ref_cnt++;
  inlet->module_outlet = outlet;

  return 0;
}

/*Establish physical channel*/
int audio_establish_physical_chnl ( void )
{
  int ret;
  int index;
  const audio_module* current_module_ptr = NULL;
  const audio_module* next_module_ptr = NULL;
  //audio_physical_chnl_name chnl;
  int chnl;

  for ( chnl = 0; chnl < AUDIO_PHYSICAL_CHNLS_CNT; chnl++ ) {
    memset ( physical_chnl_info[chnl].modules, 0, sizeof ( physical_chnl_info[chnl].modules ) );
    index = 0;
    current_module_ptr = & ( audio_modules[physical_chnl_modules[chnl].modules[index]] );
    physical_chnl_info[chnl].inlet = current_module_ptr->module_inlet;
    physical_chnl_info[chnl].modules[current_module_ptr->feature] = current_module_ptr;
    index++;
    MSG ( "Channel:%d\n", chnl );
    while ( index < physical_chnl_modules[chnl].cnt ) {
      next_module_ptr = & ( audio_modules[physical_chnl_modules[chnl].modules[index]] );
      if ( next_module_ptr->module_inlet != current_module_ptr->module_inlet ) {
        MSG ( "Index:%d\n", index );
        ret = audio_connect_inlet_with_outlet ( next_module_ptr->module_inlet, current_module_ptr->module_outlet );
        if ( ret < 0 )
          return ret;
      }

      current_module_ptr = next_module_ptr;
      physical_chnl_info[chnl].modules[current_module_ptr->feature] = current_module_ptr;
      index++;
    }

    physical_chnl_info[chnl].outlet = current_module_ptr->module_outlet;
  }

  return 0;
}

/*Disconnect specified inlet*/
int audio_disconnect_inlet ( audio_module_inlet* const inlet )
{
  if ( inlet == NULL ) {
    DBG ();
    return -EINVAL;
  }

  if ( ( inlet->module_outlet != NULL ) && ( inlet->module_outlet->ref_cnt != 0 ) ) {
    inlet->module_outlet->ref_cnt--;
    inlet->module_outlet = NULL;
  }

  return 0;
}

/*Attach input to channel*/
int audio_attach_input_to_chnl ( audio_input_name input, audio_physical_chnl_name chnl )
{
  int err;
  char dsp_cfg_data[2];
  char dsp_high_lvl_data;
  int  dsp_low_lvl_subaddr;
  int  dsp_low_lvl_data;

  audio_disconnect_inlet ( physical_chnl_info[chnl].inlet );
  /*Set peripheral configuration*/
  if ( physical_inputs[input].pid != AUDIO_NEA ) {
    dsp_cfg_data[0] = physical_inputs[input].pid;
    dsp_cfg_data[1] = physical_inputs[input].format;
    audio_write ( AUDIO_PERIPHERAL_CFG_ADDR, dsp_cfg_data, 2 );
  }

  /*Set input value to input register*/
  switch ( chnl ) {
  case AUDIO_PRIM_FRONT_PHYSICAL_CHNL:
  case AUDIO_PRIM_REAR_PHYSICAL_CHNL:
    if ( AUDIO_NEA == physical_inputs[input].p_s_value ) {
      MSG("Invalid input for Primary channel.\n");
      return -EINVAL;
    }
    dsp_high_lvl_data = physical_inputs[input].p_s_value;
    err = audio_write ( AUDIO_PRIM_INPUT_ADDR, &dsp_high_lvl_data, 1 );
    if ( err != 0 ) {
      DBG();
      return err;
    }
    break;
  case AUDIO_NAV_PHYSICAL_CHNL:
    if ( input == AUDIO_IIS0 || input == AUDIO_IIS1 || input == AUDIO_IIS2 || input == AUDIO_SPDIF ) {
      dsp_low_lvl_subaddr = AUDIO_X_NChanSelPntr;
      dsp_low_lvl_data = AUDIO_REL_ADDR ( AUDIO_X_SRC2InL );
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
      dsp_low_lvl_subaddr = AUDIO_X_pSRCInputFlag2;
      dsp_low_lvl_data = physical_inputs[input].t_n_value;
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
    } else if ( input == AUDIO_HOST_IIS0 || input == AUDIO_HOST_IIS1 ) {
      dsp_low_lvl_subaddr = AUDIO_X_NChanSelPntr;
      dsp_low_lvl_data = physical_inputs[input].t_n_value;
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
    } else if ( input == AUDIO_AIN3 ) {
      dsp_low_lvl_subaddr = AUDIO_X_EasyP_Index;
      dsp_low_lvl_data = physical_inputs[input].t_n_value;
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
    } else {
      MSG("Invalid input for NAV channel.\n");
      return -EINVAL;
    }
    break;
  case AUDIO_PHONE_PHYSICAL_CHNL:
    if ( input == AUDIO_IIS0 || input == AUDIO_IIS1 || input == AUDIO_IIS2 || input == AUDIO_SPDIF ) {
      dsp_low_lvl_subaddr = AUDIO_X_TChanSelPntr;
      dsp_low_lvl_data = AUDIO_REL_ADDR ( AUDIO_X_SRC3InL );
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
      dsp_low_lvl_subaddr = AUDIO_X_pSRCInputFlag3;
      dsp_low_lvl_data = physical_inputs[input].t_n_value;
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
    } else if ( input == AUDIO_HOST_IIS0 || input == AUDIO_HOST_IIS1 ) {
      dsp_low_lvl_subaddr = AUDIO_X_TChanSelPntr;
      dsp_low_lvl_data = physical_inputs[input].t_n_value;
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
    } else if ( input == AUDIO_AIN2 ) {
      dsp_low_lvl_subaddr = AUDIO_X_EasyP_Index;
      dsp_low_lvl_data = physical_inputs[input].t_n_value;
      err = audio_write_x_reg ( dsp_low_lvl_subaddr, &dsp_low_lvl_data, 1 );
      if ( err != 0 )
        return err;
    } else {
      MSG("Invalid input for Phone channel.\n");
      return -EINVAL;
    }
    break;
  default:
    break;
  }

  return 0;
}

/*Attach output to channel*/
int audio_attach_output_to_chnl ( audio_output_name output, audio_physical_chnl_name chnl )
{
  int err;
  audio_physical_output* output_ptr = &physical_outputs[output];

  if ( output_ptr->inlet->module_outlet != physical_chnl_info[chnl].outlet ) {
    if ( physical_chnl_info[chnl].outlet->ref_cnt != 0 ) {
      DBG();
      return -EINVAL;
    }

    audio_disconnect_inlet ( output_ptr->inlet );
    audio_config_mixer_ctls_with_output ( &physical_chnl_info[chnl], output_ptr );
    physical_outputs[output].left_peq = physical_chnl_info[chnl].modules[AUDIO_LEFT_PARM_EQ];
    physical_outputs[output].right_peq = physical_chnl_info[chnl].modules[AUDIO_RIGHT_PARM_EQ];
    /*Here should to set audio mute control, it will be implemented later.TBD*/
    err = audio_mute_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_MUTE]->ctls[0]], AUDIO_UNMUTE_STATE, 50 );
    if ( err != 0 )
      return err;
    err = audio_connect_inlet_with_outlet ( output_ptr->inlet, physical_chnl_info[chnl].outlet );
    if ( err != 0 )
      return err;
  }

  return 0;
}

/*Connect default output*/
int audio_init_default_output ( void )
{
  int err;

  err = audio_attach_output_to_chnl ( AUDIO_FRONT_DAC_OUT, AUDIO_PRIM_FRONT_PHYSICAL_CHNL );
  if ( !err ) {
    err = audio_attach_output_to_chnl ( AUDIO_REAR_DAC_OUT, AUDIO_PRIM_REAR_PHYSICAL_CHNL );
  }

  return err;
}

/*Set primary channel input mode*/
int audio_set_prim_chnl_input_mode ( audio_prim_chnl_input_mode mode )
{
  int err;
  int dsp_x_data;

  switch ( mode ) {
  case AUDIO_STEREO:
    dsp_x_data = AUDIO_EASYP_PchannelStereo;
    break;
  case AUDIO_MONO_LEFT:
    dsp_x_data = AUDIO_EASYP_PchannelMonoLeft;
    break;
  case AUDIO_MONO_Right:
    dsp_x_data = AUDIO_EASYP_PchannelMonoRight;
    break;
  default:
    DBG();
    return -EINVAL;
    break;
  }
  err = audio_write_x_reg ( AUDIO_X_EasyP_Index, &dsp_x_data, 1 );
  if ( err != 0 )
    return err;

  return 0;
}

/*Set loudness parameters*/
int audio_set_loudness_parm ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;

  if ( physical_chnl_info[chnl].modules[AUDIO_LOUDNESS] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_LOUDNESS]->ctls[2]], AUDIO_BASS_BOOST_MAX );
  if ( err != 0 ) {
    DBG();
    return err;
  }
  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_LOUDNESS]->ctls[3]], AUDIO_TREBLE_BOOST_MAX );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set loudness filter parameters*/
int audio_set_loudness_filter ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  audio_filter_parm bass_filter_parm;
  audio_filter_parm treble_filter_parm;

  bass_filter_parm.type = AUDIO_FILTER_PEAK_NOTCH;
  bass_filter_parm.freq = 100;
  bass_filter_parm.fixed_point_parm = 40;
  bass_filter_parm.gain = 0;
  bass_filter_parm.size = 8;

  treble_filter_parm.type = AUDIO_FILTER_PEAK_NOTCH;
  treble_filter_parm.freq = 10000;
  treble_filter_parm.fixed_point_parm = 92;
  treble_filter_parm.gain = 0;
  treble_filter_parm.size = 5;

  if ( physical_chnl_info[chnl].modules[AUDIO_LOUDNESS] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_filter_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_LOUDNESS]->ctls[4]], &bass_filter_parm );
  if ( err != 0 ) {
    DBG();
    return err;
  }
  err = audio_filter_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_LOUDNESS]->ctls[5]], &treble_filter_parm );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set dc filter parameters*/
int audio_set_dc_filter ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  audio_filter_parm dc_filter_parm;

  dc_filter_parm.type = AUDIO_FILTER_PEAK_NOTCH;
  dc_filter_parm.freq = 7;
  dc_filter_parm.fixed_point_parm = 0;
  dc_filter_parm.gain = 0;
  dc_filter_parm.size = 3;

  if ( physical_chnl_info[chnl].modules[AUDIO_DC_FILTER] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_filter_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_DC_FILTER]->ctls[0]], &dc_filter_parm );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set channel filter just for NAV and Phone channel*/
int audio_set_chnl_filter ( void )
{
  int err;
  int chnl;
  int filter;
  audio_filter_parm filter_parm;

  filter_parm.type = AUDIO_FILTER_FLAT;
  filter_parm.freq = 20;
  filter_parm.fixed_point_parm = 70;
  filter_parm.gain = 0;
  filter_parm.size = 10;
  for ( chnl = AUDIO_NAV_PHYSICAL_CHNL; chnl < AUDIO_PHYSICAL_CHNLS_CNT; chnl++ ) {
    for ( filter = 0; filter < 2; filter++ ) {
      err = audio_filter_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_PATH_FILTER]->ctls[filter]], &filter_parm );
      if ( err != 0 ) {
        DBG();
        return err;
      }
    }
  }

  return 0;
}

/*Set channel gain*/
int audio_set_chnl_gain ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int left_gain = 0;
  int right_gain = 0;

  if ( physical_chnl_info[chnl].modules[AUDIO_CHNL_GAIN] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_CHNL_GAIN]->ctls[0]], left_gain );
  if ( err != 0 ) {
    DBG();
    return err;
  }
  if ( chnl <= AUDIO_PRIM_REAR_PHYSICAL_CHNL ) {
    err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_CHNL_GAIN]->ctls[1]], right_gain );
  }

  return 0;
}

/*Set downscaler gain*/
int audio_set_downscaler_gain ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int gain = -12;

  if ( physical_chnl_info[chnl].modules[AUDIO_DOWN_SCALER] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_DOWN_SCALER]->ctls[0]], gain );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set upscaler gain*/
int audio_set_upscaler_gain ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int gain = 0;

  if ( physical_chnl_info[chnl].modules[AUDIO_UP_SCALER] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_UP_SCALER]->ctls[0]], gain );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set prescaler gain*/
int audio_set_prescaler_gain ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int gain = -12;

  if ( physical_chnl_info[chnl].modules[AUDIO_PRESCALER] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_PRESCALER]->ctls[0]], gain );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set source scaler gain*/
int audio_set_source_scaler_gain ( audio_physical_chnl_name chnl )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int gain = 0;

  if ( physical_chnl_info[chnl].modules[AUDIO_SOURCE_SCALER] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_SOURCE_SCALER]->ctls[0]], gain );
  if ( err != 0 ) {
    DBG();
    return err;
  }

  return 0;
}


/*Config mixer controls with output*/
int audio_config_mixer_ctls_with_output ( audio_physical_chnl_info* chnl, audio_physical_output* output )
{
  if ( chnl == NULL || output == NULL ) {
    DBG();
    return -EINVAL;
  }

  const audio_module* mixer_module = NULL;
  int channel_mixer;
  //audio_mix_signal mix_signal = (audio_mix_signal)0;
  int mix_signal = 0;

  mixer_module = chnl->modules[AUDIO_SUPER_POSITION];
  if ( mixer_module != NULL ) {
    for ( channel_mixer = 0; channel_mixer < mixer_module->ctl_cnt; channel_mixer++, mix_signal++ ) {
      if ( mixer_module->ctls[channel_mixer] != AUDIO_CTL_NULL ) {
        output->left_mixer[mix_signal] = &audio_controls[mixer_module->ctls[channel_mixer]];
      } else {
        output->left_mixer[mix_signal] = NULL;
      }

      if ( mixer_module->ctls[++channel_mixer] != AUDIO_CTL_NULL ) {
        output->right_mixer[mix_signal] = &audio_controls[mixer_module->ctls[channel_mixer]];
      } else {
        output->right_mixer[mix_signal] = NULL;
      }
    }
  } else {
    for ( mix_signal = 0; mix_signal < AUDIO_MIX_SIGNALS_CNT; mix_signal++ ) {
      output->left_mixer[mix_signal] = NULL;
      output->left_mixer[mix_signal] = NULL;
    }
  }

  return 0;
}


/*Mix signal on speaker*/
int audio_mix_signal_on_spk ( audio_mix_signal_cfg* cfg )
{
  if ( cfg == NULL ) {
    DBG();
    return -EINVAL;
  }
  int err;
  int gain;
  unsigned char speaker;
  audio_physical_output* output = NULL;

  for ( speaker = 0; speaker < AUDIO_SPKS_CNT; speaker++ ) {
    if ( cfg->speakers & ( 1 << speaker ) ) {
      gain = cfg->gain;
    } else {
      gain = -90;
    }

    output = &physical_outputs[speakers[speaker].output];

    if ( output->inlet->module_outlet != NULL ) {
      switch ( speakers[speaker].spk_chnl ) {
      case AUDIO_SPK_LEFT_CHNL:
        if ( output->left_mixer[cfg->signal_name] != NULL ) {
          err = audio_scaler_ctl ( output->left_mixer[cfg->signal_name], gain );
          if ( err != 0 ) {
            DBG();
            return err;
          }
        }
        break;
      case AUDIO_SPK_RIGHT_CHNL:
        if ( output->right_mixer[cfg->signal_name] != NULL ) {
          err = audio_scaler_ctl ( output->right_mixer[cfg->signal_name], gain );
          if ( err != 0 ) {
            DBG();
            return err;
          }
        }
        break;
      default:
        DBG();
        return -EINVAL;
        break;
      }
    } else {
      DBG();
      return -EINVAL;
    }

  }

  return 0;
}
////////////////////////////////////////////////////////////////////////////////////
/*Initialize audio adapter*/
int audio_hero_init ( void )
{
  int err;

  if ( err = audio_init() ) {
    DBG();
    return err;
  }

  if ( err = audio_init_module_inlet() ) {
    DBG();
    return err;
  }

  if ( err = audio_init_module_outlet() ) {
    DBG();
    return err;
  }

  if ( err = audio_establish_physical_chnl() ) {
    DBG();
    return err;
  }

  if ( err = audio_init_default_output() ) {
    DBG();
    return err;
  }
  MSG ( "audio_set_chnl_gain(AUDIO_PRIM_FRONT_PHYSICAL_CHNL\n" );
  if ( err = audio_set_chnl_gain ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  if ( err = audio_set_chnl_gain ( AUDIO_PRIM_REAR_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  if ( err = audio_set_chnl_gain ( AUDIO_NAV_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  if ( err = audio_set_chnl_gain ( AUDIO_PHONE_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }
  MSG ( "audio_set_prescaler_gain(AUDIO_PRIM_FRONT_PHYSICAL_CHNL)\n" );
  if ( err = audio_set_prescaler_gain ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  if ( err = audio_set_prescaler_gain ( AUDIO_PRIM_REAR_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }
  MSG ( "audio_set_upscaler_gain(AUDIO_PRIM_FRONT_PHYSICAL_CHNL)\n" );
  if ( err = audio_set_upscaler_gain ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  if ( err = audio_set_upscaler_gain ( AUDIO_PRIM_REAR_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }
  MSG ( "audio_set_downscaler_gain(AUDIO_PRIM_FRONT_PHYSICAL_CHNL)\n" );
  if ( err = audio_set_downscaler_gain ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  //err = audio_set_peq_filter();
  MSG ( "audio_set_loudness_parm(AUDIO_PRIM_FRONT_PHYSICAL_CHNL)\n" );
  if ( err = audio_set_loudness_parm ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }
  MSG ( "audio_set_loudness_filter(AUDIO_PRIM_FRONT_PHYSICAL_CHNL)\n" );
  if ( err = audio_set_loudness_filter ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) ) {
    DBG();
    return err;
  }

  return 0;
}

/*Connect a physical input to a physical channel*/
int audio_hero_select_source ( audio_input_name input, audio_physical_chnl_name chnl )
{
	if ( input < AUDIO_RADIO || input >= AUDIO_INPUTS_CNT ) {
		MSG("%s:%d:Invalid input:%d.\n",__FILE__,__LINE__,input);
		return -EINVAL;
	}
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;

  if ( input != AUDIO_INPUT_NULL ) {
    if ( chnl == AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl == AUDIO_PRIM_REAR_PHYSICAL_CHNL ) {
      err = audio_set_prim_chnl_input_mode ( AUDIO_STEREO );
      if ( err ) {
        DBG();
        return err;
      }
    }

    err = audio_attach_input_to_chnl ( input, chnl );
    if ( err ) {
      DBG();
      return err;
    }

    err = audio_set_source_scaler_gain ( chnl );
    if ( err ) {
      DBG();
      return err;
    }
  } else {
    err = audio_connect_inlet_with_outlet ( physical_chnl_info[chnl].inlet, &module_outlets[AUDIO_NULL_OUTLET] );
    if ( err ) {
      DBG();
      return err;
    }
  }

  return 0;
}

/*Mix a signal on speaker*/
int audio_hero_mix_signal_on_spk ( audio_mix_signal signal )
{
	if ( signal < AUDIO_MIX_NAV_SIGNAL || signal >= AUDIO_MIX_SIGNALS_CNT) {
		MSG("%s:%d:Invalid mix signal:%d.\n",__FILE__,__LINE__,signal);
		return -EINVAL;
	}

  int err;
  audio_mix_signal_cfg cfg;

  switch ( signal ) {
  case AUDIO_MIX_NAV_SIGNAL:
    cfg.signal_name = AUDIO_MIX_NAV_SIGNAL;
    cfg.speakers = 0x03;
    cfg.gain = 0;
    break;
  case AUDIO_MIX_PHONE_SIGNAL:
    cfg.signal_name = AUDIO_MIX_PHONE_SIGNAL;
    cfg.speakers = 0x0F;
    cfg.gain = 0;
    break;
  default:
    DBG();
    return -EINVAL;
    break;
  }

  err = audio_mix_signal_on_spk ( &cfg );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}

/*Connect a physical output to a physical channel*/
int audio_hero_connect_output_to_chnl ( audio_output_name output, audio_physical_chnl_name chnl )
{
	if ( output < AUDIO_HOST_IIS_OUT0 || output >= AUDIO_OUTPUTS_CNT ) {
		MSG("%s:%d:Invalid output:%d.\n",__FILE__,__LINE__,output);
		return -EINVAL;
	}
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;

  err = audio_attach_output_to_chnl ( output, chnl );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}

/*Disconnect a physical output to physical channel*/
int audio_hero_dsiconnect_output ( audio_output_name output )
{
	if ( output < AUDIO_HOST_IIS_OUT0 || output >= AUDIO_OUTPUTS_CNT ) {
		MSG("%s:%d:Invalid output:%d.\n",__FILE__,__LINE__,output);
		return -EINVAL;
	}

  int err;

  err = audio_disconnect_inlet ( physical_outputs[output].inlet );
  if ( err ) {
    DBG();
    return err;
  }

  err = audio_connect_inlet_with_outlet ( physical_outputs[output].inlet, &module_outlets[AUDIO_NULL_OUTLET] );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}


/*Set volume scaler control of a physical channel*/
int audio_hero_set_volume ( audio_physical_chnl_name chnl, int vol )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;

  if ( physical_chnl_info[chnl].modules[AUDIO_MAIN_VOL] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_MAIN_VOL]->ctls[0]], vol );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}
/*Set loudness offset*/
int audio_hero_set_loudness ( audio_physical_chnl_name chnl, int bass_gain, int treble_gain )
{
	if ( chnl != AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl != AUDIO_PRIM_REAR_PHYSICAL_CHNL) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;

  if ( physical_chnl_info[chnl].modules[AUDIO_LOUDNESS] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_LOUDNESS]->ctls[0]], bass_gain );
  if ( err ) {
    DBG();
    return err;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_LOUDNESS]->ctls[1]], treble_gain );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set fade*/
int audio_hero_set_fade ( audio_physical_chnl_name chnl, int fade )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int front_gain = audio_fade_gain[fade + AUDIO_FADE_BALANCE_INDEX_OFFSET].gain1;
  int rear_gain = audio_fade_gain[fade + AUDIO_FADE_BALANCE_INDEX_OFFSET].gain2;

  if ( physical_chnl_info[chnl].modules[AUDIO_FADE] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_FADE]->ctls[0]], front_gain );
  if ( err ) {
    DBG();
    return err;
  }

  if ( chnl == AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) {
    err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl + 1].modules[AUDIO_FADE]->ctls[0]], rear_gain );
    if ( err ) {
      DBG();
      return err;
    }
  }

  if ( chnl == AUDIO_NAV_PHYSICAL_CHNL || chnl == AUDIO_PHONE_PHYSICAL_CHNL ) {
    err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_FADE]->ctls[1]], rear_gain );
    if ( err ) {
      DBG();
      return err;
    }
  }

  return 0;
}


/*Set balance*/
int audio_hero_set_balance ( audio_physical_chnl_name chnl, int balance )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;
  int left_gain = audio_balance_gain[balance + AUDIO_FADE_BALANCE_INDEX_OFFSET].gain1;
  int right_gain = audio_balance_gain[balance + AUDIO_FADE_BALANCE_INDEX_OFFSET].gain2;

  if ( physical_chnl_info[chnl].modules[AUDIO_BALANCE] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_BALANCE]->ctls[0]], left_gain );
  if ( err ) {
    DBG();
    return err;
  }

  err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_BALANCE]->ctls[1]], right_gain );
  if ( err ) {
    DBG();
    return err;
  }

  if ( chnl == AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) {
    err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl + 1].modules[AUDIO_BALANCE]->ctls[0]], left_gain );
    if ( err ) {
      DBG();
      return err;
    }

    err = audio_scaler_ctl ( &audio_controls[physical_chnl_info[chnl + 1].modules[AUDIO_BALANCE]->ctls[1]], right_gain );
    if ( err ) {
      DBG();
      return err;
    }
  }

  return 0;
}

/*Set tone*/
int audio_hero_set_tone ( audio_physical_chnl_name chnl, audio_tone_band band, int tone_val )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}
	if ( band < AUDIO_BASS || band >= AUDIO_TONE_BANDS_CNT ) {
		MSG("%s:%d:Invalid tone band:%d.\n",__FILE__,__LINE__,band);
		return -EINVAL;
	}

  int err;
  audio_filter_parm filter_parm;

  filter_parm.gain = audio_tone[tone_val + AUDIO_TONE_INDEX_OFFSET];
  switch ( band ) {
  case AUDIO_BASS:
    filter_parm.type = AUDIO_FILTER_LO_SHELVING;
    filter_parm.freq   = 60;
    filter_parm.fixed_point_parm = 100;
    filter_parm.size = 11;
    break;
  case AUDIO_MID:
    filter_parm.type = AUDIO_FILTER_PEAK_NOTCH;
    filter_parm.freq   = 775;
    filter_parm.fixed_point_parm = 110;
    filter_parm.size = 7;
    break;
  case AUDIO_TREBLE:
    filter_parm.type = AUDIO_FILTER_HI_SHELVING;
    filter_parm.freq   = 10000;
    filter_parm.fixed_point_parm = 100;
    filter_parm.size = 6;
    break;
  default:
    break;
  }

  if ( physical_chnl_info[chnl].modules[AUDIO_TONE] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_filter_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_TONE]->ctls[band]], &filter_parm );
  if ( err ) {
    if ( err ) {
      DBG();
      return err;
    }
  }

  return 0;
}
/*Set mute state*/
int audio_hero_set_mute ( audio_physical_chnl_name chnl, int is_unmute, int time )
{
	if ( chnl < AUDIO_PRIM_FRONT_PHYSICAL_CHNL || chnl >= AUDIO_PHYSICAL_CHNLS_CNT ) {
		MSG("%s:%d:Invalid channel:%d.\n",__FILE__,__LINE__,chnl);
		return -EINVAL;
	}

  int err;

  if ( physical_chnl_info[chnl].modules[AUDIO_MUTE] == NULL ) {
  	MSG("%s:%d:Audio control unavailable\n",__FILE__,__LINE__);
  	return -EINVAL;
  }

  err = audio_mute_ctl ( &audio_controls[physical_chnl_info[chnl].modules[AUDIO_MUTE]->ctls[0]], is_unmute, time );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}


/*Set graphic equalizer*/
int audio_hero_set_geq ( audio_geq_band band, int geq_val )
{
  if ( band < AUDIO_GEQ_BAND_1 || band > AUDIO_GEQ_BAND_5 ) {
    MSG("%s:%d:Invalid geq band:%d.\n",__FILE__,__LINE__,band);
    return -EINVAL;
  }

  int err;

  if ( geq_val < AUDIO_GEQ_GAIN_MIN )
    geq_val = AUDIO_GEQ_GAIN_MIN;
  else if ( geq_val > AUDIO_GEQ_GAIN_MAX )
    geq_val = AUDIO_GEQ_GAIN_MAX;

  audio_geq_filter_parm[band].gain = geq_val;

  err = audio_filter_ctl ( &audio_controls[physical_chnl_info[AUDIO_PRIM_FRONT_PHYSICAL_CHNL].modules[AUDIO_GRAPHIC_EQ]->ctls[band]], &audio_geq_filter_parm[band] );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}

/*Set preset graphic equalizer*/
int audio_hero_set_preset_geq ( audio_preset_geq_mode mode )
{
  if ( mode < AUDIO_GEQ_POP || mode > AUDIO_GEQ_DEFAULT ) {
    MSG("%s:%d:Invalid preset geq mode:%d.\n",__FILE__,__LINE__,mode);
    return -EINVAL;
  }

  int err, band;
  audio_filter_parm filter_parm[AUDIO_GEQ_BANDS_CNT] = {
    {AUDIO_FILTER_PEAK_NOTCH, 80, 100, 0, 7},
    {AUDIO_FILTER_PEAK_NOTCH, 500, 100, 0, 7},
    {AUDIO_FILTER_PEAK_NOTCH, 1000, 100, 0, 7},
    {AUDIO_FILTER_PEAK_NOTCH, 5000, 100, 0, 4},
    {AUDIO_FILTER_PEAK_NOTCH, 16000, 100, 0, 4},
  };

  if ( mode == AUDIO_GEQ_DEFAULT ) {
    filter_parm[0].fixed_point_parm = 80;
    filter_parm[1].freq = 200;
    filter_parm[1].fixed_point_parm = 300;
  }

  for ( band = 0; band < AUDIO_GEQ_BANDS_CNT; band++ ) {
    filter_parm[band].gain = audio_preset_geq_gain[mode][band];
    err = audio_filter_ctl ( &audio_controls[physical_chnl_info[AUDIO_PRIM_FRONT_PHYSICAL_CHNL].modules[AUDIO_GRAPHIC_EQ]->ctls[band]], &filter_parm[band] );
    if ( err ) {
      DBG();
      return err;
    }
  }

  return 0;
}

/*Initialize chime*/
int audio_hero_chime_init()
{
  return audio_chime_init();
}

/*Start chime*/
int audio_hero_start_chime ( int freq, int rept )
{
  int err;
  short int coeff = 0x7FF;

  audio_chime_type chime = {
    {
      {40, 0, 0, CHIME_SINE_WAVE},
      {40, 0, 0, CHIME_SINE_WAVE},
      {40, 0, 0, CHIME_SINE_WAVE},
      {40, 0, 0, CHIME_SINE_WAVE}
    },
    0,
    1,
    1,
    CHIME_LINEAR_RELEASE,
    1,
    0,
    0,
    -20,
    0,
    -20
  };

  chime.vol = -5;
  chime.signal_on_time = 396;
  chime.signal_off_time = 604;
  chime.slots[0].freq = freq;
  chime.slots[0].dur = chime.signal_on_time;
  chime.slots[0].vol = 0;

  err = audio_write_y_reg ( AUDIO_Y_Sup_ChionFL, &coeff, 1 );
  if ( err ) {
    DBG();
    return err;
  }

  err = audio_write_y_reg ( AUDIO_Y_Sup_ChionFR, &coeff, 1 );
  if ( err ) {
    DBG();
    return err;
  }

  err = audio_chime_generate ( &chime, rept );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}

/*Start click*/
int audio_hero_start_click_clack ( int click_clack )
{
  if ( ! ( click_clack == 0xFFFFFF || click_clack == 0x000001 ) ) {
    DBG();
    return -EINVAL;
  }
  int err;

  err = audio_chime_tone ( click_clack );
  if ( err ) {
    DBG();
    return err;
  }

  return 0;
}

