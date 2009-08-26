#grep '\.o' TODO | while read line ; do
#	file=$(basename $line .o)
#	source=$(awk '/ '$file'\./ { print $2 }' Library.SC)
#	case $source in
#		*.c)	type="C";;
#		*)type="ASM";;
#	esac
#	printf "%-20s %-20s %s\n" "$line" "$source" "$type"
#	
#done > TODO.new

while read line ; do
	file=$(echo "$line" | awk '{if(NF==3)print $2}')
	if [ -n "$file" ] ; then
		found=$(find . -name "$file")
        	if [ -n "$found" ] ; then
			echo "$line" >> TODO.done
		else
			echo "$line" >> TODO.undone
		fi
	else
		echo "$line" >> TODO.undone
	fi
done < TODO
mv TODO.undone TODO
