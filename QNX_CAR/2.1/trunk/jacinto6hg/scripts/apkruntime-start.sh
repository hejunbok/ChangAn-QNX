#!/bin/sh
##############################################################
# APK Runtime-start
# This script is used to start the CAR2 APK Runtime Player
# Enable APK Runtime Player in /var/etc/services-enabled
#
# APK Runtime applications are be exposed or hidden when the runtime
# bar is first started (based on the presence of a sandbox in /accounts/1000)
# bar-install adds the required navigator line as an element in the jobs.*
# PPS object in installer/upd/current so that this script does not need to
# be aware of bar-internals and parsing
#
# This script is intended to run once/boot only, at startup
##############################################################
# Hard coded app ID is deeply embedded in APK Runtime player code

AP_APP=sys.apkruntime.gYABgKAOw1czN6neiAT72SGO.ns

PPS_NAV=/pps/system/navigator/applications/applications
JOBS=/pps/system/installer/upd/current

function reinstall_apps
{
   # due to 'sticky bit' in pps dir sed cannot make a temp file there, so use a tmp file
   TMPFILE=/tmp/apkruntimestart.tmp

   for f in `ls $JOBS`
   do
      chmod 666 $JOBS/$f
      sed s/apk\:\:done/apk\:\:go/ $JOBS/$f > $TMPFILE
      cp $TMPFILE $JOBS/$f
   done
   rm $TMPFILE
}

set_apps_visible ()
{

   if [ $1 == 0 ]; then
      prefix="-"
      # just need the first entry for delete
   else
      prefix=""
   fi
   for f in `ls $JOBS`
   do
      if [ $1 == 0 ]; then
         # for delete need only the first element
         PPSLINE=`cat $JOBS/$f | grep ppsline | sed s/ppsline::// | sed s/::.*//`
      else
         PPSLINE=`cat $JOBS/$f | grep ppsline | sed s/ppsline:://`
      fi
      echo "Issue $prefix$PPSLINE"
      echo $prefix$PPSLINE >> $PPS_NAV
      echo $prefix$PPSLINE >> /tmp/debug
   done
}


#Only proceed if APKRUNTIME is enabled in /var/etc/services-enabled
if cat /var/etc/services-enabled | grep -q "APKRUNTIME:true"; then

    #
    # If starting player for first time sandbox will not exist
    # 1) Rename built in apps
    # 2) Force all APK Runtime applications previously installed to be re-installed.
    #
    if [ ! -d /accounts/1000/appdata/$AP_APP ]; then
        echo "[APKRUNTIME] first run."
        # this PPS object is required for APK Runtime app uninstall
        reinstall_apps
        if [ ! -d /apps/com.android.calculator2.testEKifwHYW04TEfWw0WnN5IXc ]; then
            bar-install /appinstall/bars/unsigned/apkruntime/apps/CalculatorJB.bar
            bar-install /appinstall/bars/unsigned/apkruntime/apps/ApiDemosJB.bar
        fi
    fi

    # Removal of apps from navigator results in them being uninstalled
    # if the app directory is empty then must be going from AP disabled->enabled.
    if [ -z `ls /accounts/1000/appdata/$AP_APP/data/app` ]; then
       set_apps_visible 1
       reinstall_apps
    fi

    #################################################
    # Create APS partitions (if they do not exit)
    #################################################
    if aps show | grep -q "foreground"; then
       echo "APS partitions already exist"
    else
       echo "Creating APS partitions foreground/background/stop"
       aps create -b 50 foreground
       aps create -b 0  background
       aps create -b 0 stop
    fi
    #################################################
    # Start APK Runtime Player via launcher
    # If APK Runtime player is already running this will force a kill/restart inside the APK runtime Launcher
    #################################################
    if [ -e /pps/services/launcher/control ]; then
       #echo "msg::start\ndat::$AP_APP,ORIENTATION=0,WIDTH=1280,HEIGHT=584\nid:n:3080\nmsg::start\n" >> /pps/services/launcher/control?nopersist
       echo "msg::start\ndat::$AP_APP,ORIENTATION=0,WIDTH=800,HEIGHT=1200\nid:n:3080\nmsg::start\n" >> /pps/services/launcher/control?nopersist
    fi
    /opt/setapkwin.sh &
else
    set_apps_visible 0
    # app removal does not happen until AP next starts and sees deletion in PPS. Need to force removal
    rm /accounts/1000/appdata/$AP_APP/data/app/*.apk
    echo "[APKRUNTIME] INFO: APK Runtime Player not enabled in /var/etc/services-enabled. Apk apps hidden." >&2
fi
