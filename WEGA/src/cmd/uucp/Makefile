# uucp: the programs of BSD uucp (WEGA's uucp is BSD's: uuname.c of 4BSD with uucp.h of 3BSD, UNAME defined, UUCP
# /bin/uucp; uucpname.c alike in both; uulog and its modules of 3BSD: uucpdefs.h with the version string of WEGA and one
# Sysfiles entry, ulockf.c without lock() and its sccs string, rmlock keeps the result of unlink), built by the shipped cc with /lib/libc.a; and SQFILE (/usr/lib/uucp) and remotelines (/usr/spool/uucp) of the WEGA root, and the files of uucp in /usr/lib/uucp that System III's uucp.mk copies (cp $(UPATH)/L* $(UPATH)/USERFILE $(INSDIR); chmod 400 L.sys USERFILE); the files are those of the WEGA root. Of the programs uuname, uulog and uux are here yet. uux: 3BSD uux.c and modules with the changes of WEGA (fread/fwrite
# of stdin with the count in ret (no variable n), fprintf for the unknown flag, no expfile in the local receive, no sccs strings; gename: sprintf then fprintf; chkpth:
# DEBUG lines in chkperm, no ANYWRITE test, (char *)u in rdpth); uucp.h: PUBDIR /usr/spool/uucp/uucppublic.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /
CC = cc
CFLAGS = -O
STRIP = strip

all: uuname uulog uux

uuname: uuname.o uucpname.o
	$(CC) -o uuname uuname.o uucpname.o

uulog: uulog.o prefix.o xqt.o ulockf.o gnamef.o
	$(CC) -o uulog uulog.o prefix.o xqt.o ulockf.o gnamef.o

uulog.o: uulog.c uucp.h uucpdefs.h
	$(CC) $(CFLAGS) -c uulog.c

prefix.o: prefix.c uucp.h
	$(CC) $(CFLAGS) -c prefix.c

xqt.o: xqt.c uucp.h
	$(CC) $(CFLAGS) -c xqt.c

ulockf.o: ulockf.c uucp.h
	$(CC) $(CFLAGS) -c ulockf.c

gnamef.o: gnamef.c uucp.h
	$(CC) $(CFLAGS) -c gnamef.c

UUXO = uux.o gwd.o anyread.o cpmv.o expfile.o gename.o getpwinfo.o index.o lastpart.o prefix.o shio.o ulockf.o xqt.o chkpth.o getargs.o getprm.o uucpname.o versys.o

uux: $(UUXO)
	$(CC) -o uux $(UUXO)

uux.o: uux.c uucp.h
	$(CC) $(CFLAGS) -c uux.c

gwd.o: gwd.c uucp.h
	$(CC) $(CFLAGS) -c gwd.c

anyread.o: anyread.c uucp.h
	$(CC) $(CFLAGS) -c anyread.c

cpmv.o: cpmv.c uucp.h
	$(CC) $(CFLAGS) -c cpmv.c

expfile.o: expfile.c uucp.h
	$(CC) $(CFLAGS) -c expfile.c

gename.o: gename.c uucp.h
	$(CC) $(CFLAGS) -c gename.c

getpwinfo.o: getpwinfo.c uucp.h
	$(CC) $(CFLAGS) -c getpwinfo.c

index.o: index.c uucp.h
	$(CC) $(CFLAGS) -c index.c

lastpart.o: lastpart.c uucp.h
	$(CC) $(CFLAGS) -c lastpart.c

shio.o: shio.c uucp.h
	$(CC) $(CFLAGS) -c shio.c

chkpth.o: chkpth.c uucp.h
	$(CC) $(CFLAGS) -c chkpth.c

getargs.o: getargs.c uucp.h
	$(CC) $(CFLAGS) -c getargs.c

getprm.o: getprm.c uucp.h
	$(CC) $(CFLAGS) -c getprm.c

versys.o: versys.c uucp.h
	$(CC) $(CFLAGS) -c versys.c

uuname.o: uuname.c uucp.h
	$(CC) $(CFLAGS) -c uuname.c

uucpname.o: uucpname.c uucp.h
	$(CC) $(CFLAGS) -c uucpname.c

install: uuname uulog uux
	cp uux $(PREFIX)/bin/uux ; $(STRIP) $(PREFIX)/bin/uux ; chmod 555 $(PREFIX)/bin/uux
	cp uulog $(PREFIX)/bin/uulog ; $(STRIP) $(PREFIX)/bin/uulog ; chmod 555 $(PREFIX)/bin/uulog
	cp uuname $(PREFIX)/bin/uuname ; $(STRIP) $(PREFIX)/bin/uuname ; chmod 555 $(PREFIX)/bin/uuname
	mkdir -p $(PREFIX)/usr/lib/uucp
	rm -f $(PREFIX)/usr/lib/uucp/L-cmd ; cp L-cmd $(PREFIX)/usr/lib/uucp/L-cmd ; chmod 400 $(PREFIX)/usr/lib/uucp/L-cmd
	rm -f $(PREFIX)/usr/lib/uucp/L-devices ; cp L-devices $(PREFIX)/usr/lib/uucp/L-devices ; chmod 400 $(PREFIX)/usr/lib/uucp/L-devices
	rm -f $(PREFIX)/usr/lib/uucp/L.sys ; cp L.sys $(PREFIX)/usr/lib/uucp/L.sys ; chmod 400 $(PREFIX)/usr/lib/uucp/L.sys
	rm -f $(PREFIX)/usr/lib/uucp/USERFILE ; cp USERFILE $(PREFIX)/usr/lib/uucp/USERFILE ; chmod 400 $(PREFIX)/usr/lib/uucp/USERFILE
	rm -f $(PREFIX)/usr/lib/uucp/SQFILE ; cp SQFILE $(PREFIX)/usr/lib/uucp/SQFILE ; chmod 440 $(PREFIX)/usr/lib/uucp/SQFILE
	mkdir -p $(PREFIX)/usr/spool/uucp
	cp remotelines $(PREFIX)/usr/spool/uucp/remotelines ; chmod 664 $(PREFIX)/usr/spool/uucp/remotelines

clean:
	rm -f uuname uulog uux *.o

.PHONY: all install clean
