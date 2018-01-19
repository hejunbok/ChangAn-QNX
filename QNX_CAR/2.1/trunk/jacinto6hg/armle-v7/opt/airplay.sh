

CMD=$1
case $CMD in
    "start")
        #slay 6
        #/root/i2c-omap35xx-omap4 -v -p 0x4807c000 -i 92 -c3 -s 0x11 --u4 &
        #ln -sP /dev/i2c4 /dev/i2c99
        display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=/base/usr/share/images/airplaywaiting.png -pipeline=1 &
        echo "carplay::start" >> /pps/hinge-tech/application_status
        sleep 1
        pid="`pidin ar | grep airplaywaiting.png | grep -v grep | awk '{print $1}'`"
        #echo $pid
        window_number="`ls /dev/screen/$pid | grep win`"
        #echo $window_number
        screencmd setiv $window_number SCREEN_PROPERTY_ZORDER 1 &
        airplaydemo --voffset 90 --wwidth 1200 --wheight 800 --width 1200 --height 800 --maxfps 60 --no-knob --logfile /dev/shmem/carplay.log &
        sleep 1
        pid="`pidin ar | grep airplaydemo | grep -v grep | awk '{print $1}'`"
        #echo $pid
        window_number="`ls /dev/screen/$pid | grep win`"
        #echo $window_number
        #screencmd setiv $window_number SCREEN_PROPERTY_POSITION 0,90 1200,1920 &
        screencmd setiv $window_number SCREEN_PROPERTY_ZORDER 2 &
        ;;
    "stop")
        echo "carplay::stop" >> /pps/hinge-tech/application_status
        pid="`pidin ar | grep airplaydemo | grep -v grep | awk '{print $1}'`"
        #echo $pid
        slay -f $pid
        pid="`pidin ar | grep airplaywaiting | grep -v grep | awk '{print $1}'`"
        #echo $pid
        slay -f $pid
        #window_number="`ls /dev/screen/$pid | grep win`"
        #echo $window_number
        #screencmd setiv $window_number SCREEN_PROPERTY_ZORDER -1 &
        ;;
    *)
        echo "please add start or stop to the command, exit!"
        ;;
esac

exit 0;


