.TH RC M WEGA
.\" @[$]rc.M	1.2
.ad
.fi
.SH NAME
rc     \- \'read command\' WEGA-Startskript
.br
rc_csh \- \'read command\' C-Shell-Multi-User-Startskript
.SH SYNTAX
.B rc
.br
.B rc_csh
.SH BESCHREIBUNG
.B Rc
wird von
.B INIT 
ausgefuehrt, wenn das System den Status wechselt (z.B. vom
Multi-User- in den Single-User-Mode oder umgekehrt).
.B Rc_csh
wird von
.B rc
ausgefuehrt, wenn das System in den Multi-User-Mode faehrt.
.B Rc_csh
ist ein C=Shell-Skript und kann als solcher leicht den Erfordernissen
des Systems angepasst werden. Kommandos im Skript
.B rc_csh
fuehren im allgemeinen die verschiedensten Funktionen aus, wie
die Ueberpruefung der Konsistenz der Dateisysteme, das Verbinden
der Dateisysteme usw.
.PP
.B INIT
ruft
.B rc
mit 3 Argumenten auf: Zielstatus, Anzahl der
Wechsel in diesen
Status und alter Status. Dies ermoeglicht
.B rc
andere Programme, in Abhaengigkeit vom Statuswechsel, auszufuehren.
Der alte Status hat den Wert 0, wenn das System automatisch
in den Multi-User-Mode gestartet wird.
.SH DATEIEN
/etc/rc		dieses Programm
.br
/etc/rc_csh	dieses Programm
.SH SIEHE AUCH
INIT(M).
.SH DIE PROGRAMME
.PP
Die \fBrc\fR-Datei:
.nf
.na

: "@[$]rc	1.1"
: "This is the rc control script. Init invokes this script
: "and then for the other important states where real work 
: "occurs we use csh(1) scripts"

TZ=MEZ-1MES
export TZ
trap "INIT 1" 2

: "In all cases i/o is re-directed to the console"

: "zero out the mount table if this is a bootup"
if [ $3 = "0" ]
	then /bin/cat /dev/null > /etc/mnttab
	     /etc/devnm / | grep root | /etc/setmnt
fi

(
case ${1-2} in
1)
	echo
	uname -sn
	echo Single-User Mode
	if [ $3 = "2" ]
		then /etc/killall
	fi
	/etc/umfs > /dev/null 2>&1
	;;
2)
	if [ $1 != $3 ]
		then exec rc_csh
	fi
	;;

*)
	echo "Unknown state for init:" $*
	;;
esac
)

.sp
.PP
Die \fBrc_csh\fR-Datei:
.nf
.na

# "@[$]rc_csh	1.1"
	echo
	uname -sn
	echo "Multi-user Startup"
	echo

# Check for filesystem consistency
	/etc/fsck -y /dev/root
	/etc/fsck -y /dev/rusr /dev/rtmp
	/etc/fsck -y -t /scratch /dev/rz

# Set the date
	/etc/datem

# Now mount the filesystems
	/etc/mfs

# Give the user a chance to interrupt, then go multi-user
	/bin/echo -n 'The date the system knows is ' ; date
	echo 'Going multi-user in 30 seconds\!'

# configure system for correct modem/tty configuration
	/etc/ttyconfig -t 0-7

# Remove the remote lock file
	cat /dev/null > /usr/spool/uucp/LCK..tmp
	rm -f /usr/spool/uucp/LCK..*

# Save the su and cron log files
	if ( -e /usr/adm/sulog ) then
		mv /usr/adm/sulog /usr/adm/osulog
	endif
	if ( -e /usr/lib/cronlog ) then
		mv /usr/lib/cronlog /usr/lib/ocronlog
	endif

# Start cron, update and the dqueuer
	/etc/dqueuer -r
	/etc/cron > /usr/lib/cronlog
	/etc/update

# zero out utmp file
	cat /dev/null > /etc/utmp

	exit 0
.fi
.ad
