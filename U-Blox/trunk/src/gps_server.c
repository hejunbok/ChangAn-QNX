#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "ubx_m8l.h"
#include "nmea_dec.h"
#include "update_pps_data.h"

#define UART_DEV "/dev/ser3"

int main(void)
{
  int uart_fd;
  uint8_t read_byte;
  int32_t status_code;
  slip_frame_t frame;
  gps_data_t gps_data = {0};
  gps_pps_data_t gps_pps_data = {0};

  uart_fd = open ( UART_DEV, O_RDWR | O_NOCTTY );
  if ( uart_fd < 0 ) {
    printf ( "open %s error: %s\n", UART_DEV, strerror ( errno ) );
    return -1;
  }

  //init_ubx ( uart_fd );

  while ( 1 ) {
    read ( uart_fd, &read_byte, 1 );

    status_code = slip_receive_frame ( &frame, &read_byte, 1 );

    if ( status_code == SLIP_SUCCESS ) {
      printf("%s\n",frame.data);
      if(nmea_decode((const char*)frame.data,&gps_data,frame.data_length) == 0) {
        convert_gps_data_format ( &gps_data, &gps_pps_data );
        update_gps_pps_data( &gps_pps_data);
      }
    }
  }

  return 0;
}