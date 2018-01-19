#!/bin/sh

# The option usbdnet_mac assigns the specified MAC address to the Host side
# The option mac assigns the specified MAC address on the device side
# Both have the locally admin bit
echo "usblauncher role switch - starting startncm.sh script (/sd/usblauncher/etc/startncm.sh)"

if ! waitfor /dev/otg/io-usb-dcd 5; then
    echo "***/dev/otg/io-usb-dcd not found, exit"
    exit 1
fi

echo "Mounting NCM driver"
mount -Tio-pkt -o verbose=6,path=/dev/otg/io-usb-dcd,iface_num=1,protocol=ncm,usbdnet_mac=000022446688,mac=020022446688,name=${CARPLAY_IFACE_NAME} devnp-usbdnet.so
