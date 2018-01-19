#include "uart.h"
#include "ipc/ipc.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <sys/time.h>

#include "uart.h"

class Data
{
public:
    explicit Data(const uint8_t *data,  int size)
    {
        mData = new uint8_t[size];
        memcpy(mData, data, size);
        mSize = size;
    }

    ~Data()
    {
        delete mData;
    }

    uint8_t *mData;
    int mSize;
};

static uint8_t doorRxdBuffer[READ_BUF_MAX_SIZE];
static uint8_t seatRxdBuffer[READ_BUF_MAX_SIZE];
static uint8_t gestureRxdBuffer[READ_BUF_MAX_SIZE];

static int s_uart_fd = -1;

std::vector<class Data*> sData(0);
static pthread_t send_tid = -1;

static pthread_mutex_t door_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t door_cond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t seat_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t seat_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t gesture_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t gesture_cond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t send_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t send_cond = PTHREAD_COND_INITIALIZER;

struct timeval tv;

void* uart_send_thread_func(void* arg)
{
    class Data *data;
     int ret = 0;

    while (1)
    {
        while (sData.size() > 0)
        {
                    data = sData[0];
                    ret = write(s_uart_fd, data->mData, data->mSize);

                    if (ret < 0)
                    {
                        printf("%s:%d write error: %s\n", __FUNCTION__, __LINE__, strerror(errno));
                    }

                    printf("uart_send data:");
                    for (int i = 0; i < data->mSize; i++)
                    {
                        printf("%02X", data->mData[i]);
                    }
                    printf("\n\n");
                    delete data;
                    sData.erase(sData.begin());
                    usleep(10000);
                    //gettimeofday(&tv, 0);
                    //printf("current time = %ld\n", tv.tv_usec);
        }

        pthread_mutex_lock(&send_mutex);
        pthread_cond_wait(&send_cond, &send_mutex);
        pthread_mutex_unlock(&send_mutex);
    }
}

int uart_init(const char* uart_dev)
{
    s_uart_fd = open(uart_dev, O_RDWR | O_NOCTTY);
    if (s_uart_fd < 0)
    {
        printf("open %s error: %s\n", uart_dev, strerror(errno));
    }

    pthread_create(&send_tid, NULL, &uart_send_thread_func, NULL);

    return 0;
}

void uart_exit()
{
    pthread_join(send_tid, NULL);

    if (s_uart_fd)
    {
        close(s_uart_fd);
    }
}

void* uart_read_thread_func(void*)
{
    int size = 0;
    uint8_t buf[READ_BUF_MAX_SIZE];

    memset(buf, 0, READ_BUF_MAX_SIZE);

    while (1)
    {
        size = read(s_uart_fd, buf, READ_BUF_MAX_SIZE);
        if(size < 0)
        {
            printf("%s:%d read error: %s\n", __FUNCTION__, __LINE__, strerror(errno));
            break;
        }

        if (size != 7)
            continue;

        printf("UART Receive\nbuf: %d\n", size);
        for (int i = 0; i < size; i++)
        {
            printf("%02X", buf[i] & 0xFF);
        }
        printf("\n");

        if (0x02 == buf[2])
        {
            memcpy(doorRxdBuffer, buf, size);

            pthread_mutex_lock(&door_mutex);
            pthread_cond_signal(&door_cond);
            pthread_mutex_unlock(&door_mutex);
        }
        else if (0x04 == buf[2])
        {
            memcpy(seatRxdBuffer, buf, size);
            pthread_mutex_lock(&seat_mutex);
            pthread_cond_signal(&seat_cond);
            pthread_mutex_unlock(&seat_mutex);
        }
        else if (0x06 == buf[2])
        {
            memcpy(gestureRxdBuffer, buf, size);
            pthread_mutex_lock(&gesture_mutex);
            pthread_cond_signal(&gesture_cond);
            pthread_mutex_unlock(&gesture_mutex);
        }

        memset(buf, 0, size);
    }

    return NULL;
}

void* uart_door_process_thread_func(void*)
{
    int pps_fd = 0;
    int length;
    char buf[16];

    if ((pps_fd = open("/pps/ui", O_WRONLY | O_CREAT, 0666)) < 0)
    {
        std::cout << "open " << "/pps/ui" << " error: " << strerror(errno) << std::endl;
        return NULL;
    }

    while (1)
    {
        pthread_mutex_lock(&door_mutex);
        pthread_cond_wait(&door_cond, &door_mutex);
        pthread_mutex_unlock(&door_mutex);

        switch ( doorRxdBuffer[4] )
        {
        case 0x01:
            printf("drive door toggle!:%d\n", doorRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "fl", doorRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x02:
            printf("passenger door toggle!:%d\n", doorRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "fr", doorRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x03:
            printf("rearleft door toggle!:%d\n", doorRxdBuffer[5]);
        sprintf (buf, "%s::%d\n", "rl", doorRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x04:
            printf("rearright door toggle!:%d\n", doorRxdBuffer[5]);
        sprintf (buf, "%s::%d\n", "rr", doorRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x05:
            printf("back door toggle!:%d\n", doorRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "backdoor", doorRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x12:
            printf("frontlights toggle!:%d\n", doorRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "frontlights", doorRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        default:
            printf("unknown command!\n\n");
            break;
        }
    }

    close(pps_fd);
    return NULL;
}


void* uart_seat_process_thread_func(void* seat_fd)
{
    int pps_fd = *((int*)seat_fd);
    int length;
    char buf[512];

//    if ((pps_fd = open("/pps/hinge-tech/seatcontrol", O_WRONLY | O_CREAT, 0666)) < 0)
//    {
//        std::cout << "open " << "/pps/hinge-tech/seatcontrol"
//        << " error: " << strerror(errno) << std::endl;

//        return NULL;
//    }

    while (1)
    {
        pthread_mutex_lock(&seat_mutex);
        pthread_cond_wait(&seat_cond, &seat_mutex);
        pthread_mutex_unlock(&seat_mutex);

        switch ( seatRxdBuffer[4] )
        {
        /* Main seat status */
        case 0x00:				//main slider movement status
            printf("main slider movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_slider_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x01:				//main recliner movement status
            printf("main recliner movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_recliner_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x02:				//main tilt movement status
            printf("main tilt movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_tilt_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x03:				//main height movement status
            printf("main height movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_height_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x04: 				//main lumbar movement status
            printf("main lumbar movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_lumbar_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x06:				//main store recall position cmd
            printf("main store recall position cmd!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_store_recall_position_cmd", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x07:				//main slider position
            printf("main slider position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_slider_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x08:				//main recliner position
            printf("main recliner position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_recliner_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x09:				//main tilt position
            printf("main tilt position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_tilt_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x0A:				//main height position
            printf("main height position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_height_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x0D:				//main massage status
            printf("main massage status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_massage_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x0E:				//main heighting status
            printf("main heighting status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_heighting_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x0F:				//main ventilation status
            printf("main ventilation status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "main_ventilation_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        /* Copilot seat status */
        case 0x10:				//copilot slider movement status
            printf("copilot slider movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_slider_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x11:				//copilot recliner movement status
            printf("copilot recliner movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_recliner_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x12:				//copilot tilt movement status
            printf("copilot tilt movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_tilt_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x13:				//copilot height movement status
            printf("copilot height movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_height_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x14: 				//copilot lumbar movement status
            printf("copilot lumbar movement status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_lumbar_movement_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x16:				//copilot store recall position cmd
            printf("copilot store recall position cmd!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_store_recall_position_cmd", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x17:				//copilot slider position
            printf("copilot slider position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_slider_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x18:				//copilot recliner position
            printf("copilot recliner position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_recliner_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x19:				//copilot tilt position
            printf("copilot tilt position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_tilt_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x1A:				//copilot height position
            printf("copilot height position!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_height_position", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x1D:				//copilot massage status
            printf("copilot massage status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_massage_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x1E:				//copilot heighting status
            printf("copilot heighting status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_heighting_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        case 0x1F:				//copilot ventilation status
            printf("copilot ventilation status!:%d\n", seatRxdBuffer[5]);
            sprintf (buf, "%s::%d\n", "copilot_ventilation_status", seatRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
                printf("wirte ok!\n\n");
            break;

        default:
            printf("unknown command!\n\n");
            break;
        }
    }

    close(pps_fd);
    return NULL;
}

void* uart_gesture_process_thread_func(void* arg)
{
    int pps_fd = *((int*)arg);
    int length;
    char buf[32];

    while (1)
    {
        pthread_mutex_lock(&gesture_mutex);
        pthread_cond_wait(&gesture_cond, &gesture_mutex);
        pthread_mutex_unlock(&gesture_mutex);

        switch ( gestureRxdBuffer[4] )
        {
        case 0x01:
            if(0x01 == gestureRxdBuffer[5])
            {
                sprintf (buf, "%s::%s\n", "gesture_tip", "show" );
            }
            else
            {
                sprintf (buf, "%s::%s\n", "gesture_tip", "hide" );
            }
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
            {
                printf("wirte ok!\n\n");
            }
            break;

        case 0x02:
            if(0x01 == gestureRxdBuffer[5])
            {
                sprintf (buf, "%s::%s\n", "gesture", "left" );
            }
            else if(0x02 == gestureRxdBuffer[5])
            {
                sprintf (buf, "%s::%s\n", "gesture", "right" );
            }
            else if(0x03 == gestureRxdBuffer[5])
            {
                sprintf (buf, "%s::%s\n", "gesture", "up" );
            }
            else if(0x04 == gestureRxdBuffer[5])
            {
                sprintf (buf, "%s::%s\n", "gesture", "down" );
            }
            else {
                sprintf (buf, "%s::%s\n", "gesture", "none" );
            }
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
            {
                printf("wirte ok!\n\n");
            }
            break;

        case 0x03:
            sprintf (buf, "%s::%d\n", "clockwise", gestureRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
            {
                printf("wirte ok!\n\n");
            }
            break;

        case 0x04:
            sprintf (buf, "%s::%d\n", "gesture_xyz", gestureRxdBuffer[5] );
            length = strlen ( buf );

            if ( write ( pps_fd, buf, length ) == length)
            {
                printf("wirte ok!\n\n");
            }
            break;

        default:
            printf("unknown command!\n\n");
            break;
        }
    }

    return NULL;
}

int uart_send(const uint8_t *data, int len)
{
    class Data *d = NULL;
    try
    {
       d = new  Data(data, len);
    }
    catch(...)
    {
        return 0;
    }

    sData.push_back(d);

    if (sData.size() == 1)
    {
        pthread_mutex_lock(&send_mutex);
        pthread_cond_signal(&send_cond);
        pthread_mutex_unlock(&send_mutex);
    }

    return len;
}
