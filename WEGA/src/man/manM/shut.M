.TH SHUT M WEGA
.\" @[$]shut.M	1.2
.ad
.fi
.SH NAME
shut \- Shutdown-Warnung der Benutzer
.SH SYNTAX
.B shut
.I [dtime btime]
.SH BESCHREIBUNG
.B Shut
wird von der Shell-Prozedur 
.B down
aufgerufen. Das Programm
sendet Warnungen an alle angemeldeten Benutzer. Die optionalen
Argumente 
.I dtime
und
.I btime
geben die Zeit in Minuten, bis zum Abfahren des Systems und
die Zeit zwischen der Aussendung der einzelnen Warnungen, an.
Fehlen die Argumente, erfolgt eine Abfrage der Werte. Zu den
angegebenen Zeiten wird die Warnung so lange an alle angemeldeten
Benutzer geschickt, bis die Endewarnung ausgesendet wird und
das Programm endet.
.LP
Ab der 5 Minuten Marke beginnt
.B shut
die Warnungen im 1 Minuten Intervall auszusenden.
.SH DATEIEN
/etc/shut	dieses Programm
.br
/etc/utmp
.SH SIEHE AUCH
down(M).
.SH DIAGNOSTIK
Fehlerausgaben sind selbsterklaerend.
