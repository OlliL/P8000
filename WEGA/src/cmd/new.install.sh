# Installation der WEGA-Dateisysteme nach Neufuellung der Platte

echo Bei einer Veraenderung der Plattenaufteilung, bei der sich die Offset-Werte
echo der Dateisysteme /dev/tmp und /dev/z verschieben, sind diese Dateisysteme
echo neu anzulegen.
echo Dabei werden die alten Inhalte dieser Dateisysteme zerstoert!
echo
echo Sollen in Ihrem System die Dateisysteme /dev/tmp und /dev/z
echo -n "neu angelegt werden ? (j/n) : "
set i=`gets`
if ($i != "j") then
	echo Benennen der Dateisysteme
	echo
	goto bb
endif

echo Eingabe der Anzahl der Bloecke in  den Dateisystemen
echo "(fuer Standardwerte RETURN eingeben)"
echo
set TMP=4000
set Z=60732

echo -n "/dev/tmp (Standard 4000) : "
set n=`gets`
if ($n != "") set TMP=$n

echo
echo -n "/dev/z  (Standard 60732) : "
set n=`gets`
if ($n != "") set Z=$n

echo
echo /etc/mkfs /dev/rtmp $TMP
/etc/mkfs /dev/rtmp $TMP
echo
echo /etc/mkfs /dev/rz $Z
/etc/mkfs /dev/rz $Z

echo
echo Benennen der Dateisysteme
echo
/etc/labelit /dev/rtmp /tmp 1
/etc/labelit /dev/rz /z 1
bb:
/etc/labelit /dev/rroot root 1
/etc/labelit /dev/rusr /usr 1

echo
echo Schreiben des Boot-Blocks
echo
echo dd if=/pb.image of=/dev/rmd0 conv=sync count=1
dd if=/pb.image of=/dev/rmd0 conv=sync count=1

echo
/etc/mfs
echo
echo Anlegen der Directories lost+found
echo
/etc/makelost+found /
/etc/makelost+found /usr
if ($i == "j") then
	/etc/makelost+found /tmp
	/etc/makelost+found /z
endif

echo
echo Expandieren der Handbucheintraege in man0
/usr/bin/unpack /usr/man/man0/* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in man1
/usr/bin/unpack /usr/man/man1/INTRO* 2>/dev/null
/usr/bin/unpack /usr/man/man1/[a-m]* 2>/dev/null
/usr/bin/unpack /usr/man/man1/[n-z]* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in man2
/usr/bin/unpack /usr/man/man2/* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in man3
/usr/bin/unpack /usr/man/man3/* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in man4
/usr/bin/unpack /usr/man/man4/* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in man5
/usr/bin/unpack /usr/man/man5/* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in man7
/usr/bin/unpack /usr/man/man7/* 2>/dev/null
echo
echo Expandieren der Handbucheintraege in manM
/usr/bin/unpack /usr/man/manM/* 2>/dev/null
/etc/umfs

echo
echo Damit ist das System vollstaendig eingerichtet.
echo
echo Uebergang in den Multi-User-Mode durch Eingabe von
echo "      init 2"
echo
