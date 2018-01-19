#include <ctype.h>        // for isdigit
#include <netinet/in.h>   // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>   // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>       // for exit
#include <string.h>       // for bzero
#include <errno.h>        // for errno
#include <pthread.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#include <img/img.h>
#include <screen/screen.h>

#include "display.h"

#define DEFAULT_PKT_SIZE 1500
#define IMG_BUF_SIZE 1048576
#define IMG_BUF_COUNT 4

#define PPS_CAMERA_FILE "/pps/hinge-tech/camera"

enum DECODE_STATUS
{
    DECODE_IDLE = 0,
    DECODE_INPUT,
    DECODE_READY,
    DECODE_DECODING
};

static int g_capture;
static int g_port;
static int g_direct;
static unsigned char *g_img_buf[IMG_BUF_COUNT];
static int g_nwrite[IMG_BUF_COUNT];
static int g_decode_status[IMG_BUF_COUNT];
static int g_cnt = 0;

static pthread_mutex_t capture_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t capture_cond = PTHREAD_COND_INITIALIZER;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

void* display_thread(void* arg)
{
    screen_context_t screen_ctx;
    screen_window_t screen_win;
    screen_buffer_t screen_buf[2];
    screen_event_t screen_ev;
    int size[2] = { 816, 459 };
    int pos[2] = { 192, 1078 };
    int hide_pos[2] = {1200, 1920};
    int spos[2] = { 0, 0 };
    int format = SCREEN_FORMAT_RGBX8888;
    int val;
    const char *tok;
    int stride;
    void *pointer;
    int count = 1; //for images count, default 1
    int interval = 0; //0ms as default
    int skip = 0;//image switching skip
    unsigned int img_format = IMG_FMT_PKHE_ARGB8888;
    int img_scale = 0;
    img_lib_t img_lib = NULL;
    img_t img;
    int rc, i = 0;
    int ndisplays;                                 /* number of displays */
    const char *display = "2";              /* the display to create our window on */
    screen_display_t screen_disp;                  /* native handle for our display */
    int verbose = 0;
    int init_display = 1;
    int mode_idx = -1;
    int pipeline = 1;
    int transp = SCREEN_TRANSPARENCY_NONE;
    int transp_set = 0;
    int zorder = 20;

    io_stream_t *input;
    int cnt = 0;

    int rect[4] = {0, 0, size[0], size[1]};

    int data = 0x01;
    ThreadCtl(_NTO_TCTL_RUNMASK, &data);

start:

    rc = screen_create_context(&screen_ctx,SCREEN_APPLICATION_CONTEXT);
    if (rc) 
    {
	perror("screen_context_create");
	goto fail1;
    }

    rc = screen_get_context_property_iv(screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &ndisplays);
    if (rc) 
    {
	perror("screen_get_context_property_iv(SCREEN_PROPERTY_DISPLAY_COUNT)");
	goto fail2;
    }

    rc = screen_create_window(&screen_win, screen_ctx);
    if (rc) 
    {
	perror("screen_create_window");
	goto fail2;
    }

    screen_display_t *displays;
    displays = calloc(ndisplays, sizeof(*displays));
    if (displays == NULL) 
    {
	perror("could not allocate memory for display list");
	goto fail3;
    }

    rc = screen_get_context_property_pv(screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)displays);
    if (rc) 
    {
	perror("screen_get_context_property_pv(SCREEN_PROPERTY_DISPLAYS)");
	free(displays);
	goto fail3;
    }

    if (isdigit(*display)) 
    {
	int want_id = atoi(display);
	for (i = 0; i < ndisplays; ++i) 
	{
	    int actual_id = 0;  // invalid
	    (void)screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_ID, &actual_id);
	    if (want_id == actual_id) 
	    {
		break;
	    }
	}
    } 
    else 
    {
	int type = -1;
	if (strcmp(display, "internal") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_INTERNAL;
	} 
	else if (strcmp(display, "composite") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_COMPOSITE;
	} 
	else if (strcmp(display, "svideo") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_SVIDEO;
	} 
	else if (strcmp(display, "YPbPr") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_COMPONENT_YPbPr;
	} 
	else if (strcmp(display, "rgb") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_COMPONENT_RGB;
	} 
	else if (strcmp(display, "rgbhv") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_COMPONENT_RGBHV;
	} 
	else if (strcmp(display, "dvi") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_DVI;
	} 
	else if (strcmp(display, "hdmi") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_HDMI;
	} 
	else if (strcmp(display, "other") == 0) 
	{
	    type = SCREEN_DISPLAY_TYPE_OTHER;
	} 
	else 
	{
	    fprintf(stderr, "unknown display type %s\n", display);
	    free(displays);
	    goto fail3;
	}

	for (i = 0; i < ndisplays; i++) 
	{
	    screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_TYPE, &val);
	    if (val == type) 
	    {
		break;
	    }
	}
    }

    if (i >= ndisplays) 
    {
	fprintf(stderr, "couldn't find display %s\n", display);
	free(displays);
	goto fail3;
    }

    screen_disp = displays[i];
    free(displays);

    if (init_display)
    {
	if ( display_setup(screen_ctx,  i, verbose) == -1) 
	{
	    perror("display setup failed");
	    goto fail3;
	}
    } 
    else if (mode_idx >= 0) 
    {
	int mode_count = 0, rc;
	screen_display_mode_t *modes;
	rc = screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_MODE_COUNT, &mode_count);
	
	if (verbose) 
	{
	    printf("get MODE_COUNT property, count = %d, rc = %d\n", mode_count, rc );
	}

	if (mode_idx >= mode_count) 
	{
	    fprintf(stderr, "couldn't find mode %d, last mode is %d\n", mode_idx, mode_count);
	    goto fail3;
	}

	modes = malloc(mode_count * sizeof(modes[0]));
	if (!modes) 
	{
	    perror("display modes list malloc failed");
	    goto fail3;
	}

	rc = screen_get_display_modes(screen_disp, mode_count, modes);
	if (verbose) 
	{
	    printf("get display modes, rc = %d\n", rc );
	}
	
	rc = screen_set_display_property_iv(screen_disp, SCREEN_PROPERTY_MODE, &mode_idx);
	if (verbose) 
	{
	    printf("set size %dx%d, rc = %d\n", modes[mode_idx].width, modes[mode_idx].height, rc );
	}
	rc = screen_flush_context(screen_ctx, 0);
	if (verbose) 
	{
	    printf("flushed, rc = %d\n", rc );
	}
    }

    rc = screen_set_window_property_pv(screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&screen_disp);
    if (rc) 
    {
	perror("screen_set_window_property_pv(SCREEN_PROPERTY_DISPLAY)");
	goto fail3;
    }

    val = SCREEN_USAGE_WRITE;
    if (pipeline != -1) 
    {
	val |= SCREEN_USAGE_OVERLAY;
    }
    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &val);
    if (rc) 
    {
	perror("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");
	goto fail3;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, &format);
    if (rc) 
    {
	perror("screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
	goto fail3;
    }

    if (size[0] == -1 || size[1] == -1) 
    {
        rc = screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, size);
	if (rc) 
	{
	    perror("screen_get_window_property_iv(SCREEN_PROPERTY_SIZE)");
	    goto fail3;
	}
    }

    if (transp_set) 
    {
	if (verbose) 
	{
	    printf("transp = %d\n", transp);
	}
	
	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_TRANSPARENCY, &transp);
	if (rc) 
	{
	    perror("screen_set_window_property_iv(SCREEN_PROPERTY_TRANSPARENCY)");
	    goto fail3;
	}
    } 
    else 
    {
	if (verbose) 
	{
	    printf("transp = (not set)\n", transp);
	}
    }

    if (verbose) 
    {
        printf("zorder = %d\n", zorder);
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ZORDER, &zorder);
    if (rc) 
    {
	perror("screen_set_window_property_iv(SCREEN_PROPERTY_ZORDER)");
	goto fail3;
    }

    if (pipeline != -1) 
    {
	if (verbose) 
	{
	    printf("pipeline = %d\n", pipeline);
	}
	
	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_PIPELINE, &pipeline);
	if (rc) 
	{
	    perror("screen_set_window_property_iv(SCREEN_PROPERTY_PIPELINE)");
	    goto fail3;
	}
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
    if (rc) 
    {
	perror("screen_set_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE)");
	goto fail3;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SOURCE_SIZE, size);
    if (rc) 
    {
	perror("screen_set_window_property_iv(SCREEN_PROPERTY_SOURCE_SIZE)");
	goto fail3;
    }

    if ( spos[0] != 0 || spos[1] != 0 ) 
    {
	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SOURCE_POSITION, spos);
	if (rc) 
	{
	    perror("screen_set_window_property_iv(SCREEN_PROPERTY_SOURCE_POSITION)");
	    goto fail3;
	}
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SIZE, size);
    if (rc) 
    {
	perror("screen_set_window_property_iv(SCREEN_PROPERTY_SIZE)");
	goto fail3;
    }

    if (pos[0] != 0 || pos[1] != 0) 
    {
	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_POSITION, pos);
	if (rc) 
	{
	    perror("screen_set_window_property_iv(SCREEN_PROPERTY_POSITION)");
	    goto fail3;
	}
    }

    // Set ID string for debugging via /dev/screen.
    {
	const char *idstr = "display_camera";
	screen_set_window_property_cv(screen_win,
			SCREEN_PROPERTY_ID_STRING, strlen(idstr), idstr);
    }

    rc = screen_create_window_buffers(screen_win, 1);
    if (rc) 
    {
	perror("screen_create_window_buffers");
	goto fail3;
    }

    rc = screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)screen_buf);
    if (rc) 
    {
	perror("screen_get_window_property_pv(SCREEN_PROPERTY_RENDER_BUFFERS)");
	goto fail3;
    }

    rc = screen_get_buffer_property_pv(*screen_buf, SCREEN_PROPERTY_POINTER, &pointer);
    if (rc)
    {
	perror("screen_get_buffer_property_pv(SCREEN_PROPERTY_POINTER)");
	goto fail3;
    }

    rc = screen_get_buffer_property_iv(*screen_buf, SCREEN_PROPERTY_STRIDE, &stride);
    if (rc) 
    {
	perror("screen_get_buffer_property_iv(SCREEN_PROPERTY_STRIDE)");
	goto fail3;
    }

    rc = screen_create_event(&screen_ev);
    if (rc) 
    {
	perror("screen_create_event");
	goto fail3;
    }

    memset(&img, 0, sizeof(img));
    img.w = size[0];
    img.h = size[1];
    img.format = img_format;
    img.flags = IMG_DIRECT |IMG_FORMAT | IMG_W | IMG_H;

    img.access.direct.data = (uint8_t *)pointer;
    img.access.direct.stride = stride;

    rc = img_lib_attach(&img_lib);
    if (rc != IMG_ERR_OK)
    {
	fprintf(stderr, "img_lib_attach() failed: img_errno: %d\n", rc);
	goto fail4;
    }

    if (g_capture != 1)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }

    while (1)
    {
	if (g_capture == 1)
	{
	    pthread_mutex_lock(&mutex);
	    pthread_cond_wait(&cond, &mutex);     
	    pthread_mutex_unlock(&mutex);
	}

	if (g_capture != 1)
	{
	    img_lib_detach(img_lib);
	    screen_destroy_event(screen_ev);
	    screen_destroy_window(screen_win);
	    screen_destroy_context(screen_ctx);

	    printf("window hide\n");

	    while (1)
	    {
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);     
		pthread_mutex_unlock(&mutex);

		if (g_capture == 1)
		{
		    printf("window show\n");
		    goto start;
		}
	    }
	}

	pthread_mutex_lock(&status_mutex);
	cnt = g_cnt;
	if (g_decode_status[cnt] != DECODE_INPUT)
	{
	    g_decode_status[cnt] = DECODE_DECODING;
	}
	pthread_mutex_unlock(&status_mutex);

	if (g_decode_status[cnt] != DECODE_DECODING)
	{
	    continue;
	}

	g_decode_status[cnt] = DECODE_DECODING;

	/* open an io_stream to the file in memory */
	if ((input = io_open(IO_MEM, IO_READ, g_nwrite[cnt], g_img_buf[cnt])) == NULL)
	{
	    fprintf(stderr, "io_open() failed: %d\n", errno);
	}
	else
	{
	    rc = img_load(img_lib, input, NULL, &img);
	    if (rc != IMG_ERR_OK)
	    {
		fprintf(stderr, "img_load() failed: img_errno: %d\n", rc);
	    }
	    else
	    {
		rc = screen_post_window(screen_win, *screen_buf, 1, rect, 0);
		if (rc) 
		{
		    fprintf(stderr, "screen_post_window() failed: img_errno: %d\n", rc);
		}
	    }

            io_close(input);
	}

	g_decode_status[cnt] = DECODE_IDLE;
    }

fail5:
    img_lib_detach(img_lib);

fail4:
    screen_destroy_event(screen_ev);

fail3:
    screen_destroy_window(screen_win);

fail2:
    screen_destroy_context(screen_ctx);

fail1:

    return 0;
}

void* udp_thread(void *arg)
{
    unsigned int cnt = 0;
    int begin = 0;
    int done = 0;
    int nread = 0;
    int server_socket = -1;
    unsigned char *buf = NULL;
    unsigned char tmp_buf[DEFAULT_PKT_SIZE];
    struct sockaddr_in server_addr;
    struct timeval timeout = {0, 10000};
    int direct = 0;
    int data = 0x02;
    int port = 0;
    ThreadCtl(_NTO_TCTL_RUNMASK, &data);

    pthread_mutex_lock(&capture_mutex);
    pthread_cond_wait(&capture_cond, &capture_mutex);      
    pthread_mutex_unlock(&capture_mutex);

    direct = g_direct;
    bzero(&server_addr,sizeof(server_addr)); 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_port);

    port = g_port;

    server_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) 
    {
	printf("open udp socket failed\n");
	
	return NULL;
    }

    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) 
    {
	printf("port %d: udp bind failed\n", port);
	close(server_socket);
	
	return NULL;
    }

    printf("port %d: start capture!\n", port);
    
    buf = tmp_buf;

    while (1)
    {
	if (g_capture && direct == g_direct)
	{
	    nread = recv(server_socket, buf, DEFAULT_PKT_SIZE, MSG_WAITALL);
	    if (nread < 0 && errno == EAGAIN) 
	    {
		continue;
	    }
	    
	    if (buf[0] == 0xff && buf[1] == 0xd8 && buf[2] == 0xff && buf[3] == 0xe0)	
	    {
		pthread_mutex_lock(&status_mutex);
		if (g_decode_status[cnt] != DECODE_DECODING)
		{   
		    g_decode_status[cnt] = DECODE_INPUT;
		}
		pthread_mutex_unlock(&status_mutex);

		if (g_decode_status[cnt] == DECODE_INPUT)
		{
		    memcpy(g_img_buf[cnt], buf, nread);
		    buf = g_img_buf[cnt] + nread;
		    g_nwrite[cnt] = nread;
		    done = 0;
		    begin = 1; 
		    continue;
		}
	    }

	    if (begin == 0)
	    {
		continue;
	    }

	    if ((buf[nread-3] == 0xff && buf[nread-2] == 0xd9 && buf[nread-1] == 0x00) 
		|| (buf[nread-2] == 0xff && buf[nread-1] == 0xd9))	
	    {
		g_nwrite[cnt] += nread;
		
		pthread_mutex_lock(&status_mutex);
		g_decode_status[cnt] = DECODE_READY;
		g_cnt = cnt;
		pthread_mutex_unlock(&status_mutex);

		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);      
		pthread_mutex_unlock(&mutex);

		if (++cnt == IMG_BUF_COUNT)
		{
		    cnt = 0;
		}

		buf = tmp_buf;
		
		done = 1;
		begin = 0;
	    }

	    if (done != 1)
	    {
		buf += nread;
		g_nwrite[cnt] += nread;

		if (g_nwrite[cnt] >= (IMG_BUF_SIZE - DEFAULT_PKT_SIZE))
		{
		    buf = tmp_buf;

		    begin = 0;
		}
	    }
	}
	else
	{
            close(server_socket);
	    begin = 0;
	    cnt = 0;
	    done = 0;
	    buf = tmp_buf;
            printf("port %d: stop capture!\n", port);

	    if (g_capture == 0)
	    {
		//notify to hide window
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);      
		pthread_mutex_unlock(&mutex);
		printf("notify to hide window\n");

		//wait for start capture
		pthread_mutex_lock(&capture_mutex);
		pthread_cond_wait(&capture_cond, &capture_mutex);      
		pthread_mutex_unlock(&capture_mutex);
	    }

	    direct = g_direct;

	    bzero(&server_addr,sizeof(server_addr)); 
	    server_addr.sin_family = AF_INET;
	    server_addr.sin_addr.s_addr = INADDR_ANY;
	    server_addr.sin_port = htons(g_port);

	    port = g_port;

	    server_socket = socket(PF_INET, SOCK_DGRAM, 0);
	    if (server_socket < 0) 
	    {
		printf("open udp socket failed\n");
		
		return NULL;
	    }

            setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) 
	    {
		printf("port %d: udp bind failed\n", port);
		close(server_socket);
		
		return NULL;
	    }

	    printf("port %d: start capture!\n", port);
	}
    }
    
    close(server_socket);
}

int main(int argc, char **argv)
{
    int i = 0;
    pthread_t udp_tid = -1, display_tid = -1;
    char cmd[64];
    int pps_fd = -1;
    fd_set rfd;
    char buf[256];
    char *port_str = NULL;
    int direct = 0;
    int length = 0;

    sprintf(cmd, "/opt/setzorder.sh %s 20 &", argv[0]);
    
    for (i = 0; i < IMG_BUF_COUNT; i++)
    {
        g_img_buf[i] = malloc(IMG_BUF_SIZE);
    }

    pthread_create(&udp_tid, NULL, udp_thread, NULL);
    pthread_create(&display_tid, NULL, display_thread, NULL);

    if (pthread_setschedprio(udp_tid, 255) != EOK)
    {
	printf("udp thread: pthread_setschedprio() failed\n");
    }

    if (pthread_setschedprio(display_tid, 63) != EOK)
    {
	printf("display thread: pthread_setschedprio() failed\n");
    }

    if (pthread_setschedprio(pthread_self(), 63) != EOK)
    {
	printf("main thread: pthread_setschedprio() failed\n");
    }

    g_port  = 1236;
    g_capture = 0;

    pps_fd = open(PPS_CAMERA_FILE, O_RDWR | O_CREAT, 0666);
    
    if (pps_fd < 0)
    {
	printf("open pps file failed\n");
	goto out;
    }

    write(pps_fd, "capture::stop\n", strlen("capture::stop\n"));

    while (1)
    {
	FD_ZERO(&rfd);
	FD_SET(pps_fd, &rfd);

	if (select(1 + pps_fd, &rfd, 0, 0, NULL) > 0)
	{
	    if (FD_ISSET(pps_fd, &rfd))
	    {
		length = read(pps_fd, buf, sizeof(buf) - 1);

		if (strstr(buf, "capture::start") != NULL)
		{
		    g_capture = 1;
		}
		else
		{
		    g_capture = 0;
		}

                if (strstr(buf, "direct::left") != NULL)
		{
		    direct = 0;
		}
		else
		{
		    direct = 1;
		}

		if (direct == 0)
		{
		    if ((port_str = strstr(buf, "left_port::")) != NULL)
		    {
			g_port = atoi(port_str + strlen("left_port::"));
		    }
		}
		else
		{
		    if ((port_str = strstr(buf, "right_port::")) != NULL)
		    {
			g_port = atoi(port_str + strlen("right_port::"));
		    }
		}

		printf("capture:%d, direct:%d, g_port:%d\n", g_capture, direct, g_port);

		if (g_capture == 1)
		{
		    g_direct = direct;
		    pthread_mutex_lock(&capture_mutex);
		    pthread_cond_signal(&capture_cond);
		    pthread_mutex_unlock(&capture_mutex);

		    system(cmd);
		}
	    }
	}
    }
 
out:
    pthread_join(udp_tid, NULL);
    pthread_join(display_tid, NULL);

    for (i = 0; i < IMG_BUF_COUNT; i++)
    {
        free(g_img_buf[i]);
    }

    return 0;
}
