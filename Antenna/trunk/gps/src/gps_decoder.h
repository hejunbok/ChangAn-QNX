#ifndef GPS_DECODER_H
#define GPS_DECODER_H

#include <stdint.h>

typedef struct _satellite_info
{
   uint8_t PRN;
   int8_t elevation;
   uint8_t azimuth;
   uint8_t SNR;
   uint8_t used;
} satellite_info;

typedef struct _gps_data
{
   uint16_t year;
   uint8_t month;
   uint8_t day;
   uint8_t hour;
   uint8_t minute;
   uint8_t second;
   uint16_t millisecond;
   uint8_t quality;
   uint8_t mode;
   uint8_t north_south;
   double latitude;
   uint8_t east_west;
   double longitude;
   double altitude;
   double geoidal_separation;
   double pdop;
   double hdop;
   double vdop;
   double course;
   double speed;
   uint8_t satellites_view;
   uint8_t satellites_used;
   satellite_info satellite_infos[12];
} gps_data;

/***
 * @param buf The data received from udp
 *
 * @param len The length of the buf
 *
 * @param gps The poiter of decoded data
 *
 * @return 0 if decode success, else -1 
 ***/
int gps_decode(const char *buf, int len, gps_data *gps);

void time_zone_fix(gps_data *gps, int fix);

int gps_week(int year, int month, int day, int hour);

int gps_tow(int year, int month, int day, int hour, int minute, int second);

#endif
