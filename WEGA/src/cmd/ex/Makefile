# ex: ex/vi 2.13 (Berkeley, from the Plexus tree cmd/plx/ex) with the edits of WEGA (compare/ex/findings.json); ex_subr.c has
# "File not found" for the error number 2, as the WEGA root has it.
#
# The build follows vi.mk of Plexus, with WEGA's step between the compiler and cas (shrink.sh: the jp stub tables and the @fp
# operands). For each source, in the order of the objects: mkstr takes the strings of the error messages out (into
# ex2.13strings, made first over all sources, so the offsets are those of one run in this order), the preprocessed source
# goes through xstr (the shared strings, collected in the file strings, in this order), the compiler writes x.s, shrink.sh
# changes it, cas makes the object. Then xstr writes xs.c with the strings (strings.o). mkstr (../mkstr.c) and xstr
# (../xstr.c) are programs of the WEGA system that make runs itself: RUN is "wrun" under the emulator, empty on WEGA.
# printf.c is WEGA's printf (printf and doprnt of the libc of the time printing by putchar), not pprintf.c of Plexus.
#
#   make                       ex (not installed yet) and ex2.13strings
#   make install PREFIX=dir    install dir/lib/ex2.13strings

CC = cc
CCV0 = ccv0
CAS = cas
LD = ld
PREFIX = /
VERSION = 2.13
OPTIONS = -DZCMD -DZZCMD -DRDONLY -DWRAPMGN -DOPENCODE -DLISPCODE -DTAGSCODE -DCRNL -DSOFTTABS -DCHDIR -DBEEHIVE -DUCVISUAL
CFLAGS = -O
CRT = ../../lib/libc/nonseg/crt
TERMLIB = ../../lib/libtermlib/build/nonseg/libtermlib.a
LIBC_V1 = ../../lib/libc-v1.a
SRCS = ex ex_addr ex_cmds ex_cmds2 ex_cmdsub ex_data ex_get ex_io ex_put ex_re ex_set ex_subr ex_temp ex_tty ex_unix ex_v ex_vadj ex_vget \
	ex_vmain ex_voper ex_vops ex_vops2 ex_vops3 ex_vput ex_vwind printf
CFILES = ex.c ex_addr.c ex_cmds.c ex_cmds2.c ex_cmdsub.c ex_data.c ex_get.c ex_io.c ex_put.c ex_re.c ex_set.c ex_subr.c \
	ex_temp.c ex_tty.c ex_unix.c ex_v.c ex_vadj.c ex_vget.c ex_vmain.c ex_voper.c ex_vops.c ex_vops2.c ex_vops3.c ex_vput.c ex_vwind.c printf.c
HFILES = ex.h ex_argv.h ex_re.h ex_temp.h ex_tty.h ex_tune.h ex_vars.h ex_vis.h

all: ex ex$(VERSION)strings

mkstr: ../mkstr.c
	$(CC) -O -o mkstr ../mkstr.c

xstr: ../xstr.c
	$(CC) -O -o xstr ../xstr.c

ex$(VERSION)strings: mkstr $(CFILES)
	rm -f ex$(VERSION)strings
	for f in $(SRCS) ; do $(RUN) ./mkstr - ex$(VERSION)strings x $$f.c || exit 1 ; done
	rm -f x*.c

# all objects in one run: xstr numbers the strings in the order it sees them
objs: mkstr xstr ex$(VERSION)strings shrink.sh $(CFILES) $(HFILES)
	rm -f strings
	for f in $(SRCS) ; do \
		$(RUN) ./mkstr - ex$(VERSION)strings x $$f.c || exit 1 ; \
		$(CCV0) -E $(CFLAGS) $(OPTIONS) x$$f.c | $(RUN) ./xstr -c - || exit 1 ; \
		$(CCV0) -S $(CFLAGS) $(OPTIONS) x.c || exit 1 ; \
		sh shrink.sh x.s ; \
		$(CAS) -u -o $$f.o x.s || exit 1 ; \
		rm -f x$$f.c x.c x.s ; \
	done
	$(RUN) ./xstr
	$(CCV0) -S xs.c
	$(CAS) -u -o strings.o xs.s
	rm -f xs.c xs.s
	touch objs

crt0-ex.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ex > $@

crt0-ex.o: crt0-ex.s
	$(CAS) -u -o $@ crt0-ex.s

ex: objs crt0-ex.o $(TERMLIB) $(LIBC_V1)
	$(LD) -i -e start -o $@ crt0-ex.o ex.o ex_addr.o ex_cmds.o ex_cmds2.o ex_cmdsub.o ex_data.o ex_get.o ex_io.o ex_put.o ex_re.o \
		ex_set.o ex_subr.o ex_temp.o ex_tty.o ex_unix.o ex_v.o ex_vadj.o ex_vget.o ex_vmain.o ex_voper.o ex_vops.o ex_vops2.o \
		ex_vops3.o ex_vput.o ex_vwind.o printf.o strings.o $(TERMLIB) $(LIBC_V1)

install: ex$(VERSION)strings
	mkdir -p $(PREFIX)/lib
	cp ex$(VERSION)strings $(PREFIX)/lib/ex$(VERSION)strings ; chmod 644 $(PREFIX)/lib/ex$(VERSION)strings

clean:
	rm -f mkstr xstr ex ex$(VERSION)strings x*.c x.s xs.c xs.s strings objs *.o crt0-ex.s

.PHONY: all install clean
