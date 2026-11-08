.TH MAKEWHATIS M WEGA
.\" @[$]makewhatis.M	1.2
.ad
.fi
.SH NAME
makewhatis \- Erstellung der Datenbasis fuer whatis und apropos
.SH SYNTAX
.B makewhatis
.SH BESCHREIBUNG
.B Makewhatis
erstellt fuer die Kommandos
.BR whatis (1)
und
.BR apropos (1)
die Datenbasis.
Es benutzt
.BR getNAME (1)
zur Auswahl des relevanten Textes aus jedem Manual-Eintrag
in
.I /usr/man/*
und sortiert dann diese Texte.
.SH DATEIEN
/etc/makewhatis		dieses Programm
.br
/usr/lib/whatis		erstellte Datenbasis
.SH SIEHE AUCH
getNAME(1).
