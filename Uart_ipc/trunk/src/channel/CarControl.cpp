#include "CarControl.h"
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

//#define UART_DEV_FILE "/dev/ser2"
#define PPS_CAR_FILE "/pps/ui"

extern uint8_t RxdBuffer[READ_BUF_MAX_SIZE];
extern int uart_update_flag;

CarControl::CarControl(int channel)
   : IChannel(channel), mLights(-1), mAllWindows(-1)
{
}

CarControl::~CarControl()
{
}

bool CarControl::process(int pps_fd)
{

	int length, i;
	char buf[512];
	char buf_send[256] = {0};
	//char windows[4][10] = {"window_fl", "window_fr", "window_rl", "window_rr"};
	int frontlights, window;
    const char *frontlightsStr = NULL;
    const char *windowStr = NULL;
	int nread = 0;
	pps_decoder_t decoder;
	pps_decoder_error_t err;
	int index = 0;
	unsigned char data[512] = {0};
    static int windows = -1;

	//printf("CarControl::process\n");

	if (pps_fd < 0)
	{
		return false;
	}

//********* READ PPS, SEND MSG TO CAN*********//
//Light
	//E6 04 02 00 21 00 xx
//Window
	//E6 04 02 00 22 00 xx
	
    memset(buf, 0, 512);
	
	if ((nread = read(pps_fd, buf, 512)) <= 0)
	{
		std::cout << __FUNCTION__ << "read error: " << strerror(errno) << std::endl;
		return false;
		//goto out;
	}

	printf("Car PPS Send\nbuf: %s\n", buf);
	
	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buf);
	pps_decoder_push(&decoder, NULL);

	data[index++] = m_channel & 0xFF;
	data[index++] = 0;
	//data[index++] = cmd & 0xFF;

	//LIGHT
	frontlights = -1;
	err = pps_decoder_get_string(&decoder, "frontlights", &frontlightsStr);
	if (err == PPS_DECODER_OK)
	{
                frontlights = atoi(frontlightsStr);
		if (mLights != frontlights)
		{
			mLights = frontlights;
			printf("Command: frontlights: %d\n", frontlights);
			data[index] = 0x21;
			data[index+1] = frontlights & 0xFF;
			ipc_send(data, index+2);
		}
	}

	//WINDOW
	window = -1;
	err = pps_decoder_get_string(&decoder, "window_fl", &windowStr);
	if (err == PPS_DECODER_OK)
	{
                window = atoi(windowStr);
printf("test Command: window: %d\n", window);
printf("test Command: windows: %d\n", windows);
		if (windows != window)
		{
			windows = window;
			printf("Command: window: %d\n", window);
			data[index] = 0x22;
			data[index+1] = window & 0xFF;
			ipc_send(data, index+2);
		}
	}
	
out:
        pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	return true;
}

