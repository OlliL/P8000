# remote: the S8000 / Development System Communication Package of Zilog (remote, local, putfile, getfile). The sources
# are those of ZEUS 3.21 (/usr/src: remote.c 2.10, local.c 1.3, putfile.c 1.8, getfile.c 1.7, Zilog 11/19/84), set back
# to the versions of WEGA (remote.c 2.8, putfile.c 1.5, getfile.c 1.6 of 11/18/83: ccvm; local.c 1.5 of 02/28/84: the
# shipped cc, a WEGA version with the X request). See compare/NAME/findings.json.
#
#   make              build ./remote ./local ./putfile ./getfile
#   make install PREFIX=dir
#   make clean

CC = cc
CCVM = ccvm
CAS = cas
LD = ld
CFLAGS = -O
PREFIX = /
STRIP = strip
LIBC = -lc
CRT = ../../lib/libc/nonseg/crt

PROGS = remote putfile getfile local

all: $(PROGS)

# remote, putfile, getfile: ccvm (the cparse of 11/18/83), the stdio.h of /usr/include, their own crt0 (crt0.list), libc
remote: crt0-remote.o remote.o
	$(LD) -e start -o $@ crt0-remote.o remote.o $(LIBC)
putfile: crt0-putfile.o putfile.o
	$(LD) -e start -o $@ crt0-putfile.o putfile.o $(LIBC)
getfile: crt0-getfile.o getfile.o
	$(LD) -e start -o $@ crt0-getfile.o getfile.o $(LIBC)

remote.o: remote.c
	$(CCVM) $(CFLAGS) -c remote.c
putfile.o: putfile.c
	$(CCVM) $(CFLAGS) -c putfile.c
getfile.o: getfile.c
	$(CCVM) $(CFLAGS) -c getfile.c

crt0-remote.s crt0-putfile.s crt0-getfile.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh `basename $@ .s | sed 's/^crt0-//'` > $@

crt0-remote.o crt0-putfile.o crt0-getfile.o: crt0-remote.s crt0-putfile.s crt0-getfile.s
	$(CAS) -u -o $@ `basename $@ .o`.s

# local: built in one step by cc (/lib/crt0.o, libc) and not stripped by the original: its symbol table names the object
# by the scratch file of cc, cc.3998.o (the PID), patched in as for putud (../Makefile)
local: local.c
	$(CC) -O -o $@ local.c
	@echo "Patching local with scratchfilename used in original cc (contains PID)"
	@dd if=local of=local1 bs=1 count=9853 >/dev/null 2>&1
	@echo -n '3998.' >>local1
	@dd if=local of=local2 bs=1 skip=9858 >/dev/null 2>&1
	@cat local2 >> local1
	@mv local1 local
	@rm local2

# local is not installed yet: partial (compare/local/findings.json); the owner decides how checkall is to take it
install: $(PROGS)
	for f in remote putfile getfile ; do cp $$f $(PREFIX)/bin/$$f ; $(STRIP) $(PREFIX)/bin/$$f ; done

clean:
	rm -f *.o crt0-*.s $(PROGS)

.PHONY: all install clean
