#ifndef AUDIO_HERO_DRIVER_API
#define AUDIO_HERO_DRIVER_API

#ifdef __cpluscplus
extern"C" {
#endif
//Audio physical channel
typedef enum {
  AUDIO_PRIM_FRONT_PHYSICAL_CHNL,
  AUDIO_PRIM_REAR_PHYSICAL_CHNL,
  AUDIO_NAV_PHYSICAL_CHNL,
  AUDIO_PHONE_PHYSICAL_CHNL,
  AUDIO_PHYSICAL_CHNLS_CNT
} audio_physical_chnl_name;

//Audio physical input
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

//Audio physical output
typedef enum {
  AUDIO_HOST_IIS_OUT0,
  AUDIO_HOST_IIS_OUT1,
  AUDIO_FRONT_DAC_OUT,
  AUDIO_REAR_DAC_OUT,
  AUDIO_OUTPUTS_CNT
} audio_output_name;

//Audio mix sianal
typedef enum {
  AUDIO_MIX_NAV_SIGNAL,
  AUDIO_MIX_PHONE_SIGNAL,
  AUDIO_MIX_SIGNALS_CNT
} audio_mix_signal;

//Audio tone band
typedef enum {
  AUDIO_BASS,
  AUDIO_MID,
  AUDIO_TREBLE,
  AUDIO_TONE_BANDS_CNT
} audio_tone_band;

//Audio geq band
typedef enum {
  AUDIO_GEQ_BAND_1,
  AUDIO_GEQ_BAND_2,
  AUDIO_GEQ_BAND_3,
  AUDIO_GEQ_BAND_4,
  AUDIO_GEQ_BAND_5,
  AUDIO_GEQ_BANDS_CNT
} audio_geq_band;
//Audio geq mode
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

/*Initialize audio hero driver*/
int audio_hero_init ( void );
/*Connect a physical input to a physical channel*/
int audio_hero_select_source ( audio_input_name input, audio_physical_chnl_name chnl );
/*Mix a signal on speaker*/
int audio_hero_mix_signal_on_spk ( audio_mix_signal signal );
/*Set volume scaler control of a physical channel*/
int audio_hero_set_volume ( audio_physical_chnl_name chnl, int vol );
/*Set loudness offset*/
int audio_hero_set_loudness ( audio_physical_chnl_name chnl, int bass_gain, int treble_gain );
/*Set fade*/
int audio_hero_set_fade ( audio_physical_chnl_name chnl, int fade );
/*Set balance*/
int audio_hero_set_balance ( audio_physical_chnl_name chnl, int balance );
/*Set tone*/
int audio_hero_set_tone ( audio_physical_chnl_name chnl, audio_tone_band band, int tone_val );
/*Set mute state*/
int audio_hero_set_mute ( audio_physical_chnl_name chnl, int is_unmute, int time );
/*Set graphic equalizer*/
int audio_hero_set_geq ( audio_geq_band band, int geq_val );
/*Set preset graphic equalizer*/
int audio_hero_set_preset_geq ( audio_preset_geq_mode mode );
/*Initialize chime*/
int audio_hero_chime_init();
/*Start chime*/
int audio_hero_start_chime ( int freq, int rept );
/*Start click*/
int audio_hero_start_click_clack ( int click_clack );

int audio_recv ( const unsigned char *addr, const int addr_size, unsigned char *data, const int data_num );
int audio_send ( unsigned char *data, const int data_num );

#ifdef __cpluscplus
}
#endif

#endif
