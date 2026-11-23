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
LIBC_V6 = ../lib/libc-v6.a
LIBC_V10 = ../lib/libc-v10.a
LIBC_V11 = ../lib/libc-v11.a
LIBC_V7 = ../lib/libc-v7.a

PROGS = ar

# single-file commands, each with its own compiler and its own libc (byte-identical with the original)
SIMPLE=		mv rm rmdir write size strip ranlib sum pr cpio clist cat echo cmp chmod chgrp chown pwd kill sleep tail uname tee du env nohup gets sort touch grep nl su mkdir find mkstr

FILES=		getud putud bcmp bd dump dumpdir restor sync tar
CLEANFILES=	sync.o ar.o  getud.o  putud.o  bcmp.o  bd.o
SUBDIRS=	doscopy BOOT0 pburn standalone verify copydisk format cc cas cc/cpp cc/cparse cc/codgen cc/popt cc/gopt dc ld sld text sccs cref spell lint ex me yacc more bc acct uucp tabset pub fse plzcg units news queuer problems screen

all: $(PROGS) $(FILES) $(SIMPLE) subdirs

# shipped: everything built with the shipped cc and the shipped libc (CC=cc, /lib/libc.a, /lib/slibc.a) and -DSHIPPED, which
# switches the sources to what the shipped libc and cc need (the old libc's readsym, sprintf value, swap_quad; the exit status
# of size). Without -DSHIPPED the bytes of the originals are rebuilt. The files of /lib are not made here: empty rules below.
SHIPFLAGS = -O -DSHIPPED
SHIPCC = CC=cc CCV0=cc CCV0P=cc CCV1=cc CCV1R=cc CFLAGS="$(SHIPFLAGS)" LIBC_V1=/lib/libc.a LIBC_V2=/lib/libc.a LIBC_V3=/lib/libc.a LIBC_V4=/lib/libc.a LIBC_V6=/lib/libc.a LIBC_V10=/lib/libc.a LIBC_V11=/lib/libc.a LIBC_V7=/lib/libc.a

shipped:
	$(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) $(PROGS) $(FILES) $(SIMPLE) $(SHIPCC) LIBC=/lib/libc.a
	cd ../lib/oldlibc/printf ; cp 0_5.c s1.c ; cc -O -c s1.c ; rm -f s1.c
	cd ../lib/oldlibc/doprnt ; $(CAS) -u -o 0_5.o 0_5.s
	for d in $(SUBDIRS) ; do \
		case $$d in \
		sld) X="SCC=scc LIBC=/lib/slibc.a" ;; \
		fse) X="CC=scc" ;; \
		cc/gopt) X="SCC=scc SLD=sld LIBC=/lib/slibc.a" ;; \
		cas) X="SCC=scc SLIBC=/lib/slibc.a SCCFLAGS=\"$(SHIPFLAGS)\"" ;; \
		cc/codgen|cc/popt) X='LIBC="../../../../../lib/oldlibc/printf/s1.o ../../../../../lib/oldlibc/doprnt/0_5.o /lib/libc.a"' ;; \
		cc/cpp|cc/cparse|cc|dc|bc|ld) X="LIBC=/lib/libc.a" ;; \
		*) X="" ;; \
		esac ; \
		(cd $$d ; eval $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all '$(SHIPCC)' $$X) ; \
	done

/lib/libc.a /lib/slibc.a:

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

ar.o: ar.c
	$(CC) $(CFLAGS) -c ar.c

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
	$(CC) $(CFLAGS) -c cmp.c
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

tee.o: tee.c
	$(CCV1) $(CFLAGS) -I../head/oldv7 -c tee.c
tee: crt0-tee.o tee.o
	$(LD) -e start -o $@ crt0-tee.o tee.o -lc

du.o: du.c
	$(CC) $(CFLAGS) -c du.c
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
	$(CC) $(CFLAGS) -c grep.c
grep: crt0-grep.o grep.o
	$(LD) -e start -o $@ crt0-grep.o grep.o -lc

nl.o: nl.c
	$(CC) $(CFLAGS) -c nl.c
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
	$(CC) $(CFLAGS) -c pr.c
pr: crt0-pr.o pr.o
	$(LD) -e start -o $@ crt0-pr.o pr.o -lc

cpio.o: cpio.c
	$(CC) $(CFLAGS) -c cpio.c
cpio: crt0-cpio.o cpio.o
	$(LD) -e start -o $@ crt0-cpio.o cpio.o -lc

crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s crt0-sum.s crt0-pr.s crt0-cpio.s crt0-clist.s crt0-cat.s crt0-echo.s crt0-cmp.s crt0-chmod.s crt0-chgrp.s crt0-chown.s crt0-pwd.s crt0-kill.s crt0-sleep.s crt0-tail.s crt0-uname.s crt0-tee.s crt0-du.s crt0-env.s crt0-nohup.s crt0-gets.s crt0-sort.s crt0-touch.s crt0-grep.s crt0-nl.s crt0-su.s crt0-mkdir.s crt0-find.s crt0-mkstr.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh `echo $@ | sed -e 's/^crt0-//' -e 's/\.s$$//'` > $@

crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o crt0-sum.o crt0-pr.o crt0-cpio.o crt0-clist.o crt0-cat.o crt0-echo.o crt0-cmp.o crt0-chmod.o crt0-chgrp.o crt0-chown.o crt0-pwd.o crt0-kill.o crt0-sleep.o crt0-tail.o crt0-uname.o crt0-tee.o crt0-du.o crt0-env.o crt0-nohup.o crt0-gets.o crt0-sort.o crt0-touch.o crt0-grep.o crt0-nl.o crt0-su.o crt0-mkdir.o crt0-find.o crt0-mkstr.o: crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s crt0-sum.s crt0-pr.s crt0-cpio.s crt0-clist.s crt0-cat.s crt0-echo.s crt0-cmp.s crt0-chmod.s crt0-chgrp.s crt0-chown.s crt0-pwd.s crt0-kill.s crt0-sleep.s crt0-tail.s crt0-uname.s crt0-tee.s crt0-du.s crt0-env.s crt0-nohup.s crt0-gets.s crt0-sort.s crt0-touch.s crt0-grep.s crt0-nl.s crt0-su.s crt0-mkdir.s crt0-find.s crt0-mkstr.s
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
	rm -f $(PROGS) $(FILES) $(CLEANFILES) $(SIMPLE)
	rm -f mv.o rm.o rmdir.o write.o size.o strip.o ranlib.o sum.o pr.o cpio.o clist.o cat.o echo.o cmp.o chmod.o chgrp.o chown.o pwd.o kill.o sleep.o tail.o uname.o tee.o du.o env.o nohup.o gets.o sort.o touch.o grep.o nl.o su.o mkdir.o find.o mkstr.o
	rm -f crt0-ar.o crt0-sync.o crt0-ar.s crt0-sync.s
	rm -f crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o crt0-sum.o crt0-pr.o crt0-cpio.o crt0-clist.o crt0-cat.o crt0-echo.o crt0-cmp.o crt0-chmod.o crt0-chgrp.o crt0-chown.o crt0-pwd.o crt0-kill.o crt0-sleep.o crt0-tail.o crt0-uname.o crt0-tee.o crt0-du.o crt0-env.o crt0-nohup.o crt0-gets.o crt0-sort.o crt0-touch.o crt0-grep.o crt0-nl.o crt0-su.o crt0-mkdir.o crt0-find.o crt0-mkstr.o
	rm -f crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s crt0-sum.s crt0-pr.s crt0-cpio.s crt0-clist.s crt0-cat.s crt0-echo.s crt0-cmp.s crt0-chmod.s crt0-chgrp.s crt0-chown.s crt0-pwd.s crt0-kill.s crt0-sleep.s crt0-tail.s crt0-uname.s crt0-tee.s crt0-du.s crt0-env.s crt0-nohup.s crt0-gets.s crt0-sort.s crt0-touch.s crt0-grep.s crt0-nl.s crt0-su.s crt0-mkdir.s crt0-find.s crt0-mkstr.s
	for d in $(SUBDIRS) ; do (cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) clean) ; done

.PHONY: all subdirs clean install
