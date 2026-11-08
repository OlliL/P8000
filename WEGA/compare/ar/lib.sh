# the case "libc.a" or "slibc.a" of progcfg.py: the table of contents of a library of the system
echo "\$ ar tv $CASE"; $PROG tv $CASE; echo "exit=$?"
