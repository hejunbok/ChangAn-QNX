#ifndef GESTURECONTROL_H
#define GESTURECONTROL_H

#include "IChannel.h"

class GestureControl : public IChannel
{
public:
	GestureControl(int channel);
	virtual ~GestureControl();

	bool process(int pps_fd);
};

#endif
