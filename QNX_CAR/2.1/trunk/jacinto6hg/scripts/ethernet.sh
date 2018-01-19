#!/bin/sh
network_start () 
{ 
    NETCFG=${NETWORKCFG:-"/var/etc/system/config/network"};
    netpps_start;
    if ! waitfor /pps/services/networking/control 10; then
	print "*** net_pps server obj /pps/services/networking/control not found! Network will not be functional"
	exit 1
    fi

    for iface in `ifconfig -l`;
    do
        if [ -e ${NETCFG}/${iface}.conf ]; then
            /scripts/netconfig.sh ${iface} up;
        fi;
    done;
    on -d proxyserver -au 99
    chmod 0666 /pps/services/networking/control;
    chmod 0664 /pps/services/networking/status;
}

netpps_start () 
{ 
    if_list="";
    NETCFG=${NETWORKCFG:-"/var/etc/system/config/network"};
    for iface in `ifconfig -l`;
    do
        if [ -e ${NETCFG}/${iface}.conf ]; then
            if_list="${if_list} ${iface}";
        fi;
    done;
    #on -d -p 16 net_pps -a -s -p /base ${if_list}
    on -d -p 16 net_pps -a -s -p /base tiw_sta0
}

netstart_flag=0
#After calling this script, our interface must be available (so wait for it in here).
if [ -e ${BASEFS?}/scripts/ethernet-variant.sh ]; then
    . ${BASEFS?}/scripts/ethernet-variant.sh
    netstart_flag=1
else 
    echo "ethernet.sh: missing mount of board-specific ethernet driver in ${BASEFS?}/scripts/ethernet-variant.sh"
fi

#Start PF
mount -Tio-pkt /lib/dll/lsm-pf-v6.so
waitfor /dev/pf;
pfctl -e -f /etc/pf.conf        

# PR: 254285
if [ -e ${BASEFS?}/scripts/wifi-variant.sh ]; then
    ${BASEFS?}/scripts/wifi-variant.sh
    netstart_flag=1
fi

if (( $netstart_flag == 1 )); then
   network_start
    for iface in `ifconfig -l`;
    do
        if [ -e ${NETCFG}/${iface}.conf ]; then
	    echo default_interface4::${iface} >> /pps/services/networking/status_public 
	    break;
        fi;
    done;

fi
