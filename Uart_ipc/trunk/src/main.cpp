#include <iostream>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>
#include <stdio.h>

#include "uart/uart.h"
#include "channel/AirControl.h"
#include "channel/SeatControl.h"
#include "channel/LampControl.h"
#include "channel/ClusterControl.h"
#include "channel/GestureControl.h"

#define PPS_AIR_FILE  "/pps/qnxcar/hvac?delta"
#define PPS_LAMP_FILE "/pps/hinge-tech/lampstatus?delta"
#define PPS_SEAT_FILE "/pps/hinge-tech/seatcontrol?delta"
#define PPS_CAR_FILE "/pps/ui?delta"
#define PPS_SCREEN_FILE "/pps/screen?delta"
#define PPS_THEME_FILE "/pps/qnxcar/profile/theme?delta"
#define PPS_SENSORS_FILE "/pps/qnxcar/sensors?delta"
#define PPS_MUSIC_STATE_FILE "/pps/services/mm-player/HMI/state?delta"
#define PPS_MUSIC_STATUS_FILE "/pps/services/mm-player/HMI/status?delta"
#define PPS_MUSIC_TRACK_FILE "/pps/services/mm-player/HMI/track?delta"
#define PPS_GESTURE_FILE "/pps/hinge-tech/gesture_control?delta"

#define PPS_AIR_FILE_PATH  "/pps/qnxcar/hvac"
#define PPS_LAMP_FILE_PATH "/pps/hinge-tech/lampstatus"
#define PPS_SEAT_FILE_PATH "/pps/hinge-tech/seatcontrol"
#define PPS_CAR_FILE_PATH "/pps/ui"
#define PPS_SCREEN_FILE_PATH "/pps/screen"
#define PPS_THEME_FILE_PATH "/pps/qnxcar/profile/theme"
#define PPS_SENSORS_FILE_PATH "/pps/qnxcar/sensors"
#define PPS_MUSIC_STATE_FILE_PATH "/pps/services/mm-player/HMI/state"
#define PPS_MUSIC_STATUS_FILE_PATH "/pps/services/mm-player/HMI/status"
#define PPS_MUSIC_TRACK_FILE_PATH "/pps/services/mm-player/HMI/track"
#define PPS_GESTURE_FILE_PATH "/pps/hinge-tech/gesture_control"

#define UART_DEV_FILE "/dev/ser2"

int music_track_fd  = -1;

bool is_track_ok  = false;
bool is_track_new = true;

int main(int argc, char** argv)
{
    char buf[512];
    AirControl airControl(1);
    LampControl lampControl(3);
    SeatControl seatControl(4);
    ClusterControl clusterControl(5);
    GestureControl gestureControl(6);

    int seat_fd = -1;
    int air_fd = -1;
    int lamp_fd = -1;
    int car_fd = -1;
    int screen_fd = -1;
    int theme_fd = -1;
    int sensors_fd = -1;
    int music_state_fd = -1;
    int music_status_fd = -1;
    int gesture_fd = -1;

    fd_set rfds;
    int max_fd = 0;

    pthread_t readpthr;
    pthread_t doorpthr;
    pthread_t seatpthr;
    int ret;

    if (argc > 1)
    {
        uart_init(argv[argc - 1]);
    }
    else
    {
        uart_init(UART_DEV_FILE);
    }

    while(access(PPS_GESTURE_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_GESTURE_FILE_PATH);
        usleep(10000);
    }

    if ((gesture_fd = open(PPS_GESTURE_FILE, O_RDWR, 0666)) < 0)
    {
        std::cout << "open " << PPS_GESTURE_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    read(gesture_fd, buf, 512);

    while(access(PPS_AIR_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_AIR_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_LAMP_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_LAMP_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_SEAT_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_SEAT_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_CAR_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_CAR_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_SCREEN_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_SCREEN_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_THEME_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_THEME_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_SENSORS_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_SENSORS_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_MUSIC_STATE_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_MUSIC_STATE_FILE_PATH);
        usleep(10000);
    }

    while(access(PPS_MUSIC_STATUS_FILE_PATH, 0))
    {
        printf("waiting %s\n", PPS_MUSIC_STATUS_FILE_PATH);
        usleep(10000);
    }

//    while(access(PPS_MUSIC_TRACK_FILE_PATH, 0))
//    {
//        printf("waiting %s\n", PPS_MUSIC_TRACK_FILE_PATH);
//        usleep(10000);
//    }

    printf("start open files\n");

    if ((seat_fd = open(PPS_SEAT_FILE, O_RDWR | O_CREAT, 0666)) < 0)
    {
        std::cout << "open " << PPS_SEAT_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((air_fd = open(PPS_AIR_FILE, O_RDONLY | O_CREAT, 0666)) < 0)
    {
        std::cout << "open " << PPS_AIR_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((lamp_fd = open(PPS_LAMP_FILE, O_RDONLY | O_CREAT, 0666)) < 0)
    {
        std::cout << "open " << PPS_LAMP_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((car_fd = open(PPS_CAR_FILE, O_RDONLY, 0666)) < 0)
    {
        std::cout << "open " << PPS_CAR_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((screen_fd = open(PPS_SCREEN_FILE, O_RDONLY, 0666)) < 0)
    {
        std::cout << "open " << PPS_SCREEN_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((theme_fd = open(PPS_THEME_FILE, O_RDONLY, 0666)) < 0)
    {
        std::cout << "open " << PPS_THEME_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((sensors_fd = open(PPS_SENSORS_FILE, O_RDONLY, 0666)) < 0)
    {
        std::cout << "open " << PPS_SENSORS_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((music_state_fd = open(PPS_MUSIC_STATE_FILE, O_RDONLY, 0666)) < 0)
    {
        std::cout << "open " << PPS_MUSIC_STATE_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    if ((music_status_fd = open(PPS_MUSIC_STATUS_FILE, O_RDONLY, 0666)) < 0)
    {
        std::cout << "open " << PPS_MUSIC_STATUS_FILE << " error: " << strerror(errno) << std::endl;
        goto out;
    }

    max_fd = seat_fd > air_fd ? seat_fd : air_fd;
    max_fd = max_fd > lamp_fd ? max_fd : lamp_fd;
    max_fd = max_fd > car_fd ? max_fd : car_fd;
    max_fd = max_fd > screen_fd ? max_fd : screen_fd;
    max_fd = max_fd > theme_fd ? max_fd : theme_fd;
    max_fd = max_fd > sensors_fd ? max_fd : sensors_fd;
    max_fd = max_fd > music_state_fd ? max_fd : music_state_fd;
    max_fd = max_fd > music_status_fd ? max_fd : music_status_fd;
    max_fd = max_fd > gesture_fd ? max_fd : gesture_fd;

    ret = pthread_create(&doorpthr, NULL, &uart_door_process_thread_func, NULL);
    ret = pthread_create(&seatpthr, NULL, &uart_seat_process_thread_func, &seat_fd);
    ret = pthread_create(&seatpthr, NULL, &uart_gesture_process_thread_func, &gesture_fd);
    ret = pthread_create(&readpthr, NULL, &uart_read_thread_func, NULL);

    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(seat_fd, &rfds);
        FD_SET(air_fd, &rfds);
        FD_SET(lamp_fd, &rfds);
        FD_SET(car_fd, &rfds);
        FD_SET(screen_fd, &rfds);
        FD_SET(theme_fd, &rfds);
        FD_SET(sensors_fd, &rfds);
        FD_SET(music_state_fd, &rfds);
        FD_SET(music_status_fd, &rfds);
        FD_SET(gesture_fd, &rfds);

        if ( is_track_ok ) {
          FD_SET ( music_track_fd, &rfds );
        }

        if (select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            //std::cout << "select error: " << strerror(errno) << std::endl;
          //break;
        }

        if (FD_ISSET(seat_fd, &rfds))
        {
            printf("FD_ISSET seat_fd\n");
            seatControl.process(seat_fd);
        }

        if (FD_ISSET(air_fd, &rfds))
        {
            printf("FD_ISSET air_fd\n");
            airControl.process(air_fd);
        }

        if (FD_ISSET(lamp_fd, &rfds))
        {
            printf("FD_ISSET lamp_fd\n");
            lampControl.process(lamp_fd);
        }
#if 1
        if (FD_ISSET(car_fd, &rfds))
        {
            printf("FD_ISSET car_fd\n");
            clusterControl.car_process(car_fd);
        }

        if (FD_ISSET(screen_fd, &rfds))
        {
            printf("FD_ISSET screen_fd\n");
            clusterControl.screen_process(screen_fd);
        }

        if (FD_ISSET(theme_fd, &rfds))
        {
            printf("FD_ISSET theme_fd\n");
            clusterControl.theme_process(theme_fd);
        }

        if (FD_ISSET(sensors_fd, &rfds))
        {
            printf("FD_ISSET sensors_fd\n");
            clusterControl.sensors_process(sensors_fd);
        }

        if (FD_ISSET(music_state_fd, &rfds))
        {
            printf("FD_ISSET music_state_fd\n");
            clusterControl.music_state_process(music_state_fd);
            if ( is_track_ok && is_track_new ) {
              max_fd = max_fd > music_track_fd ? max_fd : music_track_fd;
              is_track_new = false;
            }
        }

        if (FD_ISSET(music_status_fd, &rfds))
        {
            printf("FD_ISSET music_status_fd\n");
            clusterControl.music_status_process(music_status_fd);
        }

        if (FD_ISSET(music_track_fd, &rfds))
        {
            printf("FD_ISSET music_track_fd\n");
            clusterControl.music_track_process(music_track_fd);
        }

        if (FD_ISSET(gesture_fd, &rfds))
        {
            printf("FD_ISSET gesture_fd\n");
            gestureControl.process(gesture_fd);
        }
#endif
    }

    pthread_join(readpthr, NULL);
    pthread_join(doorpthr, NULL);
    pthread_join(seatpthr, NULL);

out:
    if (seat_fd != -1)
    {
        close(seat_fd);
    }

    if (air_fd != -1)
    {
        close(air_fd);
    }

    if (lamp_fd != -1)
    {
        close(lamp_fd);
    }

    if (car_fd != -1)
    {
        close(car_fd);
    }

    if (screen_fd != -1)
    {
        close(screen_fd);
    }

    if (theme_fd != -1)
    {
        close(theme_fd);
    }

    if (sensors_fd != -1)
    {
        close(sensors_fd);
    }

    if (music_state_fd != -1)
    {
        close(music_state_fd);
    }

    if (music_status_fd != -1)
    {
        close(music_status_fd);
    }

    if (music_track_fd != -1)
    {
        close(music_track_fd);
    }

    if (gesture_fd != -1)
    {
        close(gesture_fd);
    }

    uart_exit();

    return 0;
}
