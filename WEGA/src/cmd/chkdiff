# "@[$]chkdiff	1.8  04/05/83 14:21:31 - 87wega3.2"
if ($#argv == 0 || $#argv > 7) then
	echo Usage: chkdiff \[-h\] \[-v rel.lev\] \[-v rel.lev\] file
	exit 1
endif
#
# parse the given arguments
#
set vers=0
set ver1=
set ver2=
set hop=
if ($1 == "-h") then
	set hop = $1
	if ($#argv == 2 || $#argv == 4 || $#argv == 6) then
		if ($2 == "-v") then
			set ver1="$2 $3"
			if ($4 == "-v") then
				set vers=1
				set ver2="$4 $5"
				set file=$6
			else
				set file=$4	
			endif
		else
			set file=$2
		endif
	else
		echo Usage: chkdiff \[-h\] \[-v rel.lev\] \[-v rel.lev\] file
		exit 1
	endif
else
	if ($#argv == 1 || $#argv == 3 || $#argv == 5) then
		if ($1 == "-v") then
			set ver1="$1 $2"
			if ($3 == "-v") then
				set vers=1
				set ver2="$3 $4"
				set file=$5
			else
				set file=$3	
			endif
		else
			set file=$1
		endif
	else
		echo Usage: chkdiff \[-h\] \[-v rel.lev\] \[-v rel.lev\] file
		exit 1
	endif
endif

#
# error if filename+
#
set len = `expr length $file`
set lastch = `expr substr $file $len 1`
if ($lastch == +) then
	echo Usage: chkdiff \[-h\] \[-v rel.lev\] \[-v rel.lev\] file
	exit 1
endif

#
# separate directory and filename
#
set dir = $file:h
set file = $file:t
if ($dir == $file) then
	set dir =
else
	set dir = $dir/
endif

#
# set up the diff command
#
if ($vers) then
	set diff=/tmp/chkdiff.$$
	chkout -p $ver2 $dir$file > $diff
	chkout -p $ver1 $dir$file | diff $hop - $diff
	rm -f $diff
else
	chkout -p $ver1 $dir$file | diff $hop - $file
endif
