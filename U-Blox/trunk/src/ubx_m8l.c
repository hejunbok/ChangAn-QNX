/**********************************************************************
 * Include header files
 *********************************************************************/
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
#include "ubx_m8l.h"
#include "nmea_dec.h"

/**********************************************************************
 * Constant and Macro Definitions
 *********************************************************************/

/**********************************************************************
 * Enumerations and Structures and Typedefs
 *********************************************************************/

/**********************************************************************
 * Function Prototypes for Private Functions with File Level Scope
 *********************************************************************/

/* UBX Tx functions */
static void ubx_cfg_prt_uart ( void );
static void ubx_cfg_cfg ( void );
static void ubx_cfg_nmea ( void );

/**********************************************************************
 * Global and Const Variable Defining Definitions / Initializations
 *********************************************************************/

/**********************************************************************
 * Static Variables and Const Variables With File Level Scope
 *********************************************************************/
uint8_t ubx_tx_buf[MAX_TX_DATA_LENTGH + 8];
uint8_t ubx_rx_buf[MAX_RX_DATA_LENTGH + 8];
static const ubx_tx_msg_list_t ubx_tx_msg_list[UBX_TX_MAX_MSG] = {
  /*  CLASS ID      MSG ID      LENGTH      BUILD FUNCTION  */
  0x01,         0x02,       0x00,       NULL,                          /* UBX_NAV_POSLLH_REQ */
  0x01,         0x03,       0x00,       NULL,                          /* UBX_NAV_STATUS_REQ */
  0x01,         0x12,       0x00,       NULL,                          /* UBX_NAV_VELNED_REQ */
  0x01,         0x30,       0x00,       NULL,                          /* UBX_NAV_SVINFO_REQ */
  0x06,         0x00,       0x00,       NULL,                          /* UBX_CFG_PRT_SPI_REQ */
  0x06,         0x01,       0x02,       NULL,                          /* UBX_CFG_MSG_REQ */
  0x06,         0x13,       0x00,       NULL,                          /* UBX_CFG_ANT_REQ */
  0x06,         0x17,       0x00,       NULL,                          /* UBX_CFG_NMEA_REQ */
  0x06,         0x24,       0x00,       NULL,                          /* UBX_CFG_NAV5_REQ */
  0x06,         0x29,       0x00,       NULL,                          /* UBX_CFG_ESFGWT_REQ */
  0x0A,         0x04,       0x00,       NULL,                          /* UBX_MON_VER_REQ */
  0x0A,         0x09,       0x00,       NULL,                          /* UBX_MON_HW_REQ */
  0x10,         0x10,       0x00,       NULL,                          /* UBX_ESF_STATUS_REQ */
  /* End of poll messages */
  0x06,         0x00,       0x14,       ubx_cfg_prt_uart,              /* UBX_CFG_PRT_UART */
  0x06,         0x00,       0x14,       NULL,                          /* UBX_CFG_PRT_SPI */
  0x06,         0x00,       0x14,       NULL,                          /* UBX_CFG_PRT_IIC */
  0x06,         0x00,       0x14,       NULL,                          /* UBX_CFG_PRT_USB */
  0x06,         0x01,       0x03,       NULL,                          /* UBX_CFG_MSG_ANT */
  0x06,         0x01,       0x03,       NULL,                          /* UBX_CFG_MSG_POSLLH */
  0x06,         0x01,       0x03,       NULL,                          /* UBX_CFG_MSG_STATUS */
  0x06,         0x01,       0x03,       NULL,                          /* UBX_CFG_MSG_VELNED */
  0x06,         0x01,       0x03,       NULL,                          /* UBX_CFG_MSG_SVINFO */
  0x06,         0x04,       0x04,       NULL,                          /* UBX_CFG_RST */
  0x06,         0x09,       0x0C,       ubx_cfg_cfg,                   /* UBX_CFG_CFG */
  0x06,         0x13,       0x04,       NULL,                          /* UBX_CFG_ANT */
  0x06,         0x17,       0x04,       ubx_cfg_nmea,                  /* UBX_CFG_NMEA */
  0x06,         0x24,       0x24,       NULL,                          /* UBX_CFG_NAV5 */
  0x06,         0x29,       0x2C,       NULL,                          /* UBX_CFG_ESFGWT */
  0x10,         0x02,       0x10,       NULL,                          /* UBX_ESF_MEAS */
  0x10,         0x02,       0x0C,       NULL,                          /* UBX_ESF_MEAS_SPEED_TICK */
  0x10,         0x02,       0x0C,       NULL                           /* UBX_ESF_MEAS_GYRO */
};

/*ubx initial msg*/
ubx_msg_t ubx_init_msg[] = {
  UBX_CFG_PRT_UART,
  UBX_CFG_NMEA,
  UBX_CFG_CFG
};


/*===========================================================*/
/**********************************************************************
 * Function Definitions
 *********************************************************************/
/**
* Calculates checksum for UBX message.
*
*  * UBX Packet Structure
* Sync   |Sync   |       |       |        |                   |         |         |
* char   |char   |class  | id    | Length |  Payload (Data)   | CK_A    | CK_B    |
* (1byte)|(1byte)|(1byte)|(1byte)|(2bytes)| (variable length) | (1byte) | (1byte) |
*
* Class - 1 byte Class ID
* ID - 1 byte Message ID
* Length - Length of the payload (2 bytes), excluding sync chars, class, id, length
*          and checksum
* Payload - variable length field
* CK_A, CK_B - 2 byte Checksum
*
*
* @param [in] pointer to buffer
* @param [in] maximum length of buffer
*/
uint16_t ubx_calc_checksum ( uint8_t *buf, uint16_t max_len )
{
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;
  uint16_t inx = 0;
  uint16_t data_length = ( ( ( ( uint16_t ) buf[5] ) << 8 ) | buf[4] );

  if ( data_length > ( max_len - 8 ) ) { /* max_len-8 = data only */
    data_length = ( max_len - 8 );
  }

  for ( inx = 2; inx < ( data_length + 6 ); inx++ ) {
    ck_a = ck_a + buf[inx];
    ck_b = ck_b + ck_a;
  }
  return ( ( ( ( uint16_t ) ck_a ) << 8 ) | ck_b );
}

static void ubx_cfg_prt_uart ( void )
{
  ubx_tx_buf[6]  = 0x01; /*portID*/
  ubx_tx_buf[7]  = 0x00; /*reserved*/
  ubx_tx_buf[8]  = 0x00; /*txReady*/
  ubx_tx_buf[9]  = 0x00;
  ubx_tx_buf[10] = 0xC0; /*Mode*/
  ubx_tx_buf[11] = 0x08;
  ubx_tx_buf[12] = 0x00;
  ubx_tx_buf[13] = 0x00;
  ubx_tx_buf[14] = 0x80; /*baudRate*/
  ubx_tx_buf[15] = 0x25;
  ubx_tx_buf[16] = 0x00;
  ubx_tx_buf[17] = 0x00;
  ubx_tx_buf[18] = 0x07; /*inProtoMask*/
  ubx_tx_buf[19] = 0x00;
  ubx_tx_buf[20] = 0x02; /*outProtoMask*/
  ubx_tx_buf[21] = 0x00;
  ubx_tx_buf[22] = 0x00; /*flags*/
  ubx_tx_buf[23] = 0x00;
  ubx_tx_buf[24] = 0x00; /*reserved*/
  ubx_tx_buf[25] = 0x00;
  ubx_tx_buf[26] = 0x00;
}

static void ubx_cfg_nmea ( void )
{
  ubx_tx_buf[6]  = 0x20; /*filter*/
  ubx_tx_buf[7]  = 0x40; /*nmeaVersion*/
  ubx_tx_buf[8]  = 0x00; /*numSV*/
  ubx_tx_buf[9]  = 0x02; /*flags*/
  ubx_tx_buf[10] = 0x72; /*gnssToFilter*/
  ubx_tx_buf[11] = 0x08;
  ubx_tx_buf[12] = 0x00;
  ubx_tx_buf[13] = 0x00;
  ubx_tx_buf[14] = 0x00; /*svNumbering*/
  ubx_tx_buf[15] = 0x01; /*mainTalkerId*/
  ubx_tx_buf[16] = 0x01; /*gsvTalkerId*/
  ubx_tx_buf[17] = 0x01; /*version*/
  ubx_tx_buf[18] = 0x00; /*bdsTalkerId*/
  ubx_tx_buf[19] = 0x00;
  ubx_tx_buf[20] = 0x00; /*reserved*/
  ubx_tx_buf[21] = 0x00;
  ubx_tx_buf[22] = 0x00;
  ubx_tx_buf[23] = 0x00;
  ubx_tx_buf[24] = 0x00;
  ubx_tx_buf[25] = 0x00;
  ubx_tx_buf[26] = 0x00;
}

static void ubx_cfg_cfg ( void )
{
  ubx_tx_buf[6]  = 0x00; /*clearMask*/
  ubx_tx_buf[7]  = 0x00;
  ubx_tx_buf[8]  = 0x00;
  ubx_tx_buf[9]  = 0x00;
  ubx_tx_buf[10] = 0x1F; /*saveMask*/
  ubx_tx_buf[11] = 0x0E;
  ubx_tx_buf[12] = 0x00;
  ubx_tx_buf[13] = 0x00;
  ubx_tx_buf[14] = 0x00; /*loadMask*/
  ubx_tx_buf[15] = 0x00;
  ubx_tx_buf[16] = 0x00;
  ubx_tx_buf[17] = 0x00;
  ubx_tx_buf[18] = 0x01; /*deviceMask*/
}

int ubx_send_msg ( int uart_fd, ubx_msg_t msg_id )
{
  int length = 0, index = 0;
  uint8_t CK_A = 0 ;
  uint8_t CK_B = 0 ;

  if ( ubx_tx_msg_list[msg_id].func != NULL ) {
    memset ( &ubx_tx_buf[6], 0, ubx_tx_msg_list[msg_id].length );

    ubx_tx_buf[0] = UBX_SYNC1;
    ubx_tx_buf[1] = UBX_SYNC2;
    ubx_tx_buf[2] = ubx_tx_msg_list[msg_id].class_id;
    ubx_tx_buf[3] = ubx_tx_msg_list[msg_id].msg_id;
    ubx_tx_buf[4] = ubx_tx_msg_list[msg_id].length;
    ubx_tx_buf[5] = 0x00; /* Length MSB fixed to zero for currently supported msgs */

    if ( ubx_tx_msg_list[msg_id].func != NULL ) {
      ( *ubx_tx_msg_list[msg_id].func ) (); /* build data */
    }

    length = ubx_tx_buf[4] + 6;

    for ( index = 2; index < length; index++ ) {
      CK_A = CK_A + ubx_tx_buf[index];
      CK_B = CK_B + CK_A;
    }
    ubx_tx_buf[length] = CK_A;      /* CK_A Char */
    ubx_tx_buf[length + 1]  = CK_B; /* CK_B Char */

    write ( uart_fd, ubx_tx_buf, length + 2 );
  }

  return 0;
}

int init_ubx ( int uart_fd )
{
  int num = sizeof ( ubx_init_msg ) / sizeof ( ubx_msg_t );
  int i = 0;
  while ( ! ( i >= num ) ) {
    ubx_send_msg ( uart_fd, ubx_init_msg[i] );
    i++;
  }

  return 0;
}

int slip_receive_frame ( slip_frame_t *frame, uint8_t *buf, uint16_t num_bytes )
{
  uint8_t  rx_byte;
  uint16_t cnt;
  static char checksum[3] = {0};

  cnt = 0;

  while ( num_bytes-- > 0 ) {
    rx_byte = buf[cnt++];


    switch ( frame->state ) {
    case RX_FRAME_HUNT:
      /*
      ** Hunt for at least one flag
      ** There must be at least one flag between frames
      */
      if ( rx_byte == GPS_NMEA_START ) {
        /*
        ** Got the required flag...
        ** Eat flag move to next state
        */
        frame->state = RX_FRAME_START;
        frame->data_cursor = 0;
        frame->checksum = 0;
      } else {
        /*
        ** Print all junk characters that we
        **  would normally just eat
        */
      }
      break;

    case RX_FRAME_START:
      switch ( rx_byte ) {
      case GPS_NMEA_CRC:
      case GPS_NMEA_CR:
      case GPS_NMEA_LF:
        /*
        ** Eat flags until they are gone
        */
        break;

      default:
        frame->data_cursor = 0;

        frame->checksum = 0;

        frame->state = RX_FRAME_NORMAL;
        frame->data[frame->data_cursor++] = rx_byte;
        frame->checksum ^=  rx_byte;

        break;
      }
      break;

    case RX_FRAME_NORMAL:
      switch ( rx_byte ) {
      case GPS_NMEA_CR:
      case GPS_NMEA_LF:
      case GPS_NMEA_START:
        /*
        ** This is an error.
        ** Begin looking for a new frame.
        */
        frame->state = RX_FRAME_HUNT;
        frame->data_cursor = 0;
        frame->checksum = 0;
        break;
      case GPS_NMEA_CRC:
        frame->state = RX_FRAME_CRC1;
        sprintf ( checksum, "%02X", frame->checksum );
        break;

      default:
        frame->data[frame->data_cursor++] = rx_byte;
        frame->checksum ^= rx_byte;
        break;
      }
      break;

    case RX_FRAME_CRC1:
      if ( rx_byte == checksum[0] ) {
        /*
        ** checksum is correct.
        */
        frame->state = RX_FRAME_CRC2;
      } else {
        /*
        ** This is an error.
        ** Begin looking for a new frame.
        */
        frame->state = RX_FRAME_HUNT;
        frame->data_cursor = 0;
        frame->checksum = 0;
      }
      break;

    case RX_FRAME_CRC2:
      if ( rx_byte == checksum[1] ) {
        /*
        ** checksum is correct.
        */
        printf("Checksum is correct.\n");
        frame->state = RX_FRAME_TERMINATE;
      } else {
        /*
        ** This is an error.
        ** Begin looking for a new frame.
        */
        frame->state = RX_FRAME_HUNT;
        frame->data_cursor = 0;
        frame->checksum = 0;
      }
      break;

    case RX_FRAME_TERMINATE:
      switch ( rx_byte ) {
      case GPS_NMEA_CR:
        frame->state = RX_FRAME_END;
        break;

      case GPS_NMEA_LF:
      case GPS_NMEA_START:
      case GPS_NMEA_CRC:
      default:
        frame->state = RX_FRAME_HUNT;
        frame->data_cursor = 0;
        frame->checksum = 0;
        break;
      }
      break;

    case RX_FRAME_END:
      switch ( rx_byte ) {
      case GPS_NMEA_LF:
        /*
          ** Frame is correct.
          */
        frame->data_length = frame->data_cursor;
        frame->data[frame->data_cursor] = 0;
        frame->state = RX_FRAME_HUNT;
        return ( SLIP_SUCCESS );
        break;

      case GPS_NMEA_CR:
      case GPS_NMEA_START:
      case GPS_NMEA_CRC:
      default:
        frame->state = RX_FRAME_HUNT;
        frame->data_cursor = 0;
        frame->checksum = 0;
        break;
      }
      break;

    default:
      frame->state = RX_FRAME_HUNT;
      frame->data_cursor = 0;
      frame->checksum = 0;
      break;
    }

    if ( frame->data_cursor >= SLIP_MAX_DATA ) {
      /*
      ** Buffer full without end of frame!
      ** Discard current frame and begin looking for new frame
      */
      frame->state = RX_FRAME_HUNT;
      frame->data_cursor = 0;
      frame->checksum = 0;
    }

  } /* end while */

  return ( SLIP_FRAME_CONTINUES );
}
