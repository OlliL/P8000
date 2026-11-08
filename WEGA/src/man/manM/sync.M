.TH SYNC M WEGA
.\" @[$]sync.M	1.2
.ad
.fi
.SH NAME
sync \- Superblock aktualisieren
.SH SYNTAX
.B sync
.SH BESCHREIBUNG
.B Sync
fuehrt den Systemruf
.B sync
aus. Soll das System angehalten werden, muss
.B sync
aufgerufen werden, um die Konsistenz der Dateisysteme zu sichern,
siehe
.BR sync (2)
fuer Details.
.SH DATEIEN
/bin/sync		dieses Programm
.SH SIEHE AUCH
sync(2),
update(M).
