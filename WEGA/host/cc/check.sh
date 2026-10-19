#!/bin/sh
# Compare ./cc with the original /bin/cc (run in z8k-emu): both are run
# with -:n -:z so that nothing is executed; the command lines and messages
# on stderr, the exit code and the files created must be identical (the
# process id in the temporary names is normalized).
#   check.sh   (WEGA=dir with bin/ lib/; E=emulator)
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
T=$(mktemp -d); ok=0; bad=0
cd "$T" || exit 1
echo 'main(){}' > a.c; echo 'f(){}' > b.c; : > x.s; : > y.o; : > z.f
: > a.err; : > b.err; : > a.ls; : > b.ls; : > a.n; : > b.n
norm() { sed -e 's/cc\.-\{0,1\}[0-9][0-9]*/cc.PID/g'; }
# pass paths that exist in both worlds, so the access() tests agree
P="-TO/bin/ld -TP/bin/ld"
while IFS= read -r args; do
  [ -z "$args" ] && continue
  rm -f *.o
  $E -r "$R" /bin/cc -:n -:z $P $args 2>a.err; ra=$?
  ls > a.ls
  rm -f *.o
  "$HERE/cc" -:n -:z $P $args 2>b.err; rb=$?
  ls > b.ls
  norm < a.err > a.n; norm < b.err > b.n
  if [ $ra = $rb ] && cmp -s a.n b.n && cmp -s a.ls b.ls; then ok=$((ok+1))
  else bad=$((bad+1)); echo "DIFF: $args (exit $ra/$rb)"; diff a.n b.n | head -6; fi
done <<'EOF'
a.c
-c a.c
-S a.c
-Sl a.c
-O a.c
-O2 a.c
-O3 a.c
-Ol a.c
-Or a.c
-O2p a.c
-p a.c
-p -O a.c
-F a.c
-F -O2 a.c
-E a.c
-P a.c
-C -E a.c
-DX=1 -Ifoo -Ubar a.c
-D X -I foo a.c
-o prog a.c
-o prog a.c b.c
a.c b.c
-c a.c b.c
a.c x.s
x.s
-c x.s
-S x.s
a.c y.o -lm
a.c -lm -lx
-s -n a.c
-Ns a.c
-Nl a.c
-:d a.c
-:v a.c
-:r a.c
-:pXX a.c
-:1YY -:2ZZ -:OWW a.c
-:ooo -:aAA -:lLL a.c
-B a.c
-Bfoo/ a.c
-Bfoo/ -aO a.c
-Bfoo/ -ap1 a.c
-Tp/bin/x -T1/bin/y a.c
-Ta/bin/as a.c
-Tl/bin/ldx a.c
-TF/lib/c0.o a.c
-TP/bin/q -F a.c
z.f
-c z.f
-p z.f
a.c z.f
-:x a.c
-Tx a.c
-Z a.c
-NZ a.c
-ax a.c
-a a.c
-Bq -ax a.c
-o a.c
-o
nonexist.c
-c nonexist.c
x.s nonexist.s
-c a.c -o a.o
-O1 -O5 a.c
-Ox a.c
-Op a.c
-O2 -S a.c
-O2 -Sl a.c
-Sl -F a.c
-cS a.c
-Sc a.c
-E -c a.c
-P -S a.c
EOF
cd /; rm -rf "$T"; echo "ok=$ok bad=$bad"; [ $bad = 0 ]
