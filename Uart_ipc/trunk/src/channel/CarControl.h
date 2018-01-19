#ifndef CARCONTROL_H
#define CARCONTROL_H

#include "IChannel.h"

class CarControl : public IChannel
{
public:
	CarControl(int channel);
	virtual ~CarControl();

	bool process(int pps_fd);

private:
        int mLights;
        int mAllWindows;
};

#endif
