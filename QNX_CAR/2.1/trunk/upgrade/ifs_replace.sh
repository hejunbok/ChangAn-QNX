#!/bin/bash

IMG_PATH="/opt/qnx660/car2img"
DOS_TAR="jacinto6hg-dos-sd-emmc.tar"
IFS_UPD="qnx-ifs.upd"

if [ ! -e $IMG_PATH/$DOS_TAR ];then
	echo "no jacinto6hg-dos-sd-emmc.tar found,exit!"
	exit 1;
fi

cd $IMG_PATH
mkdir -p temp
tar xvf $DOS_TAR -C temp
mv ./temp/dos/qnx-ifs ./temp/dos/qnx-ifs.car2
cp -f $IFS_UPD ./temp/dos/qnx-ifs 
cd $IMG_PATH/temp
tar cvf ../$DOS_TAR dos
cd $IMG_PATH
rm -rf temp

exit 0;
