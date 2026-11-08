# sysgen

echo
echo ---- sysgen ----
echo
echo 	Version 1.2
echo
echo

cd /usr/sys/conf

onintr abbr

echo -n "Do you want to change system constants ? : "
set i=`gets`
if ($i == 'y') then
	vi ../h/sysparm.h
endif

echo file system layout :
cat ../h/mdsize.h
echo

# Einbindung der Plattenaufteilung

echo -n "Do you want to change file system layout ? : "
set i=`gets`
if ($i == 'y') then

	# Voreinstellung
	set md5=0
	set md6=0
	set md7=0
	set md8=0
	set md9=0

	echo -n "	size of /usr : "
	set u=`gets`
	echo -n "	size of /swap : "
	set s=`gets`
	echo -n "	size of /root : "
	set r=`gets`
	echo -n "	size of /tmp : "
	set t=`gets`
	echo -n "	size of /z : "
	set z=`gets`
	echo -n "	size of /md5 : "
	set md5=`gets`
	if ($md5 == "0" || $md5 == "") goto m1
	echo -n "	size of /md6 : "
	set md6=`gets`
	if ($md6 == "0") goto m1
	echo -n "	size of /md7 : "
	set md7=`gets`
	if ($md7 == "0") goto m1
	echo -n "	size of /md8 : "
	set md8=`gets`
	if ($md8 == "0") goto m1
	echo -n "	size of /md9 : "
	set md9=`gets`
	echo
m1:
	if ($md5 == "") set md5=0
	if ($md6 == "") set md6=0
	if ($md7 == "") set md7=0
	if ($md8 == "") set md8=0
	if ($md9 == "") set md9=0

	echo size of all file systems on 1. disk : `expr $u + $s + $r + $t + $z + $md5 + $md6 + $md7 + $md8 + $md9`

	echo

	# Dateisystemaufteilung in ../h/mdsize.h

	echo "#define USR	$u" > ../h/mdsize.h
	echo "#define SWAP	$s" >> ../h/mdsize.h
	echo "#define ROOT	$r" >> ../h/mdsize.h
	echo "#define TMP	$t" >> ../h/mdsize.h
	echo "#define Z	$z" >> ../h/mdsize.h
	echo "#define MD5	$md5" >> ../h/mdsize.h
	echo "#define MD6	$md6" >> ../h/mdsize.h
	echo "#define MD7	$md7" >> ../h/mdsize.h
	echo "#define MD8	$md8" >> ../h/mdsize.h
	echo "#define MD9	$md9" >> ../h/mdsize.h

	# Voreinstellung der Variablen fuer 2. Platte

	set md10=0
	set md11=0
	set md12=0
	set md13=0
	set md14=0
	set md15=0
	set md16=0
	set md17=0
	set md18=0
	set md19=0

	echo -n "Do you want to add an second harddisk ? : "
	set i=`gets`
	if ($i == 'y') then
		echo -n "	size of /md10 : "
		set md10=`gets`
		if ($md10 == "0" || $md10 == "") goto m2
		echo -n "	size of /md11 : "
		set md11=`gets`
		if ($md11 == "0") goto m2
		echo -n "	size of /md12 : "
		set md12=`gets`
		if ($md12 == "0") goto m2
		echo -n "	size of /md13 : "
		set md13=`gets`
		if ($md13 == "0") goto m2
		echo -n "	size of /md14 : "
		set md14=`gets`
		if ($md14 == "0") goto m2
		echo -n "	size of /md15 : "
		set md15=`gets`
		if ($md15 == "0") goto m2
		echo -n "	size of /md16 : "
		set md16=`gets`
		if ($md16 == "0") goto m2
		echo -n "	size of /md17 : "
		set md17=`gets`
		if ($md17 == "0") goto m2
		echo -n "	size of /md18 : "
		set md18=`gets`
		if ($md18 == "0") goto m2
		echo -n "	size of /md19 : "
		set md19=`gets`
	m2:
		if ($md10 == "") set md10=0
		if ($md11 == "") set md11=0
		if ($md12 == "") set md12=0
		if ($md13 == "") set md13=0
		if ($md14 == "") set md14=0
		if ($md15 == "") set md15=0
		if ($md16 == "") set md16=0
		if ($md17 == "") set md17=0
		if ($md18 == "") set md18=0
		if ($md19 == "") set md19=0

		echo size of all file systems on 2. disk : `expr $md10 + $md11 + $md12 + $md13 + $md14 + $md15 + $md16 + $md17 + $md18 + $md19`

	endif

	echo >> ../h/mdsize.h

	echo "#define MD10	$md10" >> ../h/mdsize.h
	echo "#define MD11	$md11" >> ../h/mdsize.h
	echo "#define MD12	$md12" >> ../h/mdsize.h
	echo "#define MD13	$md13" >> ../h/mdsize.h
	echo "#define MD14	$md14" >> ../h/mdsize.h
	echo "#define MD15	$md15" >> ../h/mdsize.h
	echo "#define MD16	$md16" >> ../h/mdsize.h
	echo "#define MD17	$md17" >> ../h/mdsize.h
	echo "#define MD18	$md18" >> ../h/mdsize.h
	echo "#define MD19	$md19" >> ../h/mdsize.h

	echo file system layout now :
	cat ../h/mdsize.h
	echo

	set R=`expr $u + $s`
	echo "char dfline[50] = \042md(0,$R)wega\042;" >bstart.c
endif

echo aktual version parameters :
cat ../h/ver.h
echo
echo -n "Do you want to change version parameters ? :"
set i=`gets`
if ($i == 'y') then
	echo -n "system name (max 7 characters) : "
	set sn=`gets`
	echo -n "node name (max 7 characters) : "
	set nn=`gets`
	echo -n "version (max 7 characters) : "
	set v=`gets`
	echo "#define SYSNAME		\042$sn\042" > ../h/ver.h
	echo "#define NODENAME	\042$nn\042" >> ../h/ver.h
	echo "#define VERSION		\042 $v\042" >> ../h/ver.h
	echo
	echo new system parameters :
	cat ../h/ver.h
	echo
endif

echo -n "Do you want to add new driver to the system ? : "
set i=`gets`
if ($i == 'y') then
d1:
	echo -n "Enter the name of the driver object : "
	set i=`gets`
	if ( -e $i ) then
		ar rv ../dev/LIB2 $i
	else
		echo $i no object file
		goto d1
	endif
endif

echo New wega is building now !  Wait a minute !

make -i -f make.wega
if ($status == 0) then
	echo New wega : `pwd`/wega
endif
echo
echo New boot is building now !

make -i -f make.boot
if ($status == 0) then
	echo New boot : `pwd`/boot
endif
exit 0

# Abbruch bei INTR
abbr:
	echo
	echo --- sysgen interrupted ---
	exit 1
