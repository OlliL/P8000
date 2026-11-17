: '@[$]shutacct.sh	1.2  01/18/83 12:30:20 - 87wega3.2'
:	"shutacct [arg] - shuts down acct, called from /etc/shutdown"
:	"whenever system taken down"
:	"arg	added to /usr/adm/wtmp to record reason, defaults to shutdown"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
a=${1-shutdown}
acctwtmp  "$a"  >>/usr/adm/wtmp
turnacct off
