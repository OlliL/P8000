#!/bin/sh -u
#-
# Copyright (c) 2006 Oliver Lehmann <oliver@FreeBSD.org>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $Id: wega_backup.sh,v 1.4 2006/02/01 21:22:04 olivleh1 Exp $
#

UUENCODE=uuencode
KERMIT=kermit
whereis printf | grep /printf >/dev/null && PRINTF="printf" || PRINTF="echo -n"

while [ ! -c "${PORT}" ] ; do
	${PRINTF} "Port number                  (0-9)                  [0]:    "
	read PORTNR
	if [ -z "${PORTNR}" ] ; then
		PORTNR=0
	fi
	PORT=/dev/tty${PORTNR}
done

while [ "${SPEED}" != "9600" -a \
        "${SPEED}" != "19200" ] ; do
	${PRINTF} "Linespeed                    (9600, 19200)          [9600]: "
	read SPEED
	if [ -z "${SPEED}" ] ; then
		SPEED=9600
	fi
done

while [ "${TYPE}" != "cpio" -a \
        "${TYPE}" != "tar" -a \
        "${TYPE}" != "dd" -a \
        "${TYPE}" != "dump" ] ; do
	${PRINTF} "Type of backup               (cpio, tar, dd, dump)  [cpio]: "
	read TYPE
	if [ -z "${TYPE}" ] ; then
		TYPE="cpio"
	fi
done

${PRINTF} "Archive name                                        [wega.${TYPE}]: "
read ARCHIVE
if [ -z "${ARCHIVE}" ] ; then
	ARCHIVE="wega.${TYPE}"
fi
	
if [ "${TYPE}" != "cpio" ] ; then
	echo "We recommend using the cpio backup mode which works best with special"
	echo "files such as character, pipe or block special files"
	if [ "${TYPE}" = "tar" ] ; then
		echo "You have choosen tar, That means /dev cannot be backed up because"
		echo "tar cannot handle device files, choose cpio to include /dev into"
		echo "your backup"
	elif [ "${TYPE}" = "dump" ] ; then
		echo "If you backup the root partition you will find a special file in it"
		echo "called /dumppipe.$$ - this is just a named pipe (FIFO) because dump does"
		echo "not work with STDOUT directly"
	fi
fi

TO_BACKUP=" "
if [ "${TYPE}" = "dd" -o "${TYPE}" = "dump" ] ; then
	#PARTITIONS=`df | sed 's/^[^ ]* [ ]*\([^ ]*\) .*/\1/g'`
	while [ ! -c "${TO_BACKUP}" ] ; do
		${PRINTF} "Select a partition to backup (root, tmp, usr, z)    [root]: "
		read TO_BACKUP
		if [ -z "${TO_BACKUP}" ] ; then
			TO_BACKUP="root"
		fi
		TO_BACKUP="/dev/${TO_BACKUP}"
	done
elif [ "${TYPE}" = "tar" -o "${TYPE}" = "cpio" ] ; then
	while [ ! -d "${TO_BACKUP}" ] ; do
		${PRINTF} "Specify what to backup recursive                    [/]   : "
		read TO_BACKUP
		if [ -z "${TO_BACKUP}" ] ; then
			TO_BACKUP="/"
		fi
	done
fi

case "${TYPE}" in
	cpio)	CMD="cd ${TO_BACKUP} && find . -print | cpio -oa | ${UUENCODE} -";;
	tar)	if [ "${TO_BACKUP}" = "/" ] ; then
			FILES='`ls | grep -v '^dev$'`'
		else
			FILES='.'
		fi
		CMD="cd ${TO_BACKUP} && tar cf - ${FILES} | ${UUENCODE} -";;

	dd)	CMD="dd if=${TO_BACKUP} bs=512 | ${UUENCODE} -";;
	dump)	CMD="uuencode /dumppipe -"
		mknod p /dumppipe.$$ || exit 1
		dump 0f /dumppipe ${TO_BACKUP} &
		;;
esac

cat <<_EOF

now open a kermit on your receiving system and type:

set line /dev/ttyXX
set speed ${SPEED}
set parity none
set carrier-watch off
set file type binary
receive

_EOF


echo "press enter to start the transfer"
read LALA
set -x
eval "${CMD}" | ${KERMIT}  -i -b ${SPEED} -l ${PORT} -p n -a ${ARCHIVE} -s -

if [ "${TYPE}" = "dump" ] ; then
	rm -f /dumppipe.$$
fi
