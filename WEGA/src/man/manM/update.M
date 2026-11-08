.TH UPDATE M WEGA
.\" @[$]update.M	1.2
.ad
.fi
.SH NAME
update \- periodisches Rueckschreiben der Puffer
.SH SYNTAX
.B update
.SH BESCHREIBUNG
.B Update
fuehrt alle 30 Sekunden ein
.B sync
aus.
.B Update
endet niemals und  sollte daher nur einmal aufgerufen
werden. Das erfolgt am besten in der Shell-Prozedur
.I /etc/rc_csh
(siehe
.BR init (M)).
.SH DATEIEN
/etc/update	dieses Programm
.SH SIEHE AUCH
sync(2).
