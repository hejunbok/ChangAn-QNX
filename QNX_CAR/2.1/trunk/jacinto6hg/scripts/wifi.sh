#!/bin/ksh

FIRMWARE_PATH=/tmp/

start-wifi ()
{

	if [ -e "/dev/tiw_sta_ctrl" ]; then
		echo "Wifi already running.." >&2
		exit 1
	fi

	for file in roleSta.bin roleAp.bin roleP2pCl.bin tiwlan.bin; do
    		if [ ! -e "$FIRMWARE_PATH/$file" ]; then
        		tiwlan_cfg_ti18xx -c /etc/ -t $FIRMWARE_PATH
			break
    		fi
	done

	if [ ! -e "$FIRMWARE_PATH/firmware_ti18xx.bin" ]; then
		ln -Ps /lib/firmware/ti18xx/firmware_ti18xx.bin $FIRMWARE_PATH/firmware_ti18xx.bin
	fi
	for file in tiwlan.bin firmware_ti18xx.bin; do
    		if [ ! -e "$FIRMWARE_PATH$file" ]; then
        		echo "Wifi not started because of missing file: [$FIRMWARE_PATH$file]" >&2
        		exit 1
    		fi
	done

        mount -Tio-pkt -o firmdir=$FIRMWARE_PATH devnp-ti18xx_dra74x.so 
        waitfor /dev/tiw_sta_ctrl;
        sleep 1;
        ifconfig tiw_drv0 up;
        ifconfig tiw_sta0 up;
        wpa_supplicant_ti18xx  -Dwilink -itiw_sta0 -C /var/run/wpa_supplicant -y $FIRMWARE_PATH &
        waitfor /var/run/wpa_supplicant/tiw_sta0;
        wpa_pps -c /var/etc/wpa_pps.conf -j tiw_sap0 -h /usr/sbin/hostapd_ti18xx -y $FIRMWARE_PATH -i tiw_sta0 &
	tetherman &
	# A temp workaround. Valid fix to come.
        echo default_interface4::en0 >> /pps/services/networking/status_public 
    return 0
}


    start-wifi || exit $?
exit 0

