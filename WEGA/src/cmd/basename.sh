: "@[$]basename	2.2  01/26/83 11:45:49 - 87wega3.2"
a=`expr ${1-.}// : '\(.*[^/]\)//*$'`
if  test $0 = basename || test $0 = /usr/bin/basename ; then
expr //$a : ".*/\(.*\)$2"
else
expr $a : '\(.*\)/[^/]*'
fi
