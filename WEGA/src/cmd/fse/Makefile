# fse: the full screen editor (Jens-Peter Redlich, Humboldt-Universitaet Berlin) and its documentation (/usr/doc/fse.doc).
# Built as the author meant it: the program is linked to fsen (not installed: the parameter block parm holds
# 1,2,3,4,5,6,7 as its mark), then fsepar, fseinst stripped to the setting of that block, fills it from termcap
# (terminal P8) and encrypts it in a copy, fse. fsepar is a program of the WEGA system that make runs itself
# (RUN: "wrun" under the emulator, empty on WEGA).
#
# fsc and fc: the compile helpers of fse (the same source but the compiler they run: fsc.c /bin/scc, fc.c /bin/cc); both
# segmented, fsc stripped with separate I and D, fc not stripped: the name of its object in the symbol table is the scratch
# file of the original scc, cc.341.o (the PID)
#
#   make                       build fse, fsc and fc
#   make install PREFIX=dir    install fse, fsc and fc below dir/bin and the documentation below dir/usr/doc (mode 644)

CC = scc
CFLAGS = -O
PREFIX = /
OBJS = fsetty.o fseglob.o fsedmp.o fsetxt0.o fsetxt1.o fsetxt2.o fsemain.o

all: fse fsc fc

fsc: fsc.c fseerror.h system
	$(CC) -O -s -i -o fsc fsc.c

fc: fc.c fseerror.h system
	$(CC) -O -o fc fc.c
	@echo "Patching fc with scratchfilename used in original scc (contains PID)"
	@dd if=fc of=fc1 bs=1 count=1921 >/dev/null 2>&1
	@echo -n '341.o' >>fc1
	@dd if=fc of=fc2 bs=1 skip=1926 >/dev/null 2>&1
	@cat fc2 >> fc1
	@mv fc1 fc
	@rm fc2

fsen: $(OBJS)
	$(CC) -s -o fsen $(OBJS) /usr/lib/slibtermlib.a

fsepar: fsepar.c fsetab.h
	$(CC) -O -s -i -o fsepar fsepar.c /usr/lib/slibtermlib.a

fse: fsen fsepar
	cp fsen fse
	$(RUN) ./fsepar fse

install: fse fsc fc
	mkdir -p $(PREFIX)/bin $(PREFIX)/usr/doc
	cp fse $(PREFIX)/bin/fse
	cp fsc $(PREFIX)/bin/fsc
	cp fc $(PREFIX)/bin/fc
	cp fse.doc $(PREFIX)/usr/doc/fse.doc ; chmod 644 $(PREFIX)/usr/doc/fse.doc

clean:
	rm -f $(OBJS) fsen fsepar fse fsc fc fsc.o fc.o

.PHONY: all install clean
