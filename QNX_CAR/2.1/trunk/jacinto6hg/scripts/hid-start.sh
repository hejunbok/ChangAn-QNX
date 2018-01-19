#!/bin/sh
start-hid () 
{ 
    local screenOpts h w;
    screenOpts=$(grep options /base/etc/system/config/graphics.conf)
    h=$(print $screenOpts | sed -n -r s/.*height=\([0-9]*\).*/\\1/p)
    w=$(print $screenOpts | sed -n -r s/.*width=\([0-9]*\).*/\\1/p)
 
    print "Starting devi-hid with width: $w and height: $h";

    #For rev G+ J6 boards, with 10.1" display attached:
    #devi-lg-tsc-101 -P -r -R$w,$h touch -r -X;
    devi-hid -P -r -R$w,$h kbd mouse touch;

}

#This script is intended to run once/boot only, at startup
running=/dev/shmem/hid-start.run
[ -e $running ] && exit

#start-hid;

touch $running
unset running

