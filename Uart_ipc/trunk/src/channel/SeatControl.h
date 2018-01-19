#ifndef SEATCONTROL_H
#define SEATCONTROL_H

#include "IChannel.h"

class SeatControl : public IChannel
{
public:
	SeatControl(int channel);
	virtual ~SeatControl();

	bool process(int pps_fd);
};

#endif
