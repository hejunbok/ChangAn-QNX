#include "ipc.h"
#include "uart/uart.h"
#include "ipc_utils.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <hnm/pps.h>

int fl_state=0,fr_state=0,rl_state=0,rr_state=0;

#define PPS_CAR_FILE "/pps/ui"

int ipc_recv(unsigned char* data, int len)
{
	return 0;
}

int ipc_send(unsigned char* data, int len)
{
	struct ipc_data* en_data = encode(data, len);
	return uart_send(en_data->data, en_data->len);
}


void toggleCar(int pps_fd, const char* component, int state)
{
	int tfd, cnt, length;
	char buf[32];

	//    std::cout<<door<<" set to "<<state<<std::endl;
	sprintf ( buf, "%s::%d\n", component, state );
	length = strlen ( buf );
/*
	if ( ( tfd = open ( PPS_CAR_FILE, O_WRONLY ) ) <= 0 )
		if ( ( tfd = open ( PPS_CAR_FILE, O_WRONLY | O_CREAT, 0666 ) ) <= 0 )
			return;
*/
	cnt = 0;
	// do {
	if ( write ( pps_fd, buf, length ) == length )
		printf("wirte ok\n\n");
	//   sleep ( 1 );
	// } while ( cnt++ < 3 );

	//close ( tfd );

}

/*
void toggleFL(int fl_state)
{
	std::cout<<"toggleFL called, current "<<fl_state<<std::endl;
	//(fl_state==0) ? (fl_state=1) : (fl_state=0);
	toggleCar("fl", fl_state);
}

void toggleFR(int fr_state)
{
	std::cout<<"toggleFR called"<<std::endl;
	//(fr_state==0) ? (fr_state=1) : (fr_state=0);
	toggleCar("fr", fr_state);
}
void toggleRL(int rl_state)
{
	std::cout<<"toggleRL called"<<std::endl;
	//(rl_state==0) ? (rl_state=1) : (rl_state=0);
	toggleCar("rl", rl_state);
}
void toggleRR(int rr_state)
{
	std::cout<<"toggleRR called"<<std::endl;
	//(rr_state==0) ? (rr_state=1) : (rr_state=0);
	toggleCar("rr", rr_state);
}
*/
