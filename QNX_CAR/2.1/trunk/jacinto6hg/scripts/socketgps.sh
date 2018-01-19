#!/bin/ksh

location_pps_defaults &

if [ ! -f  /pps/services/geolocation/settings ]
then
	mkdir -p /pps/services/geolocation/settings
fi
if [ ! -f /pps/system/navigator/proc ]
then
	mkdir -p /pps/system/navigator/proc
fi

rm -f /pps/services/geolocation/control
location_manager &
io-locp -p /var/etc/prov_skel.xml &
socket_server_test &
echo "done!"
exit 0;
