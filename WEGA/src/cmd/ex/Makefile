# ex: the sources of ex/vi 2.13 (Berkeley, from the Plexus tree cmd/plx/ex; ex_subr.c has "File not found" for the error number 2, as the WEGA
# root has it). So far only the file of the error messages, /lib/ex2.13strings, is made, the way vi.mk makes it: mkstr takes the strings of the
# error messages out of the sources, one after the other in the order of the object files (mkstr - ex2.13strings x FILE.c). mkstr (../mkstr.c)
# is a program of the WEGA system that make runs itself: RUN is "wrun" under the emulator, empty on WEGA. The other files of the program
# are not built here yet.
#
#   make install PREFIX=dir    install dir/lib/ex2.13strings

CC = cc
PREFIX = /
VERSION = 2.13
SRCS = ex ex_addr ex_cmds ex_cmds2 ex_cmdsub ex_data ex_get ex_io ex_put ex_re ex_set ex_subr ex_temp ex_tty ex_unix ex_v ex_vadj ex_vget \
	ex_vmain ex_voper ex_vops ex_vops2 ex_vops3 ex_vput ex_vwind pprintf

all: ex$(VERSION)strings

mkstr: ../mkstr.c
	$(CC) -O -o mkstr ../mkstr.c

ex$(VERSION)strings: mkstr ex.c ex_addr.c ex_cmds.c ex_cmds2.c ex_cmdsub.c ex_data.c ex_get.c ex_io.c ex_put.c ex_re.c ex_set.c ex_subr.c \
	ex_temp.c ex_tty.c ex_unix.c ex_v.c ex_vadj.c ex_vget.c ex_vmain.c ex_voper.c ex_vops.c ex_vops2.c ex_vops3.c ex_vput.c ex_vwind.c pprintf.c
	rm -f ex$(VERSION)strings
	for f in $(SRCS) ; do $(RUN) ./mkstr - ex$(VERSION)strings x $$f.c || exit 1 ; done
	rm -f x*.c

install: all
	mkdir -p $(PREFIX)/lib
	cp ex$(VERSION)strings $(PREFIX)/lib/ex$(VERSION)strings ; chmod 644 $(PREFIX)/lib/ex$(VERSION)strings

clean:
	rm -f mkstr ex$(VERSION)strings x*.c

.PHONY: all install clean
