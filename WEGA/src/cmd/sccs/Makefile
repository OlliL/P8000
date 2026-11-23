# sccs: the SCCS commands of System III (usr/src/cmd/sccs): so far the help files of the command help (help.d); and the
# commands of WEGA's own source control (qcv(5)): chkwhat (byte-identical with /bin/chkwhat: ccv1, libc-v4.a).
#
#   make              build
#   make install PREFIX=dir
#   make clean

CCV1 = ccv1
CAS = cas
LD = ld
CFLAGS = -O -I../../head/oldv7
PREFIX = /
STRIP = strip
LIBC_V4 = ../../lib/libc-v4.a
CRT = ../../lib/libc/nonseg/crt
SUBDIRS = help.d

all: chkwhat
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) all ; cd .. ; done

chkwhat: crt0-chkwhat.o chkwhat.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-chkwhat.o chkwhat.o $(LIBC_V4)

crt0-chkwhat.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh chkwhat > $@

crt0-chkwhat.o: crt0-chkwhat.s
	$(CAS) -u -o $@ crt0-chkwhat.s

chkwhat.o: chkwhat.c
	$(CCV1) $(CFLAGS) -c chkwhat.c

install: chkwhat
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done
	mkdir -p $(PREFIX)/bin
	rm -f $(PREFIX)/bin/chkwhat ; cp chkwhat $(PREFIX)/bin/chkwhat ; $(STRIP) $(PREFIX)/bin/chkwhat ; chmod 755 $(PREFIX)/bin/chkwhat

clean:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done
	rm -f chkwhat chkwhat.o crt0-chkwhat.s crt0-chkwhat.o

.PHONY: all install clean

# the library of /lib is not made here (make shipped)
/lib/libc.a /lib/slibc.a:
