#include "ClusterControl.h"
#include "ipc/ipc.h"

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <hnm/pps.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>

#define MSG(x...) printf(x)

#define SET_SCREEN_KEY  "set_cluster"

#define UI_CFG_KEY "theme"

#define DOORS_FL_KEY    "fl"
#define DOORS_FR_KEY    "fr"
#define DOORS_RL_KEY    "rl"
#define DOORS_RR_KEY    "rr"
#define DOORS_TRUNK_KEY "backdoor"
#define LIGHTS_STATE_KEY "lights_state"

#define SENSORS_SPEED_KEY                 "speed"
#define SENSORS_RPM_KEY                   "rpm"
#define SENSORS_BATTERY_KEY               "batteryLevel"
#define SENSORS_BRAKE_FLUID_KEY           "brakeFluidLevel"
#define SENSORS_WASHER_FLUID_KEY          "washerFluidLevel"
#define SENSORS_FUEL_KEY                  "fuelLevel"
#define SENSORS_COOLANT_KEY               "coolantLevel"
#define SENSORS_TRANSMISSION_FLUID_KEY    "transmissionFluidLevel"
#define SENSORS_ENGINE_OIL_PRESSURE_KEY   "engineOilPressure"
#define SENSORS_TIRE_PRESSURE_FL_KEY      "tirePressureFrontLeft"
#define SENSORS_TIRE_PRESSURE_FR_KEY      "tirePressureFrontRight"
#define SENSORS_TIRE_PRESSURE_RL_KEY      "tirePressureRearLeft"
#define SENSORS_TIRE_PRESSURE_RR_KEY      "tirePressureRearRight"
#define SENSORS_HAND_BRAKE_KEY            "handBrake"
#define SENSORS_BRAKE_PAD_WEAR_FL_KEY     "brakePadWearFrontLeft"
#define SENSORS_BRAKE_PAD_WEAR_FR_KEY     "brakePadWearFrontRight"
#define SENSORS_BRAKE_PAD_WEAR_RL_KEY     "brakePadWearRearLeft"
#define SENSORS_BRAKE_PAD_WEAR_RR_KEY     "brakePadWearRearRight"
#define SENSORS_BLIND_ZONE_LIGHT_L_KEY    "blindZoneLightLeft"
#define SENSORS_BLIND_ZONE_LIGHT_R_KEY    "blindZoneLightRight"

#define MUSIC_POSITION_KEY  "position"
#define MUSIC_INDEX_KEY     "index"
#define MUSIC_DURATION_KEY  "duration"

typedef union car_status_tag {
  uint8_t status;
  struct {
    uint8_t doors_fl: 1;
    uint8_t doors_fr: 1;
    uint8_t doors_rl: 1;
    uint8_t doors_rr: 1;
    uint8_t doors_tk: 1;
    uint8_t lights_f: 1;
    uint8_t lights_r: 1;
  } car;
} car_status_t;

static car_status_t car_control;

static int music_position_value = 0;
static int music_duration_value = 0;

extern int music_track_fd;

extern bool is_track_ok;
extern bool is_track_new;

ClusterControl::ClusterControl(int channel)
   : IChannel(channel)
{
}

ClusterControl::~ClusterControl()
{
}

bool ClusterControl::screen_process(int pps_screen_fd)
{
  if (pps_screen_fd < 0){
    return false;
  }

  const char* screen_val;
  int screen;
  int index = 0;
  unsigned char data[512] = {0};
  char pps_buf[256] = {0};

  pps_decoder_t decoder;
  pps_decoder_error_t err;

  data[index++] = m_channel & 0xFF;
  data[index++] = 0;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_screen_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_string ( &decoder, SET_SCREEN_KEY, &screen_val );
  if ( err == PPS_DECODER_OK ) {
    screen = atoi ( screen_val );
    if (screen >= 0 && screen <= 3) {
        data[index] = 0x0D;
      data[index+1] = screen & 0xFF;
      ipc_send(data, index+2);
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return true;
}

bool ClusterControl::theme_process(int pps_theme_fd)
{
  if (pps_theme_fd < 0){
    return false;
  }

  const char* theme;
  int theme_value = 0x00;
  int index = 0;
  unsigned char data[512] = {0};
  char pps_buf[64] = {0};

  pps_decoder_t decoder;
  pps_decoder_error_t err;

  data[index++] = m_channel & 0xFF;
  data[index++] = 0;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_theme_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_string ( &decoder, UI_CFG_KEY, &theme );
  if ( err == PPS_DECODER_OK ) {
    if ( strcmp ( theme, "classic" ) == 0 ) {
      theme_value = 0x00;
    } else {
      if ( strcmp ( theme, "technology" ) == 0 ) {
        theme_value = 0x01;
      } else {
        if ( strcmp ( theme, "sports" ) == 0 ) {
          theme_value = 0x02;
        }
      }
    }

    data[index] = 0x0C;
    data[index+1] = theme_value & 0xFF;
    ipc_send(data, index+2);
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return true;
}

bool ClusterControl::car_process(int pps_car_fd)
{
    if (pps_car_fd < 0){
    return false;
  }

  const char* car_val;
  int index = 0;
  int tmp_val = 0;
  unsigned char data[512] = {0};
  char pps_buf[255] = {0};

  pps_decoder_t decoder;
  pps_decoder_error_t err;

  data[index++] = m_channel & 0xFF;
  data[index++] = 0;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_car_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_string ( &decoder, DOORS_FL_KEY, &car_val );
  if ( err == PPS_DECODER_OK ) {
    car_control.car.doors_fl = ( uint8_t ) atoi ( car_val );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_FR_KEY, &car_val );
  if ( err == PPS_DECODER_OK ) {
    car_control.car.doors_fr = ( uint8_t ) atoi ( car_val );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_RL_KEY, &car_val );
  if ( err == PPS_DECODER_OK ) {
    car_control.car.doors_rl = ( uint8_t ) atoi ( car_val );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_RR_KEY, &car_val );
  if ( err == PPS_DECODER_OK ) {
    car_control.car.doors_rr = ( uint8_t ) atoi ( car_val );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_TRUNK_KEY, &car_val );
  if ( err == PPS_DECODER_OK ) {
    car_control.car.doors_tk = ( uint8_t ) atoi ( car_val );
  }

  err = pps_decoder_get_string ( &decoder, LIGHTS_STATE_KEY, &car_val );
  if ( err == PPS_DECODER_OK ) {
    tmp_val = atoi ( car_val );
    switch (tmp_val) {
        case 1:
            car_control.car.lights_f = 0x00;
            car_control.car.lights_r = 0x00;
            break;
      case 2:
        car_control.car.lights_f = 0x01;
            car_control.car.lights_r = 0x00;
        break;
      case 3:
        car_control.car.lights_f = 0x01;
            car_control.car.lights_r = 0x01;
        break;
      default:
        break;
    }
  }

  data[index] = 0x0E;
  data[index+1] = car_control.status & 0xFF;
  ipc_send(data, index+2);

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

    return true;
}

bool ClusterControl::sensors_process(int pps_sensors_fd)
{
  if (pps_sensors_fd < 0){
    return false;
  }

  int sensors_val;
  int index = 0;
  unsigned char data[512] = {0};
  char pps_buf[512] = {0};

  pps_decoder_t decoder;
  pps_decoder_error_t err;

  data[index++] = m_channel & 0xFF;
  data[index++] = 0;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_sensors_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, SENSORS_BLIND_ZONE_LIGHT_R_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x00;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_BLIND_ZONE_LIGHT_L_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x01;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_BRAKE_PAD_WEAR_FL_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x02;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_HAND_BRAKE_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x03;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_ENGINE_OIL_PRESSURE_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x04;
    data[index+1] = ( uint8_t )(sensors_val == 0? 0x00:0x01);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_WASHER_FLUID_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x05;
    data[index+1] = ( uint8_t )(sensors_val == 0? 0x00:0x01);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_BRAKE_FLUID_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x06;
    data[index+1] = ( uint8_t )(sensors_val == 0? 0x00:0x01);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_BATTERY_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x07;
    data[index+1] = ( uint8_t )(sensors_val == 0? 0x00:0x01);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_SPEED_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x08;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_RPM_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x09;
    data[index+1] = ( uint8_t )((sensors_val  >> 8)& 0xFF);
    data[index+2] = ( uint8_t ) ( sensors_val & 0xFF);
    ipc_send(data, index+3);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_COOLANT_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x0A;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_FUEL_KEY, &sensors_val );
  if ( err == PPS_DECODER_OK ) {
    data[index] = 0x0B;
    data[index+1] = ( uint8_t )(sensors_val & 0xFF);
    ipc_send(data, index+2);
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return true;
}

bool ClusterControl::music_state_process(int pps_music_state_fd)
{
    if (pps_music_state_fd < 0){
    return false;
  }

  const char* status;
  int length;
  char pps_buf[1024] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  length = read ( pps_music_state_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "***length:%d\n%s\n", length, pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_string ( &decoder, "status", &status );
  if ( err == PPS_DECODER_OK ) {
    if ( strcmp ( status, "idle" ) != 0 && strcmp ( status, "stopped" ) != 0 ) {
      if ( is_track_new ) {
        if ( ( music_track_fd = open ( "/pps/services/mm-player/HMI/track?delta", O_RDWR ) ) <= 0 ) {
          MSG ( "PPS music track file open error:%d!\n", music_track_fd );
          pps_decoder_pop ( &decoder );
          pps_decoder_cleanup ( &decoder );
          return -1;
        }
        MSG ( "PPS music track file open success:%d!\n", music_track_fd );
        is_track_ok = true;
      }
    } else {
      is_track_ok = false;
      is_track_new = true;
      if ( music_track_fd > 0 ) {
        close ( music_track_fd );
        music_track_fd = -1;
      }
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return true;
}

bool ClusterControl::music_status_process(int pps_music_status_fd)
{
    if (pps_music_status_fd < 0){
    return false;
  }

  int position_value;
  int length;
  int index = 0;
  unsigned char data[512] = {0};
  char pps_buf[1024] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  data[index++] = m_channel & 0xFF;
  data[index++] = 0;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  length = read ( pps_music_status_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "***length:%d\n%s\n", length, pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, MUSIC_POSITION_KEY, &position_value );
  if ( err == PPS_DECODER_OK ) {
    position_value /= 1000;
    if ( position_value !=  music_position_value ) {
      music_position_value = position_value ;
      MSG ( "Get music position %d:%d.\n", music_position_value / 60, music_position_value % 60 );
      data[index] = 0x12;
      data[index+1] = ( uint8_t )(( music_position_value / 60 ) & 0xFF);
      ipc_send(data, index+2);
      data[index] = 0x13;
      data[index+1] = ( uint8_t )(( music_position_value % 60 ) & 0xFF);
      ipc_send(data, index+2);
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return true;
}

bool ClusterControl::music_track_process(int pps_music_track_fd)
{
    if (pps_music_track_fd < 0){
    return false;
  }

  int music_index_value;
  int duration_value;
  int length;
  int index = 0;
  unsigned char data[512] = {0};
  char pps_buf[1024] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  data[index++] = m_channel & 0xFF;
  data[index++] = 0;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  length = read ( pps_music_track_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  MSG ( "***length:%d\n%s\n", length, pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );
  err = pps_decoder_get_int ( &decoder, MUSIC_INDEX_KEY, &music_index_value );
  if ( err == PPS_DECODER_OK ) {
    MSG ( "Get music index value:%d.\n", music_index_value );
    data[index] = 0x0F;
    data[index+1] = ( uint8_t )(music_index_value & 0xFF);
    ipc_send(data, index+2);
  }

  pps_decoder_push ( &decoder, "metadata" );
  err = pps_decoder_get_int ( &decoder, MUSIC_DURATION_KEY, &duration_value );
  if ( err == PPS_DECODER_OK ) {
    duration_value /= 1000;
    if ( duration_value !=  music_duration_value ) {
      music_duration_value = duration_value ;
      data[index] = 0x10;
      data[index+1] = ( uint8_t )((music_duration_value / 60) & 0xFF);
      ipc_send(data, index+2);
      data[index] = 0x11;
      data[index+1] = ( uint8_t )((music_duration_value % 60) & 0xFF);
      ipc_send(data, index+2);
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return true;
}
