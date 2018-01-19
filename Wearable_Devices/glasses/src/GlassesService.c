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
#define PPS_SENSORS_FILE "/pps/qnxcar/sensors?delta"

#define SENSORS_FUEL_KEY        "fuelLevel"
#define SENSORS_SPEED_KEY       "speed"
#define SENSORS_STEERING_KEY    " "
#define SENSORS_LANE_KEY        " "
#define SENSORS_BLIND_KEY       " "
#define SENSORS_FAULT_KEY       " "
#define SENSORS_PERSONALITY_KEY " "

#define PROT_ID        0xE5

#define MSG_HEARTBEAT_ID   0x00
#define MSG_ACK_ID         0x01
#define MSG_DEVICE_ID      0x10
#define MSG_FUEL_ID        0x40
#define MSG_SPEED_ID       0x41
#define MSG_STEERING_ID    0x42
#define MSG_LANE_ID        0x43
#define MSG_BLIND_ID       0x44
#define MSG_FAULT_ID       0x45
#define MSG_PERSONALITY_ID 0x46

#define PROT_ID_INDEX   0x00
#define LEN_INDEX       0x01
#define MSG_ID_INDEX    0x05
#define DATA_LEN_INDEX  0x06
#define DATA_INDEX      0x0A
#define CHECKSUM_INDEX  0x0B

#define DEV_TYPE_INDEX 0x06
#define DEV_ID_INDEX   0x07

#define HEARTBEAT_DEV_ID_INDEX 0x06

#define GLASSES_MAC_1  0x01
#define GLASSES_MAC_2  0x02
#define GLASSES_MAC_3  0x03
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

static int fuel_value;
static int speed_value;
static int steering_value;
static int lane_value;
static int blind_value;
static int fault_value;
static int personality_value;

static int pps_wifi_status_fd = 0;
static int pps_sensors_fd  = 0;

static int socket_fd;
static struct sockaddr_in address, serveraddr;
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

/*Get all status*/
static int get_all_status ( void )
{
  pps_decoder_t decoder;
  pps_decoder_error_t err;
  int length = 0;
  char pps_buf[1024] = {0};

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

  err = pps_decoder_get_int ( &decoder, SENSORS_SPEED_KEY, &speed_value );
  if ( !err ) {
    printf ( "Get speed value:%d.\n", speed_value );
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  return 0;
}

/*Update all status to glasses*/
static int update_all_status ( void )
{
  int length = 0;
  int data_len;// Used for checksum.
  int tx_len;
  uint8_t tx_buf[16] = {0};

  data_len = 6;
  tx_buf[PROT_ID_INDEX] = PROT_ID;
  tx_buf[LEN_INDEX] = data_len + 1;

  printf ( "Current fuel value:%d.\n", fuel_value );
  tx_buf[MSG_ID_INDEX] = MSG_FUEL_ID;
  tx_buf[DATA_LEN_INDEX] = 0x01;
  tx_buf[DATA_INDEX] = ( uint8_t ) fuel_value;
  tx_buf[CHECKSUM_INDEX] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 12;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  printf ( "Current speed value:%d.\n", speed_value );
  tx_buf[MSG_ID_INDEX] = MSG_SPEED_ID;
  tx_buf[DATA_LEN_INDEX] = 0x01;
  tx_buf[DATA_INDEX] = ( uint8_t ) speed_value;
  tx_buf[CHECKSUM_INDEX] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
  tx_len = 12;
  if ( sendto ( socket_fd, tx_buf, tx_len, 0, ( struct sockaddr * ) &clientaddr, sizeof ( clientaddr ) ) == tx_len ) {
    printf ( "Send success with MSG ID:%x.\n", tx_buf[MSG_ID_INDEX] );
  }

  return 0;
}

static int heartbeat_parse ( uint8_t* heartbeat )
{
  switch ( heartbeat[HEARTBEAT_DEV_ID_INDEX] ) {
  case GLASSES_MAC_1:
    printf ( "Receive heartbeat from glasses 1.\n" );
    client1_heartbeat_ok = true;
    is_client1_connected = true;
    break;
  case GLASSES_MAC_2:
    printf ( "Receive heartbeat from glasses 2.\n" );
    client2_heartbeat_ok = true;
    is_client2_connected = true;
    break;
  case GLASSES_MAC_3:
    printf ( "Receive heartbeat from glasses 3.\n" );
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
  case GLASSES_MAC_1:
    //system ( "/opt/notification_tone.sh" );
    printf ( "Glasses 1 register success.\n" );
    memcpy ( &clientaddr1, &clientaddr, sizeof ( clientaddr ) );
    update_all_status();
    is_client1_connected = true;
    break;
  case GLASSES_MAC_2:
    //system ( "/opt/notification_tone.sh" );
    memcpy ( &clientaddr2, &clientaddr, sizeof ( clientaddr ) );
    update_all_status();
    is_client2_connected = true;
    break;
  case GLASSES_MAC_3:
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

/*Hand wifi status pps when wifi status changed*/
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
      system ( "/opt/notification_tone.sh" );
      is_connected = true;
    } else {
      is_connected = false;
    }
  }

  pps_decoder_pop ( &decoder );
  pps_decoder_pop ( &decoder );
  pps_decoder_cleanup ( &decoder );

  if ( is_connected ) {
    printf ( "Update all status.\n" );
    update_all_status();
  }

  return 0;
}

/*Hand sensors pps when sensors status changed*/
static int pps_sensors_hander ( void )
{
  int data_len;// Used for checksum.
  int tx_len;
  bool is_valid = false;
  uint8_t tx_buf[16] = {0};
  char pps_buf[1024] = {0};
  pps_decoder_t decoder;
  pps_decoder_error_t err;

  memset ( pps_buf, 0, sizeof ( pps_buf ) );
  read ( pps_sensors_fd, pps_buf, sizeof ( pps_buf ) - 1 );
  printf ( "***pps_sensors_hander***\n%s\n", pps_buf );
  pps_decoder_initialize ( &decoder, NULL );
  pps_decoder_parse_pps_str ( &decoder, pps_buf );
  pps_decoder_push ( &decoder, NULL );

  err = pps_decoder_get_int ( &decoder, SENSORS_FUEL_KEY, &fuel_value );
  if ( !err ) {
    printf ( "Get fuel value:%d.\n", fuel_value );
    tx_buf[MSG_ID_INDEX] = MSG_FUEL_ID;
    tx_buf[DATA_LEN_INDEX] = 0x01;
    tx_buf[DATA_INDEX] = ( uint8_t ) fuel_value;
    is_valid = true;
    goto transfer;
  }

  err = pps_decoder_get_int ( &decoder, SENSORS_SPEED_KEY, &speed_value );
  if ( !err ) {
    printf ( "Get speed value:%d.\n", speed_value );
    tx_buf[MSG_ID_INDEX] = MSG_SPEED_ID;
    tx_buf[DATA_LEN_INDEX] = 0x01;
    tx_buf[DATA_INDEX] = ( uint8_t ) speed_value;
    is_valid = true;
    goto transfer;
  }

transfer:
  if ( is_valid ) {
    data_len = 6;
    tx_buf[PROT_ID_INDEX] = PROT_ID;
    tx_buf[LEN_INDEX] = data_len + 1;
    tx_buf[CHECKSUM_INDEX] = checksum_fun ( &tx_buf[MSG_ID_INDEX], data_len );
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

void* glasses_send_func ( void* )
{
  int max_fd = 0;
  fd_set rfds;

  if ( ( pps_wifi_status_fd = open ( PPS_WIFI_STATUS_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "PPS wifi stdus file open error!\n" );
    return 0;
  }

  if ( ( pps_sensors_fd = open ( PPS_SENSORS_FILE, O_RDWR ) ) <= 0 ) {
    printf ( "PPS sensors file open error!\n" );
    return 0;
  }

  get_all_status();

  max_fd = pps_wifi_status_fd > pps_sensors_fd ? pps_wifi_status_fd : pps_sensors_fd;

  while ( 1 ) {
    FD_ZERO ( &rfds );
    FD_SET ( pps_wifi_status_fd, &rfds );
    FD_SET ( pps_sensors_fd, &rfds );

    if ( select ( max_fd + 1, &rfds, NULL, NULL, NULL ) < 0 ) {
      if ( errno == EINTR ) {
        continue;
      }
    }

    if ( FD_ISSET ( pps_wifi_status_fd, &rfds ) ) {
      //pps_wifi_hander ( );
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
      printf ( "Glasses 1 is online.\n" );
      client1_heartbeat_ok = false;
    } else {
      printf ( "Glasses 1 is offline.\n" );
      is_client1_connected = false;
    }

    if ( client2_heartbeat_ok ) {
      printf ( "Glasses 2 is online.\n" );
      client2_heartbeat_ok = false;
    } else {
      is_client2_connected = false;
    }

    if ( client3_heartbeat_ok ) {
      printf ( "Glasses 3 is online.\n" );
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

  ret = pthread_create ( &readpthr, NULL, &glasses_send_func, NULL );
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
      default:
        break;
      }
    }
  }

  return 0;
}
