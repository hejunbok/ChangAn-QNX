#include "ipc_utils.h"
#include "crc8.h"

#define IPC_START 0xE6
#define BUFFER_LEN 0x20

enum decode_step
{
	STEP_START = 0,
	STEP_LENGTH,
	STEP_DATA,
	STEP_CRC
};

struct ipc_data* decode(unsigned char data)
{
	/********protocol**********
	* IPC_START     1 byte
	* len           1 byte
	* if len <= 8
	*    data_1     1~8 bytes
	*    crc_1      1 byte
	* else 
	*    data_1     8 byte
	*    crc_1      1 byte
	*    ...
	*    data_n     1~8 bytes
	*    crc_n      1 byte
	* endif
	************************/

	static struct ipc_data decode_data = {0, 0};
	static int decode_data_len = 0;
	static unsigned char* de_buf = 0;
	static int de_buf_len = 0;

	static int step = STEP_START;

	static int index = 0;
	static int len = 0;
	static int crc_index = 0;
	static int data_index = 0;

	int i = 0;
	unsigned char* temp_buf = 0;

	if (de_buf_len <= len)
	{
		temp_buf = de_buf;
		de_buf = (unsigned char*)ipc_malloc(de_buf_len + BUFFER_LEN);

		for (i = 0; i < de_buf_len; i++)
		{
			de_buf[i] = temp_buf[i];
		}

		de_buf_len += BUFFER_LEN;
		ipc_free(temp_buf);
	}

	de_buf[len++] = data;

	while (index < len)
	{
		switch (step)
		{
			case STEP_START:
				if (de_buf[index] == IPC_START)
				{
					step = STEP_LENGTH;
					index++;
				}
				else
				{
					for (i = 0; i < len - 1; i++)
					{
						de_buf[i] = de_buf[i + 1];
					}

					index = 0;
					len -= 1;
				}
		    		break;

			case STEP_LENGTH:
				decode_data.len = de_buf[index++];
				crc_index = 0;
				data_index = 0;

				if (decode_data_len <= decode_data.len)
				{
					if (decode_data.data)
					{
						ipc_free(decode_data.data);
					}

					decode_data_len = decode_data.len + 1;
					decode_data.data = (unsigned char*)ipc_malloc(decode_data_len);
				}

				step = STEP_DATA;
				break;

			case STEP_DATA:
				decode_data.data[data_index++] = de_buf[index++];

				if ((data_index - 1) % 8 == 7 || data_index == decode_data.len)
				{
					step = STEP_CRC;
				}

				break;

			case STEP_CRC:
				if (de_buf[index] == crc8(decode_data.data + crc_index * 8,
					decode_data.len - 8 * crc_index >= 8 ? 8 : decode_data.len - 8 * crc_index))
				{
					if (data_index == decode_data.len)
					{
						decode_data.data[data_index] = '\0';
						index = 0;
						len = 0;
						step = STEP_START;
						
						return &decode_data;
					}
					else
					{
						crc_index += 1;
						index++;
						step = STEP_DATA;
					}
				}
				else
				{
					for(i = 0; i < len - 1; i++)
					{
						de_buf[i] = de_buf[i + 1];
					}

					index = 0;
					len -= 1;
					step = STEP_START;
				}

				break;

			default:
				break;
		}
	}

	return 0;
}

struct ipc_data* encode(const void* raw, unsigned char len)
{
	const char* data = (const char*)raw;
	static struct ipc_data encode_data = {0, 0};
	static int en_buf_len = 0;
	int i = 0;
	int crc_num = len / 8 + (len % 8 > 0 ? 1 : 0);
	int crc_index = 0;
	int all_len = 2 + len + crc_num; // all len of START, len, data and CRCs

	if (!data || len == 0 || len > 0xE2)
	{
		return 0;
	}

	if (en_buf_len < all_len)
	{
		if (encode_data.data)
		{
			ipc_free(encode_data.data);
		}

		encode_data.data = (unsigned char*)ipc_malloc(all_len);
		en_buf_len = all_len;
	}

	encode_data.data[0] = IPC_START; //START
	encode_data.data[1] = len; //len of the data

	for (i = 0; i < len; i++)
	{
		encode_data.data[2 + crc_index + i] = data[i]; //data

		if (i % 8 == 7)
		{
			encode_data.data[2 + crc_index + i + 1] = crc8(data + 8 * crc_index, 8); //CRC
			crc_index++;
		}
	}

	if (crc_index < crc_num) //last CRC
	{
		encode_data.data[2 + crc_index + i] = crc8(data + 8 * crc_index, len - 8 * crc_index); //CRC
	}

	encode_data.len = all_len;

	return &encode_data;
}
