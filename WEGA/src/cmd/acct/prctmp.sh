: '@[$]prctmp.sh	1.1  01/04/83 09:17:51 - 87wega3.2'
:	"print session record file (ctmp.h/ascii) with headings"
:	"prctmp file [heading]"
PATH=/usr/lib/acct:/bin:/usr/bin:/etc
(cat <<!; cat $*) | pr -h "SESSIONS, SORTED BY ENDING TIME"

		LOGIN	CONN.TIME(SECS)	START TIME	START TIME


!
