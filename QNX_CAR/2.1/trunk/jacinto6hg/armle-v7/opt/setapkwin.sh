while [ 1 ];do
  pid="`pidin ar | grep system_server | grep -v grep | awk '{print $1}'`"
  #echo $pid
  if [ -n "$pid" ];then
    break;
  fi
  sleep 1
done

window_number="`ls /dev/screen/$pid | grep win`"
#echo $window_number
screencmd setiv $window_number SCREEN_PROPERTY_POSITION 0,87 &
screencmd setiv $window_number SCREEN_PROPERTY_SIZE 1200,786 &

exit 0;

