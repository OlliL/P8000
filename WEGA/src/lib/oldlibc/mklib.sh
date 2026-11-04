#!/bin/sh
# mklib.sh LIB AR OBJ...: the archive LIB with the members OBJ in this order, each named after its module (NAME/0_n.o: NAME.o).
lib=$1; ar=$2; shift 2
d=$lib.d
rm -rf $d $lib
mkdir $d
names=
for o in "$@"; do
	case $o in
	../libc/obj/*) n=`basename $o` ;;
	*) n=`dirname $o`.o ;;
	esac
	cp $o $d/$n
	names="$names $n"
done
here=`pwd`
(cd $d && $ar r ../`basename $lib` $names) || exit 1
rm -rf $d
