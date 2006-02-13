#!/bin/sh

echo "getting file $1"

SPEED=`stty | grep speed | sed 's/speed \([0-9]*\) baud.*/\1/g'`
TTY=`who am i | awk '{print $2}'`

kermit -q -b ${SPEED}  -l /dev/${TTY} -p n -g "$1"
if [ $? -ne 0 -o ! -s "$1" ] ; then
	echo "file transfer failed"
	exit 1
else
	exit 0
fi
