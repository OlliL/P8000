.TH MFS M WEGA
.\"@[$]mfs.M	1.2
.ad
.fi
.SH NAME
mfs, umfs  \ - mount-Verbindung  aller Dateisysteme herstellen / loesen
.SH SYNTAX
.B mfs
.br
.B umfs
.SH BESCHREIBUNG
.B Mfs
ist ein Shell-Skript, der die mount-Verbindung aller Dateisysteme
herstellt. Er kann aufgerufen werden, nachdem
.BR fsck (M)
die Konsistenz der Dateisysteme festgestellt hat.
Beachte, dass
.I /etc/mfs
geaendert werden muss, wenn sich die Konfiguration der Platte aendert.
.br
.B Umfs
ist ein Shell-Skript zum Loesen der mount-Verbindung aller
Dateisysteme.
.SH DATEIEN
/etc/mfs	dieses Programm
.br
/etc/umfs	dieses Programm
.br
/etc/mtab
.br
/dev/md*
.SH SIEHE AUCH
fsck(M),
mount(M),
mtab(5).
.SH EINSCHRAENKUNGEN
.B Mfs
muss ausgefuehrt werden, bevor das System in den Multi-User-Mode gebracht
wird, ansonsten wird
.I /usr
nicht korrekt verbunden. Die Ursache liegt darin, weil das
System beim Wechsel in den Multi-User-Mode in dem Directory
.I /usr
verschiedene Dateien anlegt und diese ueber die gesamte Laufzeit
des Systems eroeffnet bleiben. Um das Dateisystem
.I /usr
zu verbinden, muss das System in den Single-User-Mode gefahren
werden.
.PP
.B Mfs
/
.B umfs
verbindet bzw. loest das Dateisystem
.I /usr
nicht, wenn bereits
.BR cron (M)
.BR update (M)
oder
.BR dqueuer (M)
laufen oder das Accounting
.BR acct (M)
eingeschaltet ist.
