#include "AirControl.h"
#include "ipc/ipc.h"

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <hnm/pps.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>

//#define UART_DEV_FILE "/dev/ser2"

AirControl::AirControl(int channel)
   : IChannel(channel)
{
}

AirControl::~AirControl()
{
}

bool AirControl::process(int pps_fd)
{
    if (pps_fd < 0)
    {
        return false;
    }

    unsigned char data[512] = {0};
    pps_decoder_t decoder;
    pps_decoder_error_t err;
    int cmd = -1;
    int index = 0;

    char buf[512] = {0};
    int nread = 0;

    int mode, temperature, fan_speed, fan_direction, recirculation_all;
    bool airConditioning_all, defrost_all;
    const char* Air_direction = NULL;

    static int uart_fd = -1;

    memset(buf, 0, 512);

    if ((nread = read(pps_fd, buf, 512)) <= 0)
    {
        std::cout << __FUNCTION__ << "read error: " << strerror(errno) << std::endl;
        return false;
    }

    printf("UART Send\nbuf: %s\n", buf);

    pps_decoder_initialize(&decoder, NULL);
    pps_decoder_parse_pps_str(&decoder, buf);
    pps_decoder_push(&decoder, NULL);

    data[index++] = m_channel & 0xFF;
    data[index++] = 0;

    //Front AUto AC ON Req
    airConditioning_all = false;
    err = pps_decoder_get_bool(&decoder, "airConditioning_all", &airConditioning_all);
    if (err == PPS_DECODER_OK)
    {
        uint8_t Air_ON_OFF = 0;
        printf("Command: airConditioning_switch,channel = %d\n",m_channel);
        if(airConditioning_all)
        {
            Air_ON_OFF = 1;
        }
        else
        {
            Air_ON_OFF = 0;
        }

        data[index] = 0x00;
        data[index+1] = Air_ON_OFF & 0xFF;
        ipc_send(data, index+2);
    }

    //AC DEF Adjust Req
    defrost_all = false;
    err = pps_decoder_get_bool(&decoder, "frontwindow_heating", &defrost_all);
    if (err == PPS_DECODER_OK)
    {
        uint8_t Air_defrost = 0;
        printf("Command: defrost_all\n");
        if(defrost_all)
        {
            Air_defrost = 2;
        }
        else
        {
            Air_defrost = 1;
        }

        data[index] = 0x02;
        data[index+1] = Air_defrost & 0xFF;
        ipc_send(data, index+2);
    }

    //AC Left Temp Adjust Value Req
    temperature = -1;
    err = pps_decoder_get_int(&decoder, "temperature_row1left", &temperature);
    if (err == PPS_DECODER_OK)
    {
        printf("Command: temperature_row1left\n");

        data[index] = 0x04;
        data[index+1] = temperature & 0xFF;
        ipc_send(data, index+2);
    }

    // AC Fan Adjust Value Req
    fan_speed = -1;
        err = pps_decoder_get_int(&decoder, "fanSpeed_row1left", &fan_speed);
    if (err == PPS_DECODER_OK)
    {
        printf("Command: fanSpeed_row1left\n");
        if (fan_speed > 0xA)
            goto out;
        data[index] = 0x06;
        data[index+1] = fan_speed & 0xFF;
        ipc_send(data, index+2);
    }

    // AC Front Mode Adjust Value Req
    Air_direction = NULL;
    err = pps_decoder_get_string(&decoder, "fanDirection_row1left", &Air_direction);
    if (err == PPS_DECODER_OK)
    {
	printf("Command: fanDirection_row1left\n");
	if (!strcmp(Air_direction, "face"))
	{
	    fan_direction = 1;
	}
	else if (!strcmp(Air_direction, "defrost"))
	{
	    fan_direction = 2;
	}
	else if (!strcmp(Air_direction, "feet"))
	{
	    fan_direction = 3;
	}
	else if (!strcmp(Air_direction, "defrostAndFeet"))
	{
	    fan_direction = 4;
	}
	else if (!strcmp(Air_direction, "faceAndFeet"))
	{
	    fan_direction = 5;
	}
	else
	{
	   fan_direction = 0;
	}
	data[index] = 0x07;
	data[index+1] = fan_direction & 0xFF;
	ipc_send(data, index+2);
    }

    recirculation_all = -1;
    err = pps_decoder_get_int(&decoder, "airRecirculation_all", &recirculation_all);
    if (err == PPS_DECODER_OK)
    {
        printf("Command: airRecirculation_all\n");
        if(recirculation_all > 0x04)
            goto out;
        data[index] = 0x03;
        data[index+1] = recirculation_all & 0xFF;
        ipc_send(data, index+2);
    }

out:
    pps_decoder_pop(&decoder);
    pps_decoder_cleanup(&decoder);

    return true;
}

