#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hnm/pps.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include "audio_hero_api.h"
#include "FmHardwareInterface.h"

#define FILE_RADIO_CONTROL "/pps/radio/control?delta"
#define FILE_RADIO_STATUS  "/pps/radio/status"
#define FILE_RADIO_TUNERS  "/pps/radio/tuners"


#define BAND_FM 0
#define BAND_AM 1

#define SEEK_UP   0
#define SEEK_DOWN 1

typedef struct radio_info_tag {
  char   current_tuner[8];
  char   am_status[255];
  char   fm_status[255];
  int    pps_status_fd;
  int    band;
  int    frequency;
  int    direction;
  const char* seek;
  const char* tune;
  const char* tuner;
  double am_presets[6];
  double fm_presets[6];
  double am_station;
  double fm_station;
  pps_decoder_t decoder;
  FmHardwareInterface* radiohero;

  void ( *update_status ) ( struct radio_info_tag* radio );
  void ( *get_status ) ( struct radio_info_tag* radio, char* buf );
} radio_info;

void update_am_fm_status ( radio_info* radio )
{
  if ( radio->band == BAND_AM ) {
    sprintf ( radio->am_status, "am:json:{\"presets\": [%d,%d,%d,%d,%d,%d],\"station\": %d}\ntuner::%s", ( int ) radio->am_presets[0], ( int ) radio->am_presets[1], ( int ) radio->am_presets[2], \
              ( int ) radio->am_presets[3], ( int ) radio->am_presets[4], ( int ) radio->am_presets[5], ( int ) radio->am_station, radio->current_tuner );
    printf ( "Update am:%d\n", ( int ) radio->am_station );
    write ( radio->pps_status_fd, radio->am_status, strlen ( radio->am_status ) );
    radio->frequency = ( int ) radio->am_station;
    printf ( "radio.am_station:%f\n", radio->am_station );
    printf ( "To be set radio.frequency:%d\n", radio->frequency );
  } else {
    sprintf ( radio->fm_status, "fm:json:{\"presets\": [%f,%f,%f,%f,%f,%f],\"station\": %f}\ntuner::%s", radio->fm_presets[0], radio->fm_presets[1], radio->fm_presets[2], \
              radio->fm_presets[3], radio->fm_presets[4], radio->fm_presets[5], radio->fm_station, radio->current_tuner );
    printf ( "Update fm:%f\n", radio->fm_station );
    write ( radio->pps_status_fd, radio->fm_status, strlen ( radio->fm_status ) );
    radio->frequency = ( int ) ( radio->fm_station * 1000.0 );
    printf ( "radio.fm_station:%f\n", radio->fm_station );
    printf ( "To be set radio.frequency:%d\n", radio->frequency );
  }

  return;
}

void get_am_fm_status ( radio_info* radio, char* buf )
{
  int i, cnt;
  pps_decoder_error_t err;

  pps_decoder_initialize ( &radio->decoder, NULL );
  pps_decoder_parse_pps_str ( &radio->decoder, buf );
  pps_decoder_push ( &radio->decoder, NULL );

  pps_decoder_get_string ( &radio->decoder, "tuner", &radio->tuner );
  memcpy ( radio->current_tuner, radio->tuner, strlen ( radio->tuner ) );
  printf ( "radio.current_tuner:%s\n", radio->current_tuner );

  pps_decoder_push ( &radio->decoder, "am" );
  err = pps_decoder_push_array ( &radio->decoder, "presets" );
  cnt = pps_decoder_length ( &radio->decoder );
  for ( i = 0; i < cnt; i++ ) {
    err = pps_decoder_get_double ( &radio->decoder, NULL, &radio->am_presets[i] );
    printf ( "radio.am_presets[%d]:%d\n", i, ( int ) radio->am_presets[i] );
  }
  pps_decoder_pop ( &radio->decoder );
  pps_decoder_get_double ( &radio->decoder, "station", &radio->am_station );
  printf ( "radio.am_station:%f\n", radio->am_station );
  pps_decoder_pop ( &radio->decoder );

  pps_decoder_push ( &radio->decoder, "fm" );
  err = pps_decoder_push_array ( &radio->decoder, "presets" );
  cnt = pps_decoder_length ( &radio->decoder );
  printf ( "CNT:%d\n", cnt );
  for ( i = 0; i < cnt; i++ ) {
    err = pps_decoder_get_double ( &radio->decoder, NULL, &radio->fm_presets[i] );
    printf ( "radio.fm_presets[%d]:%d--%d\n", i, ( int ) radio->fm_presets[i], err );
  }
  pps_decoder_pop ( &radio->decoder );

  pps_decoder_get_double ( &radio->decoder, "station", &radio->fm_station );
  printf ( "radio.fm_station:%f\n", radio->fm_station );
  pps_decoder_pop ( &radio->decoder );
  pps_decoder_pop ( &radio->decoder );
  pps_decoder_cleanup ( &radio->decoder );

  return;
}

int main ( int argc, char **argv )
{
  int pps_control_fd = 0;
  int length = 0;
  bool first_time = true;
  char buf[512] = {0};
  pps_decoder_error_t err;

  radio_info radio = {0};
  radio.radiohero = FmHardwareInterface::create();
  radio.update_status = update_am_fm_status;
  radio.get_status = get_am_fm_status;

  fd_set rfds;
  int max_fd = 0;

  if ( ( radio.pps_status_fd = open ( FILE_RADIO_STATUS, O_RDWR ) ) <= 0 ) {
    printf ( "stdus file open error!\n" );
    return 0;
  }

  if ( ( pps_control_fd = open ( FILE_RADIO_CONTROL, O_RDWR ) ) <= 0 ) {
    printf ( "control file open error!\n" );
    return 0;
  }

  length = read ( radio.pps_status_fd, buf, sizeof ( buf ) - 1 );
  printf ( "%s\n", buf );

  radio.get_status ( &radio, buf );

  radio.band = strcmp ( radio.tuner, "am" ) == 0 ? BAND_AM : BAND_FM;
  radio.radiohero->setBand ( radio.band );
  radio.update_status ( &radio );
  radio.radiohero->setFreq ( radio.frequency );
  radio.radiohero->open();

  max_fd = pps_control_fd;


  while ( 1 ) {
    FD_ZERO ( &rfds );
    FD_SET ( pps_control_fd, &rfds );

    if ( select ( max_fd + 1, &rfds, NULL, NULL, NULL ) < 0 ) {
      if ( errno == EINTR ) {
        continue;
      }
    }

    if ( FD_ISSET ( pps_control_fd, &rfds ) ) {
      memset ( buf, 0, sizeof ( buf ) );
      length = read ( pps_control_fd, buf, sizeof ( buf ) - 1 );
      printf ( "%s\n*********\n", buf );
    }

    pps_decoder_initialize ( &radio.decoder, NULL );
    pps_decoder_parse_pps_str ( &radio.decoder, buf );
    pps_decoder_push ( &radio.decoder, NULL );

    err = pps_decoder_get_string ( &radio.decoder, "tuner", &radio.tuner );
    if ( !err ) {
      printf ( "tuner:%s\n", radio.tuner );
      if ( NULL != radio.tuner ) {
        printf ( "tuner:%s\n", radio.tuner );
        printf ( "radio.current_tuner:%s\n", radio.current_tuner );
        if ( strcmp ( radio.tuner, radio.current_tuner ) != 0 ) {
          radio.band = strcmp ( radio.tuner, "am" ) == 0 ? BAND_AM : BAND_FM;
          memcpy ( radio.current_tuner, radio.tuner, strlen ( radio.tuner ) );
          printf ( "radio.current_tuner:%s\n", radio.current_tuner );
          radio.radiohero->setBand ( radio.band );
          goto update;
        }
      }
    }

    err = pps_decoder_get_string ( &radio.decoder, "tune", &radio.tune );
    if ( !err ) {
      printf ( "tune:%s\n", radio.tune );
      if ( NULL != radio.tune ) {
        if ( radio.band == BAND_AM ) {
          radio.am_station = atof ( radio.tune );
          radio.frequency = ( int ) radio.am_station;
        } else {
          radio.fm_station = atof ( radio.tune );
          radio.frequency = ( int ) ( radio.fm_station * 100.0 );
        }
      }
    }

    if ( !first_time ) {
      err = pps_decoder_get_string ( &radio.decoder, "seek", &radio.seek );
      if ( !err ) {
        printf ( "seek:%s\n", radio.seek );
        if ( NULL != radio.seek ) {
          radio.direction = strcmp ( radio.seek, "up" ) == 0 ? SEEK_UP : SEEK_DOWN;
          radio.frequency = radio.radiohero->seek ( radio.direction );
          if ( radio.frequency != 0 ) {
            if ( radio.band == BAND_AM ) {
              radio.am_station = ( double ) radio.frequency;
            } else {
              radio.fm_station = ( ( double ) radio.frequency ) / 1000.0;
            }
          } else {
            printf ( "Seek failed.\n" );
          }
        }
      }
    }

update:
    pps_decoder_pop ( &radio.decoder );
    pps_decoder_cleanup ( &radio.decoder );

    radio.update_status ( &radio );
    radio.radiohero->setFreq ( radio.frequency );
    first_time = false;
  }

  return 0;
}
