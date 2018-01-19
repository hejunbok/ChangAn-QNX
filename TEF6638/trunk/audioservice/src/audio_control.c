#include <stdio.h>
#include <math.h>
#include <errno.h>

#include "audio_hero.h"


#define AUDIO_STANDARD_1MS_MUTE  0x044444
#define AUDIO_CTL_VOL_MIN_DB     90

extern int audio_data_conv ( int val, short int *audio_y_data );
extern int audio_calc_x ( double val );
extern short int audio_calc_y ( double val );
extern int audio_write_x_reg ( const int addr, const int *data, int data_num );
extern int audio_write_y_reg ( const int addr, short int *data, int data_num );
extern int audio_write_y_reg_scratch ( const int addr, short int *data, int data_num );
//Vol_Main1P Vol_Main2P from -90dB to 0dB
static const audio_ctl_val audio_ctl_vol[AUDIO_CTL_VOL_MIN_DB + 1] = {
  {0x000C, 0x000C}, {0x000C, 0x000D}, {0x000D, 0x000D}, {0x000D, 0x000E}, {0x000E, 0x000E}, {0x0010, 0x000E}, {0x0012, 0x000E},
  {0x0014, 0x000E}, {0x0017, 0x000E}, {0x001A, 0x000E}, {0x001D, 0x000E}, {0x0020, 0x000E}, {0x0024, 0x000E}, {0x0029, 0x000E},
  {0x002E, 0x000E}, {0x0033, 0x000E}, {0x003A, 0x000E}, {0x0041, 0x000E}, {0x0049, 0x000E}, {0x0052, 0x000E}, {0x005B, 0x000E},
  {0x0067, 0x000E}, {0x0073, 0x000E}, {0x0081, 0x000E}, {0x0091, 0x000E}, {0x00A3, 0x000E}, {0x00B7, 0x000E}, {0x00CD, 0x000E},
  {0x00E6, 0x000E}, {0x00E6, 0x0010}, {0x00E6, 0x0012}, {0x00E6, 0x0014}, {0x00E6, 0x0017}, {0x00E6, 0x001A}, {0x00E6, 0x001D},
  {0x00E6, 0x0020}, {0x00E6, 0x0024}, {0x00E6, 0x0029}, {0x00E6, 0x002E}, {0x00E6, 0x0033}, {0x00E6, 0x003A}, {0x00E6, 0x0041},
  {0x00E6, 0x0049}, {0x00E6, 0x0052}, {0x00E6, 0x005B}, {0x00E6, 0x0067}, {0x00E6, 0x0073}, {0x00E6, 0x0081}, {0x00E6, 0x0091},
  {0x00E6, 0x00A3}, {0x00E6, 0x00B7}, {0x00E6, 0x00CD}, {0x00E6, 0x00E6}, {0x00E6, 0x0102}, {0x00E6, 0x0121}, {0x00E6, 0x0145},
  {0x00E6, 0x016C}, {0x00E6, 0x0199}, {0x00E6, 0x01CA}, {0x00E6, 0x0202}, {0x00E6, 0x0241}, {0x00E6, 0x0288}, {0x00E6, 0x02D7},
  {0x00E6, 0x032F}, {0x00E6, 0x0393}, {0x00E6, 0x0402}, {0x00E6, 0x0480}, {0x00E6, 0x050C}, {0x00E6, 0x05AA}, {0x00E6, 0x065B},
  {0x00E6, 0x0721}, {0x00E6, 0x07FF}, {0x0102, 0x07FF}, {0x0121, 0x07FF}, {0x0145, 0x07FF}, {0x016C, 0x07FF}, {0x0199, 0x07FF},
  {0x01CA, 0x07FF}, {0x0202, 0x07FF}, {0x0241, 0x07FF}, {0x0288, 0x07FF}, {0x02D7, 0x07FF}, {0x032F, 0x07FF}, {0x0393, 0x07FF},
  {0x0402, 0x07FF}, {0x0480, 0x07FF}, {0x050C, 0x07FF}, {0x05AA, 0x07FF}, {0x065B, 0x07FF}, {0x0721, 0x07FF}, {0x07FF, 0x07FF}
};

int audio_mute_ctl ( const audio_ctl_parm* ctl, int is_unmute, int mute_time )
{
  if ( ctl == NULL ) {
    DBG ();
    return -EINVAL;
  }
  int val;

  if ( ctl->name == AUDIO_PRIM_MUTE || ctl->name == AUDIO_NAV_MUTE || ctl->name == AUDIO_PHONE_MUTE )
    val = AUDIO_STANDARD_1MS_MUTE / mute_time;
  if ( audio_write_x_reg ( ctl->addr[is_unmute + 1], &val, 1 ) ) {
    DBG();
    return -EIO;
  }

  val = is_unmute ? 0x7FF : 0;
  if ( audio_write_y_reg ( ctl->addr[0], (short int*)&val, 1 ) ) {
    DBG();
    return -EIO;
  }

  return 0;
}

static int audio_scaler_val ( audio_ctl_name ctl, int gain, short int* val )
{
  if ( ( val == NULL ) || ( gain < -AUDIO_CTL_VOL_MIN_DB ) ) {
    DBG ();
    return -EINVAL;
  }

  double tmp;

  switch ( ctl ) {
  case AUDIO_PRIM_VOL:
    val[0] = audio_ctl_vol[gain + AUDIO_CTL_VOL_MIN_DB].a;
    val[1] = audio_ctl_vol[gain + AUDIO_CTL_VOL_MIN_DB].b;
    return 0;
    break;
  case AUDIO_PRIM_SRC_SCALER:
  case AUDIO_NAV_SRC_SCALER:
  case AUDIO_PHONE_SRC_SCALER:
    tmp = pow ( 10.0, ( gain - 6.02 ) / 20.0 );
    break;
  case AUDIO_PRIM_BALANCE_L:
  case AUDIO_PRIM_BALANCE_R:
  case AUDIO_PRIM_FADER_F:
  case AUDIO_PRIM_FADER_R:
    tmp = pow ( 10.0, -gain / 20.0 );
    break;
  case AUDIO_NAV_FADE_F:
  case AUDIO_NAV_FADE_R:
  case AUDIO_PHONE_FADE_F:
  case AUDIO_PHONE_FADE_R:
    tmp = pow ( ( double ) 10.0, ( double ) ( -gain / 20.0 ) );
    val[1] = audio_calc_y ( tmp );
    break;
  case AUDIO_LOUD_B_STATE_LVL:
  case AUDIO_LOUD_T_STATE_LVL:
    tmp = 1.0 - sqrt ( (double)( pow ( 10.0, gain / 20.0 ) - 1 ) / ( pow ( 10.0, ( ctl == AUDIO_LOUD_B_STATE_LVL ? AUDIO_BASS_BOOST_MAX : AUDIO_TREBLE_BOOST_MAX ) / 20.0 ) - 1 ) );
    break;
  case AUDIO_LOUD_B_BOOST_MAX:
  case AUDIO_LOUD_T_BOOST_MAX:
    tmp = ( pow ( 10.0, gain / 20.0 ) - 1 ) / ( ctl == AUDIO_LOUD_B_BOOST_MAX ? 8.0 : 4.0 );
    break;
  case AUDIO_NAV_VOL:
  case AUDIO_PHONE_VOL:
    tmp = ( pow ( ( double ) 10.0, ( double ) ( gain  / 20.0 ) ) ) / 4.0;
    break;
  case AUDIO_P_F_CHNL_GAIN_L:
  case AUDIO_P_F_CHNL_GAIN_R:
  case AUDIO_P_R_CHNL_GAIN_L:
  case AUDIO_P_R_CHNL_GAIN_R:
  case AUDIO_NAV_CHNL_GAIN:
  case AUDIO_PHONE_CHNL_GAIN:
  case AUDIO_PRIM_DOWN_SCALER:
  case AUDIO_P_F_UP_SCALER:
  case AUDIO_P_R_UP_SCALER:
  case AUDIO_P_F_PRE_SCALER:
  case AUDIO_P_R_PRE_SCALER:
  case AUDIO_N_ON_FRONT_L_MIX:
  case AUDIO_N_ON_FRONT_R_MIX:
  case AUDIO_N_ON_REAR_L_MIX:
  case AUDIO_N_ON_REAR_R_MIX:
  case AUDIO_T_ON_FRONT_L_MIX:
  case AUDIO_T_ON_FRONT_R_MIX:
  case AUDIO_T_ON_REAR_L_MIX:
  case AUDIO_T_ON_REAR_R_MIX:
    tmp = pow ( 10.0, gain / 20.0 );
    break;
  default:
    DBG();
    return -EINVAL;
    break;
  }

  val[0] = audio_calc_y ( tmp );
  return 0;
}

int audio_scaler_ctl ( const audio_ctl_parm* ctl, int gain )
{
  if ( ctl == NULL ) {
    DBG ();
    return -EINVAL;
  }

  int index;
  short int y[ctl->addr_cnt];

  if ( audio_scaler_val ( ctl->name, gain, y ) ) {
    DBG ();
    return -EINVAL;
  }
  for ( index = 0; index < ctl->addr_cnt; index++ )
    if ( audio_write_y_reg ( ctl->addr[index], y + index, 1 ) ) {
      DBG ();
      return -EIO;
    }

  return 0;
}

static int audio_tone_bass ( audio_filter_parm* filter_parm, short int *val )
{
  * ( int* ) ( val + 8 ) = * ( int* ) ( val + 2 ) = 0;

  double tmp = tan ( M_PI * ( filter_parm->freq / AUDIO_SAMPLE_FREQ - 0.25 ) ) ;
  audio_data_conv ( -tmp * 0x400000, val );
  audio_data_conv ( 0x200000 + tmp * 0x200000, val + 4 );
  * ( int* ) ( val + 6 ) = * ( int* ) ( val + 4 );

  tmp = pow ( 10.0, abs ( (double)filter_parm->gain ) / 20.0 ) / 16 - 0.0625;
  * ( val + 10 ) = audio_calc_y ( filter_parm->gain > 0 ? tmp : -tmp );

  return 0;
}

static int audio_tone_mid ( audio_filter_parm* filter_parm, short int *val )
{
  double q, tmp = pow ( 10.0, filter_parm->gain / 20.0 );
  * ( val + 6 ) = audio_calc_y ( tmp / 16 - 0.0625 );

  tmp = M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;
  if ( filter_parm->gain >= 0 )
    q = 100.0 * tmp;
  else q = 100.0;

  q = ( filter_parm->fixed_point_parm - q ) / ( filter_parm->fixed_point_parm + q );
  audio_data_conv ( -q * 0x400000, val + 2 );
  audio_data_conv ( ( 0x400000 + q * 0x400000 ) * cos ( tmp * 2 ), val );
  audio_data_conv ( 0x200000 - q * 0x200000, val + 4 );

  return 0;
}

static int audio_tone_treble ( audio_filter_parm* filter_parm, short int *val )
{
  double tmp = filter_parm->freq / AUDIO_SAMPLE_FREQ;

  if ( filter_parm->gain >= 0 )
    tmp -= 0.25;
  else tmp = tmp * pow ( 10.0, filter_parm->gain / 20.0 ) - 0.25;

  tmp = -0.5 * tan ( M_PI * tmp );
  *val = audio_calc_y ( tmp );

  tmp = 0.25 + 0.5 * tmp;
  * ( val + 2 ) = audio_calc_y ( tmp );
  * ( val + 3 ) = audio_calc_y ( -tmp );
  * ( val + 5 ) = audio_calc_y ( pow ( 10.0, filter_parm->gain / 20.0 ) / 16 - 0.0625 );
  * ( val + 4 ) = * ( val + 1 ) = 0;

  return 0;
}

static int audio_loudness_bass ( audio_filter_parm* filter_parm, short int *val )
{
  double tmp = M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;
  double q = ( filter_parm->fixed_point_parm - 100 * tmp ) / ( filter_parm->fixed_point_parm + 100 * tmp );

  audio_data_conv ( -q * 0x400000, val + 2 );
  audio_data_conv ( ( 0x400000 + q * 0x400000 ) * cos ( tmp * 2 ) , val );
  audio_data_conv ( 0x200000 - q * 0x200000, val + 4 );
  audio_data_conv ( q * 0x200000 - 0x200000, val + 6 );

  return 0;
}

static int audio_loudness_treble ( audio_filter_parm* filter_parm, short int *val )
{
  double tmp = 2 * M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;
  double q = -0.5 * ( filter_parm->fixed_point_parm - 50 * tmp ) / ( filter_parm->fixed_point_parm + 50 * tmp );

  * ( val + 1 ) = audio_calc_y ( q );
  *val = audio_calc_y ( ( 0.5 - q ) * cos ( tmp ) );
  * ( val + 2 ) = audio_calc_y ( 0.25 + q / 2 );
  * ( val + 4 ) = audio_calc_y ( -0.25 - q / 2 );
  * ( val + 3 ) = 0;

  return 0;
}

static int audio_dc_filter ( audio_filter_parm* filter_parm, short int *val )
{
  double tmp = tan ( -M_PI * ( filter_parm->freq / AUDIO_SAMPLE_FREQ - 0.25 ) );
  *val = audio_calc_y ( tmp );
  * ( val + 1 ) = * ( val + 2 ) = audio_calc_y ( -0.5 - tmp / 2 );
  return 0;
}

static int audio_peq_peak_notch ( audio_filter_parm* filter_parm, short int *val )
{
  double q, tmp = pow ( 10.0, filter_parm->gain / 20.0 );
  * ( val + 6 ) = audio_calc_y ( tmp / 4.0 - 0.25 );

  if ( filter_parm->gain >= 0 )
    q = 1.0;
  else q = 1.0 / tmp;
  tmp = M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;

  q = 100 * q * tmp / filter_parm->fixed_point_parm;
  q = ( 1 - q ) / ( 1 + q );

  audio_data_conv ( -q * 0x400000, val + 4 );
  audio_data_conv ( 0x200000 - q * 0x200000, val );
  audio_data_conv ( ( 0x400000 + q * 0x400000 ) * cos ( tmp * 2 ), val + 2 );

  return 0;
}

static int audio_iir_peak_notch ( audio_filter_parm* filter_parm, short int *val, int is_path_filter )
{
  double tmp = M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;
  double q = pow ( 10.0, filter_parm->gain / 20.0 );
  double p = 20 * log10 ( 2 + filter_parm->fixed_point_parm / 100 / tmp );

  if ( p > filter_parm->gain ) {
    if ( q >= 1 )
      p = 100;
    else p = 100 / q;
    p = p * tmp / filter_parm->fixed_point_parm;
    p = ( 1 - p ) / ( 1 + p );
  }

  audio_data_conv ( -p * 0x400000, val + 8 );
  tmp = ( 0x400000 + p * 0x400000 ) * cos ( tmp * 2 );
  audio_data_conv ( tmp, val + 6 );
  if ( is_path_filter )
    audio_data_conv ( -tmp / 2, val + 2 );
  else
    audio_data_conv ( -tmp, val + 2 );
  tmp = ( q - 1 ) * ( 1 - p );
  if ( is_path_filter ) {
    audio_data_conv ( 0x200000 + tmp * 0x100000, val );
    audio_data_conv ( p * 0x200000 - tmp * 0x100000, val + 4 );
  } else {
    audio_data_conv ( 0x400000 + tmp * 0x200000, val );
    audio_data_conv ( p * 0x400000 - tmp * 0x200000, val + 4 );
  }

  return 0;
}

static int audio_geq_band_123_filter ( audio_filter_parm* filter_parm, short int *val )
{
  double q, tmp = pow ( 10.0, filter_parm->gain / 20.0 );
  * ( val + 6 ) = audio_calc_y ( tmp / 4.0 - 0.25 );

  if ( filter_parm->gain >= 0 )
    q = 1.0;
  else q = 1.0 / tmp;
  tmp = M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;

  q = 100 * q * tmp / filter_parm->fixed_point_parm;
  q = ( 1 - q ) / ( 1 + q );

  audio_data_conv ( -q * 0x400000, val + 2 );
  audio_data_conv ( 0x200000 - q * 0x200000, val );
  audio_data_conv ( ( 0x400000 + q * 0x400000 ) * cos ( tmp * 2 ), val + 4 );
  /*
  double g, t, d = 1.0;
  double a2, a1, b0, q, temp;

  g = ( pow ( 10.0, filter_parm->gain / 20.0 ) - 1.0 ) / 4.0;
  t = 2 * M_PI * ( ( double ) filter_parm->freq / AUDIO_SAMPLE_FREQ );
  q = ( double ) filter_parm->fixed_point_parm / 100.0;

  if ( filter_parm->gain < 0 )
    d = 1 / ( 4 * g + 1 );

  temp = d * ( t / ( 2 * q ) );
  a2 = -0.5 * ( 1 - temp ) / ( 1 + temp );
  a1 = ( 0.5 - a2 ) * cos ( t );
  b0 = ( 0.5 + a2 ) / 2;

  audio_data_conv ( b0 * 0x800000, val );
  audio_data_conv ( a2 * 0x800000, val + 2 );
  audio_data_conv ( a1 * 0x800000, val + 4 );
  * ( val + 6 ) = audio_calc_y ( g );
  */

  return 0;
}
static int audio_geq_band_45_filter ( audio_filter_parm* filter_parm, short int *val )
{
  double q, tmp = pow ( 10.0, filter_parm->gain / 20.0 );
  * ( val + 3 ) = audio_calc_y ( tmp / 4.0 - 0.25 );

  if ( filter_parm->gain >= 0 )
    q = 1.0;
  else q = 1.0 / tmp;
  tmp = M_PI * filter_parm->freq / AUDIO_SAMPLE_FREQ;

  q = 100 * q * tmp / filter_parm->fixed_point_parm;
  q = ( 1 - q ) / ( 1 + q );

  * ( val + 1 ) = audio_calc_y ( -q /2 );
  * val = audio_calc_y ( 0.25 - q /4 );
  * ( val + 2 ) = audio_calc_y ( ( 0.5 + q * 2 ) * cos ( tmp * 2 ) );
  /*
  double g, t, d = 1.0;
  double a2, a1, b0, q, temp;

  g = ( pow ( 10.0, filter_parm->gain / 20.0 ) - 1.0 ) / 4.0;
  t = 2 * ( M_PI ) * ( ( double ) filter_parm->freq / AUDIO_SAMPLE_FREQ );
  q = ( double ) filter_parm->fixed_point_parm / 100.0;

  if ( filter_parm->gain < 0 )
    d = 1 / ( 4 * g + 1 );

  temp = d * ( t / ( 2 * q ) );
  a2 = -0.5 * ( 1 - temp ) / ( 1 + temp );
  a1 = ( 0.5 - a2 ) * cos ( t );
  b0 = ( 0.5 + a2 ) / 2;

  *val = audio_calc_y ( b0 );
  * ( val + 1 ) = audio_calc_y ( a2 );
  * ( val + 2 ) = audio_calc_y ( a1 );
  * ( val + 3 ) = audio_calc_y ( g );
  */

  return 0;
}


static int audio_filter_val ( audio_ctl_name ctl, audio_filter_parm* filter_parm, short int *val )
{
  if ( ( filter_parm == NULL ) || ( val == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  switch ( ctl ) {
  case AUDIO_TONE_BASS:
    audio_tone_bass ( filter_parm, val );
    break;
  case AUDIO_TONE_MID:
    audio_tone_mid ( filter_parm, val );
    break;
  case AUDIO_TONE_TREBLE:
    audio_tone_treble ( filter_parm, val );
    break;
  case AUDIO_LOUD_B_FILTER:
    audio_loudness_bass ( filter_parm, val );
    break;
  case AUDIO_LOUD_T_FILTER:
    audio_loudness_treble ( filter_parm, val );
    break;
  case AUDIO_PRIM_DC_FILTER:
    audio_dc_filter ( filter_parm, val );
    break;
  case AUDIO_NAV_FILTER_1:
  case AUDIO_NAV_FILTER_2:
  case AUDIO_PHONE_FILTER_1:
  case AUDIO_PHONE_FILTER_2:
    break;
  case AUDIO_GEQ_BAND_1_FILTER:
  case AUDIO_GEQ_BAND_2_FILTER:
  case AUDIO_GEQ_BAND_3_FILTER:
    audio_geq_band_123_filter ( filter_parm, val );
    break;
  case AUDIO_GEQ_BAND_4_FILTER:
  case AUDIO_GEQ_BAND_5_FILTER:
    audio_geq_band_45_filter ( filter_parm, val );
    break;
  default:
    /*Default to calculate PEQ filter coefficients*/
    if ( filter_parm->size == 7 )
      audio_peq_peak_notch ( filter_parm, val );
    else if ( filter_parm->size == 10 )
      audio_iir_peak_notch ( filter_parm, val , 0 );
    else {
      DBG ();
      return -EINVAL;
    }
  }

  return 0;
}

int audio_filter_ctl ( const audio_ctl_parm* ctl, audio_filter_parm* filter_parm )
{
  if ( ( ctl == NULL ) || ( filter_parm == NULL ) ) {
    DBG ();
    return -EINVAL;
  }

  short int y[11];
  audio_filter_val ( ctl->name, filter_parm, y );
  if ( audio_write_y_reg_scratch ( ctl->addr[0], y, filter_parm->size ) ) {
    DBG ();
    return -EIO;
  }

  return 0;
}

