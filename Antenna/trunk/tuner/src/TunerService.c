#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hnm/pps.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define FILE_RADIO_CONTROL "/pps/radio/control?delta"
#define FILE_RADIO_STATUS  "/pps/radio/status"
#define FILE_RADIO_TUNERS  "/pps/radio/tuners"

#define TUNER_FREQ 0
#define TUNER_SEEK 1

#define BAND_AM 0
#define BAND_FM 1

#define SEEK_UP   0
#define SEEK_DOWN 1

#define AM_SEEK_STEP 1
#define FM_SEEK_STEP 10

#define AM_FREQ_MIN 526
#define AM_FREQ_MAX 1606
#define FM_FREQ_MIN 8750
#define FM_FREQ_MAX 10800

typedef struct radio_info_tag {
  char   current_tuner[8];
  char   am_status[255];
  char   fm_status[255];
  int    pps_status_fd;
  char   operation;
  int    band;
  unsigned short    frequency;
  int    direction;
  short  seek_step;
  const char* seek;
  const char* tune;
  const char* tuner;
  double am_presets[6];
  double fm_presets[6];
  double am_station;
  double fm_station;
  pps_decoder_t decoder;
  int socket_fd;
  struct sockaddr_in address;
  struct sockaddr_in rx_address;

  void ( *update_status ) ( struct radio_info_tag* radio );
  void ( *get_status ) ( struct radio_info_tag* radio, char* buf );
  bool ( *control ) ( struct radio_info_tag* radio );
} radio_info;

void update_am_fm_status ( radio_info* radio )
{
  if ( radio->band == BAND_AM ) {
    sprintf ( radio->am_status, "am:json:{\"presets\": [%d,%d,%d,%d,%d,%d],\"station\": %d}\ntuner::%s", ( int ) radio->am_presets[0], ( int ) radio->am_presets[1], ( int ) radio->am_presets[2], \
              ( int ) radio->am_presets[3], ( int ) radio->am_presets[4], ( int ) radio->am_presets[5], ( int ) radio->am_station, radio->current_tuner );
    printf ( "Update am:%d\n", ( int ) radio->am_station );
    write ( radio->pps_status_fd, radio->am_status, strlen ( radio->am_status ) );
    printf ( "radio.am_station:%f\n", radio->am_station );
  } else {
    sprintf ( radio->fm_status, "fm:json:{\"presets\": [%f,%f,%f,%f,%f,%f],\"station\": %f}\ntuner::%s", radio->fm_presets[0], radio->fm_presets[1], radio->fm_presets[2], \
              radio->fm_presets[3], radio->fm_presets[4], radio->fm_presets[5], radio->fm_station, radio->current_tuner );
    printf ( "Update fm:%f\n", radio->fm_station );
    write ( radio->pps_status_fd, radio->fm_status, strlen ( radio->fm_status ) );
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

bool am_fm_control ( radio_info* radio )
{
  bool retval = false;
  unsigned char tx_buf[4];
  unsigned char rx_buf[5];
  bool success;
  bool find = false;
  int trys = 0;
  socklen_t addrlen = sizeof ( radio->rx_address );
  unsigned short next_freq = radio->frequency;
  unsigned short rx_freq;

  switch ( radio->operation ) {
  case TUNER_FREQ:
    tx_buf[0] = radio->operation;
    tx_buf[1] = radio->band;
    tx_buf[2] = ( radio->frequency >> 8 ) & 0xFF;
    tx_buf[3] = radio->frequency & 0xFF;
    printf ( "radio.frequency:%x,tx_buf[0]:%d,tx_buf[1]:%d,tx_buf[2]:%d,tx_buf[3]:%d\n", radio->frequency, tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3] );
    success = false;
    while ( !success ) {
      if ( sendto ( radio->socket_fd, tx_buf, 4, 0, ( struct sockaddr * ) &radio->address, sizeof ( radio->address ) ) == 4 ) {
        printf ( "Send Ok.\n" );
      }
      if ( recvfrom ( radio->socket_fd, rx_buf, 3, 0, ( struct sockaddr * ) &radio->rx_address, &addrlen ) == 3 ) {
        printf ( "Receive Ok.rx_buf[0]:%d,rx_buf[1]:%d,rx_buf[2]:%d\n", rx_buf[0], rx_buf[1], rx_buf[2] );
        success = true;
        retval = ( bool ) rx_buf[2];
        break;
      }
      printf ( "Timeout\n" );
    }
    break;
  case TUNER_SEEK:
    while ( 1 ) {
      next_freq += radio->seek_step;
      switch ( radio->band ) {
      case BAND_AM:
        if ( next_freq < AM_FREQ_MIN ) {
          next_freq = AM_FREQ_MAX;
        } else if ( next_freq > AM_FREQ_MAX ) {
          next_freq = AM_FREQ_MIN;
        }
        break;
      case BAND_FM:
        if ( next_freq < FM_FREQ_MIN ) {
          next_freq = FM_FREQ_MAX;
        } else if ( next_freq > FM_FREQ_MAX ) {
          next_freq = FM_FREQ_MIN;
        }
        break;
      default:
        break;
      }
      tx_buf[0] = radio->operation;
      tx_buf[1] = radio->band;
      tx_buf[2] = ( next_freq >> 8 ) & 0xFF;
      tx_buf[3] = next_freq & 0xFF;
      printf ( "next_freq:%x,tx_buf[0]:%d,tx_buf[1]:%d,tx_buf[2]:%d,tx_buf[3]:%d\n", next_freq, tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3] );
      trys = 0;
      while ( trys < 5 ) {
        if ( sendto ( radio->socket_fd, tx_buf, 4, 0, ( struct sockaddr * ) &radio->address, sizeof ( radio->address ) ) == 4 ) {
          printf ( "Send Success.\n" );
        } else {
          printf ( "Send Failed" );
        }
        find = false;
        while ( 1 ) {
          if ( recvfrom ( radio->socket_fd, rx_buf, 5, 0, ( struct sockaddr * ) &radio->rx_address, &addrlen ) == 5 ) {
            printf ( "Receive Success.\n" );
            rx_freq = ( rx_buf[2] << 8 );
            rx_freq += rx_buf[3];
            printf ( "rx_buf[2]:%x,rx_buf[3]:%x,rx_freq:%d\n", rx_buf[2], rx_buf[3], rx_freq );
            if ( rx_freq != next_freq ) {
              continue;
            }
            find = ( bool ) rx_buf[4];
            if ( find ) {
              printf ( "Find.\n" );
            } else {
              printf ( "Not Find.\n" );
            }
            trys = 5;
            break;
          } else {
            printf ( "Receive Timeout.\n" );
            trys++;
            break;
          }
        }
      }
      if ( find || next_freq == radio->frequency ) {
        printf ( "Find:%d\n", find );
        radio->frequency = next_freq;
        if ( radio->band == BAND_AM ) {
          radio->am_station = ( double ) next_freq;
        } else {
          radio->fm_station = ( ( double ) next_freq ) / 100.0;
        }
        retval = true;
        printf ( "Break 1" );
        break;
      }
    }
    printf ( "Break 2" );
    break;
  default:
    break;
  }

  return retval;
}

int main ( int argc, char **argv )
{
  int pps_control_fd = 0;
  int length = 0;
  bool first_time = true;
  char buf[512] = {0};
  struct timeval timeout = {1, 0};
  pps_decoder_error_t err;

  radio_info radio = {0};
  radio.update_status = update_am_fm_status;
  radio.get_status = get_am_fm_status;
  radio.control = am_fm_control;

  bzero ( &radio.address, sizeof ( radio.address ) );
  radio.address.sin_family = AF_INET;
  radio.address.sin_addr.s_addr = inet_addr ( "192.168.0.100" ); //这里不一样
  radio.address.sin_port = htons ( 2808 );

  radio.socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
  setsockopt ( radio.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof ( timeout ) );

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

  max_fd = pps_control_fd;

  length = read ( radio.pps_status_fd, buf, sizeof ( buf ) - 1 );
  printf ( "%s\n", buf );

  radio.get_status ( &radio, buf );

  radio.band = strcmp ( radio.tuner, "am" ) == 0 ? BAND_AM : BAND_FM;
  if ( radio.band == BAND_AM ) {
    radio.frequency = ( unsigned short ) radio.am_station;
  } else {
    radio.frequency = ( unsigned short ) ( radio.fm_station * 100.0 );
  }
  //radio.operation = TUNER_FREQ;
  //if ( radio.control ( &radio ) ) {
  //radio.update_status ( &radio );
  //}

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
      if ( NULL != radio.tuner ) {
        printf ( "tuner:%s\n", radio.tuner );
        printf ( "radio.current_tuner:%s\n", radio.current_tuner );
        if ( strcmp ( radio.tuner, radio.current_tuner ) != 0 ) {
          radio.band = strcmp ( radio.tuner, "am" ) == 0 ? BAND_AM : BAND_FM;
          memcpy ( radio.current_tuner, radio.tuner, strlen ( radio.tuner ) );
          if ( radio.band == BAND_AM ) {
            radio.frequency = ( unsigned short ) radio.am_station;
          } else {
            radio.frequency = ( unsigned short ) ( radio.fm_station * 100.0 );
          }
          radio.operation = TUNER_FREQ;
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
          radio.frequency = ( unsigned short ) radio.am_station;
        } else {
          radio.fm_station = atof ( radio.tune );
          radio.frequency = ( unsigned short ) ( radio.fm_station * 100.0 );
        }
        radio.operation = TUNER_FREQ;
      }
    }

    if ( !first_time ) {
      err = pps_decoder_get_string ( &radio.decoder, "seek", &radio.seek );
      if ( !err ) {
        printf ( "seek:%s\n", radio.seek );
        if ( NULL != radio.seek ) {
          radio.direction = strcmp ( radio.seek, "up" ) == 0 ? SEEK_UP : SEEK_DOWN;
          if ( radio.band == BAND_AM ) {
            radio.seek_step = radio.direction == SEEK_UP ? AM_SEEK_STEP : -AM_SEEK_STEP;
          } else {
            radio.seek_step = radio.direction == SEEK_UP ? FM_SEEK_STEP : -FM_SEEK_STEP;
          }
          radio.operation = TUNER_SEEK;
        }
      }
    }


update:
    pps_decoder_pop ( &radio.decoder );
    pps_decoder_cleanup ( &radio.decoder );

    if ( radio.control ( &radio ) ) {
      radio.update_status ( &radio );
    }
    first_time = false;
  }

  return 0;
}
