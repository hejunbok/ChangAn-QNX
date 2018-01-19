/********************  Dependent Include files here **********************/
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
#include <pthread.h>
#include <errno.h>

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/
#define PPS_WIFI_STATUS_FILE "/pps/services/wifi/status?delta"
#define PPS_CAR_FILE "/pps/ui?delta"
#define PPS_HVAC_FILE  "/pps/qnxcar/hvac?delta"
#define PPS_SENSORS_FILE "/pps/qnxcar/sensors?delta"

#define PROT_ID        0xE5

#define MSG_HEARTBEAT_ID 0x00
#define MSG_ACK_ID       0x01
#define MSG_DEVICE_ID    0x10
#define MSG_MUSIC_ID     0x30
#define MSG_DOORS_ID     0x31
#define MSG_LIGHTS_ID    0x32
#define MSG_WINDOWS_ID   0x33
#define MSG_HVAC_ID      0x34
#define MSG_FUEL_ID      0x40
#define MSG_LANE_ID      0x43

#define DOORS_FL_KEY    "fl"
#define DOORS_FR_KEY    "fr"
#define DOORS_RL_KEY    "rl"
#define DOORS_RR_KEY    "rr"
#define DOORS_TRUNK_KEY "backdoor"

#define LIGHTS_FRONT_KEY "frontlights"
#define LIGHTS_REAR_KEY  "rearlights"

#define WINDOWS_FL_KEY "window_fl"
#define WINDOWS_FR_KEY "window_fr"
#define WINDOWS_RL_KEY "window_rl"
#define WINDOWS_RR_KEY "window_rr"

#define HVAC_ON_KEY             "airConditioning_all"
#define HVAC_ZONE_ALL_KEY       "zoneLink_all"
#define HVAC_DEFROST_KEY        "defrost_all"
#define HVAC_RECIRCULATION_KEY  "airRecirculation_all"
#define HVAC_TEMPERATURE_L_KEY  "temperature_row1left"
#define HVAC_TEMPERATURE_R_KEY  "temperature_row1right"
#define HVAC_HEATESEAT_L_KEY    "heatedSeat_row1left"
#define HVAC_HEATESEAT_R_KEY    "heatedSeat_row1right"
#define HVAC_FANSPEED_L_KEY     "fanSpeed_row1left"
#define HVAC_FANSPEED_R_KEY     "fanSpeed_row1right"
#define HVAC_FANDIRECTION_L_KEY "fanDirection_row1left"
#define HVAC_FANDIRECTION_R_KEY "fanDirection_row1right"

#define SENSORS_FUEL_KEY        "fuelLevel"
#define SENSORS_LANE_KEY        " "

#define DOORS_FL_ID    0x01
#define DOORS_FR_ID    0x02
#define DOORS_RL_ID    0x03
#define DOORS_RR_ID    0x04
#define DOORS_TRUNK_ID 0x05

#define LIGHTS_FRONT_ID 0x01
#define LIGHTS_REAR_ID  0x02

#define WINDOWS_FL_ID 0x01
#define WINDOWS_FR_ID 0x02
#define WINDOWS_RL_ID 0x03
#define WINDOWS_RR_ID 0x04

#define HVAC_ON_ID             0x00
#define HVAC_ZONE_ALL_ID       0x01
#define HVAC_DEFROST_ID        0x02
#define HVAC_RECIRCULATION_ID  0x03
#define HVAC_TEMPERATURE_L_ID  0x04
#define HVAC_TEMPERATURE_R_ID  0x05
#define HVAC_HEATESEAT_L_ID    0x06
#define HVAC_HEATESEAT_R_ID    0x07
#define HVAC_FANSPEED_L_ID     0x08
#define HVAC_FANSPEED_R_ID     0x09
#define HVAC_FANDIRECTION_L_ID 0x0A
#define HVAC_FANDIRECTION_R_ID 0x0B

#define SWITCH_OFF 0
#define SWITCH_ON  1

#define FACE         0x01
#define FACE_FEET    0x02
#define FEET         0x03
#define DEFROST_FEET 0x04
#define DEFROST      0x05

#define WATCH_MAC_1  0x01
#define WATCH_MAC_2  0x02
#define WATCH_MAC_3  0x03

#define PROT_ID_INDEX 0x00
#define LEN_INDEX     0x01
#define MSG_ID_INDEX  0x05
#define CTL_ID_INDEX  0x06
#define CTL_VAL_INDEX 0x07

#define DEV_TYPE_INDEX 0x06
#define DEV_ID_INDEX   0x07

#define HEARTBEAT_DEV_ID_INDEX 0x06

/*-----------------------------------------------------------
 * LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
 *----------------------------------------------------------*/
typedef signed char    int8_t;
typedef unsigned char  uint8_t;
typedef signed short   int16_t;
typedef unsigned short uint16_t;
typedef signed int     int32_t;
typedef unsigned int   uint32_t;
/*-----------------------------------------------------------
 * GLOBAL CONSTANTS
 *----------------------------------------------------------*/

/*-----------------------------------------------------------
 * GLOBAL VARIABLES
 *----------------------------------------------------------*/
static bool is_connected = false;
static bool is_client1_connected = false;
static bool is_client2_connected = false;
static bool is_client3_connected = false;

static bool client1_heartbeat_ok = false;
static bool client2_heartbeat_ok = false;
static bool client3_heartbeat_ok = false;

static int pps_wifi_status_fd = 0;
static int pps_car_fd  = 0;
static int pps_car_w_fd  = 0;
static int pps_hvac_fd = 0;
static int pps_hvac_w_fd = 0;
static int pps_sensors_fd  = 0;

static int fuel_value;

static int doors_fl_value;
static int doors_fr_value;
static int doors_rl_value;
static int doors_rr_value;
static int doors_trunk_value;
static int lights_f_value;
static int lights_r_value;
static int windows_fl_value;
static int windows_fr_value;
static int windows_rl_value;
static int windows_rr_value;

static int hvac_on_value;
static int hvac_defrost_value;
static int hvac_recir_value;
static int temp_l_value;
static int temp_r_value;
static int heateseat_l_value;
static int heateseat_r_value;
static int fanspeed_l_value;
static int fanspeed_r_value;
static int fandirt_l_value;
static int fandirt_r_value;

static int socket_fd;
static struct sockaddr_in serveraddr;
static struct sockaddr_in clientaddr, clientaddr1, clientaddr2, clientaddr3;

/*===========================================================*/
uint8_t checksum_fun ( uint8_t *data, int len )
{
  uint8_t checksum = 0;

  for ( int i = 0; i < len; i++ ) {
    checksum ^= data[i];
  }

  return checksum;
}

static int get_all_status ( void )
{
  const char* car_val;
  int hvac_val_int;
  bool hvac_val_bool;
  const char* hvac_val_string = NULL;
  int length = 0;
  char pps_buf[1024] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  /*--------------------Get all sensors status start------------------*/
  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  length = read ( pps_sensors_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "***length:%d\n%s\n", length, pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, SENSORS_FUEL_KEY, &fuel_value );
  if ( !err ) {
    printf ( "Get fuel value:%d.\n", fuel_value );
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );
  /*--------------------Get all sensors status end------------------*/

  /*--------------------Get all car status start------------------*/
  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_car_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "***%s\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_string ( &decoder, DOORS_FL_KEY, &car_val );
  if ( !err ) {
    doors_fl_value = atoi ( car_val );
    printf ( "Get doors front left value:%d.\n", doors_fl_value );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_FR_KEY, &car_val );
  if ( !err ) {
    doors_fr_value = atoi ( car_val );
    printf ( "Get doors front right value:%d.\n", doors_fr_value );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_RL_KEY, &car_val );
  if ( !err ) {
    doors_rl_value = atoi ( car_val );
    printf ( "Get doors rear left value:%d.\n", doors_rl_value );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_RR_KEY, &car_val );
  if ( !err ) {
    doors_rr_value = atoi ( car_val );
    printf ( "Get doors rear right value:%d.\n", doors_rr_value );
  }

  err = pps_decoder_get_string ( &decoder, DOORS_TRUNK_KEY, &car_val );
  if ( !err ) {
    doors_trunk_value = atoi ( car_val );
    printf ( "Get doors trunk right value:%d.\n", doors_trunk_value );
  }

  err = pps_decoder_get_string ( &decoder, LIGHTS_FRONT_KEY, &car_val );
  if ( !err ) {
    lights_f_value = atoi ( car_val );
    printf ( "Get lights front value:%d.\n", lights_f_value );
  }

  err = pps_decoder_get_string ( &decoder, LIGHTS_REAR_KEY, &car_val );
  if ( !err ) {
    lights_r_value = atoi ( car_val );
    printf ( "Get lights rear value:%d.\n", lights_r_value );
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_FL_KEY, &car_val );
  if ( !err ) {
    windows_fl_value = atoi ( car_val );
    printf ( "Get windows front left value:%d.\n", windows_fl_value );
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_FR_KEY, &car_val );
  if ( !err ) {
    windows_fr_value = atoi ( car_val );
    printf ( "Get windows front right value:%d.\n", windows_fr_value );
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_RL_KEY, &car_val );
  if ( !err ) {
    windows_rl_value = atoi ( car_val );
    printf ( "Get windows rear left value:%d.\n", windows_rl_value );
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_RR_KEY, &car_val );
  if ( !err ) {
    windows_rr_value = atoi ( car_val );
    printf ( "Get windows rear right value:%d.\n", windows_rr_value );
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );
  /*--------------------Get all car status end------------------*/

  /*--------------------Get all hvac status starts------------------*/
  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_hvac_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, HVAC_TEMPERATURE_L_KEY, &hvac_val_int );
  if ( !err ) {
    temp_l_value = hvac_val_int;
    printf ( "Get temperature left value:%d.\n", temp_l_value );
  }

  err = pps_decoder_get_int ( &decoder, HVAC_TEMPERATURE_R_KEY, &hvac_val_int );
  if ( !err ) {
    temp_r_value = hvac_val_int;
    printf ( "Get temperature right value:%d.\n", temp_r_value );
  }

  err = pps_decoder_get_int ( &decoder, HVAC_HEATESEAT_L_KEY, &hvac_val_int );
  if ( !err ) {
    heateseat_l_value = hvac_val_int;
    printf ( "Get heateseat left value:%d.\n", heateseat_l_value );
  }

  err = pps_decoder_get_int ( &decoder, HVAC_HEATESEAT_R_KEY, &hvac_val_int );
  if ( !err ) {
    heateseat_r_value = hvac_val_int;
    printf ( "Get heateseat right value:%d.\n", heateseat_r_value );
  }

  err = pps_decoder_get_int ( &decoder, HVAC_FANSPEED_L_KEY, &hvac_val_int );
  if ( !err ) {
    fanspeed_l_value = hvac_val_int;
    printf ( "Get fan speed left value:%d.\n", fanspeed_l_value );
  }

  err = pps_decoder_get_int ( &decoder, HVAC_FANSPEED_R_KEY, &hvac_val_int );
  if ( !err ) {
    fanspeed_r_value = hvac_val_int;
    printf ( "Get fan speed right value:%d.\n", fanspeed_r_value );
  }

  err = pps_decoder_get_bool ( &decoder, HVAC_ON_KEY, &hvac_val_bool );
  if ( !err ) {
    hvac_on_value = hvac_val_bool ? SWITCH_ON : SWITCH_OFF;
    printf ( "Get hvac on value:%d.\n", hvac_on_value );
  }

  err = pps_decoder_get_bool ( &decoder, HVAC_DEFROST_KEY, &hvac_val_bool );
  if ( !err ) {
    hvac_defrost_value = hvac_val_bool ? SWITCH_ON : SWITCH_OFF;
    printf ( "Get hvac defrost value:%d.\n", hvac_defrost_value );
  }

  err = pps_decoder_get_bool ( &decoder, HVAC_RECIRCULATION_KEY, &hvac_val_bool );
  if ( !err ) {
    hvac_recir_value = hvac_val_bool ? SWITCH_ON : SWITCH_OFF;
    printf ( "Get hvac recirculation value:%d.\n", hvac_recir_value );
  }

  err = pps_decoder_get_string ( &decoder, HVAC_FANDIRECTION_L_KEY, &hvac_val_string );
  if ( !err ) {
    if ( NULL != hvac_val_string ) {
      printf ( "Fan Direction:%s\n", hvac_val_string );
      if ( strcmp ( hvac_val_string, "face" ) == 0 ) {
        fandirt_l_value = FACE;
      }

      if ( strcmp ( hvac_val_string, "faceAndFeet" ) == 0 ) {
        fandirt_l_value = FACE_FEET;
      }

      if ( strcmp ( hvac_val_string, "feet" ) == 0 ) {
        fandirt_l_value = FEET;
      }

      if ( strcmp ( hvac_val_string, "defrostAndFeet" ) == 0 ) {
        fandirt_l_value = DEFROST_FEET;
      }

      if ( strcmp ( hvac_val_string, "defrost" ) == 0 ) {
        fandirt_l_value = DEFROST;
      }
    }
    printf ( "Get fan direction left value:%d.\n", fandirt_l_value );
  }

  err = pps_decoder_get_string ( &decoder, HVAC_FANDIRECTION_R_KEY, &hvac_val_string );
  if ( !err ) {
    if ( NULL != hvac_val_string ) {
      printf ( "Fan Direction:%s\n", hvac_val_string );
      if ( strcmp ( hvac_val_string, "face" ) == 0 ) {
        fandirt_r_value = FACE;
      }

      if ( strcmp ( hvac_val_string, "faceAndFeet" ) == 0 ) {
        fandirt_r_value = FACE_FEET;
      }

      if ( strcmp ( hvac_val_string, "feet" ) == 0 ) {
        fandirt_r_value = FEET;
      }

      if ( strcmp ( hvac_val_string, "defrostAndFeet" ) == 0 ) {
        fandirt_r_value = DEFROST_FEET;
      }

      if ( strcmp ( hvac_val_string, "defrost" ) == 0 ) {
        fandirt_r_value = DEFROST;
      }
    }
    printf ( "Get fan direction right value:%d.\n", fandirt_r_value );
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );
  /*--------------------Get all hvac status end------------------*/

  return 0;
}

/*Update all status to watch*/
static int update_all_status ( void )
{
  int length = 0;
  int data_len;// Used for checksum.
  int tx_len;
  uint8_t tx_buf[16] = {0};

  /*--------------------Update all sensors status start------------------*/
  data_len = 6;
  tx_buf[PROT_ID_INDEX] = PROT_ID;
  tx_buf[LEN_INDEX] = data_len + 1;

  printf ( "Current fuel value:%d.\n", fuel_value );
  tx_buf[MSG_ID_INDEX] = MSG_FUEL_ID;
  tx_buf[MSG_ID_INDEX + 1] = 0x01;
  tx_buf[MSG_ID_INDEX + 5] = ( uint8_t ) fuel_value;
  tx_buf[MSG_ID_INDEX + 6] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 12;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }
  /*--------------------Update all sensors status end------------------*/

  /*--------------------Update all car status start------------------*/
  data_len = 3;
  tx_buf[PROT_ID_INDEX] = PROT_ID;
  tx_buf[LEN_INDEX] = data_len + 1;

  printf ( "Current doors front left value:%d.\n", doors_fl_value );
  tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
  tx_buf[CTL_ID_INDEX] = DOORS_FL_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) doors_fl_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current doors front right value:%d.\n", doors_fr_value );
  tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
  tx_buf[CTL_ID_INDEX] = DOORS_FR_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) doors_fr_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current doors rear left value:%d.\n", doors_rl_value );
  tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
  tx_buf[CTL_ID_INDEX] = DOORS_RL_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) doors_rl_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current doors rear right value:%d.\n", doors_rr_value );
  tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
  tx_buf[CTL_ID_INDEX] = DOORS_RR_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) doors_rr_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current doors trunk value:%d.\n", doors_trunk_value );
  tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
  tx_buf[CTL_ID_INDEX] = DOORS_TRUNK_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) doors_trunk_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current lights front value:%d.\n", lights_f_value );
  tx_buf[MSG_ID_INDEX] = MSG_LIGHTS_ID;
  tx_buf[CTL_ID_INDEX] = LIGHTS_FRONT_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) lights_f_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current lights rear value:%d.\n", lights_r_value );
  tx_buf[MSG_ID_INDEX] = MSG_LIGHTS_ID;
  tx_buf[CTL_ID_INDEX] = LIGHTS_REAR_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) lights_r_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current windows front left value:%d.\n", windows_fl_value );
  tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
  tx_buf[CTL_ID_INDEX] = WINDOWS_FL_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) windows_fl_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current windows front right value:%d.\n", windows_fr_value );
  tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
  tx_buf[CTL_ID_INDEX] = WINDOWS_FR_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) windows_fr_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current windows rear left value:%d.\n", windows_rl_value );
  tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
  tx_buf[CTL_ID_INDEX] = WINDOWS_RL_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) windows_rl_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current windows rear right value:%d.\n", windows_rr_value );
  tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
  tx_buf[CTL_ID_INDEX] = WINDOWS_RR_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) windows_rr_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }
  /*--------------------Update all car status end------------------*/

  /*--------------------Update all hvac status start------------------*/
  data_len = 3;
  tx_buf[PROT_ID_INDEX] = PROT_ID;
  tx_buf[LEN_INDEX] = data_len + 1;
  tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;

  printf ( "Current temperature left value:%d.\n", temp_l_value );
  tx_buf[CTL_ID_INDEX] = HVAC_TEMPERATURE_L_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) temp_l_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current temperature right value:%d.\n", temp_r_value );
  tx_buf[CTL_ID_INDEX] = HVAC_TEMPERATURE_R_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) temp_r_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current heateseat left value:%d.\n", heateseat_l_value );
  tx_buf[CTL_ID_INDEX] = HVAC_HEATESEAT_L_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) heateseat_l_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current heateseat right value:%d.\n", heateseat_r_value );
  tx_buf[CTL_ID_INDEX] = HVAC_HEATESEAT_R_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) heateseat_r_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current fan speed left value:%d.\n", fanspeed_l_value );
  tx_buf[CTL_ID_INDEX] = HVAC_FANSPEED_L_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) fanspeed_l_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current fan speed right value:%d.\n", fanspeed_r_value );
  tx_buf[CTL_ID_INDEX] = HVAC_FANSPEED_R_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) fanspeed_r_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current hvac on value:%d.\n", hvac_on_value );
  tx_buf[CTL_ID_INDEX] = HVAC_ON_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_on_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current hvac defrost value:%d.\n", hvac_defrost_value );
  tx_buf[CTL_ID_INDEX] = HVAC_DEFROST_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_defrost_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current hvac recirculation value:%d.\n", hvac_recir_value );
  tx_buf[CTL_ID_INDEX] = HVAC_RECIRCULATION_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_recir_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current fan direction left value:%d.\n", fandirt_l_value );
  tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_L_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) fandirt_l_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current fan direction right value:%d.\n", fandirt_r_value );
  tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_R_ID;
  tx_buf[CTL_VAL_INDEX] = ( uint8_t ) fandirt_r_value;
  tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 9;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }
  /*--------------------Update all hvac status end------------------*/

  return 0;
}

static int pps_wifi_hander ( void )
{
  int length = 0;
  int client_cnt = 0;
  char pps_buf[4096] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  length = read ( pps_wifi_status_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "%s\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  pps_decoder_push ( &decoder, "lan_tether_status" );
  err = pps_decoder_get_int ( &decoder, "num_clients", &client_cnt );
  if ( !err ) {
    if ( client_cnt != 0 ) {
      //system ( "/opt/notification_tone.sh" );
      update_all_status();
      is_connected = true;
    } else {
      is_connected = false;
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}

static int pps_car_hander ( void )
{
  const char* car_val;
  int data_len;// Used for checksum.
  int tx_len;
  uint8_t tx_buf[16] = {0};
  bool is_valid = false;
  char pps_buf[255] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_car_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_string ( &decoder, DOORS_FL_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
    tx_buf[CTL_ID_INDEX] = DOORS_FL_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, DOORS_FR_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
    tx_buf[CTL_ID_INDEX] = DOORS_FR_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, DOORS_RL_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
    tx_buf[CTL_ID_INDEX] = DOORS_RL_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, DOORS_RR_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
    tx_buf[CTL_ID_INDEX] = DOORS_RR_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, DOORS_TRUNK_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_DOORS_ID;
    tx_buf[CTL_ID_INDEX] = DOORS_TRUNK_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, LIGHTS_FRONT_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_LIGHTS_ID;
    tx_buf[CTL_ID_INDEX] = LIGHTS_FRONT_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, LIGHTS_REAR_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_LIGHTS_ID;
    tx_buf[CTL_ID_INDEX] = LIGHTS_REAR_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_FL_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
    tx_buf[CTL_ID_INDEX] =  WINDOWS_FL_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_FR_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
    tx_buf[CTL_ID_INDEX] =  WINDOWS_FR_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_RL_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
    tx_buf[CTL_ID_INDEX] =  WINDOWS_RL_ID;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, WINDOWS_RR_KEY, &car_val );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_WINDOWS_ID;
    tx_buf[CTL_ID_INDEX] =  WINDOWS_FL_ID;
    is_valid = true;
    goto transfer;
  }

transfer:
  if ( is_valid ) {
    data_len = 3;
    tx_buf[PROT_ID_INDEX] = PROT_ID;
    tx_buf[LEN_INDEX] = data_len + 1;
    tx_buf[CTL_VAL_INDEX] = atoi ( car_val );
    tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
    tx_len = 9;

    if ( is_client1_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr1, sizeof ( clientaddr1 ) ) == tx_len ) {
        printf ( "Send to client1 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }

    if ( is_client2_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr2, sizeof ( clientaddr2 ) ) == tx_len ) {
        printf ( "Send to client2 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }

    if ( is_client3_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr3, sizeof ( clientaddr3 ) ) == tx_len ) {
        printf ( "Send to client3 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}

static int pps_hvac_hander ( void )
{
  int hvac_val_int;
  bool hvac_val_bool;
  const char* hvac_val_string = NULL;
  int data_len;// Used for checksum.
  int tx_len;
  uint8_t tx_buf[64] = {0};
  char pps_buf[1024] = {0};
  bool is_valid = false;
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_hvac_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, HVAC_TEMPERATURE_L_KEY, &hvac_val_int );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_TEMPERATURE_L_ID;
    tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_val_int;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_int ( &decoder, HVAC_TEMPERATURE_R_KEY, &hvac_val_int );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_TEMPERATURE_R_ID;
    tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_val_int;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_int ( &decoder, HVAC_HEATESEAT_L_KEY, &hvac_val_int );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_HEATESEAT_L_ID;
    tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_val_int;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_int ( &decoder, HVAC_HEATESEAT_R_KEY, &hvac_val_int );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_HEATESEAT_R_ID;
    tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_val_int;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_int ( &decoder, HVAC_FANSPEED_L_KEY, &hvac_val_int );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_FANSPEED_L_ID;
    tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_val_int;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_int ( &decoder, HVAC_FANSPEED_R_KEY, &hvac_val_int );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_FANSPEED_R_ID;
    tx_buf[CTL_VAL_INDEX] = ( uint8_t ) hvac_val_int;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_bool ( &decoder, HVAC_ON_KEY, &hvac_val_bool );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_ON_ID;
    tx_buf[CTL_VAL_INDEX] = hvac_val_bool ? SWITCH_ON : SWITCH_OFF;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_bool ( &decoder, HVAC_DEFROST_KEY, &hvac_val_bool );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_DEFROST_ID;
    tx_buf[CTL_VAL_INDEX] = hvac_val_bool ? SWITCH_ON : SWITCH_OFF;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_bool ( &decoder, HVAC_RECIRCULATION_KEY, &hvac_val_bool );
  if ( !err ) {
    tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
    tx_buf[CTL_ID_INDEX] = HVAC_RECIRCULATION_ID;
    tx_buf[CTL_VAL_INDEX] = hvac_val_bool ? SWITCH_ON : SWITCH_OFF;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_string ( &decoder, HVAC_FANDIRECTION_L_KEY, &hvac_val_string );
  if ( !err ) {
    if ( NULL != hvac_val_string ) {
      printf ( "Fan Direction:%s\n", hvac_val_string );
      if ( strcmp ( hvac_val_string, "face" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_L_ID;
        tx_buf[CTL_VAL_INDEX] = FACE;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "faceAndFeet" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_L_ID;
        tx_buf[CTL_VAL_INDEX] = FACE_FEET;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "feet" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_L_ID;
        tx_buf[CTL_VAL_INDEX] = FEET;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "defrostAndFeet" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_L_ID;
        tx_buf[CTL_VAL_INDEX] = DEFROST_FEET;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "defrost" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_L_ID;
        tx_buf[CTL_VAL_INDEX] = DEFROST;
        is_valid = true;
        goto transfer;
      }
    }
  }

  err = pps_decoder_get_string ( &decoder, HVAC_FANDIRECTION_R_KEY, &hvac_val_string );
  if ( !err ) {
    if ( NULL != hvac_val_string ) {
      printf ( "Fan Direction:%s\n", hvac_val_string );
      if ( strcmp ( hvac_val_string, "face" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_R_ID;
        tx_buf[CTL_VAL_INDEX] = FACE;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "faceAndFeet" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_R_ID;
        tx_buf[CTL_VAL_INDEX] = FACE_FEET;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "feet" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_R_ID;
        tx_buf[CTL_VAL_INDEX] = FEET;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "defrostAndFeet" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_R_ID;
        tx_buf[CTL_VAL_INDEX] = DEFROST_FEET;
        is_valid = true;
        goto transfer;
      }

      if ( strcmp ( hvac_val_string, "defrost" ) == 0 ) {
        tx_buf[MSG_ID_INDEX] = MSG_HVAC_ID;
        tx_buf[CTL_ID_INDEX] = HVAC_FANDIRECTION_R_ID;
        tx_buf[CTL_VAL_INDEX] = DEFROST;
        is_valid = true;
        goto transfer;
      }
    }
  }

transfer:
  if ( is_valid ) {
    data_len = 3;
    tx_buf[PROT_ID_INDEX] = PROT_ID;
    tx_buf[LEN_INDEX] = data_len + 1;
    tx_buf[CTL_VAL_INDEX + 1] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
    tx_len = 9;

    if ( is_client1_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr1, sizeof ( clientaddr1 ) ) == tx_len ) {
        printf ( "Send to client1 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }

    if ( is_client2_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr2, sizeof ( clientaddr2 ) ) == tx_len ) {
        printf ( "Send to client2 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }

    if ( is_client3_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr3, sizeof ( clientaddr3 ) ) == tx_len ) {
        printf ( "Send to client3 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}

static int pps_sensors_hander ( void )
{
  int sensors_val;
  int data_len;// Used for checksum.
  int tx_len;
  uint8_t tx_buf[16] = {0};
  char pps_buf[1024] = {0};
  bool is_valid = false;
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_sensors_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "%s\n*********\n", pps_buf );

  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, SENSORS_FUEL_KEY, &sensors_val );
  if ( !err ) {
    printf ( "Get fuel value:%d.\n", sensors_val );
    tx_buf[MSG_ID_INDEX] = MSG_FUEL_ID;
    tx_buf[MSG_ID_INDEX + 1] = 1;
    tx_buf[MSG_ID_INDEX + 5] = ( uint8_t ) sensors_val;
    is_valid = true;
    goto transfer;
  }

transfer:
  if ( is_valid ) {
    data_len = 6;
    tx_buf[PROT_ID_INDEX] = PROT_ID;
    tx_buf[LEN_INDEX] = data_len + 1;
    tx_buf[MSG_ID_INDEX + 6] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
    tx_len = 12;

    if ( is_client1_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr1, sizeof ( clientaddr1 ) ) == tx_len ) {
        printf ( "Send to client1 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }

    if ( is_client2_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr2, sizeof ( clientaddr2 ) ) == tx_len ) {
        printf ( "Send to client2 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }

    if ( is_client3_connected ) {
      if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr3, sizeof ( clientaddr3 ) ) == tx_len ) {
        printf ( "Send to client3 success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
      }
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}

static int heartbeat_parse ( uint8_t* heartbeat )
{
  switch ( heartbeat[HEARTBEAT_DEV_ID_INDEX] ) {
  case WATCH_MAC_1:
    printf("Receive heartbeat from watch 1.\n");
    client1_heartbeat_ok = true;
    is_client1_connected = true;
    break;
  case WATCH_MAC_2:
    printf("Receive heartbeat from watch 2.\n");
    client2_heartbeat_ok = true;
    is_client2_connected = true;
    break;
  case WATCH_MAC_3:
    printf("Receive heartbeat from watch 3.\n");
    client3_heartbeat_ok = true;
    is_client3_connected = true;
    break;
  default:
    break;
  }

  return 0;
}

static int device_info_parse ( uint8_t* dev_info )
{
  switch ( dev_info[DEV_ID_INDEX] ) {
  case WATCH_MAC_1:
    //system ( "/opt/notification_tone.sh" );
    printf("Watch 1 register success.\n");
    memcpy ( &clientaddr1, &clientaddr, sizeof ( clientaddr ) );
    update_all_status();
    is_client1_connected = true;
    break;
  case WATCH_MAC_2:
    //system ( "/opt/notification_tone.sh" );
    memcpy ( &clientaddr2, &clientaddr, sizeof ( clientaddr ) );
    update_all_status();
    is_client2_connected = true;
    break;
  case WATCH_MAC_3:
    //system ( "/opt/notification_tone.sh" );
    memcpy ( &clientaddr3, &clientaddr, sizeof ( clientaddr ) );
    update_all_status();
    is_client3_connected = true;
    break;
  default:
    break;
  }

  return 0;
}

static int music_cmd_parse ( uint8_t* cmd )
{

  return 0;
}

static int doors_cmd_parse ( uint8_t* cmd )
{
  int ctl_id;
  int ctl_val;
  char tmp_buf[64];
  if ( cmd != NULL ) {
    ctl_id = cmd[CTL_ID_INDEX];
    ctl_val = cmd[CTL_VAL_INDEX];
    switch ( ctl_id ) {
    case DOORS_FL_ID:
      sprintf ( tmp_buf, "%s::%d", DOORS_FL_KEY, ctl_val );
      break;
    case DOORS_FR_ID:
      sprintf ( tmp_buf, "%s::%d", DOORS_FR_KEY, ctl_val );
      break;
    case DOORS_RL_ID:
      sprintf ( tmp_buf, "%s::%d", DOORS_RL_KEY, ctl_val );
      break;
    case DOORS_RR_ID:
      sprintf ( tmp_buf, "%s::%d", DOORS_RR_KEY, ctl_val );
      break;
    case DOORS_TRUNK_ID:
      sprintf ( tmp_buf, "%s::%d", DOORS_RR_KEY, ctl_val );
      break;
    default:
      break;
    }

    write ( pps_car_w_fd, tmp_buf, strlen ( tmp_buf ) );
  }

  return 0;
}

static int lights_cmd_parse ( uint8_t* cmd )
{
  int ctl_id;
  int ctl_val;
  char tmp_buf[64];
  if ( cmd != NULL ) {
    ctl_id = cmd[CTL_ID_INDEX];
    ctl_val = cmd[CTL_VAL_INDEX];
    switch ( ctl_id ) {
    case LIGHTS_FRONT_ID:
      sprintf ( tmp_buf, "%s::%d", LIGHTS_FRONT_KEY, ctl_val );
      break;
    case LIGHTS_REAR_ID:
      sprintf ( tmp_buf, "%s::%d", LIGHTS_REAR_KEY, ctl_val );
      break;
    default:
      break;
    }

    write ( pps_car_w_fd, tmp_buf, strlen ( tmp_buf ) );
  }

  return 0;
}

static int windows_cmd_parse ( uint8_t* cmd )
{
  int ctl_id;
  int ctl_val;
  char tmp_buf[64];
  if ( cmd != NULL ) {
    ctl_id = cmd[CTL_ID_INDEX];
    ctl_val = cmd[CTL_VAL_INDEX];
    switch ( ctl_id ) {
    case WINDOWS_FL_ID:
      sprintf ( tmp_buf, "%s::%d", WINDOWS_FL_KEY, ctl_val );
      break;
    case WINDOWS_FR_ID:
      sprintf ( tmp_buf, "%s::%d", WINDOWS_FR_KEY, ctl_val );
      break;
    case WINDOWS_RL_ID:
      sprintf ( tmp_buf, "%s::%d", WINDOWS_RL_KEY, ctl_val );
      break;
    case WINDOWS_RR_ID:
      sprintf ( tmp_buf, "%s::%d", WINDOWS_RR_KEY, ctl_val );
      break;
    default:
      break;
    }

    write ( pps_car_w_fd, tmp_buf, strlen ( tmp_buf ) );
  }

  return 0;
}

static int hvac_cmd_parse ( uint8_t* cmd )
{
  int ctl_id;
  int ctl_val;
  char tmp_buf[64];
  if ( cmd != NULL ) {
    ctl_id = cmd[CTL_ID_INDEX];
    ctl_val = cmd[CTL_VAL_INDEX];
    switch ( ctl_id ) {
    case HVAC_ON_ID:
      sprintf ( tmp_buf, "%s:b:%s", HVAC_ON_KEY, ctl_val == SWITCH_ON ? "true" : "false" );
      break;
    case HVAC_DEFROST_ID:
      sprintf ( tmp_buf, "%s:b:%s", HVAC_DEFROST_KEY, ctl_val == SWITCH_ON ? "true" : "false" );
      break;
    case HVAC_RECIRCULATION_ID:
      sprintf ( tmp_buf, "%s:b:%s", HVAC_RECIRCULATION_KEY, ctl_val == SWITCH_ON ? "true" : "false" );
      break;
    case HVAC_TEMPERATURE_L_ID:
      sprintf ( tmp_buf, "%s:n:%d", HVAC_TEMPERATURE_L_KEY, ctl_val );
      break;
    case HVAC_TEMPERATURE_R_ID:
      sprintf ( tmp_buf, "%s:n:%d", HVAC_TEMPERATURE_R_KEY, ctl_val );
      break;
    case HVAC_HEATESEAT_L_ID:
      sprintf ( tmp_buf, "%s:n:%d", HVAC_HEATESEAT_L_KEY, ctl_val );
      break;
    case HVAC_HEATESEAT_R_ID:
      sprintf ( tmp_buf, "%s:n:%d", HVAC_HEATESEAT_R_KEY, ctl_val );
      break;
    case HVAC_FANSPEED_L_ID:
      sprintf ( tmp_buf, "%s:n:%d", HVAC_FANSPEED_L_KEY, ctl_val );
      break;
    case HVAC_FANSPEED_R_ID:
      sprintf ( tmp_buf, "%s:n:%d", HVAC_FANSPEED_R_KEY, ctl_val );
      break;
    case HVAC_FANDIRECTION_L_ID:
      switch ( ctl_val ) {
      case FACE:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_L_KEY, "face" );
        break;
      case FACE_FEET:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_L_KEY, "faceAndFeet" );
        break;
      case FEET:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_L_KEY, "feet" );
        break;
      case DEFROST_FEET:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_L_KEY, "defrostAndFeet" );
        break;
      case DEFROST:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_L_KEY, "defrost" );
        break;
      default:
        break;
      }
      break;
    case HVAC_FANDIRECTION_R_ID:
      switch ( ctl_val ) {
      case FACE:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_R_KEY, "face" );
        break;
      case FACE_FEET:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_R_KEY, "faceAndFeet" );
        break;
      case FEET:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_R_KEY, "feet" );
        break;
      case DEFROST_FEET:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_R_KEY, "defrostAndFeet" );
        break;
      case DEFROST:
        sprintf ( tmp_buf, "%s::%s", HVAC_FANDIRECTION_R_KEY, "defrost" );
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }

    write ( pps_hvac_w_fd, tmp_buf, strlen ( tmp_buf ) );
  }

  return 0;
}

void* watch_send_func ( void* )
{
  fd_set rfds;
  int max_fd = 0;

  printf ( "Watch send thread start.\n" );
  printf ( "PPS_WIFI_STATUS_FILE:%s.\n", PPS_WIFI_STATUS_FILE );
  printf ( "pps_wifi_status_fd:%d.\n", pps_wifi_status_fd );
  if ( ( pps_wifi_status_fd = open ( PPS_WIFI_STATUS_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "PPS wifi stdus file open error!\n" );
    return 0;
  }

  if ( ( pps_car_fd = open ( PPS_CAR_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "PPS car file open error!\n" );
    return 0;
  }

  if ( ( pps_car_w_fd = open ( PPS_CAR_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "PPS car file open error!\n" );
    return 0;
  }

  if ( ( pps_hvac_fd = open ( PPS_HVAC_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "HVAC file open error!\n" );
    return 0;
  }

  if ( ( pps_hvac_w_fd = open ( PPS_HVAC_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "HVAC file open error!\n" );
    return 0;
  }

  if ( ( pps_sensors_fd = open ( PPS_SENSORS_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "PPS sensors file open error!\n" );
    return 0;
  }

  printf ( "Get all status start.\n" );
  get_all_status();

  max_fd = pps_wifi_status_fd > pps_car_fd ? pps_wifi_status_fd : pps_car_fd;
  max_fd = max_fd > pps_hvac_fd ? max_fd : pps_hvac_fd;
  max_fd = max_fd > pps_sensors_fd ? max_fd : pps_sensors_fd;

  while ( 1 ) {
    FD_ZERO ( &rfds );
    FD_SET ( pps_wifi_status_fd, &rfds );
    FD_SET ( pps_car_fd, &rfds );
    FD_SET ( pps_hvac_fd, &rfds );
    FD_SET ( pps_sensors_fd, &rfds );

    if ( select ( max_fd + 1, &rfds, NULL, NULL, NULL ) < 0 ) {
      if ( errno == EINTR ) {
        continue;
      }
    }

    if ( FD_ISSET ( pps_wifi_status_fd, &rfds ) ) {
      //pps_wifi_hander ( );
    }

    if ( FD_ISSET ( pps_car_fd, &rfds ) ) {
      pps_car_hander ( );
    }

    if ( FD_ISSET ( pps_hvac_fd, &rfds ) ) {
      pps_hvac_hander ( );
    }

    if ( FD_ISSET ( pps_sensors_fd, &rfds ) ) {
      pps_sensors_hander ( );
    }
  }
}

void* heartbeat_monitor_func ( void* )
{
  while ( 1 ) {
    sleep ( 5 );
    if ( client1_heartbeat_ok ) {
      printf("Watch 1 is online.\n");
      client1_heartbeat_ok = false;
    } else {
      is_client1_connected = false;
    }

    if ( client2_heartbeat_ok ) {
      printf("Watch 2 is online.\n");
      client2_heartbeat_ok = false;
    } else {
      is_client2_connected = false;
    }

    if ( client3_heartbeat_ok ) {
      printf("Watch 3 is online.\n");
      client3_heartbeat_ok = false;
    } else {
      is_client3_connected = false;
    }
  }
}

int main ( int argc, char **argv )
{
  int ret;
  pthread_t readpthr;
  uint8_t rx_buf[64];
  socklen_t addrlen = sizeof ( clientaddr );
  struct timeval timeout = {1, 0};

  socket_fd = socket ( AF_INET, SOCK_DGRAM, 0 );
  bzero ( &serveraddr, sizeof ( serveraddr ) );
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons ( 2809 );
  bind ( socket_fd, ( struct sockaddr * ) &serveraddr, sizeof ( serveraddr ) );
  //setsockopt ( socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof ( timeout ) );

  ret = pthread_create ( &readpthr, NULL, &watch_send_func, NULL );
  ret = pthread_create ( &readpthr, NULL, &heartbeat_monitor_func, NULL );

  while ( 1 ) {
    printf ( "Start receive.\n" );
    if ( recvfrom ( socket_fd, rx_buf, sizeof ( rx_buf ), 0, ( struct sockaddr * ) &clientaddr, &addrlen ) != -1 ) {
      printf ( "Receive Ok.\n" );
      switch ( rx_buf[MSG_ID_INDEX] ) {
      case MSG_HEARTBEAT_ID:
        heartbeat_parse ( rx_buf );
        break;
      case MSG_DEVICE_ID:
        device_info_parse ( rx_buf );
        break;
      case MSG_MUSIC_ID:
        music_cmd_parse ( rx_buf );
        break;
      case MSG_DOORS_ID:
        doors_cmd_parse ( rx_buf );
        break;
      case MSG_LIGHTS_ID:
        lights_cmd_parse ( rx_buf );
        break;
      case MSG_WINDOWS_ID:
        windows_cmd_parse ( rx_buf );
        break;
      case MSG_HVAC_ID:
        hvac_cmd_parse ( rx_buf );
        break;
      default:
        break;
      }
    }
  }

  return 0;
}
