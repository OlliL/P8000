# '@[$]makewhatis

rm -f /tmp/whatis
cd /usr/man
onintr out
foreach i ( 1 2 3 4 5 6 7 M )
	cd man$i
	echo > $$.$i
	/usr/bin/getNAME *.* >> /tmp/whatis
	rm -f $$.$i
	cd ..
end
ed - /tmp/whatis << 'EOF'
g/\\-/s//-/
g/\\\*-/s//-/
g/\( [1-7M]\)	/s//\1 x	/
g/\( [1-7M]S\)	/s//\1 x	/
1,$s/^[^	]* \([1-7M][^ 	]*\) [^	]*	\([^-]*\)/\2(\1)	/
g/	 /s//	/g
w /tmp/whatis2
'EOF'
/usr/bin/expand -24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100 /tmp/whatis2 | sort >/usr/lib/whatis
/etc/chmog 0664 bin 0 /usr/lib/whatis

out:
rm -f /tmp/whatis
rm -f /tmp/whatis2
rm -f $$.$i
