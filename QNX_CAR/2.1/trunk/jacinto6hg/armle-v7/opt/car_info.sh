
CMD="$1"

if [ $CMD == "start" ];then
    echo "app start"
    /bin/car_info_demo &
else
    slay -f car_info_demo &
    echo rpm:n:0 >> /pps/qnxcar/sensors
    echo speed:n:0 >> /pps/qnxcar/sensors
    echo laneDeparture:n:0 >> /pps/qnxcar/sensors
fi

exit 0;
