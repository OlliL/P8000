#
# '@[$]man.dbmgr	1.6  04/18/83 11:52:46 - 87wega3.2'
# rjr 21Jul82 changed from /dev/zd3 to /dev/tmp

set age = 3
set fs_size = `df -f /dev/tmp | sed 's/\(.*:[ 	]*\)\([0-9]*\)\(.*\)/\2/'`
set database = "/tmp/man"

if (! -e $database) then
	mkdir $database
	chmog 777 bin 0  $database
	set dbsize = 0
else
	if (! -d $database) then
	set dbsize = 0
else
	set dbsize = `du -s $database`
	endif
endif

if ($fs_size[1] > 5000) then
	set maxdbsize = 500
else
	if ($fs_size[1] > 1000) then
		set maxdbsize = 100
else
	set maxdbsize = 2
	endif
endif

while ($dbsize[1] > $maxdbsize)
	foreach i (`find $database -atime +$age -print`)
		if (! -d $i) then
			rm -f $i
		endif
	end
	if (`ls $database | wc -w` == 0) break
	set dbsize = `du -s $database`
	@ age--
end
