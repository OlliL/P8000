# cmd: WEGA system commands, in the V7 layout: a command with a single source
# file is a file here (ar.c, mv.c, size.c, ...); a command with several files, several versions or its own
# libc has its own subdirectory with its own Makefile (BOOT0, doscopy, pburn, standalone, dc, cc, cas,
# cc/cpp, cc/cparse, cc/codgen, cc/popt, ld, sld, verify, copydisk, format). Start-up variants of the commands live in
# ../lib/libc/nonseg/crt (crt0.s.in + crt0.list, made into crt0-<command>.s by mkcrt0.sh).
# cc/cc.c is the source of both drivers, cc and scc (scc is cc.c compiled with -DSCC).
#
#   make                       build everything (with the WEGA cc/scc/cas/as/ld/sld/gld/yacc)
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CAS = cas
CC  = cc
CCV0 = ccv0
CCV0P = ccv0p
CCV1 = ccv1
CCV1R = ccv1r
CCV1L = ccv1l
CCVM = ccvm
YACC = yacc
LD  = ld
STRIP = strip
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
CRT = ../lib/libc/nonseg/crt
# the libc of each single-file command (the one of its time, as in its own Makefile before)
LIBC_V1 = ../lib/libc-v1.a
LIBC_V2 = ../lib/libc-v2.a
LIBC_V3 = ../lib/libc-v3.a
LIBC_V4 = ../lib/libc-v4.a
LIBC_V15 = ../lib/libc-v15.a
LIBC_V6 = ../lib/libc-v6.a
LIBC_V10 = ../lib/libc-v10.a
LIBC_V11 = ../lib/libc-v11.a
LIBC_V7 = ../lib/libc-v7.a
LIBC_V9 = ../lib/libc-v9.a
TERMLIB = ../lib/libtermlib/build/nonseg/libtermlib.a
LIBC_V12 = ../lib/libc-v12.a
LIBC_V13 = ../lib/libc-v13.a
LIBPW = ../lib/libPW/build/nonseg/libPW.a

PROGS = ar

# single-file commands, each with its own compiler and its own libc (byte-identical with the original)
SIMPLE=		mv rm rmdir write size strip ranlib sum pr cpio clist cat echo cmp chmod chgrp chown pwd kill sleep tail uname tee du env nohup gets sort touch grep nl su mkdir find mkstr test tr nice od df time date dd regcmp who diff newgrp passwd mail echo2 printenv whereis dog gpasswd egrep nm file stty ls login

FILES=		getud putud bcmp bd dump dumpdir restor sync tar
CLEANFILES=	sync.o ar.o  getud.o  putud.o  bcmp.o  bd.o
SUBDIRS=	remote ps doscopy BOOT0 pburn standalone verify copydisk format cc cas cc/cpp cc/cparse cc/codgen cc/popt cc/gopt dc ed make ld sld text sccs cref spell lint ex me yacc more bc acct uucp tabset pub fse plzcg units news queuer problems screen lex

all: $(PROGS) $(FILES) $(SIMPLE) subdirs

# shipped: everything built with the shipped cc and the shipped libc (CC=cc, /lib/libc.a, /lib/slibc.a) and -DSHIPPED, which
# switches the sources to what the shipped libc and cc need (the old libc's readsym, sprintf value, swap_quad; the exit status
# of size). Without -DSHIPPED the bytes of the originals are rebuilt. The files of /lib are not made here: empty rules below.
SHIPFLAGS = -O -DSHIPPED
SHIPCC = CC=cc CCV0=cc CCV0P=cc CCV1=cc CCV1R=cc CCV1L=cc CCVM=cc CCVZ=cc CFLAGS="$(SHIPFLAGS)" LIBC_V1=/lib/libc.a LIBC_V2=/lib/libc.a LIBC_V3=/lib/libc.a LIBC_V4=/lib/libc.a LIBC_V6=/lib/libc.a LIBC_V10=/lib/libc.a LIBC_V11=/lib/libc.a LIBC_V7=/lib/libc.a LIBC_V9=/lib/libc.a LIBC_V12=/lib/libc.a LIBC_V13=/lib/libc.a LIBC_V15=/lib/libc.a LIBPW=/usr/lib/libPW.a TERMLIB=/usr/lib/libtermlib.a

shipped:
	$(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) $(PROGS) $(FILES) $(SIMPLE) $(SHIPCC) LIBC=/lib/libc.a
	cd ../lib/oldlibc/printf ; cp 0_5.c s1.c ; cc -O -c s1.c ; rm -f s1.c
	cd ../lib/oldlibc/doprnt ; $(CAS) -u -o 0_5.o 0_5.s
	for d in $(SUBDIRS) ; do \
		case $$d in \
		sld) X="SCC=scc LIBC=/lib/slibc.a" ;; \
		fse) X="CC=scc" ;; \
		ps) X="SCCVK=scc LIBC=/lib/slibc.a" ;; \
		cc/gopt) X="SCC=scc SLD=sld LIBC=/lib/slibc.a" ;; \
		cas) X="SCC=scc SLIBC=/lib/slibc.a SCCFLAGS=\"$(SHIPFLAGS)\"" ;; \
		cc/codgen|cc/popt) X='LIBC="../../../../../lib/oldlibc/printf/s1.o ../../../../../lib/oldlibc/doprnt/0_5.o /lib/libc.a"' ;; \
		cc/cpp|cc/cparse|cc|dc|ed|bc|ld) X="LIBC=/lib/libc.a" ;; \
		lex) X="LIBC=/lib/libc.a LIBY=/lib/liby.a" ;; \
		remote) X="LIBC_V14=" ;; \
		ex) X="CCV0A=cc TERMLIB=/usr/lib/libtermlib.a" ;; \
		*) X="" ;; \
		esac ; \
		(cd $$d ; eval $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all '$(SHIPCC)' $$X) ; \
	done

/lib/libc.a /lib/slibc.a /usr/lib/libtermlib.a:

subdirs:
	for d in $(SUBDIRS); do (cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all) ; done

getud: getud.c
	$(CC) -o $@ getud.c
	@echo "Patching getud with scratchfilename used in original cc (contains PID)"
	@dd if=getud of=getud1 bs=1 count=6975 >/dev/null 2>&1
	@echo -n '524.o' >>getud1
	@dd if=getud of=getud2 bs=1 skip=6980 >/dev/null 2>&1
	@cat getud2 >> getud1
	@mv getud1 getud
	@rm getud2

putud: putud.c
	$(CC) -o $@ putud.c
	@echo "Patching putud with scratchfilename used in original cc (contains PID)"
	@dd if=putud of=putud1 bs=1 count=6885 >/dev/null 2>&1
	@echo -n '330.o' >>putud1
	@dd if=putud of=putud2 bs=1 skip=6890 >/dev/null 2>&1
	@cat putud2 >> putud1
	@mv putud1 putud
	@rm putud2

# bcmp: a WEGA program (no System III source), separate I and D (-i), not stripped: the name of its object in the symbol table
# is the scratch file of the original cc, cc.211.o (the PID)
bcmp: bcmp.c
	$(CC) -i -O -o $@ bcmp.c
	@echo "Patching bcmp with scratchfilename used in original cc (contains PID)"
	@dd if=bcmp of=bcmp1 bs=1 count=6965 >/dev/null 2>&1
	@echo -n '211.o' >>bcmp1
	@dd if=bcmp of=bcmp2 bs=1 skip=6970 >/dev/null 2>&1
	@cat bcmp2 >> bcmp1
	@mv bcmp1 bcmp
	@rm bcmp2

# bd: a WEGA program (no System III source), not stripped: the name of its object in the symbol table is the scratch file of
# the original cc, cc.818.o (the PID)
bd: bd.c
	$(CC) -O -o $@ bd.c
	@echo "Patching bd with scratchfilename used in original cc (contains PID)"
	@dd if=bd of=bd1 bs=1 count=6539 >/dev/null 2>&1
	@echo -n '818.o' >>bd1
	@dd if=bd of=bd2 bs=1 skip=6544 >/dev/null 2>&1
	@cat bd2 >> bd1
	@mv bd1 bd
	@rm bd2

dump: dump.c
	$(CC) $(CFLAGS) -o $@ dump.c

dumpdir: dumpdir.c
	$(CC) $(CFLAGS) -o $@ dumpdir.c

restor: restor.c
	$(CC) $(CFLAGS) -o $@ restor.c

tar: tar.c
	$(CC) $(CFLAGS) -o $@ tar.c

# ccvm (CCVM): the cparse of the programs of 11/18/83 (OLD_D1S, host/OLDCC.md vm): ar, cmp, cpio, diff, du, grep,
# mail, nl, pr, who (and make and remote in their directories)
ar.o: ar.c
	$(CCVM) $(CFLAGS) -c ar.c

ar: ar.o crt0-ar.o
	$(LD) -e start -o $@ crt0-ar.o ar.o $(LIBC)

crt0-ar.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ar > $@

crt0-ar.o: crt0-ar.s
	$(CAS) -u -o $@ crt0-ar.s

crt0-sync.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh sync > $@

crt0-sync.o: crt0-sync.s
	$(CAS) -u -o $@ crt0-sync.s

sync.o: sync.c
sync: crt0-sync.o sync.o
	$(LD) -o $@ -e start crt0-sync.o sync.o -lc

# The single-file commands: each with its own compiler (as before in their own Makefiles), its own libc
# and its own start-up file crt0-NAME.o. -I../head/oldv7: the stdio.h of the time.
# mv: also cp and ln (hard links), byte-identical with the originals
mv.o: mv.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c mv.c
mv: crt0-mv.o mv.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-mv.o mv.o $(LIBC_V6)

rm.o: rm.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c rm.c
rm: crt0-rm.o rm.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-rm.o rm.o $(LIBC_V6)

rmdir.o: rmdir.c
	$(CCV1R) $(CFLAGS) -I../head/oldv7 -c rmdir.c
rmdir: crt0-rmdir.o rmdir.o $(LIBC_V10)
	$(LD) -e start -o $@ crt0-rmdir.o rmdir.o $(LIBC_V10)

write.o: write.c
	$(CCV1R) $(CFLAGS) -I../head/oldv7 -c write.c
write: crt0-write.o write.o $(LIBC_V10)
	$(LD) -e start -o $@ crt0-write.o write.o $(LIBC_V10)

size.o: size.c
	$(CCV1) $(CFLAGS) -c size.c
size: crt0-size.o size.o $(LIBC_V3)
	$(LD) -e start -o $@ crt0-size.o size.o $(LIBC_V3)

clist.o: clist.c
	$(CCV0P) $(CFLAGS) -I../head/oldv7 -c clist.c
clist: crt0-clist.o clist.o $(LIBC_V2)
	$(LD) -e start -o $@ crt0-clist.o clist.o $(LIBC_V2)

cat.o: cat.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c cat.c
cat: crt0-cat.o cat.o $(LIBC_V11)
	$(LD) -e start -o $@ crt0-cat.o cat.o $(LIBC_V11)

echo.o: echo.c
	$(CCV0) $(CFLAGS) -c echo.c
echo: crt0-echo.o echo.o $(LIBC_V11)
	$(LD) -e start -o $@ crt0-echo.o echo.o $(LIBC_V11)

cmp.o: cmp.c
	$(CCVM) $(CFLAGS) -c cmp.c
cmp: crt0-cmp.o cmp.o
	$(LD) -e start -o $@ crt0-cmp.o cmp.o -lc

chmod.o: chmod.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c chmod.c
chmod: crt0-chmod.o chmod.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-chmod.o chmod.o $(LIBC_V1)

chgrp.o: chgrp.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c chgrp.c
chgrp: crt0-chgrp.o chgrp.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-chgrp.o chgrp.o $(LIBC_V6)

chown.o: chown.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c chown.c
chown: crt0-chown.o chown.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-chown.o chown.o $(LIBC_V6)

pwd.o: pwd.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c pwd.c
pwd: crt0-pwd.o pwd.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-pwd.o pwd.o $(LIBC_V1)

# file: file.c of System III rewritten by WEGA (file.c 1.2 07/24/82: Z8000, PLZ, s.out); ccv0, libc-v1
file.o: file.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c file.c
file: crt0-file.o file.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-file.o file.o $(LIBC_V1)

kill.o: kill.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c kill.c
kill: crt0-kill.o kill.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-kill.o kill.o $(LIBC_V1)

sleep.o: sleep.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c sleep.c
sleep: crt0-sleep.o sleep.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-sleep.o sleep.o $(LIBC_V1)

tail.o: tail.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c tail.c
tail: crt0-tail.o tail.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-tail.o tail.o $(LIBC_V1)

uname.o: uname.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c uname.c
uname: crt0-uname.o uname.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-uname.o uname.o $(LIBC_V6)

# test: V7 test.c with the operators of System III (sh/test.c: -x -c -b -u -g -k, access)
test.o: test.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c test.c
test: crt0-test.o test.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-test.o test.o $(LIBC_V6)

# tr: System III tr.c with the usage message of WEGA (fprintf), the WEGA sccsid
tr.o: tr.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c tr.c
tr: crt0-tr.o tr.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-tr.o tr.o $(LIBC_V6)
# nice: V7 nice.c with exit(2) and the WEGA sccsid (libc-v4.a: fputs 0_2, execvp 0_2, sleep 0_3)
nice.o: nice.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c nice.c
nice: crt0-nice.o nice.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-nice.o nice.o $(LIBC_V4)
# od: System III od.c with the main of WEGA (hd by argv[0], the name in the message, base per option, exit(1)), putn of base in cput; partial: cput, putn (compiler)
od.o: od.c
	$(CCV1R) $(CFLAGS) -I../head/oldv7 -c od.c
od: crt0-od.o od.o $(LIBC_V3)
	$(LD) -e start -o $@ crt0-od.o od.o $(LIBC_V3)
# df: System III df.c as WEGA changed it: the mount table of Nmount entries (getnmount: nlist of /wega, /dev/kmem, calloc), free blocks counted from the super-block (no ustat), sync; partial: alloc, calloc
df.o: df.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c df.c
df: crt0-df.o df.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-df.o df.o $(LIBC_V6)
# login: System III login.c as WEGA changed it (the node name in the prompt, -name and a leading - for no motd, /bin/csh,
# HOME SHELL PATH LOGNAME TERM put into the environment of getty, motd by fgets, mail in /usr/spool/mail) and ttytype.c
# (the type from /etc/ttytype; getcl, not called, brings in termcap); ccv1, libc-v9, termcap of libtermlib.
# utscan: the lseek offset as -(long)(char *)sizeof(wtmp): the pointer cast keeps the constant from cparse's folding,
# so the original negates 20L at run time (compare/login/findings.json).
login.o: login.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c login.c
ttytype.o: ttytype.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c ttytype.c
login: crt0-login.o login.o ttytype.o $(TERMLIB) $(LIBC_V9)
	$(LD) -e start -o $@ crt0-login.o login.o ttytype.o $(TERMLIB) $(LIBC_V9)
# time: written from /bin/time (WEGA's own output); register p in front of the longs (its frame place, OLD_D1)
time.o: time.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c time.c
time: crt0-time.o time.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-time.o time.o $(LIBC_V4)
# date: System III date.c as WEGA changed it (-u, the usage message, ftime for the zone, asctime and printf for the output); register tzn in front of tb (its frame place, OLD_D1)
date.o: date.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c date.c
date: crt0-date.o date.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-date.o date.o $(LIBC_V6)
# dd: System III dd.c as WEGA changed it (files=, chkres: the message of reserv(1), the zero count message); ddname behind number (the place of the string), an unused label sync: (ibc = ibs in r2), libc-v13 (open 0_1)
dd.o: dd.c
	$(CCV1R) $(CFLAGS) -I../head/oldv7 -c dd.c
dd: crt0-dd.o dd.o $(LIBC_V13)
	$(LD) -e start -o $@ crt0-dd.o dd.o $(LIBC_V13)
# regcmp: System III regcmp.c (char %s[] = {, the WEGA sccsid), ccv1; the regcmp of libPW in it is the source of the
# libPW member (../lib/libPW/regcmp.c) built by ccv1l, the compiler of lex (host/OLDCC.md, v1l)
regcmp.o: regcmp.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c regcmp.c
regcmppw.o: ../lib/libPW/regcmp.c
	mkdir -p regcmppw.d ; cd regcmppw.d ; $(CCV1L) $(CFLAGS) -c ../../lib/libPW/regcmp.c ; mv regcmp.o ../regcmppw.o
regcmp: crt0-regcmp.o regcmp.o regcmppw.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-regcmp.o regcmp.o regcmppw.o $(LIBPW) $(LIBC_V4)
# who: System III who.c with -g (the group) and the WEGA messages, ccvm and libc
who.o: who.c
	$(CCVM) $(CFLAGS) -c who.c
who: crt0-who.o who.o
	$(LD) -e start -o $@ crt0-who.o who.o -lc
# diff: diff.c of Plexus (as System III) with the WEGA sccsid, ccvm (the frame words of prepare, equiv, main, stone: OLD_D1S) and libc, split I/D
diff.o: diff.c
	$(CCVM) $(CFLAGS) -c diff.c
diff: crt0-diff.o diff.o
	$(LD) -i -e start -o $@ crt0-diff.o diff.o -lc
# newgrp: V7 newgrp.c as WEGA changed it (no argument: the passwd group; root; stderr; done starts the shell as -name), the shipped cc, libc-v12.a
newgrp.o: newgrp.c
	$(CC) $(CFLAGS) -c newgrp.c
newgrp: crt0-newgrp.o newgrp.o $(LIBC_V12)
	$(LD) -e start -o $@ crt0-newgrp.o newgrp.o $(LIBC_V12)
# passwd: System III passwd.c with the lock of WEGA (lkdata/unlk of the passwd file), chmod 0644, a spare long in main (the frame); ccv1, libc-v4.a
passwd.o: passwd.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c passwd.c
passwd: crt0-passwd.o passwd.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-passwd.o passwd.o $(LIBC_V4)
# mail: System III mail.c as WEGA changed it (/usr/spool/mail, setret, ~ in getarg, setuid in system, getuid for chown; an unused <sys/dir.h> for the frame words of OLD_D1S), ccvm, libc-v12.a; rmail is a link
mail.o: mail.c
	$(CCVM) $(CFLAGS) -c mail.c
mail: crt0-mail.o mail.o $(LIBC_V12)
	$(LD) -e start -o $@ crt0-mail.o mail.o $(LIBC_V12)
# echo2: the echo of V7 on stderr (written from /bin/echo2), ccv0, libc-v1.a
echo2.o: echo2.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c echo2.c
echo2: crt0-echo2.o echo2.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-echo2.o echo2.o $(LIBC_V1)
# printenv: 2BSD printenv.c (Bill Joy) with the WEGA sccsid, ccv0, libc-v1.a
printenv.o: printenv.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c printenv.c
printenv: crt0-printenv.o printenv.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-printenv.o printenv.o $(LIBC_V1)
# whereis: 2.9BSD whereis.c as WEGA changed it, ccv0, libc-v1.a
whereis.o: whereis.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c whereis.c
whereis: crt0-whereis.o whereis.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-whereis.o whereis.o $(LIBC_V1)
# dog: written from /bin/dog (WEGA, no source), ccv1, libc-v6.a
dog.o: dog.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c dog.c
dog: crt0-dog.o dog.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-dog.o dog.o $(LIBC_V6)
# gpasswd: written from /bin/gpasswd (WEGA, after System III passwd.c), the shipped cc, libc-v12.a
gpasswd.o: gpasswd.c
	$(CC) $(CFLAGS) -c gpasswd.c
gpasswd: crt0-gpasswd.o gpasswd.o $(LIBC_V12)
	$(LD) -e start -o $@ crt0-gpasswd.o gpasswd.o $(LIBC_V12)




















tee.o: tee.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c tee.c
tee: crt0-tee.o tee.o
	$(LD) -e start -o $@ crt0-tee.o tee.o -lc

du.o: du.c
	$(CCVM) $(CFLAGS) -c du.c
du: crt0-du.o du.o
	$(LD) -e start -o $@ crt0-du.o du.o -lc

env.o: env.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c env.c
env: crt0-env.o env.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-env.o env.o $(LIBC_V1)

nohup.o: nohup.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c nohup.c
nohup: crt0-nohup.o nohup.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-nohup.o nohup.o $(LIBC_V1)

gets.o: gets.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c gets.c
gets: crt0-gets.o gets.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-gets.o gets.o $(LIBC_V1)

sort.o: sort.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c sort.c
sort: crt0-sort.o sort.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-sort.o sort.o $(LIBC_V6)

touch.o: touch.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c touch.c
touch: crt0-touch.o touch.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-touch.o touch.o $(LIBC_V6)

grep.o: grep.c
	$(CCVM) $(CFLAGS) -c grep.c
grep: crt0-grep.o grep.o
	$(LD) -e start -o $@ crt0-grep.o grep.o -lc

# egrep: egrep.y (System III), yacc (the yaccpar of WEGA), y.tab.c -> egrep.c; ccvm (egrep.y 2.2 11/18/83)
egrep.c: egrep.y
	$(YACC) egrep.y ; mv y.tab.c egrep.c
egrep.o: egrep.c
	$(CCVM) $(CFLAGS) -c egrep.c
egrep: crt0-egrep.o egrep.o
	$(LD) -e start -o $@ crt0-egrep.o egrep.o -lc

# nm: nm.c of System III rewritten by WEGA for s.out (nm.c 2.6 11/18/83); ccvm
nm.o: nm.c
	$(CCVM) $(CFLAGS) -c nm.c
nm: crt0-nm.o nm.o
	$(LD) -e start -o $@ crt0-nm.o nm.o -lc

# stty: stty.c of System III with the WEGA speeds (stty.c 2.3 12/21/83), the shipped cc, libc-v15 (its doprnt sets two
# more locals to 0)
stty.o: stty.c
	$(CC) $(CFLAGS) -c stty.c
stty: crt0-stty.o stty.o $(LIBC_V15)
	$(LD) -e start -o $@ crt0-stty.o stty.o $(LIBC_V15)

# ls: the ls of 2BSD/Plexus (bls.c: columns, -C -F -R -m -x) with the WEGA options -D (directories only) -g -o, the names by
# getpwuid/getgrgid, -R collected in pentry, and its own printf (lsprintf) and _doprnt (integers and strings, through
# pputchar) (ls.c 1.7 02/23/83); ccv1, libc-v6
ls.o: ls.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c ls.c
ls: crt0-ls.o ls.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-ls.o ls.o $(LIBC_V6)

nl.o: nl.c
	$(CCVM) $(CFLAGS) -c nl.c
nl: crt0-nl.o nl.o
	$(LD) -e start -o $@ crt0-nl.o nl.o -lc

su.o: su.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c su.c
su: crt0-su.o su.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-su.o su.o $(LIBC_V4)

mkdir.o: mkdir.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c mkdir.c
mkdir: crt0-mkdir.o mkdir.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-mkdir.o mkdir.o $(LIBC_V1)

# find: the stdio.h of the tree (getc with --_cnt < 0 first), not oldv7
find.o: find.c
	$(CCV0P) $(CFLAGS) -c find.c
find: crt0-find.o find.o $(LIBC_V7)
	$(LD) -e start -o $@ crt0-find.o find.o $(LIBC_V7)

mkstr.o: mkstr.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c mkstr.c
mkstr: crt0-mkstr.o mkstr.o $(LIBC_V11)
	$(LD) -e start -o $@ crt0-mkstr.o mkstr.o $(LIBC_V11)

strip.o: strip.c
	$(CCV1) $(CFLAGS) -c strip.c
strip: crt0-strip.o strip.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-strip.o strip.o $(LIBC_V4)

ranlib.o: ranlib.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c ranlib.c
ranlib: crt0-ranlib.o ranlib.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-ranlib.o ranlib.o $(LIBC_V1)

sum.o: sum.c
	$(CCV0) $(CFLAGS) -I../head/oldv7 -c sum.c
sum: crt0-sum.o sum.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-sum.o sum.o $(LIBC_V1)

pr.o: pr.c
	$(CCVM) $(CFLAGS) -c pr.c
pr: crt0-pr.o pr.o
	$(LD) -e start -o $@ crt0-pr.o pr.o -lc

cpio.o: cpio.c
	$(CCVM) $(CFLAGS) -c cpio.c
cpio: crt0-cpio.o cpio.o
	$(LD) -e start -o $@ crt0-cpio.o cpio.o -lc

crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s crt0-sum.s crt0-pr.s crt0-cpio.s crt0-clist.s crt0-cat.s crt0-echo.s crt0-cmp.s crt0-chmod.s crt0-chgrp.s crt0-chown.s crt0-pwd.s crt0-kill.s crt0-sleep.s crt0-tail.s crt0-uname.s crt0-tee.s crt0-du.s crt0-env.s crt0-nohup.s crt0-gets.s crt0-sort.s crt0-touch.s crt0-grep.s crt0-nl.s crt0-su.s crt0-mkdir.s crt0-find.s crt0-mkstr.s crt0-test.s crt0-tr.s crt0-nice.s crt0-od.s crt0-df.s crt0-time.s crt0-date.s crt0-dd.s crt0-regcmp.s crt0-who.s crt0-diff.s crt0-newgrp.s crt0-passwd.s crt0-mail.s crt0-echo2.s crt0-printenv.s crt0-whereis.s crt0-dog.s crt0-gpasswd.s crt0-egrep.s crt0-nm.s crt0-file.s crt0-stty.s crt0-ls.s crt0-login.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh `echo $@ | sed -e 's/^crt0-//' -e 's/\.s$$//'` > $@

crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o crt0-sum.o crt0-pr.o crt0-cpio.o crt0-clist.o crt0-cat.o crt0-echo.o crt0-cmp.o crt0-chmod.o crt0-chgrp.o crt0-chown.o crt0-pwd.o crt0-kill.o crt0-sleep.o crt0-tail.o crt0-uname.o crt0-tee.o crt0-du.o crt0-env.o crt0-nohup.o crt0-gets.o crt0-sort.o crt0-touch.o crt0-grep.o crt0-nl.o crt0-su.o crt0-mkdir.o crt0-find.o crt0-mkstr.o crt0-test.o crt0-tr.o crt0-nice.o crt0-od.o crt0-df.o crt0-time.o crt0-date.o crt0-dd.o crt0-regcmp.o crt0-who.o crt0-diff.o crt0-newgrp.o crt0-passwd.o crt0-mail.o crt0-echo2.o crt0-printenv.o crt0-whereis.o crt0-dog.o crt0-gpasswd.o crt0-egrep.o crt0-nm.o crt0-file.o crt0-stty.o crt0-ls.o crt0-login.o: crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s crt0-sum.s crt0-pr.s crt0-cpio.s crt0-clist.s crt0-cat.s crt0-echo.s crt0-cmp.s crt0-chmod.s crt0-chgrp.s crt0-chown.s crt0-pwd.s crt0-kill.s crt0-sleep.s crt0-tail.s crt0-uname.s crt0-tee.s crt0-du.s crt0-env.s crt0-nohup.s crt0-gets.s crt0-sort.s crt0-touch.s crt0-grep.s crt0-nl.s crt0-su.s crt0-mkdir.s crt0-find.s crt0-mkstr.s crt0-test.s crt0-tr.s crt0-nice.s crt0-od.s crt0-df.s crt0-time.s crt0-date.s crt0-dd.s crt0-regcmp.s crt0-who.s crt0-diff.s crt0-newgrp.s crt0-passwd.s crt0-mail.s crt0-echo2.s crt0-printenv.s crt0-whereis.s crt0-dog.s crt0-gpasswd.s crt0-egrep.s crt0-nm.s crt0-file.s crt0-stty.s crt0-ls.s crt0-login.s
	$(CAS) -u -o $@ `echo $@ | sed -e 's/\.o$$/.s/'`

install: $(PROGS) $(FILES) $(SIMPLE)
	for file in ar getud putud bcmp bd tar sync ; do cp $$file $(PREFIX)/bin/$$file ; done
	for file in chkdiff uuclear whodo ; do cp $$file.sh $(PREFIX)/bin/$$file ; chmod 755 $(PREFIX)/bin/$$file ; done
	for file in ar tar sync ; do $(STRIP) $(PREFIX)/bin/$$file ; done
	for file in dump dumpdir restor ; do cp $$file $(PREFIX)/etc/$$file ; $(STRIP) $(PREFIX)/etc/$$file ; done
	cp rm $(PREFIX)/bin/rm ; $(STRIP) $(PREFIX)/bin/rm
	cp rmdir $(PREFIX)/bin/rmdir ; $(STRIP) $(PREFIX)/bin/rmdir
	cp write $(PREFIX)/bin/write ; $(STRIP) $(PREFIX)/bin/write
	cp size $(PREFIX)/bin/size ; $(STRIP) $(PREFIX)/bin/size
	cp strip $(PREFIX)/usr/bin/strip ; $(STRIP) $(PREFIX)/usr/bin/strip
	cp ranlib $(PREFIX)/usr/bin/ranlib ; $(STRIP) $(PREFIX)/usr/bin/ranlib
	cp sum $(PREFIX)/bin/sum ; $(STRIP) $(PREFIX)/bin/sum
	cp test $(PREFIX)/bin/test ; $(STRIP) $(PREFIX)/bin/test
	cp gpasswd $(PREFIX)/bin/gpasswd ; $(STRIP) $(PREFIX)/bin/gpasswd
	cp dog $(PREFIX)/bin/dog ; $(STRIP) $(PREFIX)/bin/dog
	cp whereis $(PREFIX)/bin/whereis ; $(STRIP) $(PREFIX)/bin/whereis
	cp printenv $(PREFIX)/bin/printenv ; $(STRIP) $(PREFIX)/bin/printenv
	cp echo2 $(PREFIX)/bin/echo2 ; $(STRIP) $(PREFIX)/bin/echo2
	cp mail $(PREFIX)/bin/mail ; $(STRIP) $(PREFIX)/bin/mail ; rm -f $(PREFIX)/bin/rmail ; ln $(PREFIX)/bin/mail $(PREFIX)/bin/rmail
	cp passwd $(PREFIX)/bin/passwd ; $(STRIP) $(PREFIX)/bin/passwd
	cp newgrp $(PREFIX)/bin/newgrp ; $(STRIP) $(PREFIX)/bin/newgrp
	cp diff $(PREFIX)/bin/diff ; $(STRIP) $(PREFIX)/bin/diff
	cp who $(PREFIX)/bin/who ; $(STRIP) $(PREFIX)/bin/who
	cp regcmp $(PREFIX)/bin/regcmp ; $(STRIP) $(PREFIX)/bin/regcmp
	cp dd $(PREFIX)/bin/dd ; $(STRIP) $(PREFIX)/bin/dd
	cp date $(PREFIX)/bin/date ; $(STRIP) $(PREFIX)/bin/date
	cp time $(PREFIX)/bin/time ; $(STRIP) $(PREFIX)/bin/time
	cp df $(PREFIX)/bin/df ; $(STRIP) $(PREFIX)/bin/df
	cp od $(PREFIX)/bin/od ; $(STRIP) $(PREFIX)/bin/od ; rm -f $(PREFIX)/bin/hd ; ln $(PREFIX)/bin/od $(PREFIX)/bin/hd
	cp nice $(PREFIX)/bin/nice ; $(STRIP) $(PREFIX)/bin/nice
	cp tr $(PREFIX)/bin/tr ; $(STRIP) $(PREFIX)/bin/tr
	cp pr $(PREFIX)/bin/pr ; $(STRIP) $(PREFIX)/bin/pr
	cp cpio $(PREFIX)/usr/bin/cpio ; $(STRIP) $(PREFIX)/usr/bin/cpio
	mkdir -p $(PREFIX)/lib
	cp clist $(PREFIX)/lib/clist ; $(STRIP) $(PREFIX)/lib/clist
	cp mv $(PREFIX)/bin/mv ; $(STRIP) $(PREFIX)/bin/mv
	cp cat $(PREFIX)/bin/cat ; $(STRIP) $(PREFIX)/bin/cat
	cp echo $(PREFIX)/bin/echo ; $(STRIP) $(PREFIX)/bin/echo
	cp cmp $(PREFIX)/bin/cmp ; $(STRIP) $(PREFIX)/bin/cmp
	cp chmod $(PREFIX)/bin/chmod ; $(STRIP) $(PREFIX)/bin/chmod
	cp chgrp $(PREFIX)/bin/chgrp ; $(STRIP) $(PREFIX)/bin/chgrp
	cp chown $(PREFIX)/bin/chown ; $(STRIP) $(PREFIX)/bin/chown
	cp pwd $(PREFIX)/bin/pwd ; $(STRIP) $(PREFIX)/bin/pwd
	cp kill $(PREFIX)/bin/kill ; $(STRIP) $(PREFIX)/bin/kill
	cp sleep $(PREFIX)/bin/sleep ; $(STRIP) $(PREFIX)/bin/sleep
	cp tail $(PREFIX)/bin/tail ; $(STRIP) $(PREFIX)/bin/tail
	cp uname $(PREFIX)/bin/uname ; $(STRIP) $(PREFIX)/bin/uname
	cp tee $(PREFIX)/bin/tee ; $(STRIP) $(PREFIX)/bin/tee
	cp du $(PREFIX)/bin/du ; $(STRIP) $(PREFIX)/bin/du
	cp env $(PREFIX)/bin/env ; $(STRIP) $(PREFIX)/bin/env
	cp nohup $(PREFIX)/bin/nohup ; $(STRIP) $(PREFIX)/bin/nohup
	cp gets $(PREFIX)/bin/gets ; $(STRIP) $(PREFIX)/bin/gets
	cp sort $(PREFIX)/bin/sort ; $(STRIP) $(PREFIX)/bin/sort
	cp touch $(PREFIX)/bin/touch ; $(STRIP) $(PREFIX)/bin/touch
	cp grep $(PREFIX)/bin/grep ; $(STRIP) $(PREFIX)/bin/grep
	cp egrep $(PREFIX)/bin/egrep ; $(STRIP) $(PREFIX)/bin/egrep
	cp nm $(PREFIX)/bin/nm ; $(STRIP) $(PREFIX)/bin/nm
	cp file $(PREFIX)/bin/file ; $(STRIP) $(PREFIX)/bin/file
	cp stty $(PREFIX)/bin/stty ; $(STRIP) $(PREFIX)/bin/stty
	cp ls $(PREFIX)/bin/ls ; $(STRIP) $(PREFIX)/bin/ls
	cp login $(PREFIX)/bin/login ; $(STRIP) $(PREFIX)/bin/login ; rm -f $(PREFIX)/etc/login ; ln $(PREFIX)/bin/login $(PREFIX)/etc/login
	cp nl $(PREFIX)/bin/nl ; $(STRIP) $(PREFIX)/bin/nl
	cp su $(PREFIX)/bin/su ; $(STRIP) $(PREFIX)/bin/su ; chmod 4751 $(PREFIX)/bin/su
	cp mkdir $(PREFIX)/bin/mkdir ; $(STRIP) $(PREFIX)/bin/mkdir ; chmod 4751 $(PREFIX)/bin/mkdir
	cp find $(PREFIX)/bin/find ; $(STRIP) $(PREFIX)/bin/find
	cp mkstr $(PREFIX)/usr/bin/mkstr ; $(STRIP) $(PREFIX)/usr/bin/mkstr
	rm -f $(PREFIX)/bin/cp $(PREFIX)/bin/ln
	ln $(PREFIX)/bin/mv $(PREFIX)/bin/cp
	ln $(PREFIX)/bin/mv $(PREFIX)/bin/ln
	# the scripts of /etc (NAME.sh, as in System III), with the modes of the originals; mfs=umfs and makelost+found=mlf are links
	cp adduser.sh $(PREFIX)/etc/adduser ; chmod 700 $(PREFIX)/etc/adduser
	cp down.sh $(PREFIX)/etc/down ; chmod 700 $(PREFIX)/etc/down
	cp halt.sh $(PREFIX)/etc/halt ; chmod 700 $(PREFIX)/etc/halt
	cp install.sh $(PREFIX)/etc/install ; chmod 750 $(PREFIX)/etc/install
	cp killall.sh $(PREFIX)/etc/killall ; chmod 744 $(PREFIX)/etc/killall
	cp makelost+found.sh $(PREFIX)/etc/makelost+found ; chmod 700 $(PREFIX)/etc/makelost+found
	cp makewhatis.sh $(PREFIX)/etc/makewhatis ; chmod 700 $(PREFIX)/etc/makewhatis
	cp man.dbmgr.sh $(PREFIX)/etc/man.dbmgr ; chmod 700 $(PREFIX)/etc/man.dbmgr
	cp mfs.sh $(PREFIX)/etc/mfs ; chmod 750 $(PREFIX)/etc/mfs
	cp mvdir.sh $(PREFIX)/etc/mvdir ; chmod 700 $(PREFIX)/etc/mvdir
	cp new.install.sh $(PREFIX)/etc/new.install ; chmod 700 $(PREFIX)/etc/new.install
	cp rmuser.sh $(PREFIX)/etc/rmuser ; chmod 700 $(PREFIX)/etc/rmuser
	cp sysgen.sh $(PREFIX)/etc/sysgen ; chmod 751 $(PREFIX)/etc/sysgen
	cp UGETT1.sh $(PREFIX)/etc/UGETT1 ; chmod 700 $(PREFIX)/etc/UGETT1
	mkdir -p $(PREFIX)/usr/bin
	cp basename.sh $(PREFIX)/usr/bin/basename ; chmod 755 $(PREFIX)/usr/bin/basename
	cp calendar.sh $(PREFIX)/usr/bin/calendar ; chmod 755 $(PREFIX)/usr/bin/calendar
	cp cxref.sh $(PREFIX)/usr/bin/cxref ; chmod 755 $(PREFIX)/usr/bin/cxref
	cp diff3.sh $(PREFIX)/usr/bin/diff3 ; chmod 755 $(PREFIX)/usr/bin/diff3
	cp diffmk.sh $(PREFIX)/usr/bin/diffmk ; chmod 755 $(PREFIX)/usr/bin/diffmk
	cp dircmp.sh $(PREFIX)/usr/bin/dircmp ; chmod 755 $(PREFIX)/usr/bin/dircmp
	cp dirname.sh $(PREFIX)/usr/bin/dirname ; chmod 755 $(PREFIX)/usr/bin/dirname
	cp false.sh $(PREFIX)/usr/bin/false ; chmod 755 $(PREFIX)/usr/bin/false
	cp lint.sh $(PREFIX)/usr/bin/lint ; chmod 755 $(PREFIX)/usr/bin/lint
	cp lorder.sh $(PREFIX)/usr/bin/lorder ; chmod 755 $(PREFIX)/usr/bin/lorder
	cp mkmenu.sh $(PREFIX)/usr/bin/mkmenu ; chmod 755 $(PREFIX)/usr/bin/mkmenu
	cp mm.sh $(PREFIX)/usr/bin/mm ; chmod 755 $(PREFIX)/usr/bin/mm
	cp mmt.sh $(PREFIX)/usr/bin/mmt ; chmod 755 $(PREFIX)/usr/bin/mmt
	cp sccsdiff.sh $(PREFIX)/usr/bin/sccsdiff ; chmod 755 $(PREFIX)/usr/bin/sccsdiff
	cp spell.sh $(PREFIX)/usr/bin/spell ; chmod 755 $(PREFIX)/usr/bin/spell
	cp strprint.sh $(PREFIX)/usr/bin/strprint ; chmod 755 $(PREFIX)/usr/bin/strprint
	cp true.sh $(PREFIX)/usr/bin/true ; chmod 755 $(PREFIX)/usr/bin/true
	rm -f $(PREFIX)/usr/bin/mvt
	ln $(PREFIX)/usr/bin/mmt $(PREFIX)/usr/bin/mvt
	mkdir -p $(PREFIX)/usr/lib
	cp crontab.sh $(PREFIX)/usr/lib/crontab ; chmod 644 $(PREFIX)/usr/lib/crontab
	rm -f $(PREFIX)/etc/umfs $(PREFIX)/etc/mlf
	ln $(PREFIX)/etc/mfs $(PREFIX)/etc/umfs
	ln $(PREFIX)/etc/makelost+found $(PREFIX)/etc/mlf
	for d in $(SUBDIRS) ; do (cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) install PREFIX=$(PREFIX)) ; done

clean:
	rm -f $(PROGS) $(FILES) $(CLEANFILES) $(SIMPLE) regcmppw.o
	rm -rf regcmppw.d
	rm -f egrep.o egrep.c nm.o file.o stty.o ls.o login.o ttytype.o
	rm -f mv.o rm.o rmdir.o write.o size.o strip.o ranlib.o sum.o pr.o cpio.o clist.o cat.o echo.o cmp.o chmod.o chgrp.o chown.o pwd.o kill.o sleep.o tail.o uname.o tee.o du.o env.o nohup.o gets.o sort.o touch.o grep.o nl.o su.o mkdir.o find.o mkstr.o
	rm -f crt0-ar.o crt0-sync.o crt0-ar.s crt0-sync.s
	rm -f crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o crt0-sum.o crt0-pr.o crt0-cpio.o crt0-clist.o crt0-cat.o crt0-echo.o crt0-cmp.o crt0-chmod.o crt0-chgrp.o crt0-chown.o crt0-pwd.o crt0-kill.o crt0-sleep.o crt0-tail.o crt0-uname.o crt0-tee.o crt0-du.o crt0-env.o crt0-nohup.o crt0-gets.o crt0-sort.o crt0-touch.o crt0-grep.o crt0-nl.o crt0-su.o crt0-mkdir.o crt0-find.o crt0-mkstr.o
	rm -f crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s crt0-sum.s crt0-pr.s crt0-cpio.s crt0-clist.s crt0-cat.s crt0-echo.s crt0-cmp.s crt0-chmod.s crt0-chgrp.s crt0-chown.s crt0-pwd.s crt0-kill.s crt0-sleep.s crt0-tail.s crt0-uname.s crt0-tee.s crt0-du.s crt0-env.s crt0-nohup.s crt0-gets.s crt0-sort.s crt0-touch.s crt0-grep.s crt0-nl.s crt0-su.s crt0-mkdir.s crt0-find.s crt0-mkstr.s
	for d in $(SUBDIRS) ; do (cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) clean) ; done

.PHONY: all subdirs clean install
