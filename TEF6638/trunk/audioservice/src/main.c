#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hnm/pps.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include "audio_hero_api.h"

#define MSG(x...) printf(x)

#define FILE_AUDIO_MIXER "/pps/services/audio/mixer?delta"
#define FILE_AUDIO_VOLUME "/pps/services/audio/volume?delta"
#define VOLUME_INDEX_MAX 11

/*
int volume_gain[11] = {
  -90, -80, -70, -65, -60, -50, -40, -30, -20, -10, 0
};
*/

int volume_gain[11] = {
  -90, -40, -35, -30, -25, -20, -15, -10, -7, -4, 0
};

static int pps_audio_volume_fd;
static int pps_audio_mixer_fd;

static int get_index(double mixer_val)
{
  if (mixer_val <= 5.0) {
    return -7;
  }
  if (mixer_val >5.0 && mixer_val <= 15.0) {
    return -5;
  }
  if (mixer_val >15.0 && mixer_val <= 25.0) {
    return -3;
  }
  if (mixer_val >25.0 && mixer_val <= 35.0) {
    return -2;
  }
  if (mixer_val >35.0 && mixer_val <= 45.0) {
    return -1;
  }
  if (mixer_val >45.0 && mixer_val <= 55.0) {
    return 0;
  }
  if (mixer_val >55.0 && mixer_val <= 65.0) {
    return 1;
  }
  if (mixer_val >65.0 && mixer_val <= 75.0) {
    return 2;
  }
  if (mixer_val >75.0 && mixer_val <= 85.0) {
    return 3;
  }
  if (mixer_val >85.0 && mixer_val <= 95.0) {
    return 5;
  }
  if (mixer_val >95 && mixer_val <= 100.0) {
    return 7;
  }
}

static int pps_volume_hander()
{
  pps_decoder_t decoder;
  pps_decoder_error_t err;
  double volume;
  int volume_index;
  char buf[512] = {0};

  memset ( buf, 0, sizeof ( buf ) );
  read ( pps_audio_volume_fd, buf, sizeof ( buf ) - 1 );
  MSG ( "%s\n*********\n", buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, buf );
  pps_decoder_push ( &decoder, NULL );
  pps_decoder_push ( &decoder, "dat" );

  err = pps_decoder_get_double ( &decoder, "level", &volume );
  if ( !err ) {
    volume_index = ( int ) ( volume / 10 );
    volume_index = volume_index > VOLUME_INDEX_MAX ? VOLUME_INDEX_MAX : volume_index;
    MSG ( "Volume:%f;Volume_index:%d;Volume_gain:%d\n", volume, volume_index, volume_gain[volume_index] );
    if ( audio_hero_set_volume ( AUDIO_PRIM_FRONT_PHYSICAL_CHNL, volume_gain[volume_index] ) == 0 ) {
      MSG ( "*****audio_hero_set_volume() success.\n" );
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}
static int pps_mixer_hander()
{
  double mixer_value;
  int index;
  pps_decoder_t decoder;
  pps_decoder_error_t err;
  char buf[512] = {0};

  memset ( buf, 0, sizeof ( buf ) );
  read ( pps_audio_mixer_fd, buf, sizeof ( buf ) - 1 );
  MSG ( "%s\n*********\n", buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_double ( &decoder, "fade", &mixer_value );
  if(!err){
    MSG("Get fade value:%f.\n",mixer_value);
    index = get_index(mixer_value);
    MSG("Set fade value:%d.\n",index);
    audio_hero_set_fade(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,index);
  }

  err = pps_decoder_get_double ( &decoder, "balance", &mixer_value );
  if(!err){
    MSG("Get balance value:%f.\n",mixer_value);
    index = get_index(mixer_value);
    MSG("Set balance value:%d.\n",index);
    audio_hero_set_balance(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,index);
  }

  err = pps_decoder_get_double ( &decoder, "bass", &mixer_value );
  if(!err){
    MSG("Get bass value:%f.\n",mixer_value);
    index = get_index(mixer_value);
    MSG("Set bass value:%d.\n",index);
    //audio_hero_set_tone(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,AUDIO_BASS,index);
  }

  err = pps_decoder_get_double ( &decoder, "mid", &mixer_value );
  if(!err){
    MSG("Get mid value:%f.\n",mixer_value);
    index = get_index(mixer_value);
    MSG("Set mid value:%d.\n",index);
    //audio_hero_set_tone(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,AUDIO_MID,index);
  }

  err = pps_decoder_get_double ( &decoder, "treble", &mixer_value );
  if(!err){
    MSG("Get treble value:%f.\n",mixer_value);
    index = get_index(mixer_value);
    MSG("Set treble value:%d.\n",index);
    //audio_hero_set_tone(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,AUDIO_TREBLE,index);
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}

int main ( void )
{
  fd_set rfds;
  int max_fd = 0;

  if ( audio_hero_init() == 0 )
    MSG ( "*****audio_hero_init() success.\n" );

  if ( audio_hero_select_source ( AUDIO_HOST_IIS0, AUDIO_PRIM_FRONT_PHYSICAL_CHNL ) == 0 )
    MSG ( "*****audio_hero_select_source() success.\n" );

  if ( ( pps_audio_volume_fd = open ( FILE_AUDIO_VOLUME, O_RDWR | O_CREAT ) ) <= 0 ) {
    MSG ( "PPS volume file open error!\n" );
    return 0;
  }

  if ( ( pps_audio_mixer_fd = open ( FILE_AUDIO_MIXER, O_RDWR | O_CREAT ) ) <= 0 ) {
    MSG ( "PPS mixer file open error!\n" );
    return 0;
  }

  max_fd = pps_audio_volume_fd > pps_audio_mixer_fd ? pps_audio_volume_fd : pps_audio_mixer_fd;
  while ( 1 ) {
    FD_ZERO ( &rfds );
    FD_SET ( pps_audio_volume_fd, &rfds );
    FD_SET ( pps_audio_mixer_fd, &rfds );

    if ( select ( max_fd + 1, &rfds, NULL, NULL, NULL ) < 0 ) {
      if ( errno == EINTR ) {
        continue;
      }
    }

    if ( FD_ISSET ( pps_audio_volume_fd, &rfds ) ) {
      MSG ( "pps_volume_hander!\n" );
      pps_volume_hander();
    }

    if ( FD_ISSET ( pps_audio_mixer_fd, &rfds ) ) {
      MSG ( "pps_mixer_hander!\n" );
      pps_mixer_hander();
    }
  }

  return 0;
}

