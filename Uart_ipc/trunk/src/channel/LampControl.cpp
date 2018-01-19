#include "LampControl.h"
#include "ipc/ipc.h"
#include "uart/uart.h"

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <hnm/pps.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>

LampControl::LampControl(int channel)
   : IChannel(channel)
{
}

LampControl::~LampControl()
{
}

bool LampControl::process(int pps_fd)
{
    char buf[512];
    
    int lamp_brightness,lamp_ambience,steerwheelshock;
    bool lamp_turn_left,lamp_turn_right,lamp_intersection;
    
    int nread = 0;
    pps_decoder_t decoder;
    pps_decoder_error_t err;
    int index = 0;
    unsigned char data[512] = {0};
    
    if (pps_fd < 0)
    {
	return false;
    }

    //********* READ PPS, SEND MSG TO CAN*********//

    memset(buf, 0, 512);	
    
    if ((nread = read(pps_fd, buf, 512)) <= 0)
    {
	std::cout << __FUNCTION__ << "read error: " << strerror(errno) << std::endl;
	return false;
    }

    printf("Lamp PPS Send\nbuf: %s\n", buf);
    
    pps_decoder_initialize(&decoder, NULL);
    pps_decoder_parse_pps_str(&decoder, buf);
    pps_decoder_push(&decoder, NULL);

    data[index++] = m_channel & 0xFF;
    data[index++] = 0;

    //Atomosphere LIGHT
    //E6 04 03 00 07 00 xx
    lamp_ambience = -1;
    err = pps_decoder_get_int(&decoder, "ambience", &lamp_ambience);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: light ambience\n");
	if ((lamp_ambience > 0x2)||(lamp_ambience < 0))
	    goto out;
	
	data[index] = 0x07;
	data[index+1] = lamp_ambience & 0xFF;
	ipc_send(data, index+2);
    }

    //Lamp_brightness
    //E6 04 03 00 08 01 xx
    lamp_brightness = -1;
    err = pps_decoder_get_int(&decoder, "brightness",&lamp_brightness);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: lamp_brightness: %d\n", lamp_brightness);
	if ((lamp_brightness > 0x3)||(lamp_brightness < 0))
	    goto out;

	data[index] = 0x08;
	data[index+1] = lamp_brightness & 0xFF;
	ipc_send(data, index+2);
    }

    //turn left lamp
    //E6 04 03 00 09 01 xx
    lamp_turn_left = false;
    err = pps_decoder_get_bool(&decoder, "turn_left", &lamp_turn_left);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: lamp_turn_left\n");
	if(lamp_turn_left)
	{
	    //turn left true
	    data[index] = 0x09;
	    data[index+1] = 0x1;
	    ipc_send(data, index+2);			
	}
	else
	{
	    //turn left false
	    data[index] = 0x09;
	    data[index+1] = 0x0;
	    ipc_send(data, index+2);			
	}
    }

    //turn right lamp
    //E6 04 03 00 0A 01 xx
    lamp_turn_right = false;
    err = pps_decoder_get_bool(&decoder, "turn_right", &lamp_turn_right);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: lamp_turn_right\n");
	if(lamp_turn_right)
	{
	    //turn right true
	    data[index] = 0x0A;
	    data[index+1] = 0x1;
	    ipc_send(data, index+2);
	}
	else
	{
	    //turn right false
	    data[index] = 0x0A;
	    data[index+1] = 0x0;
	    ipc_send(data, index+2);
	}
    }

    //intersection lamp
    //E6 04 03 00 0B 01 xx
    lamp_intersection = false;
    err = pps_decoder_get_bool(&decoder, "intersection", &lamp_intersection);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: lamp_intersection\n");
	if(lamp_intersection)
	{
	    //red
	    data[index] = 0x0B;
	    data[index+1] = 0x1;
	    ipc_send(data, index+2);
	}
	else
	{
	    //red
	    data[index] = 0x0B;
	    data[index+1] = 0x0;
	    ipc_send(data, index+2);
	}
    }

    //Steer wheel shock
    //E6 04 03 00 0C 01 xx
    steerwheelshock = -1;
    err = pps_decoder_get_int(&decoder, "steerwheelshock",&steerwheelshock);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: steerwheelshock: %d\n", steerwheelshock);
	if ((steerwheelshock > 0x1)||(steerwheelshock < 0))
	    goto out;
	
	data[index] = 0x0C;
	data[index+1] = steerwheelshock & 0xFF;
	ipc_send(data, index+2);
    }
    
out:
    pps_decoder_pop(&decoder);
    pps_decoder_cleanup(&decoder);

    return true;
}

