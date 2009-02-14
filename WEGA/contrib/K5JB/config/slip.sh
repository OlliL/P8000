#!/bin/sh

# this script is used on the FreeBSD end of the SLIP connection


kldload if_sl
slattach -l -s 9600 /dev/ttyd0
sleep 2
stty cstopb < /dev/ttyd0
sl=`ifconfig | grep 'sl[0-9]*:' | tail -1 | sed 's|:.*||g'`
ifconfig $sl 10.1.1.1 10.1.1.2 netmask 255.255.255.0 mtu 1500

