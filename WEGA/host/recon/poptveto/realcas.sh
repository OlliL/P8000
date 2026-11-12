#!/bin/sh
# realcas.sh ROOTNAME: the real build of src/cmd/cas (its Makefile) with sccvc of $S/o_ROOTNAME (or OLDROOT for "cur"); compares with /bin/cas
S=/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad
E=/home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu; W=/home/olivleh1/git/P8000/tmp/wega31_root
r=$1; root=$S/o_$r; [ "$r" = cur ] && root=/home/olivleh1/git/P8000/tmp/oldroot
B=$S/rc_$r; rm -rf $B; mkdir -p $B/src/cmd $B/src/lib $B/bin
cp -r /home/olivleh1/git/P8000/WEGA/src/cmd/cas $B/src/cmd/cas; cp /home/olivleh1/git/P8000/WEGA/src/lib/slibc-v11.a $B/src/lib/
rm -rf $B/src/cmd/cas/build/*.o $B/src/cmd/cas/build/cas
for t in $S/wbin/*; do cp $t $B/bin/; done
printf '#!/bin/sh\nexec %s -r %s /bin/sccvc "$@"\n' $E $root > $B/bin/sccvc
cd $B/src/cmd/cas && PATH=$B/bin:$PATH make -s build/cas MAKEARGS=-j8 > $B/make.log 2>&1
[ -f build/cas ] || { tail -5 $B/make.log; exit 1; }
cp build/cas $B/cas.unstripped; (cd build && $E -r $W /usr/bin/strip cas)
ls -l build/cas | awk '{print "size",$5}'
cmp -l build/cas $W/bin/cas 2>/dev/null | wc -l
