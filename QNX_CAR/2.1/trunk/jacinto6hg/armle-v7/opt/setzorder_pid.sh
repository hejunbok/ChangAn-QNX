#!/bin/sh
PID=$1
ORDER=$2
if [ -z "$PID" ] || [ -z "$ORDER" ];then
    echo "usage: ./setzorder.sh apppid zorder"
    exit 1;
fi

window_number="`ls /dev/screen/$PID | grep win`"
#echo $window_number
for i in $window_number
do
    #echo "$i"
    if [ -n "$i" ];then
        screencmd setiv $i SCREEN_PROPERTY_ZORDER $ORDER &
    fi
done

exit 0;
