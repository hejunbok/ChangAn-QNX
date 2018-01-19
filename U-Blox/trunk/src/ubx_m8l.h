#ifndef _UBX_M8L_H_
#define _UBX_M8L_H_

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include <stdint.h>
/*===========================================================================*
 * Define Constants
 *===========================================================================*/


/*===========================================================================*
 * Define MACROS
 *===========================================================================*/
#define MAX_TX_DATA_LENTGH  60
#define MAX_RX_DATA_LENTGH  300

#define UBX_SYNC1    0xB5
#define UBX_SYNC2    0x62
#define UBX_HEADER   0xB562
#define UBX_VERSION  0x0A04

#define GPS_NMEA_START 0x24 //'$'
#define GPS_NMEA_CRC   0x2A //'*'
#define GPS_NMEA_CR    0x0D
#define GPS_NMEA_LF    0x0A
/* States for receive */
#define  RX_FRAME_HUNT        (0)
#define  RX_FRAME_START       (1)
#define  RX_FRAME_NORMAL      (2)
#define  RX_FRAME_CRC1        (3)
#define  RX_FRAME_CRC2        (4)
#define  RX_FRAME_TERMINATE   (5)
#define  RX_FRAME_END         (6)

/* Return values */
#define SLIP_FAILURE          (-1)
#define SLIP_SUCCESS          (0)
#define SLIP_FRAME_EMPTY      (1)
#define SLIP_FRAME_CONTINUES  (2)

#define SLIP_MAX_DATA         255

typedef  void (*void_fptr) (void);
typedef  void (*uint8_fptr) (uint8_t*,void*);

typedef enum ubx_msg_tag
{
   UBX_NAV_POSLLH_REQ,
   UBX_NAV_STATUS_REQ,
   UBX_NAV_VELNED_REQ,
   UBX_NAV_SVINFO_REQ,
   UBX_CFG_PRT_SPI_REQ,
   UBX_CFG_MSG_REQ,
   UBX_CFG_ANT_REQ,
   UBX_CFG_NMEA_REQ,
   UBX_CFG_NAV5_REQ,
   UBX_CFG_ESFGWT_REQ,
   UBX_MON_VER_REQ,
   UBX_MON_HW_REQ,
   UBX_ESF_STATUS_REQ,
   /* End of poll messages */
   UBX_CFG_PRT_UART,
   UBX_CFG_PRT_SPI,
   UBX_CFG_PRT_IIC,
   UBX_CFG_PRT_USB,
   UBX_CFG_MSG_ANT,
   UBX_CFG_MSG_POSLLH,
   UBX_CFG_MSG_STATUS,
   UBX_CFG_MSG_VELNED,
   UBX_CFG_MSG_SVINFO,
   UBX_CFG_RST,
   UBX_CFG_CFG,
   UBX_CFG_ANT,
   UBX_CFG_NMEA,
   UBX_CFG_NAV5,
   UBX_CFG_ESFGWT,
   UBX_ESF_MEAS,
   UBX_ESF_MEAS_SPEED_TICK,
   UBX_ESF_MEAS_GYRO,
   UBX_TX_MAX_MSG
} ubx_msg_t;

typedef enum ubx_rx_msg_tag
{
   UBX_RX_POSllH,
   UBX_RX_STATUS,
   UBX_RX_VELNED,
   UBX_RX_SVINFO,
   UBX_RX_ACK_NAK,
   UBX_RX_ACK_ACK,
   UBX_RX_CFG_PRT_SPI,
   UBX_RX_CFG_MSG,
   UBX_RX_CFG_ANT,
   UBX_RX_CFG_NMEA,
   UBX_RX_CFG_NAV5,
   UBX_RX_CFG_ESFGWT,
   UBX_RX_MON_VER,
   UBX_RX_MON_HW,
   UBX_RX_ESF_STATUS,
   UBX_RX_MAX_MSG
} ubx_rx_msg_t;

typedef struct ubx_tx_msg_list_tag
{
   uint8_t   class_id;
   uint8_t   msg_id;
   uint8_t   length;
   void_fptr func;
} ubx_tx_msg_list_t;

typedef struct ubx_rx_msg_list_tag
{
   uint8_t   class_id;
   uint8_t   msg_id;
   uint8_fptr func;
} ubx_rx_msg_list_t;

/* Slip frame context block */
typedef struct _slip_frame_tag {
  uint16_t data_length;
  uint16_t data_cursor;
  uint16_t state;
  uint8_t  checksum;
  uint32_t align;
  uint8_t  data[SLIP_MAX_DATA];
} slip_frame_t;
 /*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
int init_ubx(int uart_fd);
int slip_receive_frame ( slip_frame_t *frame, uint8_t *buf, uint16_t num_bytes );



















#endif