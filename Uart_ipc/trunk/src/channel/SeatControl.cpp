#include "SeatControl.h"
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

SeatControl::SeatControl(int channel)
   : IChannel(channel)
{
}

SeatControl::~SeatControl()
{
}

bool SeatControl::process(int pps_fd)
{
    char buf[1024];

    int main_slide_movement,main_recliner_movement,main_tilt_movement,main_height_movement;
    int main_lumbar_adjustment,main_store_recall_position_cmd,main_massage,main_massage_level,main_heatingstatus,main_ventilation;
    int copilot_slide_movement,copilot_recliner_movement,copilot_tilt_movement,copilot_height_movement;
    int copilot_lumbar_adjustment,copilot_store_recall_position_cmd,copilot_massage,copilot_massage_level,copilot_heatingstatus,copilot_ventilation;

    int old_main_slide_movement = -1,old_main_recliner_movement = -1,old_main_tilt_movement = -1,old_main_height_movement = -1;
    int old_main_lumbar_adjustment = -1,old_main_store_recall_position_cmd = -1,old_main_massage = -1,old_main_massage_level = -1,old_main_heatingstatus = -1,old_main_ventilation = -1;
    int old_copilot_slide_movement = -1,old_copilot_recliner_movement = -1,old_copilot_tilt_movement = -1,old_copilot_height_movement = -1;
    int old_copilot_lumbar_adjustment = -1,old_copilot_store_recall_position_cmd = -1,old_copilot_massage = -1,old_copilot_massage_level = -1,old_copilot_heatingstatus = -1,old_copilot_ventilation = -1;

    int nread = 0;
    pps_decoder_t decoder;
    pps_decoder_error_t err;
    int index = 0;
    unsigned char data[16] = {0};

    if (pps_fd < 0)
    {
        return false;
    }

    //********* READ PPS, SEND MSG TO CAN*********//

    memset(buf, 0, 1024);

    if ((nread = read(pps_fd, buf, 1024)) <= 0)
    {
        std::cout << __FUNCTION__ << " read error: " << strerror(errno) << std::endl;
        return false;
    }

    printf("Seat PPS Send\nbuf: %s\n", buf);

    pps_decoder_initialize(&decoder, NULL);
    pps_decoder_parse_pps_str(&decoder, buf);
    pps_decoder_push(&decoder, NULL);

    data[index++] = m_channel & 0xFF;
    data[index++] = 0;

    //Main Slide Movement
    //E6 04 04 00 00 00 xx
    main_slide_movement = -1;
    err = pps_decoder_get_int(&decoder, "main_slide_movement", &main_slide_movement);
    if (err == PPS_DECODER_OK && main_slide_movement != old_main_slide_movement)
    {
        printf("Command: main_slide_movement: %d\n", main_slide_movement);
        if ((main_slide_movement > 0x2)||(main_slide_movement < 0))
            goto out;

        data[index] = 0x00;
        data[index+1] = main_slide_movement & 0xFF;
        ipc_send(data, index+2);
        old_main_slide_movement = main_slide_movement;
    }

    //Main Recliner Movement
    //E6 04 04 00 01 00 xx
    main_recliner_movement = -1;
    err = pps_decoder_get_int(&decoder, "main_recliner_movement",&main_recliner_movement);
    if (err == PPS_DECODER_OK && main_recliner_movement != old_main_recliner_movement)
    {
        printf("Command: main_recliner_movement: %d\n", main_recliner_movement);
        if ((main_recliner_movement > 0x2)||(main_recliner_movement < 0))
            goto out;
        data[index] = 0x01;
        data[index+1] = main_recliner_movement & 0xFF;
        ipc_send(data, index+2);
        old_main_recliner_movement = main_recliner_movement;
    }

    //Main Tilt Movement
    //E6 04 04 00 02 00 xx
    main_tilt_movement = -1;
    err = pps_decoder_get_int(&decoder, "main_tilt_movement",&main_tilt_movement);
    if (err == PPS_DECODER_OK && main_tilt_movement != old_main_tilt_movement)
    {
        printf("Command: main_tilt_movement: %d\n", main_tilt_movement);
        if ((main_tilt_movement > 0x2)||(main_tilt_movement < 0))
            goto out;
        data[index] = 0x02;
        data[index+1] = main_tilt_movement & 0xFF;
        ipc_send(data, index+2);
        old_main_tilt_movement = main_tilt_movement;
    }

    //Main Height Movement
    //E6 04 04 00 03 00 xx
    main_height_movement = -1;
    err = pps_decoder_get_int(&decoder, "main_height_movement",&main_height_movement);
    if (err == PPS_DECODER_OK && main_height_movement != old_main_height_movement)
    {
        printf("Command: main_height_movement: %d\n", main_height_movement);
        if ((main_height_movement > 0x2)||(main_height_movement < 0))
            goto out;
        data[index] = 0x03;
        data[index+1] = main_height_movement & 0xFF;
        ipc_send(data, index+2);
        old_main_height_movement = main_height_movement;
    }

    //Main Lumbar Adjustment
    //E6 04 04 00 04 00 xx
    main_lumbar_adjustment = -1;
    err = pps_decoder_get_int(&decoder, "main_lumbar_adjustment",&main_lumbar_adjustment);
    if (err == PPS_DECODER_OK && main_lumbar_adjustment != old_main_lumbar_adjustment)
    {
        printf("Command: main_lumbar_adjustment: %d\n", main_lumbar_adjustment);
        if ((main_lumbar_adjustment > 0x4)||(main_lumbar_adjustment < 0))
            goto out;
        data[index] = 0x04;
        data[index+1] = main_lumbar_adjustment & 0xFF;
        ipc_send(data, index+2);
        old_main_lumbar_adjustment = main_lumbar_adjustment;
    }

    //Main store/recall commands
    //E6 04 04 00 05 00 xx
    main_store_recall_position_cmd = -1;
    err = pps_decoder_get_int(&decoder, "main_store_recall_position_cmd",&main_store_recall_position_cmd);
    if (err == PPS_DECODER_OK && main_store_recall_position_cmd != old_main_store_recall_position_cmd)
    {
        printf("Command: main_store_recall_position_cmd: %d\n", main_store_recall_position_cmd);
        if ((main_store_recall_position_cmd > 0x8)||(main_store_recall_position_cmd < 0))
            goto out;
        data[index] = 0x05;
        data[index+1] = main_store_recall_position_cmd & 0xFF;
        ipc_send(data, index+2);
        old_main_store_recall_position_cmd = main_store_recall_position_cmd;
    }

    //Main message commands
    //E6 04 04 00 0B 00 xx
    main_massage = -1;
    err = pps_decoder_get_int(&decoder, "main_massage",&main_massage);
    if (err == PPS_DECODER_OK && main_massage != old_main_massage)
    {
        printf("Command: main_massage: %d\n", main_massage);
        if ((main_massage > 0x1)||(main_massage < 0))
            goto out;
        data[index] = 0x0B;
        data[index+1] = main_massage & 0xFF;
        ipc_send(data, index+2);
        old_main_massage = main_massage;
    }

    //Main message level commands
    //E6 04 04 00 0C 00 xx
    main_massage_level = -1;
    err = pps_decoder_get_int(&decoder, "main_massage_level",&main_massage_level);
    if (err == PPS_DECODER_OK && main_massage_level != old_main_massage_level)
    {
        printf("Command: main_massage_level: %d\n", main_massage_level);
        if ((main_massage_level > 0x1)||(main_massage_level < 0))
            goto out;
        data[index] = 0x0C;
        data[index+1] = main_massage_level & 0xFF;
        ipc_send(data, index+2);
        old_main_massage_level = main_massage_level;
    }

    //Main Heating Status
    //E6 04 04 00 0E 00 xx
    main_heatingstatus = -1;
    err = pps_decoder_get_int(&decoder, "main_heatingstatus",&main_heatingstatus);
    if (err == PPS_DECODER_OK && main_heatingstatus != old_main_heatingstatus)
    {
        printf("Command: main_heatingstatus: %d\n", main_heatingstatus);
        if ((main_heatingstatus > 0x3)||(main_heatingstatus < 0))
            goto out;
        data[index] = 0x0E;
        data[index+1] = main_heatingstatus & 0xFF;
        ipc_send(data, index+2);
        old_main_heatingstatus = main_heatingstatus;
    }

    //Main ventilation command
    //E6 04 04 00 0F 00 xx
    main_ventilation = -1;
    err = pps_decoder_get_int(&decoder, "main_ventilation",&main_ventilation);
    if (err == PPS_DECODER_OK && main_ventilation != old_main_ventilation)
    {
        printf("Command: main_ventilation: %d\n", main_ventilation);
        if ((main_ventilation > 0x3)||(main_ventilation < 0))
            goto out;
        data[index] = 0x0F;
        data[index+1] = main_ventilation & 0xFF;
        ipc_send(data, index+2);
        old_main_ventilation = main_ventilation;
    }

    //Copilot Slide Movement
    //E6 04 04 00 10 00 xx
    copilot_slide_movement = -1;
    err = pps_decoder_get_int(&decoder, "copilot_slide_movement",&copilot_slide_movement);
    if (err == PPS_DECODER_OK && copilot_slide_movement != old_copilot_slide_movement)
    {
        printf("Command: copilot_slide_movement: %d\n", copilot_slide_movement);
        if ((copilot_slide_movement > 0x2)||(copilot_slide_movement < 0))
            goto out;
        data[index] = 0x10;
        data[index+1] = copilot_slide_movement & 0xFF;
        ipc_send(data, index+2);
        old_copilot_slide_movement = copilot_slide_movement;
    }

    //Copilot recliner movement
    //E6 04 04 00 11 00 xx
    copilot_recliner_movement = -1;
    err = pps_decoder_get_int(&decoder, "copilot_recliner_movement",&copilot_recliner_movement);
    if (err == PPS_DECODER_OK && copilot_recliner_movement != old_copilot_recliner_movement)
    {
        printf("Command: copilot_recliner_movement: %d\n", copilot_recliner_movement);
        if ((copilot_recliner_movement > 0x2)||(copilot_recliner_movement < 0))
            goto out;
        data[index] = 0x11;
        data[index+1] = copilot_recliner_movement & 0xFF;
        ipc_send(data, index+2);
        old_copilot_recliner_movement = copilot_recliner_movement;
    }

    //Copilot Tilt Movement
    //E6 04 04 00 12 00 xx
    copilot_tilt_movement = -1;
    err = pps_decoder_get_int(&decoder, "copilot_tilt_movement",&copilot_tilt_movement);
    if (err == PPS_DECODER_OK && copilot_tilt_movement != old_copilot_tilt_movement)
    {
        printf("Command: copilot_tilt_movement: %d\n", copilot_tilt_movement);
        if ((copilot_tilt_movement > 0x2)||(copilot_tilt_movement < 0))
            goto out;
        data[index] = 0x12;
        data[index+1] = copilot_tilt_movement & 0xFF;
        ipc_send(data, index+2);
        old_copilot_tilt_movement = copilot_tilt_movement;
    }

    //Copilot height Movement
    //E6 04 04 00 13 00 xx
    copilot_height_movement = -1;
    err = pps_decoder_get_int(&decoder, "copilot_height_movement",&copilot_height_movement);
    if (err == PPS_DECODER_OK && copilot_height_movement != old_copilot_height_movement)
    {
        printf("Command: copilot_height_movement: %d\n", copilot_height_movement);
        if ((copilot_height_movement > 0x2)||(copilot_height_movement < 0))
            goto out;
        data[index] = 0x13;
        data[index+1] = copilot_height_movement & 0xFF;
        ipc_send(data, index+2);
        old_copilot_height_movement = copilot_height_movement;
    }

    //Copilot Lumbar Adjustment
    //E6 04 04 00 14 00 xx
    copilot_lumbar_adjustment = -1;
    err = pps_decoder_get_int(&decoder, "copilot_lumbar_adjustment",&copilot_lumbar_adjustment);
    if (err == PPS_DECODER_OK && copilot_lumbar_adjustment != old_copilot_lumbar_adjustment)
    {
        printf("Command: copilot_lumbar_adjustment: %d\n", copilot_lumbar_adjustment);
        if ((copilot_lumbar_adjustment > 0x4)||(copilot_lumbar_adjustment < 0))
            goto out;
        data[index] = 0x14;
        data[index+1] = copilot_lumbar_adjustment & 0xFF;
        ipc_send(data, index+2);
        old_copilot_lumbar_adjustment = copilot_lumbar_adjustment;
    }

    //Copilot store/recall commands
    //E6 04 04 00 15 00 xx
    copilot_store_recall_position_cmd = -1;
    err = pps_decoder_get_int(&decoder, "copilot_store_recall_position_cmd",&copilot_store_recall_position_cmd);
    if (err == PPS_DECODER_OK && copilot_store_recall_position_cmd != old_copilot_store_recall_position_cmd)
    {
        printf("Command: copilot_store_recall_position_cmd: %d\n", copilot_store_recall_position_cmd);
        if ((copilot_store_recall_position_cmd > 0x8)||(copilot_store_recall_position_cmd < 0))
            goto out;
        data[index] = 0x15;
        data[index+1] = copilot_store_recall_position_cmd & 0xFF;
        ipc_send(data, index+2);
        old_copilot_store_recall_position_cmd = copilot_store_recall_position_cmd;
    }

    //Copilot message commands
    //E6 04 04 00 1B 00 xx
    copilot_massage = -1;
    err = pps_decoder_get_int(&decoder, "copilot_massage",&copilot_massage);
    if (err == PPS_DECODER_OK && copilot_massage != old_copilot_massage)
    {
        printf("Command: copilot_massage: %d\n", copilot_massage);
        if ((copilot_massage > 0x1)||(copilot_massage < 0))
            goto out;
        data[index] = 0x1B;
        data[index+1] = copilot_massage & 0xFF;
        ipc_send(data, index+2);
        old_copilot_massage = copilot_massage;
    }

    //Copilot message level commands
    //E6 04 04 00 1C 00 xx
    copilot_massage_level = -1;
    err = pps_decoder_get_int(&decoder, "copilot_massage_level",&copilot_massage_level);
    if (err == PPS_DECODER_OK && copilot_massage_level != old_copilot_massage_level)
    {
        printf("Command: copilot_massage_level: %d\n", copilot_massage_level);
        if ((copilot_massage_level > 0x3)||(copilot_massage_level < 0))
            goto out;
        data[index] = 0x1C;
        data[index+1] = copilot_massage_level & 0xFF;
        ipc_send(data, index+2);
        old_copilot_massage_level = copilot_massage_level;
    }

    //Copilot Heating Status
    //E6 04 04 00 1E 00 xx
    copilot_heatingstatus = -1;
    err = pps_decoder_get_int(&decoder, "copilot_heatingstatus",&copilot_heatingstatus);
    if (err == PPS_DECODER_OK && copilot_heatingstatus != old_copilot_heatingstatus)
    {
        printf("Command: copilot_heatingstatus: %d\n", copilot_heatingstatus);
        if ((copilot_heatingstatus > 0x3)||(copilot_heatingstatus < 0))
            goto out;
        data[index] = 0x1E;
        data[index+1] = copilot_heatingstatus & 0xFF;
        ipc_send(data, index+2);
        old_copilot_heatingstatus = copilot_heatingstatus;
    }

    //copilot ventilation command
    //E6 04 04 00 1F 00 xx
    copilot_ventilation = -1;
    err = pps_decoder_get_int(&decoder, "copilot_ventilation",&copilot_ventilation);
    if (err == PPS_DECODER_OK && copilot_ventilation != old_copilot_ventilation)
    {
        printf("Command: copilot_ventilation: %d\n", copilot_ventilation);
        if ((copilot_ventilation > 0x3)||(copilot_ventilation < 0))
            goto out;
        data[index] = 0x1F;
        data[index+1] = copilot_ventilation & 0xFF;
        ipc_send(data, index+2);
        old_copilot_ventilation = copilot_ventilation;
    }

out:
    pps_decoder_pop(&decoder);
    pps_decoder_cleanup(&decoder);


    return true;
}

