# "@[$]adduser
# addusr - shell script to add a new user and create misc stuff for her/him

# set up temp file names
	set tmp1 = /tmp/newu$$
	set tmp2 = /tmp/nu1$$
	set tmp3 = /tmp/nu2$$
	set tmp4 = /tmp/uid$$

# what to do if signals occur
	onintr byebye

# set password and group file name
	set pwfile = /etc/passwd
	set grpfile = /etc/group
	set whoisfile = /etc/whois

# get new user name
	while ( 1 )
		/bin/echo -n "New user's name: "
		set uname = `gets`
		if ( $uname == "" ) continue
		set len = `expr length $uname`
		if ( $len > 8 ) then
			echo "'$uname': User names limited to 8 characters maximum; Try again"
			continue
		endif
		egrep "^${uname}:" $pwfile >&! /dev/null
		if ( $status == 0 ) then
			echo "${uname}: already in use. Please use another name."
		else
			break
		endif
	end

# get the uid to assign
	echo Scanning for a uid number. One moment, please.
# this lovely mess finds an available uid number and sets newuid to it
	set i = 0
	set j = 1
	sort -nt: +2 -3 $pwfile | sed 's/[^:]*:[^:]*:\([^:]*\):.*/\1/' > $tmp4
next:
	set try = `head -$j $tmp4 | tail -1l`
	if ($try == $i) then
		@ i++
		@ j++
		goto next
	else if ($i < 20) then
		if ($i < $try) then
			set i = $try
		endif
		@ i++
		@ j++
		goto next
	endif
	endif
	set newuid = $i

# get a home directory, and try to make it.
# Home directory path must be explict, we check for it by an initial slash
#
# newdir is 1 if we just created a login directory, else 0. this prevents the
# removal of an exiting directory on interupts
	set newdir = 0
	while ( 1 )
		/bin/echo -n "Home directory: "
		set homedir = `gets`
		if ( "$homedir" == "" ) then
			echo You must specify a home directory
			continue
		endif
		if ( "$homedir" == "/" ) then
			set dup = 1
			break
		endif
		if ( ! `expr $homedir : '^/'` ) then
			echo "A full explict pathname must be used for the home directory"
			echo e.g. /z/$uname
		else
			if ( -e $homedir ) then
				echo Warning: $homedir exists.
				echo -n "Okay to use it? (y/n) "
				set ok = `gets`
				if ( $ok == "y" || $ok == "yes" )  then
					set dup=1
					break
				else
				echo Please use another directory
				continue
				endif
			endif
			mkdir $homedir
			if ( $status == 0 ) then
				set newdir = 1
				break
			else
				echo "${homedir}: could not create. Please try another home directory."
			endif
		endif
	end

# get the login shell to use
	/bin/echo -n 'Login shell (/bin/csh is the default): '
	set lshell = `gets`
	if ( $lshell == "" )  then
		set lshell = /bin/csh
	else
	if ( ! -e $lshell ) then
		echo \'$lshell\' does not exist. Substituting /bin/csh.
		set lshell = /bin/csh
	endif
	endif

# now the default group at login
	while ( 1 )
		/bin/echo -n "Default group at login ('other' is the default): "
		set defgrp = `gets`
		if ( $defgrp == "" ) set defgrp = other
		egrep  "^${defgrp}:" $grpfile >! $tmp3
		if ( $status == 0 ) then
			set defnum = `sed -e "s/^[^:]*:[^:]*:\([0123456789]*\):.*/\1/" $tmp3`
			break
		else
			echo \'$defgrp\' not found in group file. Please try again.
		endif
	end

# now get group names
	/bin/echo
	echo Enter groups OTHER THAN the default group that \`$uname\' should
	echo belong to.  Enter one group name per line, with a control-d to end.
	echo If the user should belong only to her/his default group, just
	echo type control-d to the question.
	cat /dev/null > $tmp1

# $tmp1 will be a sed script to actually add the user to the groups
	while ( 1 )
		/bin/echo -n "Group: "
		set group = `gets`
		if ( $group == "" ) break
		egrep "^${group}:" $grpfile
		if ( $status == 0 ) then
			echo "/${group}:.*[^:]"'$'"/s/.*/&,$uname/" >> $tmp1
			echo "/${group}:.*:"'$'"/s/.*/&$uname/" >> $tmp1 
		else
			echo $group was not found in $grpfile. Please try again.
		endif
	end

	echo "END"

# Verify that the info is correct for this user
	/bin/echo
	echo "User:            $uname"
	echo "Uid:             $newuid"
	echo "Gid:             $defnum"
	echo "Dir:             $homedir"
	echo "Login shell:     $lshell"
	/bin/echo
	while ( 1 )
		/bin/echo -n "Okay to add (yes or no)? "
		set reply = `gets`
		if ( $reply == "" ) continue
		if ( $reply == "yes" || $reply == "y" ) then
			break
		else
			echo No action taken
			goto byebye
		endif
	end

# now that we have everything we do not want to be interrupted.
	onintr -
	echo ${uname}: Added

# create entry for this user, x for password is just in case passwd call
# fails the user at least has some kind of passwd
	echo ${uname}:x:${newuid}:${defnum}::${homedir}:${lshell} >> $pwfile

# give the user a password, the users name
	passwd $uname

# add this user name to the appropriate groups if there are any to be added
	if ( -z $tmp1 ) then
		cp $grpfile $tmp2
	else
		sed -f $tmp1 $grpfile > $tmp2
	endif

# Copy over new group file to the normal group file, adjust mode also
	mv $tmp2 $grpfile
	chmod 644 $grpfile

#	Add the user to the whois database
	echo "${uname}:::::" >> $whoisfile

# remove all temp files
	rm -f $tmp1 $tmp2 $tmp3 $tmp4

# Create mail file for user and fix modes for mail file and home directory
	cat /dev/null > /usr/spool/mail/$uname
	/etc/chmog 0600  $uname $defgrp $homedir /usr/spool/mail/$uname
	endif
	/etc/chmog 775 $uname $defgrp $homedir
	exit 0

byebye:
	echo
	rm -f $tmp1 $tmp2 $tmp3
	if ( $?homedir && ! $?dup ) then
		if ( -e $homedir ) then
			if ( $newdir ) then
				rmdir $homedir
			endif
		endif
	endif
	exit 1
