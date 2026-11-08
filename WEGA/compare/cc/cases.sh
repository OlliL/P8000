# the case of compare/cc and compare/scc (script mode of host/recon/stagecorpus.py): the driver PROG with the command lines below; nothing is executed
# (-:n -:z): compared are the command lines it builds, its messages, its exit status and the files afterwards (the process id in the names of the
# temporary files is normalized in progcfg.py).
umask 022
echo 'main(){}' > a.c; echo 'f(){}' > b.c; : > x.s; : > y.o; : > z.f
# pass paths that exist in both worlds, so the access() tests agree
P="-TO/bin/ld -TP/bin/ld"
while IFS= read -r args; do
	[ -z "$args" ] && continue
	rm -f *.o
	echo "\$ cc $args"
	$PROG -:n -:z $P $args 2>&1
	echo "exit=$?"
	ls
done <<'EOS'
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
EOS
