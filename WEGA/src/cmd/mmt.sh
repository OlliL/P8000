: "@[$]mmt	1.2  08/19/83 13:45:31 - 87wega3.2"
u=-cm;	if test `basename $0` = "mvt";	then u=-mv;	fi
if test $# = 0;	then help text1;	exit 1;	fi
PATH=/bin:/usr/bin:/usr/lbin;	O="";		o="";	y=""
:			'If C/A/T connected to SM-4, use O=""; o=""'
while test -n "$1" -a ! -r "$1"
do case $1 in
	-a)		O="-a";		o="" ;;
	-e)		e=eqn ;;
	-t)		f=tbl ;;
	-y)		if test "$u" = "-cm";	then u=-mm;	fi ;;
	-)		break ;;
	*)		a="$a $1" ;;
   esac
   shift
done
if test -z "$1";	then echo "$0: no input file";	exit 1;	fi
if test "$O" = "-g";	then x="-f$1";	fi
d="$*"
if test "$d" = "-";	then shift;	x="";	d="";	fi
if test -n "$f";	then f="tbl $*|";	d="";	fi
if test -n "$e"
	then	if test -n "$f"
			then e="eqn|"
			else e="eqn $*|";	d=""
		fi
fi
if test "$u" = "-mm" -o "$u" = "-cm" ;	then y="";	fi
eval $f $e troff $y $O $u $a $d $o $x; exit 0
