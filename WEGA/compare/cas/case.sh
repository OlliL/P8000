# One case of the cas corpus (script mode of host/recon/stagecorpus.py): every line of the file "runs" is
#   <source> <options>
# and runs the program under test on the source in the work directory. The output of the run, its exit status and every file it
# leaves are the result. Two kinds of undefined contents are cut away the way the original leaves them:
#  - after an error (exit status other than 0) the files hold what happened to be in memory, and a warning behind an error depends on
#    leftover values: only the messages without the warnings count;
#  - the intermediate file (-i) holds addresses of the tables of the program: only its size counts.
while read -r src opts; do
	b=${src%.s}
	echo "== $src $opts"
	rm -f "$b.o"
	$PROG $opts -o "$b.o" "$src" >"$b.out" 2>"$b.err"
	rc=$?
	echo "rc=$rc"
	if [ $rc -ne 0 ]; then
		grep -v '^Warning' "$b.err"
		rm -f "$b.o" "$b".l* "$b".i
	else
		cat "$b.err"
	fi
	cat "$b.out"
	for f in "$b".i; do
		[ -f "$f" ] && { echo "$f: `wc -c <"$f"` bytes"; rm -f "$f"; }
	done
	rm -f "$b.out" "$b.err"
done <runs
