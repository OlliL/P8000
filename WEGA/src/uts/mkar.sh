# mkar.sh ARCHIVE ARHDR.O MEMBER...: the archive from the members, with the headers of the table in ARHDR.O (arhdr.s), in its order.
# The table starts at byte 40 of the object: 2 bytes magic, 26 bytes per member. ar takes these fields from the files it is given,
# so make cannot reach them. All members have an even size, so there is no padding. dd and cat also exist on WEGA.
out=$1
h=$2
shift
shift
dd if=$h bs=1 skip=40 count=2 > $out 2>/dev/null
n=42
for f in $*
do
	dd if=$h bs=1 skip=$n count=26 >> $out 2>/dev/null
	cat $f >> $out
	n=`expr $n + 26`
done
