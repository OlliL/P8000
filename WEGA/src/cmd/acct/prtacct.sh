: '@[$]prtacct.sh	1.1  01/04/83 09:17:52 - 87wega3.2'
#	"print daily/summary total accounting (any file in tacct.h format)"
#	"prtacct file [heading]"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
(cat <<!; acctmerg -t -a <$1; acctmerg -p <$1) | pr -h "$2"
	LOGIN 	   CPU (MINS)	  KCORE-MINS	CONNECT (MINS)	DISK	# OF	# OF	# DISK	FEE
UID	NAME 	 PRIME	NPRIME	PRIME	NPRIME	PRIME	NPRIME	BLOCKS	PROCS	SESS	SAMPLES	
!
