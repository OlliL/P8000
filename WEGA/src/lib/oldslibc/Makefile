# oldlibc: the libraries libc-vN.a of the time of the old programs, in ..
#
# Each original program was linked from a libc of its time. Their modules are here in the versions the originals hold:
# NAME/0_n, from C with the compiler of the rule (a source of ../libc or NAME/...) or, where a compiler or a source is still
# missing, from NAME/0_n.s; the other members are those of ../libc. The order of a library is the order of the members in
# the original programs, ld takes what is referenced. Version numbers of the modules are invented.
#
#   libc-v9.a  sld (07/83)
#
#   make        (needs ../libc made, and ccv1, ccv0, ccv0p)
#   make clean

CAS = cas
AR = ar

LIBS =  ../slibc-v9.a ../slibc-v11.a
# slibc-v11.a: cas; NAME/0_2.s are cut from /bin/cas (host/tools/oldslibc_cas.py), the other members are those of /lib/slibc.a
SCCVC = sccvc

all: $(LIBS)


LIBv9 = tmp/putchar.o tmp/scanf.o tmp/doscan.o printf/0_1.o \
        	doprnt/0_1.o tmp/ffltioc.o tmp/ungetc.o tmp/filbuf.o tmp/flsbuf.o tmp/data.o \
        	tmp/__callex.o tmp/cuexit.o tmp/atoi.o tmp/cnvt.o tmp/decimal.o tmp/length.o \
        	tmp/malloc.o tmp/isatty.o tmp/mktemp.o tmp/ctype_.o tmp/stty.o tmp/strchr.o \
        	tmp/strrchr.o tmp/swab.o tmp/rindex.o tmp/strcat.o tmp/strncmp.o tmp/strcpy.o \
        	tmp/swap.o tmp/access.o tmp/tolower.o tmp/chmod.o tmp/close.o tmp/creat.o \
        	tmp/fstat.o tmp/getpid.o tmp/ioctl.o tmp/link.o tmp/lseek.o tmp/mkseg.o tmp/open.o \
        	tmp/read.o tmp/ssgbrk.o tmp/signal.o tmp/umask.o tmp/unlink.o tmp/write.o \
        	tmp/cerror.o

../slibc-v9.a: tmp/created $(LIBv9)
	sh -x mklib.sh $@ $(AR) $(LIBv9)

LIBv11 = tmp/freopen.o fseek/0_2.o rdwr/0_2.o tmp/fopen.o printf/0_2.o doprnt/0_2.o tmp/ungetc.o \
	 filbuf/0_2.o flsbuf/0_2.o endopen/0_2.o tmp/data.o findiop/0_2.o tmp/__callex.o tmp/cuexit.o \
	 perror/0_2.o cnvt/0_2.o tmp/decimal.o length/0_2.o malloc/0_2.o tmp/errlst.o tmp/isatty.o \
	 mktemp/0_2.o tmp/ctype_.o tmp/stty.o tmp/strcmp.o strcpy/0_2.o strncpy/0_2.o strlen/0_2.o \
	 tmp/access.o tmp/tolower.o tmp/chmod.o tmp/close.o tmp/creat.o tmp/getpid.o tmp/ioctl.o tmp/lseek.o \
	 tmp/mkseg.o tmp/open.o tmp/read.o tmp/ssgbrk.o tmp/signal.o tmp/umask.o tmp/unlink.o tmp/write.o \
	 tmp/cerror.o

../slibc-v11.a: tmp/created $(LIBv11)
	sh -x mklib.sh $@ $(AR) $(LIBv11)

tmp/created:
	-mkdir -p tmp
	cd tmp ; ar x /lib/slibc.a
	touch tmp/created

doprnt/0_1.o: doprnt/0_1.s
	cd doprnt ; $(CAS) -o 0_1.o -u 0_1.s

printf/0_1.o: printf/0_1.s
	cd printf ; $(CAS) -o 0_1.o -u 0_1.s

fseek/0_2.o: fseek/0_2.s
	cd fseek ; $(SCCVC) -c 0_2.s

rdwr/0_2.o: rdwr/0_2.s
	cd rdwr ; $(SCCVC) -c 0_2.s

printf/0_2.o: printf/0_2.s
	cd printf ; $(SCCVC) -c 0_2.s

doprnt/0_2.o: doprnt/0_2.s
	cd doprnt ; $(SCCVC) -c 0_2.s

filbuf/0_2.o: filbuf/0_2.s
	cd filbuf ; $(SCCVC) -c 0_2.s

flsbuf/0_2.o: flsbuf/0_2.s
	cd flsbuf ; $(SCCVC) -c 0_2.s

endopen/0_2.o: endopen/0_2.s
	cd endopen ; $(SCCVC) -c 0_2.s

findiop/0_2.o: findiop/0_2.s
	cd findiop ; $(SCCVC) -c 0_2.s

perror/0_2.o: perror/0_2.s
	cd perror ; $(SCCVC) -c 0_2.s

cnvt/0_2.o: cnvt/0_2.s
	cd cnvt ; $(SCCVC) -c 0_2.s

length/0_2.o: length/0_2.s
	cd length ; $(SCCVC) -c 0_2.s

malloc/0_2.o: malloc/0_2.s
	cd malloc ; $(SCCVC) -c 0_2.s

mktemp/0_2.o: mktemp/0_2.s
	cd mktemp ; $(SCCVC) -c 0_2.s

strcpy/0_2.o: strcpy/0_2.s
	cd strcpy ; $(SCCVC) -c 0_2.s

strncpy/0_2.o: strncpy/0_2.s
	cd strncpy ; $(SCCVC) -c 0_2.s

strlen/0_2.o: strlen/0_2.s
	cd strlen ; $(SCCVC) -c 0_2.s

clean:
	rm -f */0_*.o */*.o $(LIBS)
	rm -rf *.d
	rm -f tmp/*.o tmp/created tmp/Library.SC

.PHONY: all clean
