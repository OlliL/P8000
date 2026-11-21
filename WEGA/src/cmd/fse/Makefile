# fse: the full screen editor (Jens-Peter Redlich, Humboldt-Universitaet Berlin) and its documentation (/usr/doc/fse.doc).
# Built as the author meant it: the program is linked to fsen (not installed: the parameter block parm holds
# 1,2,3,4,5,6,7 as its mark), then fsepar, fseinst stripped to the setting of that block, fills it from termcap
# (terminal P8) and encrypts it in a copy, fse. fsepar is a program of the WEGA system that make runs itself
# (RUN: "wrun" under the emulator, empty on WEGA).
#
#   make                       build fse
#   make install PREFIX=dir    install fse below dir/bin and the documentation below dir/usr/doc (mode 644)

CC = scc
CFLAGS = -O
PREFIX = /
OBJS = fsetty.o fseglob.o fsedmp.o fsetxt0.o fsetxt1.o fsetxt2.o fsemain.o

all: fse

fsen: $(OBJS)
	$(CC) -s -o fsen $(OBJS) /usr/lib/slibtermlib.a

fsepar: fsepar.c fsetab.h
	$(CC) -O -s -i -o fsepar fsepar.c /usr/lib/slibtermlib.a

fse: fsen fsepar
	cp fsen fse
	$(RUN) ./fsepar fse

install: fse
	mkdir -p $(PREFIX)/bin $(PREFIX)/usr/doc
	cp fse $(PREFIX)/bin/fse
	cp fse.doc $(PREFIX)/usr/doc/fse.doc ; chmod 644 $(PREFIX)/usr/doc/fse.doc

clean:
	rm -f $(OBJS) fsen fsepar fse

.PHONY: all install clean
