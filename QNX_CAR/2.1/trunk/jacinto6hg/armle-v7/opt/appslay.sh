pid="`pidin ar | grep com. | grep -v android |grep -v grep | awk '{print $1}'`"
if [ -z "$pid" ];then
    exit 1;
fi
for app in "$pid"; do
    #echo "$app"
    kill -9 $app
done
