: "@[$]rmuser	1.3  08/06/82 16:20:02 - 87wega3.2"
: shell proc to remove a user
pwfile=/etc/passwd
whoisfile=/etc/whois
tmp=/tmp/rmu$$
root=/
tmp2=/tmp/rmugrp$$
tmp3=/tmp/rmuwhois
group=/etc/group
trap "rm -f $tmp $tmp2;exit" 0 1 2 13 15
: do we know about this user at all, if so do it, if not exit
if egrep "^$1:" $pwfile > /dev/null; then
	passwd $1
	echo deleting $1 from groups in $group
: make up a sed script to delete the user from /etc/group
		echo s/,$1//>$tmp
		echo s/:$1,/:/>>$tmp
		echo s/:$1/:/>>$tmp
		sed -f $tmp $group>$tmp2
		mv $tmp2 $group
		chmod 644 $group
		sed -e "/^${1}:.*/d" < $whoisfile > $tmp3
		mv $tmp3 $whoisfile
		chmod 0666 $whoisfile
		rm -f /usr/spool/mail/$1
	echo listing all files owned by $1 on files.$1 \(this will take a while\)
	find $root -user $1 -exec ls -ld {} ";">files.$1
else  echo $1 is not in the passwd file.;exit 1;
fi
