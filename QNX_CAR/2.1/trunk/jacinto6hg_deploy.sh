#!/bin/bash

MXNAVI="MXNavi"
VIDEOFILES="videofiles.tar.gz"
#TF="/dev/sdb"
#TF1="$TF"1
#TF4="$TF"4

new_partition()
{
	umount $TF1 2>/dev/null
	umount $TF4 2>/dev/null
	fdisk $TF<<EOF
n
p             

              
p
t
4
c
p
w
EOF
	echo "done"
	echo "please re-plugin the sd card before next step!"
}

make_vfat()
{
	umount $TF1 2>/dev/null
	umount $TF4 2>/dev/null
	mkfs.vfat $TF4
	echo "done"
	echo "please re-plugin the sd card before next step!"
}

deploy_mxnavi()
{
	path="`mount | grep $TF4 | awk '{print $3}'`"
	cp -rf $MXNAVI $path
	echo "done"
}

deploy_videofiles()
{
	path="`mount | grep $TF4 | awk '{print $3}'`"
	tar xzvf $VIDEOFILES -C $path
	echo "done"
}



########################### MAIN #############################
mount
echo ""
echo "Enter your device path: "
read DEV
TF=$DEV
TF1="$TF"1
TF4="$TF"4

while [ 1 ]
do
	echo "========================="
	echo "    EXTRA Deploy Tool   "
	echo "========================="
	echo " 1. create new partition"
	echo " 2. make vfat partition"
	echo " 3. deploy mxnavi to vfat partition"	
	echo " 4. deploy video files to vfat partition"	
	echo " 5. quit"
	echo "========================="
	echo "Enter your selection: "

	read Selection

	case $Selection in
	"1")
		echo "creating new partition"
		new_partition
		;;
	"2")
		echo "making vfat partition"
		make_vfat
		;;
	"3")
		echo "deploying mxnavi to vfat partition"
		deploy_mxnavi
		;;
	"4")
		echo "deploying video files to vfat partition"
		deploy_videofiles
		;;
	*)
		exit 0;
		;;
	esac
done







