#ifndef CLUSTERCONTROL_H
#define CLUSTERCONTROL_H

#include "IChannel.h"

class ClusterControl : public IChannel
{
public:
	ClusterControl(int channel);
	virtual ~ClusterControl();

	bool screen_process(int pps_screen_fd);
	bool theme_process(int pps_theme_fd);
	bool car_process(int pps_car_fd);
	bool sensors_process(int pps_sensors_fd);
	bool music_state_process(int pps_music_state_fd);
	bool music_status_process(int pps_music_status_fd);
	bool music_track_process(int pps_music_track_fd);
	
};

#endif
