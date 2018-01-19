#!/bin/sh

USB_DEV="/dev/umass0t7"
USB="/usb"
EMMC="/dev/emmc0"
EMMC_BASE=$EMMC"t179"
EMMC_DOS=$EMMC"t12"
EMMC_ROOT=$EMMC"t178"
EMMC_EXTRA=$EMMC"t12.1"
CFG=$USB"/upd.cfg"
LOG="/tmp/upd.log"

#============================== FUNCTIONS ==============================#
CheckUSB()
{
	waitfor $USB_DEV 10
	if [ "$?" != 0 ];then
		echo "no usb detected, please replug the usb device and reboot!"
		exit 1;
	fi
	echo "usb device found"  | tee -a $LOG
	mount -t nt $USB_DEV $USB
}

UpgradeAll()
{
	UPD_IMG="`cat $CFG | grep UPD_IMG | awk -F= '{print $2}'`"
	if [ ! -e $USB/$UPD_IMG ];then
		echo "no update img found: "$UPD_IMG" ,exit!"  | tee -a $LOG
		display_image -zorder=5 -noinit=1 -display=2 -size=1200x1920 -file=$USB/error.png -pipeline=1 &
		exit 1;
	else
		echo "starting dd..."  | tee -a $LOG
		display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=$USB/flashing.png -pipeline=1 &
		dd if=$USB/$UPD_IMG of=$EMMC bs=1048576 | tee -a $LOG
		mount -e $EMMC
		echo "done!"  | tee -a $LOG
		mount -t dos $EMMC_DOS /dos
		waitfor /dos 5
		cp -f /dos/qnx-ifs.car2 /dos/qnx-ifs
		UPD_VER="`cat $CFG | grep VERSION`"
		echo $UPD_VER > /dos/VERSION
		touch /tmp/updall
	fi
	PrepNaviPart
	UpgradeNavi
}

UpgradeEMMC()
{
	UPD_IMG="`cat $CFG | grep UPD_IMG | awk -F= '{print $2}'`"
	if [ ! -e $USB/$UPD_IMG ];then
		echo "no emmc img found: "$UPD_IMG" ,exit!"  | tee -a $LOG
		display_image -zorder=5 -noinit=1 -display=2 -size=1200x1920 -file=$USB/error.png -pipeline=1 &
		exit 1;
	else
		echo "start dd flashing..."  | tee -a $LOG
		display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=$USB/flashing.png -pipeline=1 &
		dd if=$EMMC of=/tmp/mbr count=1 bs=512 | tee -a $LOG
		dd if=$USB/$UPD_IMG of=$EMMC bs=1048576 | tee -a $LOG
		dd if=/tmp/mbr of=$EMMC | tee -a $LOG
		mount -e $EMMC
		echo "flash done!"  | tee -a $LOG
		mount -t dos $EMMC_DOS /dos
		waitfor /dos 5
		cp -f /dos/qnx-ifs.car2 /dos/qnx-ifs
	fi
	UPD_VER="`cat $CFG | grep VERSION`"
	echo $UPD_VER > /dos/VERSION
}

PrepNaviPart()
{
	echo "make new vfat partition for NAVI"  | tee -a $LOG
	#fdisk $EMMC add -t 12 -s 4
	fdisk $EMMC add -t 12 -s 4 -c 3724,14800
	mount -e $EMMC
	waitfor $EMMC_EXTRA 10
	mkdosfs $EMMC_EXTRA
	echo "done"  | tee -a $LOG
}

UpgradeNavi()
{
	UPD_NAV="`cat $CFG | grep UPD_NAV | awk -F= '{print $2}'`"
	UPD_VIDEO="`cat $CFG | grep UPD_VIDEO | awk -F= '{print $2}'`"
	if [ ! -e $USB/$UPD_NAV ];then
		echo "no navi pack found: "$UPD_NAV" ,exit!"  | tee -a $LOG
		slay -f display_image
		display_image -zorder=5 -noinit=1 -display=2 -size=1200x1920 -file=$USB/error.png -pipeline=1 &
		exit 1;
	else
		slay -f display_image
		display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=$USB/navi.png -pipeline=1 &
		mount -t dos $EMMC_EXTRA /extra
		waitfor /extra 5
		echo "start navi updating..."  | tee -a $LOG
		tar xvf $USB/$UPD_NAV -C /extra/
		if [ "$?" == 0 ];then
			echo "done!"  | tee -a $LOG
		else
			echo "error!"  | tee -a $LOG
		fi
		if [ -e $USB/$UPD_VIDEO ];then
			slay -f display_image
			display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=$USB/video.png -pipeline=1 &
			echo "start video files updating..."  | tee -a $LOG
			tar xvf $USB/$UPD_VIDEO -C /extra/
			echo "done!"  | tee -a $LOG
		fi
		if [ ! -e /tmp/updall ];then
			mount -t dos $EMMC_DOS /dos
			waitfor /dos 5
			cp -f /dos/qnx-ifs.old /dos/qnx-ifs
		fi
	fi
}

UpgradeRoot()
{
	UPD_ROOT="`cat $CFG | grep UPD_ROOT | awk -F= '{print $2}'`"
	if [ ! -e $USB/$UPD_ROOT ];then
		echo "no root pack found: "$UPD_ROOT" ,exit!"  | tee -a $LOG
		display_image -zorder=5 -noinit=1 -display=2 -size=1200x1920 -file=$USB/error.png -pipeline=1 &
		exit 1;
	else
		mount $EMMC_ROOT /
		mount $EMMC_BASE /base
		echo "start root updating..."  | tee -a $LOG
		tar xvf $USB/$UPD_ROOT /
		echo "done!"  | tee -a $LOG
	fi
	mount -t dos $EMMC_DOS /dos
	waitfor /dos 5
	cp -f /dos/qnx-ifs.old /dos/qnx-ifs
}

#============================== MAIN ==============================#
screen &
waitfor /dev/screen 10
UPD_MODE="`cat $CFG | grep UPD_MODE | awk -F= '{print $2}'`"
case "$UPD_MODE" in
	"1")
		echo "upgrading all..."  | tee -a $LOG
		UpgradeAll
		;;
	"2")
		echo "upgrading emmc only..."  | tee -a $LOG
		UpgradeEMMC
		;;
	"3")
		echo "upgrading navi only..."  | tee -a $LOG
		UpgradeNavi
		;;
	"4")
		echo "upgrading root..."  | tee -a $LOG
		UpgradeRoot
		;;
	*)
		echo "wrong upd mode in cfg file, exit!"  | tee -a $LOG
		exit 1;
		;;
esac

sync
echo "upgrading done!"
slay -f display_image
display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=$USB/done.png -pipeline=1 &
#shutdown

exit 0;
