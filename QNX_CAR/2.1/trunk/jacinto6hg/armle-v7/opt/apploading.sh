#!/bin/sh

CMD="$1"

if [ $CMD == "start" ];then
    #echo "app start"
    display_image -zorder=2 -noinit=1 -display=2 -size=1200x1920 -file=/base/usr/share/images/loading.png -pipeline=1 &
    echo "app::start" >> /pps/hinge-tech/application_status
    /opt/setappzorder.sh &
else
    echo "app::stop" >> /pps/hinge-tech/application_status
    pid=`pidin ar | grep loading | grep -v grep | awk '{if($2=="display_image"){print $1}}'`
    if [ -n "$pid" ]; then
        slay -f $pid &
        #echo "app stop"
        echo "msg::stop\ndat::\nid::1" > /pps/services/launcher/control
        /opt/appslay.sh &
    fi
fi

exit 0;
