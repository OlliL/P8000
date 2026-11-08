.TH MVDIR M WEGA
.\" @[$]mvdir.M	1.2
.ad
.fi
.SH NAME
mvdir \- Directory umbenennen bzw. verschieben
.SH SYNTAX
.B mvdir
.I "dirname name"
.SH BESCHREIBUNG
.B Mvdir
fuehrt innerhalb eines Dateisystems eine Umbenennung oder
Verschiebung eines Directorys (einschliesslich der enthaltenen
Dateien) aus.
.I Dirname
muss ein Directory sein,
.I name
darf noch nicht existieren.
Kein Name darf eine Untermenge des anderen sein
.RB ( /x/y
nicht zu
.B /x/y/z
und umgekehrt).
.PP
Nur der Superuser kann
.B mvdir
benutzen.
.SH DATEIEN
/etc/mvdir	dieses Programm
.SH SIEHE AUCH
mkdir(1).
.SH EINSCHRAENKUNGEN
Directorys koennen nicht zwischen Dateisystemen verschoben
werden.
