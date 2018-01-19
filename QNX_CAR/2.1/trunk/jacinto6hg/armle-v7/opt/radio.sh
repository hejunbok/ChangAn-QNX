
CMD=$1

if [ $CMD == "start" ];then
    pid="`pidin ar | grep radioplayer | grep -v grep | awk '{print $1}'`"
    if [ -n "$pid" ]; then
        echo "already run!"
        exit 0;
    else
        select_input -r &
    fi
else
    select_input -i &
fi

exit 0;


