# sccs: the SCCS commands of System III (usr/src/cmd/sccs): so far the help files of the command help (help.d); and the
# commands of WEGA's own source control (qcv(5)): chkwhat (byte-identical with /bin/chkwhat: ccv1, libc-v4.a), chkout (ccv0,
# libc-v1.a; built, not installed until it is identical), chkin (byte-identical with /bin/chkin: cc, libc-v12.a);
# common.c is in both (chkout: -DCHKOUT).
#
#   make              build
#   make install PREFIX=dir
#   make clean

CC = cc
CCV0 = ccv0
CCV1 = ccv1
CAS = cas
LD = ld
CFLAGS = -O -I../../head/oldv7
PREFIX = /
STRIP = strip
LIBC_V1 = ../../lib/libc-v1.a
LIBC_V4 = ../../lib/libc-v4.a
LIBC_V12 = ../../lib/libc-v12.a
CRT = ../../lib/libc/nonseg/crt
SUBDIRS = help.d

all: chkwhat chkout chkin
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) all ; cd .. ; done

chkwhat: crt0-chkwhat.o chkwhat.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-chkwhat.o chkwhat.o $(LIBC_V4)

crt0-chkwhat.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh chkwhat > $@

crt0-chkwhat.o: crt0-chkwhat.s
	$(CAS) -u -o $@ crt0-chkwhat.s

chkwhat.o: chkwhat.c
	$(CCV1) $(CFLAGS) -c chkwhat.c

chkout: crt0-chkout.o chkout.o chkout-common.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-chkout.o chkout.o chkout-common.o $(LIBC_V1)

crt0-chkout.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh chkout > $@

crt0-chkout.o: crt0-chkout.s
	$(CAS) -u -o $@ crt0-chkout.s

chkout.o: chkout.c chkout.h
	$(CCV0) $(CFLAGS) -c chkout.c

chkout-common.o: common.c chkout.h
	cp common.c chkout-common.c ; $(CCV0) $(CFLAGS) -DCHKOUT -c chkout-common.c ; rm -f chkout-common.c

chkin: crt0-chkin.o chkin.o chkin-common.o $(LIBC_V12)
	$(LD) -e start -o $@ crt0-chkin.o chkin.o chkin-common.o $(LIBC_V12)

crt0-chkin.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh chkin > $@

crt0-chkin.o: crt0-chkin.s
	$(CAS) -u -o $@ crt0-chkin.s

chkin.o: chkin.c chkout.h
	$(CC) -O -c chkin.c

chkin-common.o: common.c chkout.h
	cp common.c chkin-common.c ; $(CC) -O -c chkin-common.c ; rm -f chkin-common.c

install: chkwhat chkin
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done
	mkdir -p $(PREFIX)/bin
	rm -f $(PREFIX)/bin/chkwhat ; cp chkwhat $(PREFIX)/bin/chkwhat ; $(STRIP) $(PREFIX)/bin/chkwhat ; chmod 755 $(PREFIX)/bin/chkwhat
	rm -f $(PREFIX)/bin/chkin ; cp chkin $(PREFIX)/bin/chkin ; $(STRIP) $(PREFIX)/bin/chkin ; chmod 751 $(PREFIX)/bin/chkin

clean:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done
	rm -f chkwhat chkwhat.o crt0-chkwhat.s crt0-chkwhat.o chkout chkout.o chkout-common.o crt0-chkout.s crt0-chkout.o chkin chkin.o chkin-common.o crt0-chkin.s crt0-chkin.o

.PHONY: all install clean

# the library of /lib is not made here (make shipped)
/lib/libc.a /lib/slibc.a:
