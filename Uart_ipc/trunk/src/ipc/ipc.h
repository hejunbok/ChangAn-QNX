#ifndef IPC_H
#define IPC_H

#include <malloc.h>
#undef ipc_malloc
#undef ipc_free
#define ipc_malloc malloc
#define ipc_free free

int ipc_recv(unsigned char* data, int len);

int ipc_send(unsigned char* data, int len);

void toggleCar(int pps_fd, const char* component, int state);

/*
void toggleFL(int fl_state);

void toggleFR(int fl_state);

void toggleRL(int fl_state);

void toggleRR(int fl_state);
*/

#endif
