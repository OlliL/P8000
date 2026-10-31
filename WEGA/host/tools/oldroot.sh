#!/bin/sh
# oldroot.sh WEGA DIR: make DIR a WEGA tree for the emulator (z8k-emu -r DIR) that is WEGA plus the files of the old
# compiler versions. Every entry of WEGA is a symbolic link in DIR, except bin and lib: they are real directories of
# symbolic links, so that make install PREFIX=DIR (bin/ccv1, lib/v1/...) writes into DIR and never into WEGA.
# Running it again keeps what is installed in DIR.
set -e
[ $# -eq 2 ] || { echo "usage: $0 WEGA DIR" >&2; exit 2; }
w=$(cd "$1" && pwd)
mkdir -p "$2"
d=$(cd "$2" && pwd)
[ "$w" != "$d" ] || { echo "$0: DIR is WEGA" >&2; exit 2; }
for e in "$w"/* "$w"/.[!.]*; do
	[ -e "$e" ] || continue
	n=${e##*/}
	case $n in
	bin|lib)
		mkdir -p "$d/$n"
		for f in "$e"/*; do
			[ -e "$d/$n/${f##*/}" ] || [ -L "$d/$n/${f##*/}" ] || ln -s "$f" "$d/$n/${f##*/}"
		done ;;
	*)
		[ -e "$d/$n" ] || [ -L "$d/$n" ] || ln -s "$e" "$d/$n" ;;
	esac
done
