#ifndef IPC_UTILS_H
#define IPC_UTILS_H

#include "ipc.h"

struct ipc_data
{
	unsigned short len;
	unsigned char* data;
};

/***
 * return the pointer of decoded data if get a complete message, else return 0
 */
struct ipc_data* decode(unsigned char data);

/***
 *
 * len -- max is E2 = 226
 *
 * return the pointer of the encoded data if successful, else return 0
 */
struct ipc_data* encode(const void* data, unsigned char len);

#endif
