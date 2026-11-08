.TH DATEM M WEGA
.\" @[$]datem.M	1.2
.ad
.fi
.SH NAME
datem \- Benutzerfreudliche Eingabe von Datum / Zeit
.SH SYNTAX
.B datem
.SH BESCHREIBUNG
.B Datem
setzt Datum und Zeit, nachdem es die Angaben vom Benutzer abgefragt
hat.
.LP
Das Datum kann in der Form:
.I dd-mo-yy,
wobei 
.I dd
der Tag,
.I mo
die gebraeuchliche englische Abkuerzung des Monatnamens
und
.I yy
die letzten beiden Ziffern des Jahres sind, angegeben werden.
Es kann aber auch die Form:
. I mm/dd/yy,
wobei
.I mm
der
.IR mm 'te
Monat,
.I dd
der Tag
und
.I yy
die letzten beiden Ziffern des Jahres sind, benutzt werden.
Der 4. Januar 1986 kann z.B. als 4-Jan-86 oder mit 1/4/86 angegeben
werden.
.LP
Fuer die Zeit wird die 24-Stundenanzeige in der Form hh:mm
benutzt.  Es wird die oertliche Zeit eingegeben.
.LP
Alle unmoeglichen und einige unwahrscheinliche Eingaben werden
zurueckgewiesen. Mit einer fehlenden Datumseingabe (nur Return)
kann die Zeit gesetzt werden, ohne das Datum zu veraendern.
.PP
Erhaelt
.B datem
die Eingaben nicht innerhalb von 5 Minuten, tritt ein Timeout
auf und das Programm endet.
.PP
.B Datem 
wird normalerweise von
.I /etc/rc_csh,
dem WEGA-Startskript, aufgerufen.
.SH DATEIEN
/etc/datem 	dieses Programm
.SH SIEHE AUCH
date(1),
rc(M).
.SH DIAGNOSTIK
\fInot superuser\fR: Nur der Superuser kann Datum und Zeit setzen.
.SH EINSCHRAENKUNGEN
.B Datem
kann nicht Datum und Zeit anzeigen.
