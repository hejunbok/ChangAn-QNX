#ifndef LAMPCONTROL_H
#define LAMPCONTROL_H

#include "IChannel.h"

class LampControl : public IChannel
{
public:
	LampControl(int channel);
	virtual ~LampControl();

	bool process(int pps_fd);
};

#endif
