#! /bin/sh
. ${HMIROOT}/scripts/debug.sh
. ${HMIROOT}/scripts/config/bluetooth

# Environment variables that are set:

#Bluetooth Configuration from env.sh
BT_HAS_BT="yes"
BT_SRCH_BT_DONGLE="yes" 
#BT_DRIVER=""
#BT_DRIVER_OPTS=""
#BT_IOACC_CFG_FILE=""
#BT_IOBT_CFG_FILE=""

#Board information (board name, tmp dir)
#BOARD="intel945"
#TMPDIR="/fs/tmpfs"

IO_AUCOUSTIC_PRIORTY=25

# TODO: We should be using the UUID of the system rather than relying on a MAC ADDR
if [ -r ${PPSDIR}/system/carID ]; then
	BT_QNXCAR_ID=`cat ${PPSDIR}/system/carID | awk '/id::/ { print $1 }' | sed 's/id:://' | sed 's/:/_/g'`
	BT_CAR_NAME=`print "QNX_CAR_${BT_QNXCAR_ID}"`
else
	BT_CAR_NAME=`print "QNX_CAR_NOcarID"`
fi


#Bluetooth dongle class
DONGLE_CLASS=0xe0

print_usage ()
{
	print "$0 <start|stop>"
}

start_bluetooth ()
{


	PATH=/qnx-car2/bin:/qnx-car2/usr/bin/:/qnx-car2/sbin:/qnx-car2/usr/sbin:/qnx-car2/usr/photon/bin:/qnx-car2/scripts:/proc/boot:/bin/:/usr/bin:/opt/bin:/btcommands/
	export PATH
	LD_LIBRARY_PATH=/qnx-car2/usr/lib:/qnx-car2/lib/dll:/qnx-car2/lib:/qnx-car2/lib/dll/font:/qnx-car2/usr/photon/lib:/qnx-car2/usr/lib/torch-webkit:/proc/boot/:/lib/:/usr/lib:/lib/dll/:/opt/lib
	export LD_LIBRARY_PATH

	if [ $BT_SRCH_BT_DONGLE == "yes" ]; then
		waitfor /dev/io-usb/io-usb
		COUNT=0;

		# Uggg, we have to spin and wait for the dongle, as io-bluetooth will just exit if
		# No dongle is present. PR#75486
		while [ $COUNT -lt 2 ]; do
			echo "	Waiting for BTDongle...."
			DONGLE_PRESENT=`usb | awk '/Class/{ print $3 }' | grep -i "$DONGLE_CLASS"`
			if [ ${DONGLE_PRESENT} ];then
				break;
			fi
			COUNT=$(($COUNT+1))
			sleep 1
		done;
	fi

	if [ $DONGLE_PRESENT ]; then 
		print "Starting bluetooth and acoustic services"
		io-bluetooth -d ${BT_CAR_NAME} -c ${HMIROOT}/etc/system/config/bluetooth.conf
		waitfor /dev/io-bluetooth/btmgr

		# This seems to have to be AFTER BT stack as the USB support is reseting the
		# device.  If we start audio before the BT stack, the ISOCK interface goes
		# to pot.
		#io-audio -d csr_bt_usb play_name=bt-play
		#on -p ${IO_AUCOUSTIC_PRIORTY} io-acoustic -c ${HMIROOT}/etc/acoustic/acoustic.conf &
		pps-bluetooth -vvvvvvvv &
		waitfor ${PPSDIR}/bluetooth
		#pps-phone &
		echo "Initializing radio..."                                 
                sleep 2
		echo "command::radio_init" > /pps/services/bluetooth/control
	fi
	return 0
}

stop_bluetooth ()
{
	print Stopping Bluetooth Services...
	slay -fQ io-bluetooth
	print Stopping Acoustic Processing Services...
	slay -fQ io-acoustic pps-bluetooth pps-phone
}

#Bail if we're not doing BT
if [ $BT_HAS_BT == "no" ]; then
	exit 0
fi

while getopts qn opt
do
	case $opt in
	q) QUIET="TRUE";;
	\?) print "BAD OPTION"; exit 1;;
	esac
done

#move to the end of the opts and decremetn $#
shift $(($OPTIND - 1))

if [ $1 = "start" ]; then
	start_bluetooth
	if [ $? != 0 ]; then
		print "Unable to start Bluetooth services"
	fi
elif [ $1 = "stop" ]; then
	stop_bluetooth
else
	print "Unknown option $1"
fi

