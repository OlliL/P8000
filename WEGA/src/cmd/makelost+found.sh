#
foreach i ( $* )
set dir="$i/lost+found"
	if (! -e $dir) then
		mkdir $dir
		chmog 0750 wega 0 $dir
	endif
	echo make lost+found runs about 3 minutes for $i
	@ j = 158
	while $j
		echo > $dir/Z$j
     		@ j--
	end
	@ j = 158
	while $j
		rm $dir/Z$j
		@ j--
	end
end
