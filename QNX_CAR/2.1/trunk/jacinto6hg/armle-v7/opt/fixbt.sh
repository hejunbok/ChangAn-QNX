#!/bin/sh
stop_bt()
{
#	btaudio="`pidin ar | grep "mcasp=6" | head -1 | awk {'print $1'}`"
#	slay -fQ io-acoustic $btaudio io-bluetooth pps-bluetooth 
	slay -fQ io-bluetooth pps-bluetooth 
}

start_bt()
{
	io-bluetooth -vvvvvvvv -s /etc/system/config/bluetooth/TIInit_12.8.32.QNX.bts &
	waitfor /dev/io-bluetooth/btmgr

#	io-audio -d mcasp-j6 mcasp=6,clk_mode=slave,tx_serializer=1,rx_serializer=0,unit=1
#	waitfor /dev/snd/controlC1
#	io-acoustic -c /etc/acoustic/acoustic.conf
#	waitfor /dev/io-acoustic/hf
	
	pps-bluetooth -vvvvv &
}

#==============MAIN=================
FAILURE="BTMGR_EVENT_CONNECT_ALL_FAILURE BTMGR_EVENT_DISCONNECT_ALL_FAILURE BTMGR_EVENT_STACK_FAULT"
status="`cat /pps/services/bluetooth/status  | grep event | awk -F'::' {'print $2'}`"
echo $status
for i in $FAILURE;
do
	if [ $i == $status ];then
		echo "bt runs error, now fix it! @" `date` >> /tmp/fixbt.log
		stop_bt
		sleep 0.5
		start_bt
		exit 1;
	fi
done
echo "bt runs well, no need to fix it! @" `date`  >> /tmp/fixbt.log
exit 0;