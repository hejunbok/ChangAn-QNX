#include "GestureControl.h"
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

GestureControl::GestureControl(int channel)
   : IChannel(channel)
{
}

GestureControl::~GestureControl()
{
}

bool GestureControl::process(int pps_fd)
{
    char buf[512];
    
    int onoff = 0;
    
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

    printf("Gesture PPS Send\nbuf: %s\n", buf);
    
    pps_decoder_initialize(&decoder, NULL);
    pps_decoder_parse_pps_str(&decoder, buf);
    pps_decoder_push(&decoder, NULL);

    data[index++] = m_channel & 0xFF;
    data[index++] = 0;

    //onoff
    //E6 04 06 00 00 00 xx
    err = pps_decoder_get_int(&decoder, "onoff", &onoff);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: Gesture onoff\n");
	
	data[index] = 0x00;
	data[index+1] = onoff & 0xFF;
	ipc_send(data, index+2);
    }
    else
    {
    	printf("read data err\n");
    }
    
    
    pps_decoder_pop(&decoder);
    pps_decoder_cleanup(&decoder);

    return true;
}

