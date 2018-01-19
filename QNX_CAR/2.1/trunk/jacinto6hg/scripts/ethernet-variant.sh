#!/bin/sh

macConfig=/var/etc/system/config/random-mac.conf;
mac=;

#mount -Tio-pkt devnp-dm814x-j6.so
#mount -Tio-pkt -o name=dm devnp-asix.so
spi-master -u 1 -d omap4430 base=0x48098100,bitrate=125000,irq=97,sdma=1,channel=1 &
sleep 2
polorinit
mount -Tio-pkt -o verbose=7,p0emu_phy devnp-89501-j6.so
sleep 2

if_up -r60 -p dm0;
if [ $? == 0 ]; then
    echo "Network interface detected.";
    if [ ! -e $macConfig ]; then
        mac="` ifconfig dm0 | grep address | sed s/://g | cut -c10-21 `";
        echo $mac > $macConfig;
    fi
else
    echo "Network interface failed to come up"; 
fi
