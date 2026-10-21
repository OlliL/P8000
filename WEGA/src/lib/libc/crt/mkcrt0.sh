# mkcrt0.sh NAME  - write the start-up file of the program NAME to the standard output
#
# crt0.s.in is the start-up code of the system; crt0.list gives for every program the three
# things that differ: the text behind the jump, the revision and the SCCS stamp (build time).
# The list is made from the original programs by host/tools/crt0list.py. An empty stamp means that the
# program has no stamp in the data.
# Fields of crt0.list are separated by a tab; lines with # are comments.
dir=`dirname $0`
line=`grep "^$1	" $dir/crt0.list | head -1`
if [ -z "$line" ]
then
	echo "mkcrt0: $1 is not in crt0.list" >&2
	exit 1
fi
pad=`echo "$line" | cut -f2`
rev=`echo "$line" | cut -f3`
stamp=`echo "$line" | cut -f4`
if [ -z "$stamp" ]
then
	# no stamp: the line with the stamp is dropped
	sed -e "s/PADTEXT/$pad/" -e "/STAMPTIME/d" $dir/crt0.s.in
else
	sed -e "s/PADTEXT/$pad/" -e "s/REVNUM/$rev/" -e "s;STAMPTIME;$stamp;" $dir/crt0.s.in
fi
