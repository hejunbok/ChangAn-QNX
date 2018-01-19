#ifndef AIRCONTROL_H
#define AIRCONTROL_H

#include "IChannel.h"

class AirControl : public IChannel
{
public:
	AirControl(int channel);
	virtual ~AirControl();

	bool process(int pps_fd);
};

#endif
