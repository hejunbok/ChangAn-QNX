#ifndef _UPDATE_PPS_DATA_
#define _UPDATE_PPS_DATA_

#include <stdint.h>
#include "types.h"

/***
 * @param buf The data received from udp
 *
 * @param len The length of the buf
 *
 * @param gps The poiter of decoded data
 *
 * @return 0 if decode success, else -1
 ***/

int convert_gps_data_format ( const gps_data_t *gps_data, gps_pps_data_t *gps_pps_data );
int update_gps_pps_data( gps_pps_data_t* gps_pps_data);

#endif
