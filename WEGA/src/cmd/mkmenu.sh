#
# csh script to make menu files
#
onintr opcan
set umask = 0
set isthere = `printenv | grep MENUPATH`
if ( $status == 0 ) then 
   set mdir = $MENUPATH
else
   set mdir = ''
endif
set slash = '/'
#
if ($#argv == 1) then
	echo 'Menu is '$argv[1]
	set file = $argv[1]
endif
#
if("$home" == "/") then
	set home = ''
endif
if(-e $home/.menupath) then
	set udir = `cat $home/.menupath`
else
	set udir = ''
endif
if(! -r $home/.menupath) then
	set udir = ''
endif
#
if(-e /etc/menupath) then
	set gdir = `cat /etc/menupath`
else
	set gdir = ''
endif
if(! -r /etc/menupath) then
	set gdir = ''
endif
#
#
set flag = 0
if ($mdir != '') then
        echo -n "Add this menu to the $mdir (MENUPATH) directory (y/n) ? "   
        set resp = `gets`
         if($resp == 'y' || $resp == 'Y') then
              set flag = 3
              goto where
endif
endif
if ($udir != '') then
	echo -n "Add this menu to the $udir (user) menu directory (y/n) ? " 
	set resp = `gets`
	if($resp == 'y' || $resp == 'Y') then 
             set flag = 1
             goto where
endif
endif
if ($gdir != '') then
   echo -n "Add this menu to the $gdir (global) menu directory (y/n) ? " 
	set resp = `gets`
	if($resp == 'y' || $resp == 'Y') then 
             set flag = 2
endif
endif
#
where:
#
while ( 1 )
if ($#argv == 1) then
	goto skip
endif
echo -n 'Enter menu name: '
set file = `gets`
#
skip:
#
set pwd = `pwd`
if ($file == '') then
	echo You must enter a name\; try again ...
	continue
else if ($flag == 0 && -e $pwd$slash$file ) then
        echo -n "$pwd$slash$file exists; overwrite it (y/n)? "
        set resp = `gets`
        if ($resp == 'y' || $resp == 'Y') then
           goto ok
endif
else if ( $udir != '' && -e $udir$slash$file ) then
        echo -n "$udir$slash$file exists; overwrite it (y/n)? "
        set resp = `gets`
        if ($resp == 'y' || $resp == 'Y') then
           goto ok
endif
else if ( $gdir != '' && -e $gdir$slash$file ) then
        echo -n "$gdir$slash$file exists; overwrite it (y/n)? "
        set resp = `gets`
        if ($resp == 'y' || $resp == 'Y') then
           goto ok
endif
else if ( $mdir != '' && -e $mdir$slash$file ) then
        echo -n "$mdir$slash$file exists; overwrite it (y/n)? "
        set resp = `gets`
        if ($resp == 'y' || $resp == 'Y') then
          goto ok
endif
	set argv = ()
else
	break
endif
end
#
ok:
#
echo
if ($flag == 0) then
   echo Menu is $pwd$slash$file            
else if ($mdir != '' && $flag == 3) then
   echo Menu is $mdir$slash$file
else if ($udir != '' && $flag == 1) then
   echo Menu is $udir$slash$file
else if ($gdir != '' && $flag == 2) then
   echo Menu is $gdir$slash$file 
endif
echo
#
echo
echo suggested: Company Name 
echo -n 'Enter menu header - line 1, field 1: '
set hdr11 = `gets`
echo
echo suggested: System Name
echo -n 'Enter menu header - line 1, field 2: ' 
set hdr12 = `gets`
echo
echo suggested: Division Name 
echo -n 'Enter menu header - line 2, field 1: ' 
set hdr21 = `gets`
echo
echo suggested: Menu Title
echo -n 'Enter menu header - line 2, field 2: ' 
set hdr22 = `gets`
echo
#
switch($flag)
	case 1:
		set file = $udir$slash$file
		breaksw
	case 2:
		set file = $gdir$slash$file
		breaksw
        case 3: 
                set file = $mdir$slash$file
                breaksw
	default:
	case 0:
		breaksw
endsw
#
echo $hdr11':' > $file
echo $hdr12':' >> $file
echo $hdr21':' >> $file
echo $hdr22':' >> $file
#
set i = '-1'
#
while (1)
@ i++
if ($i > 16) then
	echo 'Only sixteen (16) entries allowed per menu.'
	exit(0)
endif
echo 'Menu entry '$i':'
echo 'Enter entry type -- 0 = next menu; 1 = sh cmd; 2 = csh cmd; 3 = exit;' 
echo -n '<any-other-key> to quit: '
set type = `gets`
switch ($type)
	case 0:
		echo
		echo -n 'Menu name: '
		set menu = `gets`
		echo
		echo -n 'Menu prompt: '
		set prompt = `gets`
		echo
		echo $menu':0:'$prompt >> $file
		breaksw
	case 1:
		echo
		echo -n 'Shell command: '
		set menu = `gets`
		echo
		echo -n 'Menu prompt: '
		set prompt = `gets`
		echo
		echo $menu':1:'$prompt >> $file
		breaksw
	case 2:
		echo
		echo -n 'Csh command: '
		set menu = `gets`
		echo
		echo -n 'Menu prompt: '
		set prompt = `gets`
		echo
		echo $menu':2:'$prompt >> $file
		breaksw
	case 3:
		echo
		echo -n 'Menu prompt: '
		set prompt = `gets`
		echo
		echo ':0:'$prompt >> $file
		breaksw
	default:
		echo
		echo -n 'Are you done (y,n)? '
		set resp = `gets`
		if ($resp == 'y') then
                goto done
		endif
		@ i--
		breaksw
endsw
end
#
done:
	if(-e $file) then
		chmod 644 $file
	endif
	exit 0
end 
#
opcan:
	echo
	echo mkmenu: Operator cancel
	exit (1)

