#ifndef AUDIO_HERO_H
#define AUDIO_HERO_H

#include "audio_hero_reg.h"
#include "audio_hero_tbl.h"


//#define SIMULATION

//#include <log/logd.h>
#define LOG_TAG "audio_hero"
//#define MSG(x...) ALOGE(x)
//#define MSG(x...) printf(x)
#define MSG(x...) 
#define DBG(x...) MSG("err@%s %d\n",__FILE__,__LINE__)

#define AUDIO_SAMPLE_FREQ                48000.0
#define SLOT_CNT                         4

#define AUDIO_BASS_BOOST_MAX     19
#define AUDIO_TREBLE_BOOST_MAX   5


typedef enum {
  CHIME_SINE_WAVE,
  CHIME_TRIANGULAR_WAVE,
  CHIME_WAVES_TYPES_NUM
} audio_chime_wave;

typedef enum {
  CHIME_LINEAR_RELEASE,
  CHIME_EXPONENTIAL_RELEASE,
  CHIME_RELEASE_TYPES_NUM
} audio_chime_release;

typedef enum {
  CLICK_CLACK_ROM,
  CLICK_CLACK_XRAM,
  CLICK_CLACK_ORIGINS_NUM
} audio_chime_click;

typedef struct {
  int                  freq;
  int                  dur;
  int                  vol;
  audio_chime_wave     wave_type;
} audio_chime_slot;

typedef struct {
  audio_chime_slot     slots[SLOT_CNT];
  int                  vol;
  int                  attack_time;
  int                  release_time;
  audio_chime_release  release_type;
  int                  tau;
  int                  signal_on_time;
  int                  signal_off_time;
  int                  change_threshold;
  int                  exponential_decay_threshold;
  int                  linear_decay_threshold;
} audio_chime_type;

typedef struct {
  int freq_high_addr;
  int freq_low_addr;
  int dur_addr;
  int vol_addr;
  int wave_type_addr;
  int clip_factor_addr;
} audio_chime_addr;

#define OPER1(name, subaddress, count) name,
typedef enum {
  AUDIO_CTL_TBL
  AUDIO_CTL_CNT,
  AUDIO_CTL_NULL,
} audio_ctl_name;
#undef OPER1

typedef enum {
  AUDIO_FILTER_FLAT,
  AUDIO_FILTER_PEAK_NOTCH,
  AUDIO_FILTER_HI_PASS_1ST,
  AUDIO_FILTER_HI_PASS_2ND,
  AUDIO_FILTER_HI_PASS_3RD_1ST_HI_PASS,
  AUDIO_FILTER_HI_PASS_3RD_2ND_HI_PASS,
  AUDIO_FILTER_HI_PASS_4TH_1ST_HI_PASS,
  AUDIO_FILTER_HI_PASS_4TH_2ND_HI_PASS,
  AUDIO_FILTER_HI_SHELVING,
  AUDIO_FILTER_LO_PASS_1ST,
  AUDIO_FILTER_LO_PASS_2ND,
  AUDIO_FILTER_LO_PASS_3RD_1ST_LO_PASS,
  AUDIO_FILTER_LO_PASS_3RD_2nd_LO_PASS,
  AUDIO_FILTER_LO_PASS_4TH_1ST_LO_PASS,
  AUDIO_FILTER_LO_PASS_4TH_2nd_LO_PASS,
  AUDIO_FILTER_LO_SHELVING,
  AUDIO_FILTER__LAST
} audio_filter_type;

typedef struct {
  audio_ctl_name name;
  int *addr;
  int addr_cnt;
} audio_ctl_parm;

//vol_main1, vol_main2; left, right; front, rear;
typedef struct {
  short int a, b;
} audio_ctl_val;

typedef struct {
  audio_filter_type type;
  int freq;
  int fixed_point_parm;
  int gain;
  int size;
} audio_filter_parm;

typedef enum {
  AUDIO_MAIN_VOL,
  AUDIO_SOURCE_SCALER,
  AUDIO_FADE,
  AUDIO_BALANCE,
  AUDIO_LOUDNESS,
  AUDIO_COMPRESSOR,
  AUDIO_GRAPHIC_EQ,
  AUDIO_DOWN_SCALER,
  AUDIO_TONE,
  AUDIO_PRESCALER,
  AUDIO_LEFT_PARM_EQ,
  AUDIO_RIGHT_PARM_EQ,
  AUDIO_MULTICHNL_DELAY,
  AUDIO_MUTE,
  AUDIO_LIMITER,
  AUDIO_CHNL_GAIN,
  AUDIO_UP_SCALER,
  AUDIO_PHASE_SHIFTER,
  AUDIO_INDEPENDENT_MUTE,
  AUDIO_SUPER_POSITION,
  AUDIO_PATH_FILTER,
  AUDIO_GPF1L,
  AUDIO_GPF1R,
  AUDIO_DC_FILTER,
  AUDIO_FEATURES_CNT,
  AUDIO_NULL_FEATURE
} audio_feature_name;

#define OPER1(name, subaddress) name,
typedef enum {
  AUDIO_MODULE_OUTLET_TBL
  AUDIO_MODULE_OUTLETS_CNT
} audio_module_outlet_name;
#undef OPER1

typedef struct {
  const int subaddr;
  int ref_cnt;
} audio_module_outlet;

#define OPER1(name, subaddress) name,
typedef enum {
  AUDIO_MODULE_INLET_TBL
  AUDIO_MODULE_INLETS_CNT
} audio_module_inlet_name;
#undef OPER1

typedef struct {
  const int subaddr;
  audio_module_outlet* module_outlet;
} audio_module_inlet;

#define OPER1(name, feature, controls, inlet, outlet) name,
typedef enum {
  AUDIO_MODULE_TBL
  AUDIO_MODULES_CNT
} audio_module_name;
#undef OPER1

typedef struct {
  audio_feature_name feature;
  audio_ctl_name* ctls;
  int ctl_cnt;
  audio_module_inlet* module_inlet;
  audio_module_outlet* module_outlet;
} audio_module;

/*Audio physical channel*/
typedef enum {
  AUDIO_PRIM_FRONT_PHYSICAL_CHNL,
  AUDIO_PRIM_REAR_PHYSICAL_CHNL,
  AUDIO_SEC_PHYSICAL_CHNL,
  AUDIO_NAV_PHYSICAL_CHNL,
  AUDIO_PHONE_PHYSICAL_CHNL,
  AUDIO_PHYSICAL_CHNLS_CNT
} audio_physical_chnl_name;

typedef struct {
  const audio_module_name* modules;
  int   cnt;
} audio_physical_chnl_module;

typedef struct {
  audio_module_inlet*  inlet;
  audio_module_outlet* outlet;
  const audio_module*  modules[AUDIO_FEATURES_CNT];
} audio_physical_chnl_info;

typedef enum {
  AUDIO_RADIO,
  AUDIO_AIN0,
  AUDIO_AIN1,
  AUDIO_AIN2,
  AUDIO_AIN3,
  AUDIO_IIS0,
  AUDIO_IIS1,
  AUDIO_IIS2,
  AUDIO_HOST_IIS0,
  AUDIO_HOST_IIS1,
  AUDIO_SPDIF,
  AUDIO_NOISE_GENERATOR,
  AUDIO_SINE_GENERATOR,
  AUDIO_INPUT_NULL,
  AUDIO_INPUTS_CNT
} audio_input_name;

typedef enum {
  AUDIO_DIFFERENTIAL_2VRMS,
  AUDIO_DIFFERENTIAL_1VRMS,
  AUDIO_HIGH_COMMON_MODE_2VRMS,
  AUDIO_HIGH_COMMON_MODE_1VRMS,
  AUDIO_HIGH_COMMON_MODE_0_5VRMS,
  AUDIO_IPOD_HIGH_COMMON_MODE_1_25VRMS
} audio_analog_input_format;

typedef enum {
  AUDIO_PHILIPS_I2S,
  AUDIO_LSB_ALIGNED_16BITS,
  AUDIO_LSB_ALIGNED_18BITS,
  AUDIO_LSB_ALIGNED_20BITS,
  AUDIO_LSB_ALIGNED_24BITS,
  AUDIO_I2S_OFF
} audio_i2s_input_format;

typedef enum {
  AUDIO_ANALOG_SPDIF,
  AUDIO_DIGITAL_SPDIF,
  AUDIO_SPDIF_OFF
} audio_spdif_input_format;

typedef struct {
  audio_input_name name;
  int         pid;
  int         format;
  int         p_s_value;
  int         t_n_value;
} audio_physical_input;

typedef struct {
  audio_input_name name;
  int value;
} audio_prim_sec_input_value;

typedef struct {
  audio_input_name name;
  int value;
} audio_phone_nav_input_value;

typedef enum {
  AUDIO_HOST_IIS_OUT0,
  AUDIO_HOST_IIS_OUT1,
  AUDIO_FRONT_DAC_OUT,
  AUDIO_REAR_DAC_OUT,
  AUDIO_OUTPUTS_CNT
} audio_output_name;

typedef enum {
  AUDIO_STEREO,
  AUDIO_MONO_LEFT,
  AUDIO_MONO_Right
} audio_prim_chnl_input_mode;

typedef enum {
  AUDIO_SPK_LEFT_CHNL,
  AUDIO_SPK_RIGHT_CHNL,
  AUDIO_SPK_CHNLS_CNT
} audio_spk_chnl;

typedef enum {
  AUDIO_FRONT_LEFT_SPK,
  AUDIO_FRONT_RIGHT_SPK,
  AUDIO_REAR_LEFT_SPK,
  AUDIO_REAR_RIGHT_SPK,
  AUDIO_SPKS_CNT
} audio_spk_name;

typedef struct {
  audio_output_name   output;
  audio_spk_chnl      spk_chnl;
} audio_spk;

typedef struct {
  audio_physical_chnl_name  physical_chnl;
  audio_spk_name            spk_name;
  audio_spk_chnl            spk_chnl;
} audio_physical_chnl_spk;

typedef enum {
  AUDIO_MIX_NAV_SIGNAL,
  AUDIO_MIX_PHONE_SIGNAL,
  AUDIO_MIX_SIGNALS_CNT
} audio_mix_signal;

typedef struct {
  audio_mix_signal   signal_name;
  unsigned char      speakers;
  int                gain;
} audio_mix_signal_cfg;


typedef struct {
  audio_output_name   name;
  audio_module_inlet* inlet;
  const audio_module* left_peq;
  const audio_module* right_peq;
  audio_ctl_parm*     left_mixer[AUDIO_MIX_SIGNALS_CNT];
  audio_ctl_parm*     right_mixer[AUDIO_MIX_SIGNALS_CNT];
} audio_physical_output;

typedef enum {
  AUDIO_BASS,
  AUDIO_MID,
  AUDIO_TREBLE,
  AUDIO_TONE_BANDS_CNT
} audio_tone_band;

typedef enum {
  AUDIO_GEQ_BAND_1,
  AUDIO_GEQ_BAND_2,
  AUDIO_GEQ_BAND_3,
  AUDIO_GEQ_BAND_4,
  AUDIO_GEQ_BAND_5,
  AUDIO_GEQ_BANDS_CNT
} audio_geq_band;

typedef enum {
  AUDIO_GEQ_CUSTOM   = 0,
  AUDIO_GEQ_POP      = 1,
  AUDIO_GEQ_ROCK     = 2,
  AUDIO_GEQ_CLASSIC  = 3,
  AUDIO_GEQ_JAZZ     = 4,
  AUDIO_GEQ_VOICE    = 5,
  AUDIO_GEQ_FLAT     = 6,
  AUDIO_GEQ_DEFAULT  = 7,
  AUDIO_GEQ_MODES_CNT
} audio_preset_geq_mode;

typedef struct {
  int gain1;
  int gain2;
} audio_fade_balance;


#endif
