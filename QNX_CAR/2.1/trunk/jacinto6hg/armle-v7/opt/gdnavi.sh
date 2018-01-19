mount -t dos -o exe=all /dev/cardisk0t12.1 /extra
sleep 1

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/extra/autonavi/lib/
export GAPP_PATH=/extra/autonavi/bin/
export GLIB_PATH=/extra/autonavi/lib/
export GMAP_PATH=/extra/autonavi/mapdata/
export GRES_PATH=/extra/autonavi/resource/
export GAPP_DATA_PATH=/extra/autonavi/appdata/
export GMAP_CONF_PATH=/extra/autonavi/mapconf/
export GDATA_PATH=/extra/autonavi/

cd /extra/autonavi/bin
./Navi &
sleep 5

/opt/setzorder.sh Navi -1
#/opt/setzorder.sh display_image 0


exit 0
