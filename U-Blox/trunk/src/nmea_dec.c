#include <stdio.h>
#include <string.h>
#include "nmea_dec.h"

#define GPS_NMEA_GGA   "GNGGA"
#define GPS_NMEA_GSA   "GNGSA"
#define GPS_NMEA_GSV   "GPGSV"
#define GPS_NMEA_RMC   "GNRMC"
#define GPS_NMEA_VTG   "GNVTG"
#define GPS_NMEA_ZDA   "GNZDA"

static int8_t gsv_complete = 1;

static int32_t nmea_gga_decode ( const char *data, gps_data_t* gps_data, uint16_t len );
static int32_t nmea_gsa_decode ( const char *data, gps_data_t* gps_data, uint16_t len );
static int32_t nmea_gsv_decode ( const char *data, gps_data_t* gps_data, uint16_t len );
static int32_t nmea_rmc_decode ( const char *data, gps_data_t* gps_data, uint16_t len );
static int32_t nmea_vtg_decode ( const char *data, gps_data_t* gps_data, uint16_t len );
static int32_t nmea_zda_decode ( const char *data, gps_data_t* gps_data, uint16_t len );
static uint32_t gps_tool_strtol ( const char *buf, int8_t len );
static uint64_t gps_tool_strtodtoll ( const char *buf, int8_t len, uint32_t factor );

int32_t nmea_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int32_t ret = 0;

  if ( strncmp ( data, GPS_NMEA_GGA, strlen ( GPS_NMEA_GGA ) ) == 0 ) {
    MSG("Start decode GPS_NMEA_GGA.\n");
    nmea_gga_decode ( data, gps_data, len );
  } else if ( strncmp ( data, GPS_NMEA_GSA, strlen ( GPS_NMEA_GSA ) ) == 0 ) {
    MSG("Start decode GPS_NMEA_GSA.\n");
    nmea_gsa_decode ( data, gps_data, len );
  } else if ( strncmp ( data, GPS_NMEA_GSV, strlen ( GPS_NMEA_GSV ) ) == 0 ) {
    MSG("Start decode GPS_NMEA_GSV.\n");
    nmea_gsv_decode ( data, gps_data, len );
  } else if ( strncmp ( data, GPS_NMEA_RMC, strlen ( GPS_NMEA_RMC ) ) == 0 ) {
    MSG("Start decode GPS_NMEA_RMC.\n");
    nmea_rmc_decode ( data, gps_data, len );
  } else if ( strncmp ( data, GPS_NMEA_VTG, strlen ( GPS_NMEA_VTG ) ) == 0 ) {
    MSG("Start decode GPS_NMEA_VTG.\n");
    nmea_vtg_decode ( data, gps_data, len );
  } else if ( strncmp ( data, GPS_NMEA_ZDA, strlen ( GPS_NMEA_ZDA ) ) == 0 ) {
    MSG("Start decode GPS_NMEA_ZDA.\n");
    nmea_zda_decode ( data, gps_data, len );
  } else {
    ret = -1;
  }

  return ret;
}

int32_t nmea_gga_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int32_t ret = 0;
  const char *tmp = data;

  //IDMSG
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Hours, minutes, seconds, sub-seconds of position in UTC
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    //hhmmss.xxx
    gps_data->hour = gps_tool_strtol ( data, 2 );

    gps_data->minute = gps_tool_strtol ( data + 2, 2 );

    gps_data->second = gps_tool_strtol ( data + 4, 2 );

    if ( tmp - data > 7 ) {
      gps_data->millisecond = gps_tool_strtol ( data + 7, tmp - data - 7 );
    }
  }
  data = tmp + 1;

  //Latitude
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->latitude = gps_tool_strtodtoll ( data, tmp - data, 10000 );
  }
  data = tmp + 1;

  //N (North) or S (South)
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->north_south = ( *data == 'N' ) ? 1 : 2;
  }
  data = tmp + 1;

  //Longitude
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->longitude =  gps_tool_strtodtoll ( data, tmp - data, 10000 );
  }
  data = tmp + 1;

  //E (East) or W (West)
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->east_west = ( *data == 'E' ) ? 1 : 2;
  }
  data = tmp + 1;

  //GPS quality Indicator: 0 = No GPS, 1 = GPS, 2 = DGPS, 6 = DR
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->quality = gps_tool_strtol ( data, 1 );
  }
  data = tmp + 1;

  //Number of satellites in use
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->satellites_used = gps_tool_strtol ( data, 1 );
  }
  data = tmp + 1;

  //Horizontal Dilution of Precision (HDOP)
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->hdop = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }
  data = tmp + 1;

  //Antenna altitude in meters, M = Meters
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    if ( data[0] == '-' ) {
      gps_data->altitude = -gps_tool_strtodtoll ( data + 1, tmp - data - 1, 10 );
    } else {
      gps_data->altitude = gps_tool_strtodtoll ( data, tmp - data, 10 );
    }
  }
  data = tmp + 1;

  //M
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Geoidal separation
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    if ( data[0] == '-' ) {
      gps_data->geoidal_separation = gps_tool_strtodtoll ( data + 1, tmp - data - 1, 10 );
      gps_data->geoidal_separation = -gps_data->geoidal_separation;
    } else {
      gps_data->geoidal_separation = gps_tool_strtodtoll ( data, tmp - data, 10 );
    }
  }
  data = tmp + 1;

  //M
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Age of differential GPS data
  tmp = strchr ( data, ',' );
  data = tmp + 1;

  //Differential Reference Station ID

  return ret;
}

int32_t nmea_gsa_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int8_t ret = 0;
  const char *raw = data;
  const char *tmp = data;
  int satellites = 0;
  int info = 0;
  //IDMSG
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Mode: M = Manual, A = Automatic. In manual mode, the receiver is forced to
  //operate in either 2D or 3D mode. In automatic mode, the receiver is allowed to
  //switch between 2D and 3D modes subject to the PDOP and satellite masks.
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Status: 1 = fix not available, 2 = 2D, 3 = 3D
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->mode = gps_tool_strtol ( data, 1 );
  }
  data = tmp + 1;

  //PRN numbers of the satellites used in the position solution. When less than 12
  //satellites are used, the unused fields are null.
  for ( satellites = 0; satellites < 12; satellites++ ) {
    tmp = strchr ( ( char* ) data, ',' );
    if ( tmp - data > 0 ) {
      gps_data->satellites_PRN[satellites] = gps_tool_strtol ( data, tmp - data );
      for ( info = 0; info < gps_data->satellites_view; info++ ) {
        if ( gps_data->satellite_infos[info].PRN == gps_data->satellites_PRN[satellites] ) {
          gps_data->satellite_infos[info].used = 1;
          break;
        }
      }
    }
    data = tmp + 1;
  }

  //Position dilution of precision (PDOP)
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->pdop = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }
  data = tmp + 1;

  //Horizontal dilution of precision (HDOP)
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {

    gps_data->hdop = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }
  data = tmp + 1;

  //Vertical dilution of precision (VDOP)
  tmp = raw + len;
  if ( tmp - data > 0 ) {
    gps_data->vdop = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }

  return ret;
}

int32_t nmea_gsv_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int32_t ret = 0;
  const char *raw = data;
  const char *tmp = data;
  int total = 0;
  int current = 0;
  int current_num = 0;
  int current_index = 0;
  int num = 0;
  int prn_index = 0;
  //IDMSG
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Total number of GSV messages
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    total = gps_tool_strtol ( data, tmp - data );
  }
  data = tmp + 1;

  //Message number
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    current = gps_tool_strtol ( data, tmp - data );
    current_index = ( current - 1 ) * 4;
  }
  data = tmp + 1;

  //Total number of satellites in view
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->satellites_view = gps_tool_strtol ( data, tmp - data );
  }
  data = tmp + 1;
  if ( gps_data->satellites_view > 0 ) {
    current_num = ( gps_data->satellites_view - current_index ) > 4 ? 4 : ( gps_data->satellites_view - current_index );

    for ( num = 0; num < current_num; num++ ) {
      //Satellite PRN number
      tmp = strchr ( ( char* ) data, ',' );
      if ( tmp - data > 0 ) {
        gps_data->satellite_infos[current_index + num].PRN = gps_tool_strtol ( data, tmp - data );
      }
      data = tmp + 1;

      //Elevation in degrees (90?? Maximum)
      tmp = strchr ( ( char* ) data, ',' );
      if ( tmp - data > 0 ) {
        if ( data[0] == '-' ) {
          gps_data->satellite_infos[current_index + num].elevation = gps_tool_strtol ( data + 1, tmp - data - 1 );
          gps_data->satellite_infos[current_index + num].elevation = -gps_data->satellite_infos[current_index + num].elevation;
        } else {
          gps_data->satellite_infos[current_index + num].elevation = gps_tool_strtol ( data, tmp - data );
        }
      }
      data = tmp + 1;

      //Azimuth in degrees true (000 to 359)
      tmp = strchr ( ( char* ) data, ',' );
      if ( tmp - data > 0 ) {
        gps_data->satellite_infos[current_index + num].azimuth = gps_tool_strtol ( data, tmp - data );
      }
      data = tmp + 1;

      //SNR (00 - 99 dBHZ)
      if ( num < current_num - 1 ) {
        tmp = strchr ( ( char* ) data, ',' );
        if ( tmp - data > 0 ) {
          gps_data->satellite_infos[current_index + num].SNR = gps_tool_strtol ( data, tmp - data );
        }
        data = tmp + 1;
      } else {
        tmp = raw + len;
        if ( tmp - data > 0 ) {
          gps_data->satellite_infos[current_index + num].SNR = gps_tool_strtol ( data, tmp - data );
        }
        data = tmp + 1;
      }

      gps_data->satellite_infos[current_index + num].used = 0;
      for ( prn_index = 0; prn_index < 12; prn_index++ ) {
        if ( gps_data->satellites_PRN[prn_index] == gps_data->satellite_infos[current_index + num].PRN ) {
          gps_data->satellite_infos[current_index + num].used = 1;
          break;
        }
      }
    }
  }

  if ( total != current ) {
    gsv_complete = 0;
  } else {
    gsv_complete = 1;
  }
  return ret;
}

int32_t nmea_rmc_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int32_t ret = 0;
  const char *tmp = data;

  //IDMSG
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //UTC of Position Fix (when UTC offset has been decoded by the receiver).
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    //hhmmss.xxx
    gps_data->hour = gps_tool_strtol ( data, 2 );

    gps_data->minute = gps_tool_strtol ( data + 2, 2 );

    gps_data->second = gps_tool_strtol ( data + 4, 2 );

    if ( tmp - data > 7 ) {
      gps_data->millisecond = gps_tool_strtol ( data + 7, tmp - data - 7 );
    }
  }
  data = tmp + 1;

  //Status: A - Valid, V - Data not valic
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Latitude
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->latitude = gps_tool_strtodtoll ( data, tmp - data, 10000 );
  }
  data = tmp + 1;

  //N (North) or S (South)
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->north_south = ( *data == 'N' ) ? 1 : 2;
  }
  data = tmp + 1;

  //Longitude
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->longitude =  gps_tool_strtodtoll ( data, tmp - data, 10000 );
  }
  data = tmp + 1;

  //E (East) or W (West)
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->east_west = ( *data == 'E' ) ? 1 : 2;
  }
  data = tmp + 1;

  //Speed over the ground (SOG) in knots
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Course over ground in degrees true
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->course = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }
  data = tmp + 1;

  //Date: day, month, year
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->day = gps_tool_strtol ( data, 2 );
    gps_data->month = gps_tool_strtol ( data + 2, 2 );
    gps_data->year = 2000 + gps_tool_strtol ( data + 4, 2 );
  }
  data = tmp + 1;

  //Magnetic variation in degrees. Not supported.
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //E = East / W= West. Not supported.
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Mode: A - Autonomous, E - Estimated (DR)
  return ret;
}

int32_t nmea_vtg_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int32_t ret = 0;
  const char *tmp = data;

  //IDMSG
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Course over ground in degrees true.
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->course = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }
  data = tmp + 1;

  //T
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Course over ground in degrees magnetic. Not supported.
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //M
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Speed over ground in knots.
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //N
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Speed over ground in km / hr
  tmp = strchr ( ( char* ) data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->speed = gps_tool_strtodtoll ( data, tmp - data, 100 );
  }
  data = tmp + 1;

  //K
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Mode: A - Autonomous, D - Differential, E - Estimated (DR), N - Invalid

  return ret;
}

int32_t nmea_zda_decode ( const char *data, gps_data_t* gps_data, uint16_t len )
{
  int32_t ret = 0;
  const char *tmp = data;

  //IDMSG
  tmp = strchr ( ( char* ) data, ',' );
  data = tmp + 1;

  //Hours, minutes, seconds, sub-seconds of position in UTC
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    //hhmmss.xxx
    gps_data->hour = gps_tool_strtol ( data, 2 );

    gps_data->minute = gps_tool_strtol ( data + 2, 2 );

    gps_data->second = gps_tool_strtol ( data + 4, 2 );

    if ( tmp - data > 7 ) {
      gps_data->millisecond = gps_tool_strtol ( data + 7, tmp - data - 7 );
    }
  }
  data = tmp + 1;

  //Day (01 to 31)
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->day = gps_tool_strtol ( data, 2 );
  }
  data = tmp + 1;

  //Month (01 to 12)
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->month = gps_tool_strtol ( data, 2 );
  }
  data = tmp + 1;

  //Year
  tmp = strchr ( data, ',' );
  if ( tmp - data > 0 ) {
    gps_data->year = gps_tool_strtol ( data, 4 );
  }
  data = tmp + 1;

  //Local zone hour, offset from UTC to obtain local time. Not supported.

  //Local zone minute. Not supported.

  return ret;
}


uint32_t gps_tool_strtol ( const char *buf, int8_t len )
{
  uint32_t value = 0;
  int8_t i = 0;
  for ( i = 0; i < len; i++ ) {
    value *= 10;
    value += buf[i] - '0';
  }

  return value;
}

uint64_t gps_tool_strtodtoll ( const char *buf, int8_t len, uint32_t factor )
{
  uint64_t value = 0;
  double tmp = 0.0;
  uint8_t dot = 0;
  int8_t i = 0;

  for ( i = 0; i < len && ( !dot || factor > 1 ); i++ ) {
    if ( buf[i] == '.' ) {
      dot = 1;
      continue;
    }

    tmp *= 10;
    tmp += buf[i] - '0';

    if ( dot ) {
      factor /= 10;
    }
  }

  for ( ; factor > 1; ) {
    tmp *= 10;
    factor /= 10;
  }

  value = tmp;

  return value;
}
