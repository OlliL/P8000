# the case of compare/cchain and compare/scchain (script mode of host/recon/stagecorpus.py): the driver PROG compiles (OPT: "" or -O) a C source
# of the tree, or compiles and links one of link/*.c (CASE lk_NAME); the passes are the originals under the emulator (the original driver) or
# the host programs (the host driver finds its cc.conf, CHAIN_CONF, next to itself). Compared: messages, exit status, the files afterwards.
umask 022
if [ -n "$CHAIN_CONF" ]; then
	for w in $PROG; do last=$w; done	# the driver is the last word (a prefix like strace may come before)
	printf '%s\n' "$CHAIN_CONF" > "`dirname "$last"`/$CHAIN_CONFNAME"
fi
case "$CASE" in
lk_*)	# two steps (the name of the temporary object, cc.<pid>.o, would be in the symbol table of a program linked from the source), and once from the source stripped
	$PROG -c $OPT $CASE.c 2>&1 && $PROG $OPT -o $CASE.out $CASE.o 2>&1 && $PROG -s $OPT -o $CASE.s.out $CASE.c 2>&1 ;;
*)	$PROG -c $OPT -I. $CASE.c 2>&1 ;;
esac
echo "exit=$?"
ls
