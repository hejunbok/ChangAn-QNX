#ifndef _TYPES_H_
#define _TYPES_H_

#define MSG(x...) printf(x)
/*===========================================================================*
 * Type Declarations
 *===========================================================================*/
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
   uint32_t latitude;
   uint8_t east_west;
   uint32_t longitude;
   int32_t altitude;
   int32_t geoidal_separation;
   uint16_t pdop;
   uint16_t hdop;
   uint16_t vdop;
   uint16_t course;
   uint32_t speed;
   uint8_t satellites_view;
   uint8_t satellites_used;
   uint8_t satellites_PRN[12];
   satellite_info satellite_infos[12];
} gps_data_t;

typedef struct _gps_pps_data
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
} gps_pps_data_t;

#endif