
if [ -e /RUNONCE ];then
    mount -uw /base
    mount -uw /
    echo "change qtcar" >/tmp/runonce.log
    slay qnxcar2
    slay Navi
    mv /qtcar /qtcar.old
    tar xzvf /qtcar.tar.gz -C /
    echo "deploy kanzi" >>/tmp/runonce.log
    tar xzvf /kanzi.tar.gz -C /
    echo "deploy srservice" >>/tmp/runonce.log
    tar xzvf /srservice.tar.gz -C /
    echo "uninstall default apk_bar" >>/tmp/runonce.log
    tar xzvf /apk_bar.tar.gz -C /
    bar-uninstall tunneltilt
    bar-uninstall PeaksAndValleys
    bar-uninstall QnxAppPortal
    bar-uninstall rearview_camera
    bar-uninstall LocalSearch
    bar-uninstall sys.browser
    bar-uninstall Shutdown
    bar-uninstall Settings
    echo "install new apk_bar" >>/tmp/runonce.log
    bar-install /apk_bar/MySettingsapp.bar
    #for i in `ls /apk_bar`;do bar-install /apk_bar/$i; sleep 1; done
    echo "link videos" >>/tmp/runonce.log
    cd /accounts/1000/shared/videos/
    for i in `ls /extra/videos/*.mp4`;do ln -s $i ./; done
    chmod 777 /var/pps/services/apkruntime/nav_control
    echo "cleanup" >>/tmp/runonce.log
    rm -rf /RUNONCE
    rm -rf /*.tar.gz
    #rm -rf /apk_bar
    sleep 1
    sync
    shutdown
fi

exit 0;


