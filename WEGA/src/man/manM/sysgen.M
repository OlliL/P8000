.TH SYSGEN M WEGA
.\" @[$]sysgen.M	1.2
.ad
.fi
.SH NAME
sysgen  \-   Generierung eines WEGA-Kerns                
.SH SYNTAX
.B sysgen 
.SH BESCHREIBUNG
.B Sysgen 
generiert einen neuen WEGA-Kern. Der Benutzer muss zuvor in
das Directory
.I /usr/sys/conf
wechseln. Die Generierung ist im Abschnitt 5 des WEGA-Systemhandbuchs
beschrieben. Alle Aenderungen erfolgen im Dialog.
Sysgen erstellt anschliessend einen neuen Kern
.B wega
und ein neues sekundaeres Bootstrap-Programm
.BR boot .
Der neue WEGA-Kern und das Bootstrap-Programm werden in dem
Directory
.I /usr/sys/conf
angelegt.
.SH DATEIEN
/etc/sysgen			dieses Programm
.br
/usr/sys/conf/wpar.c
.br
/usr/sys/h/mdsize.h
.br
/usr/sys/h/sysparm.h
.br
/usr/sys/h/ver.h
.SH DIAGNOSTIK  
Der Benutzer muss Schreiberlaubnis fuer das Directory
.I /usr/sys/conf
besitzen.
.SH SIEHE AUCH
WEGA-Systemhandbuch
