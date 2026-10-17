
while read line ; do
	file=$(echo "$line" | awk '{print $2}')
	grep -q $file TODO.done && continue
	comment=$(echo "$line" | awk '{print substr($NF,length($NF))}')

	found=$(find . -name "$file")
	if [ -n "$found" ] ; then
		case "$comment" in
			/)	bcom="/* "
				ecom=""
				;;
			!)	bcom="! "
				ecom=""
				;;
			3)	bcom="! "	#crt0.s
				ecom=" !"
				;;
		esac
		mv "$found" "$found.bak"
		printf "$bcom""$line""$ecom""\n\n" > "$found"
		cat "$found.bak" >> "$found"
		rm -f "$found.bak"
	fi
done < Library.SC
