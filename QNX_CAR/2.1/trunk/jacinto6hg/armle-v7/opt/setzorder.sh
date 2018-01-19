#!/bin/sh
APP=$1
ORDER=$2
if [ -z "$APP" ] || [ -z "$ORDER" ];then
    echo "usage: ./setzorder.sh app zorder"
    exit 1;
fi

pid="`pidin ar | grep $APP | grep -v grep | grep -v setzorder | awk '{print \$1}'`"
#echo $pid
if [ -z $pid ];then
    echo "no such process found, exit!"
    exit 1;
fi

window_number="`ls /dev/screen/$pid | grep win`"
#echo $window_number
for i in $window_number
do
    #echo "$i"
    if [ -n "$i" ];then
        screencmd setiv $i SCREEN_PROPERTY_ZORDER $ORDER &
    fi
done

exit 0;
