#!/bin/sh

USB_DEV="/dev/umass0t7"
USB="/fs/usb0"
CFG="/fs/usb0/upd.cfg"
UPD_IFS="/fs/usb0/qnx-ifs.upd"
LOG="/root/upd.log"

while true;do
	waitfor $USB_DEV 60
	if [ "$?" == 0 ];then
		echo "usb device found"  | tee -a $LOG
		waitfor $USB 10
		if [ -e $CFG ]&&[ -e $UPD_IFS ];then
			echo "cfg detected, start to update ifs!"  | tee -a $LOG
			mv /dos/qnx-ifs /dos/qnx-ifs.old
			cp -f $UPD_IFS /dos/qnx-ifs
			sync
			break;
		else
			echo "no cfg or ifs found, abort!"  | tee -a $LOG
			sleep 60
			continue;
		fi
	fi
done

echo "reboot for upgrade!"  | tee -a $LOG
shutdown

exit 0;
