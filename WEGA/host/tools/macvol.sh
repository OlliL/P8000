#!/bin/sh
# macOS only: makes sure the work volume of checkall/corpora is mounted and prints its mount point.
# The volume is an APFS image, case-sensitive (the corpus store holds Makefile and makefile) and mounted noatime (the guest
# clock is 1989: a host that updates atime on reading breaks find -atime in man.dbmgr, mandbmgr then loops without end).
# Environment: WEGA_VOL_IMG (default ~/.cache/wega/wegacs.sparseimage), WEGA_VOL_MNT (default /private/tmp/wega_cs),
# WEGA_VOL_SIZE (default 60g, the image is sparse: it only takes what it holds).
IMG=${WEGA_VOL_IMG:-$HOME/.cache/wega/wegacs.sparseimage}
MNT=${WEGA_VOL_MNT:-/private/tmp/wega_cs}
SIZE=${WEGA_VOL_SIZE:-60g}
if ! mount | grep -q " on $MNT ("; then
	if [ ! -f "$IMG" ]; then
		mkdir -p "`dirname "$IMG"`"
		hdiutil create -quiet -size $SIZE -type SPARSE -fs 'Case-sensitive APFS' -volname wega_cs "$IMG" >&2 || exit 1
	fi
	mkdir -p "$MNT"
	hdiutil attach -quiet -nobrowse -noverify -mountpoint "$MNT" "$IMG" >&2 || exit 1
fi
mount | grep " on $MNT (" | grep -q noatime || mount -u -o noatime "$MNT" >&2 || exit 1
echo "$MNT"
