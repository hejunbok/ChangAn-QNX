#include "gps_decoder.h"
#include <stdio.h>

#define GPS_LEN_NONE 5

uint8_t gps_checksum(const char *buf, int len);

int gps_decode_message_v1(const char *buf, int len, gps_data *gps);

int gps_decode(const char *buf, int len, gps_data *gps)
{
   int ret =  -1;
   uint8_t version = 0;
   uint8_t checksum = 0;

   if (!buf || len <= GPS_LEN_NONE || !gps)
   {
      printf("!buf || len <= GPS_LEN_NONE || !gps\n");
      return -1;
   }

   checksum = buf[len - 1];
   if (checksum != gps_checksum(buf, len - 1))
   {
      printf("gps_checksum error...\n");
      return -1;
   }

   version = buf[0];

   switch (version)
   {
      case 0x1:
	 ret = gps_decode_message_v1(buf + 4, len - GPS_LEN_NONE, gps);
	 break;

      default:
	 break;
   }

   return ret;
}

uint8_t gps_checksum(const char *buf, int len)
{
   uint8_t checksum = 0;
   int i = 0;
   for (i = 0; i < len; i++)
   {
      checksum ^= buf[i];
   }

   return checksum;
}

int gps_decode_message_v1(const char *buf, int len, gps_data *gps)
{
   int ret = 0;
   uint32_t value;
   int d;
   double m;
   int index = 0;
   int info = 0;

   gps->year = buf[index++] & 0xFF;
   gps->year += buf[index++] << 8;

   gps->month = buf[index++];

   gps->day = buf[index++];

   gps->hour = buf[index++];

   gps->minute = buf[index++];

   gps->second = buf[index++];

   gps->millisecond = buf[index++];
   gps->millisecond += buf[index++] << 8;

   gps->quality = buf[index++];

   gps->mode = buf[index++];

   gps->north_south = buf[index++];

   value = buf[index++] & 0xFF;
   value += (buf[index++] & 0xFF) << 8;
   value += (buf[index++] & 0xFF) << 16;
   value += (buf[index++] & 0xFF) << 24;

   //gps->latitude = (double)value / 10000;
   d = value / 1000000;//d
   m = ((double)value) / 10000 - d * 100;
   gps->latitude = d + m / 60;

   gps->east_west = buf[index++];
   
   value = buf[index++] & 0xFF;
   value += (buf[index++] & 0xFF) << 8;
   value += (buf[index++] & 0xFF) << 16;
   value += (buf[index++] & 0xFF) << 24;
   //gps->longitude = (double)value / 10000;
   d = value / 1000000;//d
   m = ((double)value) / 10000 - d * 100;
   gps->longitude = d + m / 60;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   value += buf[index++] << 16;
   value += buf[index++] << 24;
   gps->altitude = ((double)(int32_t)value) / 10;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   value += buf[index++] << 16;
   value += buf[index++] << 24;
   gps->geoidal_separation = ((double)(int32_t)value) / 10;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   gps->pdop = (double)value / 100;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   gps->hdop = (double)value / 100;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   gps->vdop = (double)value / 100;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   gps->course = (double)value / 100;

   value = buf[index++] & 0xFF;
   value += buf[index++] << 8;
   value += buf[index++] << 16;
   value += buf[index++] << 24;
   gps->speed = (double)value / 100;

   gps->satellites_view = buf[index++];

   gps->satellites_used = buf[index++];

   for (info = 0; info < 12; info++)
   {
      gps->satellite_infos[info].PRN = buf[index++];
      
      gps->satellite_infos[info].elevation = (int8_t)(buf[index++] & 0xFF);

      gps->satellite_infos[info].azimuth = buf[index++];
      gps->satellite_infos[info].SNR = buf[index++];
      gps->satellite_infos[info].used = buf[index++];
   }

   return ret;
}

void time_zone_fix(gps_data *gps, int fix)
{
   unsigned char leapYearFlag = 0;
   int hour = gps->hour;
  
   hour += fix;
   if (hour >= 0 && hour < 24)
   {
      gps->hour = hour;
      return;
   }
  
   if ((gps->year % 4 == 0 && gps->year % 100 != 0) || (gps->year % 400 == 0))
   {
      leapYearFlag = 1;  
   }

   if (gps->hour >= 24)
   {
      gps->hour = hour - 24;
      gps->day += 1;
      
      switch (gps->month) 
      {
         case 1:
	    if (gps->day == 32)
	    {
	       gps->day = 1;
	       gps->month += 1;
	    }  
            break;
  
	 case 2:
	    if (leapYearFlag)  
            {  
               if (gps->day == 30)  
               {  
                  gps->day = 1;  
                  gps->month += 1;  
               }  
            }  
            else  
            {  
               if (gps->day == 29)  
               {  
                  gps->day = 1;  
                  gps->month += 1;  
               }  
            }  
            break;
 
         case 3:  
            if (gps->day == 32)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
 
         case 4:  
            if (gps->day == 31)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
           
         case 5:
            if (gps->day == 32)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
 
         case 6:  
            if (gps->day == 31)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }
            break;
  
         case 7:  
            if (gps->day == 32)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
  
         case 8:  
            if (gps->day == 32)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
  
         case 9:  
            if (gps->day == 31)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
  
         case 10:  
            if (gps->day == 32)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
  
         case 11:  
            if (gps->day == 31)  
            {  
               gps->day = 1;  
               gps->month += 1;  
            }  
            break;
  
        case 12:  
            if (gps->day == 32)  
            {  
               gps->day = 1;  
               gps->month = 1;  
               gps->year += 1;  
            }  
            break;
  
        default :  
            break;
      }
   }
   else
   {
      gps->hour = hour + 24;
      gps->day -= 1;

      switch (gps->month)
      {  
         case 1:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month = 12;
               gps->year -= 1;  
            }  
            break;
  
         case 2:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month -= 1;  
            }  
            break;
 
         case 3:  
            if (gps->day == 0)  
            {  
               if (leapYearFlag)  
               {  
                  gps->day = 29;  
                  gps->month -= 1;  
               }
               else
               {  
                  gps->day = 28;  
                  gps->month -= 1;  
               } 
            }  
            break;

         case 4:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month -= 1;  
            }  
            break;
           
         case 5:
            if (gps->day == 0)  
            {  
               gps->day = 30;  
               gps->month -= 1;  
            }  
            break;
 
         case 6:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month -= 1;  
            }
            break;
  
         case 7:  
            if (gps->day == 0)  
            {  
               gps->day = 30;  
               gps->month -= 1;  
            }  
            break;
  
         case 8:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month -= 1;  
            }  
            break;
  
         case 9:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month -= 1;  
            }  
            break;
  
         case 10:  
            if (gps->day == 0)  
            {  
               gps->day = 30;  
               gps->month -= 1;  
            }  
            break;
  
         case 11:  
            if (gps->day == 0)  
            {  
               gps->day = 31;  
               gps->month -= 1;  
            }  
            break;
  
         case 12:  
            if (gps->day == 0)  
            {  
               gps->day = 30;  
               gps->month -= 1;    
            }  
            break;
  
        default :  
            break;
      }
   }
}

int gps_week(int year, int month, int day, int hour)
{
   double JD = 0;
   int y = year;
   int m = month;
   int week = 0;

   if (month <= 2)
   {
      y = year -1;
      m = month + 12;
   }

   JD = (int)(365.25 * y) + (int)(30.6001 * (m + 1)) + day + (hour / 24) + 1720981.5;

   week = (int)((JD - 2444244.5) / 7);

   return week;
}

int gps_tow(int year, int month, int day, int hour, int minute, int second)
{
   double JD = 0;
   int y = year;
   int m = month;
   int tow = 0;

   if (month <= 2)
   {
      y = year -1;
      m = month + 12;
   }

   JD = (int)(365.25 * y) + (int)(30.6001 * (m + 1)) + day + (hour / 24) + 1720981.5;

   tow = (int)(JD + 0.5) % 7;

   tow = ((tow + 1) % 7) * 3600 * 24 + hour * 3600 + minute * 60 + second;

   return tow;
}

