# mkcrt0.sh NAME  - write the start-up file of the program NAME to the standard output
#
# crt0.s.in is the start-up code of the system; crt0.list gives for every program the three
# things that differ: the text behind the jump, the revision and the SCCS stamp (build time).
# A pad field of - means: no jump and no text in front of the code (codgen, scodgen).
# Field 6 of crt0.list gives the template: empty for crt0.s.in, scrt0 for scrt0.s.in (the start-up code
# of the segmented programs and of ps, pstat, yacc; it has no jump and no text). Field 7 is the segment of
# _environ in the template (scrt0 only: 2 for segmented programs, 0 for ps and pstat).
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
kind=`echo "$line" | cut -f6`
envseg=`echo "$line" | cut -f7`
if [ "$kind" = "scrt0" ]
then
	# segmented start-up code: the same jump and pad text as crt0.s.in (field 2), the segment of _environ (field 7)
	src=`sed -e "s/PADTEXT/$pad/" -e "s/ENVSEG/$envseg/" $dir/scrt0.s.in`
elif [ "$pad" = "-" ]
then
	# no pad text: the jump over the text (and the text) is dropped, the program starts with the code
	# (codgen, scodgen); the stamp rule below still applies
	src=`sed -e '/^	jr	L1$/d' -e '/^	.byte	"PADTEXT"/d' -e '/^L1:$/d' $dir/crt0.s.in`
else
	src=`sed -e "s/PADTEXT/$pad/" $dir/crt0.s.in`
fi
if [ -z "$stamp" ]
then
	# no stamp: the line with the stamp is dropped
	echo "$src" | sed -e "/STAMPTIME/d"
else
	echo "$src" | sed -e "s/REVNUM/$rev/" -e "s;STAMPTIME;$stamp;"
fi
