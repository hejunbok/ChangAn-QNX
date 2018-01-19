#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include "gps_decoder.h"

#define PPS_GPS_FILE "/pps/services/geolocation/socketdata"

#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 1024
#define DEL "|"

void split( char **arr, char *str)
{
    char *s =NULL;
 
    s=strtok(str, DEL);
    while(s != NULL)
    {
        *arr++ = s;
        s = strtok(NULL, DEL);
    }

}

int update_pps( int pps_fd, char buf[] )
{
    int length = 0;
    
    if (pps_fd < 0)
    {
        printf("pps_fd error!\n\n");
        return -1;;
    }
    
    printf("%s\n", buf);
    length = strlen( buf );
    if ( write ( pps_fd, buf, length ) == length)
        printf("update ok!\n");

    return 0;
}

int parse_data( gps_data* gps)
{
    char buf[1024];
    int pps_fd = 0;
    int satellites = gps->satellites_view > 8 ? 8 : gps->satellites_view;
    
    memset(buf,0,sizeof(buf));
    
    if ((pps_fd = open(PPS_GPS_FILE, O_WRONLY | O_CREAT, 0666)) < 0)
    {
        printf("open %s error!\n", PPS_GPS_FILE);
        return -1;
    }
    
    printf("updating pps:\n");

    sprintf(buf, "altitude:n:%f", gps->altitude);
    update_pps(pps_fd, buf);
    
    sprintf(buf, "geoidHeight:n:%f", gps->geoidal_separation);
    update_pps(pps_fd, buf);

    sprintf(buf, gps->north_south == 1 ? "latitude:n:%f" : "latitude:n:-%f", gps->latitude);
    update_pps(pps_fd, buf);
    
    sprintf(buf, gps->east_west == 1 ? "longitude:n:%f" : "longitude:n:-%f", gps->longitude);
    update_pps(pps_fd, buf);

    sprintf(buf, "pdop:n:%f", gps->pdop);
    update_pps(pps_fd, buf);

    sprintf(buf, "hdop:n:%f", gps->hdop);
    update_pps(pps_fd, buf);

    sprintf(buf, "vdop:n:%f", gps->vdop);
    update_pps(pps_fd, buf);
 
    sprintf(buf, "speed:n:%f", gps->speed);
    update_pps(pps_fd, buf);
    
    sprintf(buf, "numSatellites:n:%d", satellites);
    update_pps(pps_fd, buf);

    sprintf(buf, "satellitesInfo:json:[");
    for (int i = 0; i < satellites; i++)
    {
       sprintf(buf + strlen(buf), "{\"id\":%d,\"cno\":0,\"ephemeris\":true,\"azimuth\":%d,\"elevation\":%d,\"tracked\":true,\"used\":%s,\"almanac\":true}",
	     gps->satellite_infos[i].PRN,
	     gps->satellite_infos[i].azimuth,
	     gps->satellite_infos[i].elevation,
	     gps->satellite_infos[i].used == 1 ? "true" : "false");

       if (i != satellites - 1)
       {
	  sprintf(buf + strlen(buf), ",");
       }
    }
    sprintf(buf + strlen(buf), "]");

    update_pps(pps_fd, buf);

    sprintf(buf, "altitudeAccuracy:n:0");
    update_pps(pps_fd, buf);

    sprintf(buf, "gpsTow:n:%d", gps_tow(gps->year, gps->month, gps->day, gps->hour, gps->minute, gps->second));
    update_pps(pps_fd, buf);

    sprintf(buf, "gpsWeek:n:%d", gps_week(gps->year, gps->month, gps->day, gps->hour));
    update_pps(pps_fd, buf);
 
    sprintf(buf, "heading:n:%f", gps->course);
    update_pps(pps_fd, buf);

    sprintf(buf, "ttff:n:0");
    update_pps(pps_fd, buf);
 
    sprintf(buf, "verticalSpeed:n:0");
    update_pps(pps_fd, buf);
    
    printf("### done!\n");
    
    close(pps_fd);

    printf("close ok...\n");

    return 0;
}

int main()
{
    struct sockaddr_in addrto; 
    struct sockaddr_in from;
    int sock = -1;
    int ret = 0;
    const int opt = 1;
    int len = sizeof(sockaddr_in); 
    char smsg[256] = {0};
    gps_data gps = {0};

    bzero(&addrto, sizeof(struct sockaddr_in));  
    addrto.sin_family = AF_INET;  
    addrto.sin_addr.s_addr = htonl(INADDR_ANY);  
    addrto.sin_port = htons(2800);  
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)   
    {     
        printf("socket error:%s\n", strerror(errno));;   
        return -1;  
    }     
   
    if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) == -1)  
    {  
        printf("setsockopt error:%s\n", strerror(errno));;   
        return -1;   
    }  
  
    if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1)   
    {     
        printf("bind error:%s\n", strerror(errno));;   
        return -1;  
    }   
    
    while(1)  
    {
        ret = recvfrom(sock, smsg, 256, 0, (struct sockaddr*)&from,(socklen_t*)&len);  
        if(ret <= 0)  
        {  
            printf("recvfrom error:%s\n", strerror(errno));  
        }  
        else  
        { 
            if (gps_decode(smsg, ret, &gps) == 0)
            {
                //time_zone_fix(&gps, 8);
#ifdef linux 
		for (int i = 0; i < gps.satellites_view; i++)
		{
		   printf("\n");
		   printf("satellite_infos[%d].PRN=%d\n", i, gps.satellite_infos[i].PRN);
                   printf("satellite_infos[%d].elevation=%d\n", i, gps.satellite_infos[i].elevation);
                   printf("satellite_infos[%d].azimuth=%d\n", i, gps.satellite_infos[i].azimuth);
                   printf("satellite_infos[%d].SNR=%d\n", i, gps.satellite_infos[i].SNR);
                   printf("satellite_infos[%d].used=%d\n", i, gps.satellite_infos[i].used);
		   printf("\n");
		}

                printf("quality=%d\n", gps.quality);
                printf(gps.north_south == 1 ? "north\n" : "north\n");
                printf("latitude=%f\n", gps.latitude);
                printf(gps.east_west == 1 ? "east\n" : "west\n");
                printf("longitude=%f\n", gps.longitude);
		printf("altitude=%f\n", gps.altitude);
		printf("geoidal_separation=%f\n", gps.geoidal_separation);
                printf("satellites: view=%d, used=%d\n", gps.satellites_view, gps.satellites_used);
		printf("PDOP=%f\n", gps.pdop);
		printf("HDOP=%f\n", gps.hdop);
		printf("VDOP=%f\n", gps.vdop);
		printf("speed=%f\n", gps.speed);
		printf("course=%f\n", gps.course);
                printf("%d-%02d-%02d %02d:%02d:%02d.%03d\n",
		      gps.year,
		      gps.month,
		      gps.day,
		      gps.hour,
		      gps.minute,
		      gps.second,
		      gps.millisecond);

		printf("gpsWeek=%d\n", gps_week(gps.year, gps.month, gps.day, gps.hour));
		printf("gpsTow=%d\n", gps_tow(gps.year, gps.month, gps.day, gps.hour, gps.minute, gps.second));

		printf("\n\n");
#else
		parse_data(&gps);
		printf("parse_data OK...\n");
#endif
            }
            else 
            {
                printf("gps_decode error\n");
            }
        }
    }  
    return 0;
}
