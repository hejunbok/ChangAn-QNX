
CMD=$1
case $CMD in
  "start")
    if [ -z "`pidin ar | grep mlink-rtp | grep -v grep`" ];then
      echo "run mlink-rtp"
      export SOCK=/mirrorlink_sandbox
      export LD_LIBRARY_PATH=/usr/lib/realvnc

      mlink-rtp -S &
		sleep 3;
    fi

    APP="ml0app.shortcut100"
    echo "msg::start\ndat::$APP,ORIENTATION=0,WIDTH=800,HEIGHT=1200,CAR_TABBAR_HEIGHT=50\nid::1\nmsg::start\n" >> /pps/services/launcher/control?nopersist

    COUNT=1
    while [ $COUNT -lt 10 ];do
      pid="`pidin ar | grep $APP | grep -v grep | grep -v runml | awk '{print \$1}'`"
      if [ -n "$pid" ];then
        echo $pid
        break;
      fi
      echo $COUNT
      let COUNT+=1;
      sleep 1
    done

    if [ $COUNT == 10 ];then
      echo "timeout,exit!"
      slay -f mlink-rtp
      exit 1;
    fi

    waitfor /dev/screen/$pid 2

    while [ 1 ];do
      node="`ls /dev/screen/$pid | grep win | wc -l`"
      if [ $node == 2 ];then
          echo "got 2!"
          break;
      fi
    done

    window_number="`ls /dev/screen/$pid | grep win `"
    echo $window_number
    for i in $window_number
    do
        echo "$i"
        if [ -n "$i" ];then
        screencmd setiv $i SCREEN_PROPERTY_POSITION 0,88 800,1200 &
        echo "mlink::start" >> /pps/hinge-tech/application_status
        fi
    done
    ;;
  "stop")
    echo "mlink::stop" >> /pps/hinge-tech/application_status
    slay -f mlink-rtp
    ;;
  "*")
    echo "please add start or stop to the command, exit!"
    ;;
esac

exit 0;
