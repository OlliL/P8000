#!/bin/sh

echo "getting file $1"
kermit -q -b 9600 -l /dev/`who am i | awk '{print $2}'` -p n -g "$1"
if [ $? -ne 0 -o ! -s "$1" ] ; then
	echo "file transfer failed"
	exit 1
else
	exit 0
fi
