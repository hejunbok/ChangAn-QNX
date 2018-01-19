#!/bin/bash

DEVICE=root@qnxcar # enter hostname/IP address
QNXCAR_SDP=/opt/qnxcar2

###############################3

QNX_TARGET=$QNXCAR_SDP/target/qnx6/armle-v7

function msg() {
  echo "$@" 2>&1
}

function die() {
  msg "$@"
  msg "Exiting,"
  exit 1
}

# copy printf, uname and ps (needed for Qt Creator)

for i in /usr/bin/passwd /usr/bin/printf /usr/bin/xargs /usr/bin/strings /bin/kill /bin/ps /bin/uname; do
    scp $QNX_TARGET/$i $DEVICE:$i || die "Could not copy to board: $i"
done


