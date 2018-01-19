#ifndef __NMEA_DEC_H__
#define __NMEA_DEC_H__

#include <stdint.h>
#include "types.h"

int32_t nmea_decode(const char *data,gps_data_t* gps_data, uint16_t len);

#endif