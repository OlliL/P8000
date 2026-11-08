# one case of compare/{ranlib,strip,size}: the program PROG with the arguments of the file .args, in a copy of the corpus tree.
# ranlib stores the time of the run: the date of the first member of the archives is cleared afterwards.
umask 022
ARGS=`cat .args`
eval "$PROG $ARGS"
echo "exit=$?"
case "$CASE" in
ranlib\ *) python3 - <<'PY'
import os
for d, dirs, names in os.walk('.'):
    for f in names:
        if f.endswith('.a'):
            p = os.path.join(d, f)
            b = bytearray(open(p, 'rb').read())
            if b[:2] == b'\xff\x65' and b[2:11] == b'__.SYMDEF':
                b[16:20] = b'\0\0\0\0'
                open(p, 'wb').write(b)
PY
;;
esac
