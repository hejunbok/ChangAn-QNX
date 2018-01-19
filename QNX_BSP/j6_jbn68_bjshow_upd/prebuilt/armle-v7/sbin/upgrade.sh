#!/bin/sh

USB="/fs/usb0"
EMMC="/dev/emmc"
EMMC_BASE=$EMMC"t179"
EMMC_DOS=$EMMC"t12"
EMMC_ROOT=$EMMC"t178"
EMMC_EXTRA=$EMMC"t12.1"
CFG=$USB"/upd.cfg"
LOG=/tmp/upd.log"

#============================== FUNCTIONS ==============================#
CheckUSB()
{
	waitfor $USB 10
	if [ "$?" != 0 ];then
		echo "no usb detected, please replug the usb device and reboot!"
		exit 1;
	fi
	echo "usb device found" >&1 | tee -a $LOG
}

PrepNaviPart()
{
	echo "make new vfat partition for NAVI" >&1 | tee -a $LOG
	fdisk $EMMC<<EOF
n
p


p
t
4
c
p
w
EOF
	mkfs.vfat $EMMC_EXTRA
	echo "done"
}

UpgradeAll()
{
	UPD_IMG="`cat $CFG | grep UPD_IMG | awk -F= '{print $2}'`"
	if [ ! -e $USB/$UPD_IMG ];then
		echo "no update img found: "$UPD_IMG" ,exit!" >&1 | tee -a $LOG
		exit 1;
	else
		echo "starting dd..." >&1 | tee -a $LOG
		dd if=$USB/$UPD_IMG of=/dev bs=1M >&1 | tee -a $LOG
		echo "done!" >&1 | tee -a $LOG
		mount -t vfat $EMMC_DOS /dos
		#cp -f /dos/qnx-ifs /
	fi
	PrepNaviPart
	UpgradeNavi
	UPD_VER="`cat $CFG | grep VERSION`"
	echo $UPD_VER > /dos/VERSION
}

UpgradeNavi()
{
	UPD_NAV="`cat $CFG | grep UPD_NAV | awk -F= '{print $2}'`"
	if [ ! -d $USB/$UPD_NAV ];then
		echo "no navi pack found: "$UPD_NAV" ,exit!" >&1 | tee -a $LOG
		exit 1;
	else
		mount -t vfat $EMMC_EXTRA /extra
		cp -rf $USB/$UPD_NAV /extra
	fi
}

UpgradeRoot()
{
	UPD_ROOT="`cat $CFG | grep UPD_ROOT | awk -F= '{print $2}'`"
	if [ ! -e $USB/$UPD_ROOT ];then
		echo "no root pack found: "$UPD_ROOT" ,exit!" >&1 | tee -a $LOG
		exit 1;
	else
		mount $EMMC_ROOT /
		mount $EMMC_BASE /base
		tar xzvf $USB/$UPD_ROOT /
	fi
}

#============================== MAIN ==============================#
CheckUSB
UPD_MODE="`cat $CFG | grep UPD_MODE | awk -F= '{print $2}'`"
case $UPD_MODE in:
	"1")
		echo "upgrading all..." >&1 | tee -a $LOG
		UpgradeAll
		;;
	"2")
		echo "upgrading navi..." >&1 | tee -a $LOG
		UpgradeNavi
		;;
	"3")
		echo "upgrading root..." >&1 | tee -a $LOG
		UpgradeRoot
		;;
	*)
		echo "wrong upd mode in cfg file, exit!" >&1 | tee -a $LOG
		exit 1;
		;;
esac

echo "done!"
sync
shutdown

exit 0;