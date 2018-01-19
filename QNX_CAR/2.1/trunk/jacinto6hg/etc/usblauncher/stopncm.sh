#!/bin/sh
echo "usblauncher role switch starting stopncm.sh script (/sd/usblauncher/etc/stopncm.sh)"
# sleep 1 is to workaround problem with io-pkt-v6-hc intermittently crashing
sleep 1
ifconfig ${CARPLAY_IFACE} destroy