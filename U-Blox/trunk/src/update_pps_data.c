#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "update_pps_data.h"

#define GPS_LEN_NONE 5

#define PPS_GPS_FILE "/pps/services/geolocation/socketdata"

int convert_gps_data_format ( const gps_data_t *gps_data, gps_pps_data_t *gps_pps_data )
{
  int ret = 0;
  uint32_t value;
  int d;
  double m;
  int index = 0;
  int info = 0;

  gps_pps_data->year = gps_data->year;

  gps_pps_data->month = gps_data->month;

  gps_pps_data->day = gps_data->day;

  gps_pps_data->hour = gps_data->hour;

  gps_pps_data->minute = gps_data->minute;

  gps_pps_data->second = gps_data->second;

  gps_pps_data->millisecond = gps_data->millisecond;

  gps_pps_data->quality = gps_data->quality;

  gps_pps_data->mode = gps_data->mode;

  gps_pps_data->north_south = gps_data->north_south;

  value = gps_data->latitude;
  //gps_pps_data->latitude = (double)value / 10000;
  d = value / 1000000;//d
  m = ( ( double ) value ) / 10000 - d * 100;
  gps_pps_data->latitude = d + m / 60;

  gps_pps_data->east_west = gps_data->east_west;

  value = gps_data->longitude;
  //gps_pps_data->longitude = (double)value / 10000;
  d = value / 1000000;//d
  m = ( ( double ) value ) / 10000 - d * 100;
  gps_pps_data->longitude = d + m / 60;

  value = gps_data->altitude;
  gps_pps_data->altitude = ( ( double ) ( int32_t ) value ) / 10;

  value = gps_data->geoidal_separation;
  gps_pps_data->geoidal_separation = ( ( double ) ( int32_t ) value ) / 10;

  value = gps_data->pdop;
  gps_pps_data->pdop = ( double ) value / 100;

  value = gps_data->hdop;
  gps_pps_data->hdop = ( double ) value / 100;

  value = gps_data->vdop;
  gps_pps_data->vdop = ( double ) value / 100;

  value = gps_data->course;
  gps_pps_data->course = ( double ) value / 100;

  value = gps_data->speed;
  gps_pps_data->speed = ( double ) value / 100;

  gps_pps_data->satellites_view = gps_data->satellites_view;

  gps_pps_data->satellites_used = gps_data->satellites_used;

  for ( info = 0; info < 12; info++ ) {
    gps_pps_data->satellite_infos[info].PRN = gps_data->satellite_infos[info].PRN;

    gps_pps_data->satellite_infos[info].elevation = ( int8_t ) ( gps_data->satellite_infos[info].elevation );

    gps_pps_data->satellite_infos[info].azimuth = gps_data->satellite_infos[info].azimuth ;
    gps_pps_data->satellite_infos[info].SNR = gps_data->satellite_infos[info].SNR;
    gps_pps_data->satellite_infos[info].used = gps_data->satellite_infos[info].used;
  }

  return ret;
}

void time_zone_fix ( gps_pps_data_t *gps_pps_data, int fix )
{
  unsigned char leapYearFlag = 0;
  int hour = gps_pps_data->hour;

  hour += fix;
  if ( hour >= 0 && hour < 24 ) {
    gps_pps_data->hour = hour;
    return;
  }

  if ( ( gps_pps_data->year % 4 == 0 && gps_pps_data->year % 100 != 0 ) || ( gps_pps_data->year % 400 == 0 ) ) {
    leapYearFlag = 1;
  }

  if ( gps_pps_data->hour >= 24 ) {
    gps_pps_data->hour = hour - 24;
    gps_pps_data->day += 1;

    switch ( gps_pps_data->month ) {
    case 1:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 2:
      if ( leapYearFlag ) {
        if ( gps_pps_data->day == 30 ) {
          gps_pps_data->day = 1;
          gps_pps_data->month += 1;
        }
      } else {
        if ( gps_pps_data->day == 29 ) {
          gps_pps_data->day = 1;
          gps_pps_data->month += 1;
        }
      }
      break;

    case 3:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 4:
      if ( gps_pps_data->day == 31 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 5:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 6:
      if ( gps_pps_data->day == 31 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 7:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 8:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 9:
      if ( gps_pps_data->day == 31 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 10:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 11:
      if ( gps_pps_data->day == 31 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month += 1;
      }
      break;

    case 12:
      if ( gps_pps_data->day == 32 ) {
        gps_pps_data->day = 1;
        gps_pps_data->month = 1;
        gps_pps_data->year += 1;
      }
      break;

    default :
      break;
    }
  } else {
    gps_pps_data->hour = hour + 24;
    gps_pps_data->day -= 1;

    switch ( gps_pps_data->month ) {
    case 1:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month = 12;
        gps_pps_data->year -= 1;
      }
      break;

    case 2:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month -= 1;
      }
      break;

    case 3:
      if ( gps_pps_data->day == 0 ) {
        if ( leapYearFlag ) {
          gps_pps_data->day = 29;
          gps_pps_data->month -= 1;
        } else {
          gps_pps_data->day = 28;
          gps_pps_data->month -= 1;
        }
      }
      break;

    case 4:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month -= 1;
      }
      break;

    case 5:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 30;
        gps_pps_data->month -= 1;
      }
      break;

    case 6:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month -= 1;
      }
      break;

    case 7:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 30;
        gps_pps_data->month -= 1;
      }
      break;

    case 8:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month -= 1;
      }
      break;

    case 9:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month -= 1;
      }
      break;

    case 10:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 30;
        gps_pps_data->month -= 1;
      }
      break;

    case 11:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 31;
        gps_pps_data->month -= 1;
      }
      break;

    case 12:
      if ( gps_pps_data->day == 0 ) {
        gps_pps_data->day = 30;
        gps_pps_data->month -= 1;
      }
      break;

    default :
      break;
    }
  }
}

int gps_week ( int year, int month, int day, int hour )
{
  double JD = 0;
  int y = year;
  int m = month;
  int week = 0;

  if ( month <= 2 ) {
    y = year - 1;
    m = month + 12;
  }

  JD = ( int ) ( 365.25 * y ) + ( int ) ( 30.6001 * ( m + 1 ) ) + day + ( hour / 24 ) + 1720981.5;

  week = ( int ) ( ( JD - 2444244.5 ) / 7 );

  return week;
}

int gps_tow ( int year, int month, int day, int hour, int minute, int second )
{
  double JD = 0;
  int y = year;
  int m = month;
  int tow = 0;

  if ( month <= 2 ) {
    y = year - 1;
    m = month + 12;
  }

  JD = ( int ) ( 365.25 * y ) + ( int ) ( 30.6001 * ( m + 1 ) ) + day + ( hour / 24 ) + 1720981.5;

  tow = ( int ) ( JD + 0.5 ) % 7;

  tow = ( ( tow + 1 ) % 7 ) * 3600 * 24 + hour * 3600 + minute * 60 + second;

  return tow;
}

int write_to_pps ( int pps_fd, char buf[] )
{
  int length = 0;

  if ( pps_fd < 0 ) {
    MSG ( "pps_fd error!\n\n" );
    return -1;;
  }

  MSG ( "%s\n", buf );
  length = strlen ( buf );
  if ( write ( pps_fd, buf, length ) == length )
    MSG ( "update ok!\n" );

  return 0;
}

int update_gps_pps_data ( gps_pps_data_t* gps_pps_data )
{
  char buf[1024];
  int pps_fd = 0;
  int satellites = gps_pps_data->satellites_view > 8 ? 8 : gps_pps_data->satellites_view;

  memset ( buf, 0, sizeof ( buf ) );

  if ( ( pps_fd = open ( PPS_GPS_FILE, O_WRONLY | O_CREAT, 0666 ) ) < 0 ) {
    MSG ( "open %s error!\n", PPS_GPS_FILE );
    return -1;
  }

  MSG ( "updating pps:\n" );

  sprintf ( buf, "altitude:n:%f", gps_pps_data->altitude );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "geoidHeight:n:%f", gps_pps_data->geoidal_separation );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, gps_pps_data->north_south == 1 ? "latitude:n:%f" : "latitude:n:-%f", gps_pps_data->latitude );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, gps_pps_data->east_west == 1 ? "longitude:n:%f" : "longitude:n:-%f", gps_pps_data->longitude );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "pdop:n:%f", gps_pps_data->pdop );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "hdop:n:%f", gps_pps_data->hdop );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "vdop:n:%f", gps_pps_data->vdop );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "speed:n:%f", gps_pps_data->speed );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "numSatellites:n:%d", satellites );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "satellitesInfo:json:[" );
  for ( int i = 0; i < satellites; i++ ) {
    sprintf ( buf + strlen ( buf ), "{\"id\":%d,\"cno\":0,\"ephemeris\":true,\"azimuth\":%d,\"elevation\":%d,\"tracked\":true,\"used\":%s,\"almanac\":true}",
              gps_pps_data->satellite_infos[i].PRN,
              gps_pps_data->satellite_infos[i].azimuth,
              gps_pps_data->satellite_infos[i].elevation,
              gps_pps_data->satellite_infos[i].used == 1 ? "true" : "false" );

    if ( i != satellites - 1 ) {
      sprintf ( buf + strlen ( buf ), "," );
    }
  }
  sprintf ( buf + strlen ( buf ), "]" );

  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "altitudeAccuracy:n:0" );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "gpsTow:n:%d", gps_tow ( gps_pps_data->year, gps_pps_data->month, gps_pps_data->day, gps_pps_data->hour, gps_pps_data->minute, gps_pps_data->second ) );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "gpsWeek:n:%d", gps_week ( gps_pps_data->year, gps_pps_data->month, gps_pps_data->day, gps_pps_data->hour ) );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "heading:n:%f", gps_pps_data->course );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "ttff:n:0" );
  write_to_pps ( pps_fd, buf );

  sprintf ( buf, "verticalSpeed:n:0" );
  write_to_pps ( pps_fd, buf );

  MSG ( "### done!\n" );

  close ( pps_fd );

  MSG ( "close ok...\n" );

  return 0;
}